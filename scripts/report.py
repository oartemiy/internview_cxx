#!/usr/bin/env python3
"""Build a markdown/JSON report from ClangBuildAnalyzer measurements.

Usage: scripts/report.py

Reads results/baseline/{cba-report-N.txt,tu-times-N.csv,phases-N.csv,timing.txt}
and writes results/baseline/comparison.md + summary.json.
"""

import csv
import glob
import json
import os
import re
import statistics

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RESULTS_ROOT = os.environ.get("RESULTS_ROOT", os.path.join(ROOT, "results"))
RESULTS = os.environ.get("RESULTS", os.path.join(RESULTS_ROOT, "baseline"))

RE_SUMMARY = re.compile(r"Compilation \((\d+) times\)")
RE_FRONTEND = re.compile(r"Parsing \(frontend\):\s+([\d.]+) s")
RE_BACKEND = re.compile(r"Codegen & opts \(backend\):\s+([\d.]+) s")
RE_RUN = re.compile(r"run=(\d+) build_rc=(\d+) wall_s=([\d.]+)")
RE_FILE = re.compile(r"^\s+(\d+) ms: (\S+)$")
RE_HEADER = re.compile(r"^(\d+) ms: (.*?) \(included (\d+) times, avg (\d+) ms\)")
RE_TARGET = re.compile(r"^CMakeFiles/([^/]+)\.dir/(.*)$")

PHASES = ["compile_end_ms", "link_start_ms", "link_ms", "total_ms"]


def median(values):
    return statistics.median(values) if values else None


def header_key(path):
    path = path.strip()
    if path.startswith(ROOT + "/"):
        path = path[len(ROOT) + 1:]
    elif path.startswith("/usr/include/"):
        path = path[len("/usr/include/"):]
    else:
        match = re.match(r"^.*?/build-[^/]+/(.*)$", path)
        if match:
            path = match.group(1)
    return re.sub(r"^build-[^/]+/", "", path)


def tu_key(relpath):
    rel = relpath.replace("\\", "/")
    if rel.endswith(".json"):
        rel = rel[:-5]
    match = RE_TARGET.match(rel)
    if not match:
        return rel
    return match.group(2)


def parse_report(path):
    text = open(path, encoding="utf-8", errors="replace").read()
    m = RE_SUMMARY.search(text)
    f = RE_FRONTEND.search(text)
    b = RE_BACKEND.search(text)
    if not (m and f and b):
        raise ValueError(f"cannot parse summary in {path}")

    files, headers = {}, {}
    for line in text.splitlines():
        fm = RE_FILE.match(line)
        if fm:
            files[os.path.basename(fm.group(2))] = int(fm.group(1))
        hm = RE_HEADER.match(line)
        if hm:
            headers[header_key(hm.group(2))] = int(hm.group(1))

    return {
        "compilations": int(m.group(1)),
        "frontend_s": float(f.group(1)),
        "backend_s": float(b.group(1)),
        "files_ms": files,
        "headers_ms": headers,
    }


def numbered(directory, prefix, suffix):
    return sorted(
        glob.glob(os.path.join(directory, f"{prefix}-[0-9]*{suffix}")),
        key=lambda p: int(re.search(r"-(\d+)" + re.escape(suffix) + r"$", p).group(1)),
    )


def load_tu_csv(directory):
    runs = []
    for path in numbered(directory, "tu-times", ".csv"):
        rows = {}
        with open(path, newline="", encoding="utf-8") as fh:
            for row in csv.DictReader(fh):
                rows[tu_key(row["tu"])] = (float(row["frontend_ms"]), float(row["backend_ms"]))
        runs.append(rows)

    result = {}
    for name in {n for run in runs for n in run}:
        frontends = [run[name][0] for run in runs if name in run]
        backends = [run[name][1] for run in runs if name in run]
        result[name] = (median(frontends), median(backends), len(frontends))
    return result


def load_phases(directory):
    runs = []
    for path in numbered(directory, "phases", ".csv"):
        with open(path, newline="", encoding="utf-8") as fh:
            rows = list(csv.DictReader(fh))
        if rows:
            runs.append({key: float(rows[0][key]) for key in PHASES})
    return {key: median([run[key] for run in runs]) for key in PHASES} if runs else {}


