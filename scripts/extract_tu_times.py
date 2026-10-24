#!/usr/bin/env python3
"""Extract per-TU frontend/backend times from clang -ftime-trace JSON files.

Usage: scripts/extract_tu_times.py <build_dir> <out.csv>
"""

import csv
import glob
import json
import os
import sys


def main():
    build_dir, out_path = sys.argv[1], sys.argv[2]
    rows = []
    pattern = os.path.join(build_dir, "**", "*.json")
    for path in glob.glob(pattern, recursive=True):
        try:
            with open(path, encoding="utf-8") as fh:
                data = json.load(fh)
        except (OSError, ValueError):
            continue
        if not isinstance(data, dict):
            continue
        events = data.get("traceEvents")
        if not isinstance(events, list):
            continue
        totals = {
            e.get("name"): e.get("dur", 0)
            for e in events
            if isinstance(e, dict) and e.get("name") in ("Total Frontend", "Total Backend")
        }
        if "Total Frontend" not in totals:
            continue
        name = os.path.relpath(path, build_dir)
        rows.append((name, totals["Total Frontend"] / 1000.0, totals.get("Total Backend", 0) / 1000.0))

    rows.sort(key=lambda row: row[1], reverse=True)
    with open(out_path, "w", newline="", encoding="utf-8") as fh:
        writer = csv.writer(fh)
        writer.writerow(["tu", "frontend_ms", "backend_ms"])
        writer.writerows(rows)
    print(f"{len(rows)} traces -> {out_path}")


if __name__ == "__main__":
    main()
