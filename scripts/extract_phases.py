#!/usr/bin/env python3
"""Parse ninja's .ninja_log into build-phase timings.

Usage: scripts/extract_phases.py <build_dir>/.ninja_log <out.csv> [target]

Ninja writes start/end timestamps in milliseconds relative to the start of the
build. We report the end of the last compilation, the start and duration of the
final link, and the total build time.
"""

import csv
import os
import sys


def main():
    log_path, out_path = sys.argv[1], sys.argv[2]
    target = sys.argv[3] if len(sys.argv) > 3 else "internview"
    if not os.path.exists(log_path):
        raise SystemExit(f"no ninja log at {log_path}")

    entries = []
    with open(log_path, encoding="utf-8", errors="replace") as fh:
        for line in fh:
            if line.startswith("#"):
                continue
            parts = line.rstrip("\n").split("\t")
            if len(parts) < 5:
                continue
            try:
                start, end = float(parts[0]), float(parts[1])
            except ValueError:
                continue
            entries.append((start, end, parts[3]))

    compilations = [e for e in entries if e[2].endswith((".o", ".a", ".pch"))]
    links = [e for e in entries if os.path.basename(e[2]) == target]

    row = {
        "compile_end_ms": max((e[1] for e in compilations), default=0),
        "link_start_ms": min((e[0] for e in links), default=0),
        "link_ms": max((e[1] - e[0] for e in links), default=0),
        "total_ms": max((e[1] for e in entries), default=0),
    }
    with open(out_path, "w", newline="", encoding="utf-8") as fh:
        writer = csv.DictWriter(fh, fieldnames=list(row))
        writer.writeheader()
        writer.writerow(row)
    print(row)


if __name__ == "__main__":
    main()