def load_mode():
    reports = numbered(RESULTS, "cba-report", ".txt")
    if not reports:
        raise SystemExit(f"no reports in {RESULTS}; run scripts/measure.sh first")

    runs = [parse_report(p) for p in reports]
    walls = []
    timing = os.path.join(RESULTS, "timing.txt")
    if os.path.exists(timing):
        for line in open(timing, encoding="utf-8"):
            rm = RE_RUN.search(line)
            if rm:
                walls.append(float(rm.group(3)))

    headers = {}
    for name in {n for r in runs for n in r["headers_ms"]}:
        values = [r["headers_ms"][name] for r in runs if name in r["headers_ms"]]
        headers[name] = median(values)

    return {
        "runs": runs,
        "walls": walls,
        "frontend": median([r["frontend_s"] for r in runs]),
        "backend": median([r["backend_s"] for r in runs]),
        "wall": median(walls),
        "compilations": runs[0]["compilations"],
        "tu": load_tu_csv(RESULTS),
        "phases": load_phases(RESULTS),
        "headers": headers,
    }


def cell(value, unit=""):
    return "n/a" if value is None else f"{value:.1f}{unit}"


def main():
    data = load_mode()
    frontend, backend = data["frontend"], data["backend"]
    cpu = None if frontend is None or backend is None else frontend + backend

    lines = []
    lines.append("# internview build-time profile (median of repeated clean builds)\n")
    lines.append(
        f"Runs: {len(data['runs'])}. Each run is a clean configure + "
        f"`ninja internview` with `-ftime-trace`. Frontend/backend are CBA sums "
        "over all traced compilations.\n"
    )
    lines.append("| metric | median |")
    lines.append("|---|---:|")
    lines.append(f"| TUs compiled | {data['compilations']} |")
    lines.append(f"| frontend, s | {cell(frontend)} |")
    lines.append(f"| backend, s | {cell(backend)} |")
    lines.append(f"| compile CPU total, s | {cell(cpu)} |")
    lines.append(f"| wall clock, s | {cell(data['wall'])} |")

    if data["phases"]:
        lines.append("\n## Build phases from .ninja_log (median, ms)\n")
        lines.append("| phase | median |")
        lines.append("|---|---:|")
        for label, key in [
            ("compilation finished at", "compile_end_ms"),
            ("link started at", "link_start_ms"),
            ("link `internview`", "link_ms"),
            ("build total", "total_ms"),
        ]:
            lines.append(f"| {label} | {cell(data['phases'].get(key))} |")

    lines.append("\n## Per-run wall clock, s\n")
    lines.append("| run | wall |")
    lines.append("|---|---:|")
    for i, wall in enumerate(data["walls"], 1):
        lines.append(f"| {i} | {wall:.2f} |")

    lines.append("\n## Translation units by frontend time (median, ms)\n")
    lines.append("| TU | frontend | backend |")
    lines.append("|---|---:|---:|")
    names = sorted(data["tu"], key=lambda n: data["tu"][n][0] or 0, reverse=True)
    for name in names:
        front, back, _ = data["tu"][name]
        lines.append(f"| `{name}` | {cell(front)} | {cell(back)} |")

    lines.append("\n## Top headers by inclusive parse time (median, ms)\n")
    lines.append("| header | inclusive |")
    lines.append("|---|---:|")
    top_headers = sorted(data["headers"], key=lambda n: data["headers"][n] or 0, reverse=True)[:15]
    for name in top_headers:
        lines.append(f"| `{name}` | {cell(data['headers'][name])} |")

    report = "\n".join(lines) + "\n"
    print(report)
    os.makedirs(RESULTS, exist_ok=True)
    with open(os.path.join(RESULTS, "comparison.md"), "w", encoding="utf-8") as fh:
        fh.write(report)

    summary = {
        "runs": len(data["runs"]),
        "compilations": data["compilations"],
        "frontend_s": frontend,
        "backend_s": backend,
        "cpu_total_s": cpu,
        "wall_s": data["wall"],
        "walls_s": data["walls"],
        "phases_median_ms": data["phases"],
        "tu_median_ms": {
            name: {"frontend": value[0], "backend": value[1], "runs": value[2]}
            for name, value in data["tu"].items()
        },
        "headers_median_ms": data["headers"],
    }
    with open(os.path.join(RESULTS, "summary.json"), "w", encoding="utf-8") as fh:
        json.dump(summary, fh, indent=2, ensure_ascii=False)


if __name__ == "__main__":
    main()
