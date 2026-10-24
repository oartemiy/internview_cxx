#!/usr/bin/env bash
# Run the whole build-time measurement and generate the report.
#
#   scripts/run_all.sh            # 5 clean builds, 4 jobs
#   REPS=1 JOBS=4 scripts/run_all.sh
#   RESULTS_ROOT=results-quick scripts/run_all.sh
#
# Results: $RESULTS_ROOT/baseline/{cba-report-N.txt,timing.txt,...}; summary in
# $RESULTS_ROOT/baseline/comparison.md and summary.json.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REPS="${REPS:-5}"
JOBS="${JOBS:-4}"
RESULTS_ROOT="${RESULTS_ROOT:-$ROOT/results}"
case "$RESULTS_ROOT" in
    /*) ;;
    *) RESULTS_ROOT="$ROOT/$RESULTS_ROOT" ;;
esac
export RESULTS_ROOT

# Resolve Clang and install ClangBuildAnalyzer into .tools/ if needed.
eval "$("$ROOT/scripts/setup_env.sh")"
eval "$("$ROOT/scripts/bootstrap_cba.sh")"

echo "Using CBA: $CBA"
echo "Using CXX: $CXX"

REPS="$REPS" JOBS="$JOBS" RESULTS="$RESULTS_ROOT/baseline" \
    bash "$ROOT/scripts/measure.sh" "$REPS"

python3 "$ROOT/scripts/report.py" | tee "$RESULTS_ROOT/comparison.txt"
echo
echo "See $RESULTS_ROOT/baseline/comparison.md and $RESULTS_ROOT/baseline/summary.json"
