#!/usr/bin/env bash
# Reproducible ClangBuildAnalyzer measurement for the internview service.
#
# Usage:
#   scripts/measure.sh [reps]
#
# Env overrides: REPS, JOBS, BUILD_DIR, RESULTS, CC, CXX, CBA, CMAKE_OPTS,
#                TARGET (default: internview).
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REPS="${1:-${REPS:-5}}"
JOBS="${JOBS:-4}"
TARGET="${TARGET:-internview}"
BUILD_DIR="${BUILD_DIR:-$ROOT/build-cba}"
RESULTS="${RESULTS:-$ROOT/results/baseline}"
CMAKE_OPTS="${CMAKE_OPTS:--DUSERVER_CHECK_PACKAGE_VERSIONS=0}"

if [[ -z "${CC:-}" || -z "${CXX:-}" ]]; then
    eval "$("$ROOT/scripts/setup_env.sh")"
fi
if [[ -z "${CBA:-}" ]]; then
    eval "$("$ROOT/scripts/bootstrap_cba.sh")"
fi

if [[ -z "${CC:-}" || -z "${CXX:-}" ]]; then
    echo "clang not found; run scripts/setup_env.sh first" >&2
    exit 3
fi
if [[ ! -x "${CBA:-}" ]]; then
    echo "ClangBuildAnalyzer not found at ${CBA:-<unset>}" >&2
    exit 3
fi

cd "$ROOT"
rm -rf "$RESULTS"
mkdir -p "$RESULTS"
: > "$RESULTS/timing.txt"
FAILED=0

{
    echo "target=$TARGET reps=$REPS jobs=$JOBS"
    echo "cxx=$($CXX --version | head -1)"
    echo "cmake=$(cmake --version | head -1)"
    echo "cba=$CBA"
    uname -a
} > "$RESULTS/meta.txt"

for i in $(seq 1 "$REPS"); do
    echo "=== run $i/$REPS ==="
    rm -rf "$BUILD_DIR"

    cmake -S "$ROOT" -B "$BUILD_DIR" -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_COMPILER="$CC" \
        -DCMAKE_CXX_COMPILER="$CXX" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DUSERVER_FEATURE_POSTGRESQL=ON \
        -DINTERNVIEW_FTIME_TRACE=ON \
        $CMAKE_OPTS > "$RESULTS/configure-$i.log" 2>&1
    if [[ $? -ne 0 ]]; then
        echo "cmake configure failed, see $RESULTS/configure-$i.log" >&2
        exit 4
    fi

    if ! "$CBA" --start "$BUILD_DIR"; then
        echo "ClangBuildAnalyzer --start failed" >&2
        exit 5
    fi

    START_NS="$(python3 -c 'import time; print(time.time_ns())')"
    cmake --build "$BUILD_DIR" -j "$JOBS" --target "$TARGET" \
        > "$RESULTS/build-$i.log" 2>&1
    BUILD_RC=$?
    END_NS="$(python3 -c 'import time; print(time.time_ns())')"
    WALL="$(python3 -c "print(f'{($END_NS - $START_NS) / 1e9:.2f}')")"
    printf '%s\n' "$WALL" > "$RESULTS/time-$i.txt"

    python3 "$ROOT/scripts/extract_tu_times.py" "$BUILD_DIR" "$RESULTS/tu-times-$i.csv" \
        > "$RESULTS/tu-times-$i.log" 2>&1 || true
    python3 "$ROOT/scripts/extract_phases.py" "$BUILD_DIR/.ninja_log" "$RESULTS/phases-$i.csv" "$TARGET" \
        > "$RESULTS/phases-$i.log" 2>&1 || true
    cp "$BUILD_DIR/.ninja_log" "$RESULTS/ninja-log-$i.log" 2>/dev/null || true

    if ! "$CBA" --stop "$BUILD_DIR" "$RESULTS/cba-capture-$i.bin"; then
        echo "ClangBuildAnalyzer --stop failed" >&2
        exit 5
    fi
    if ! "$CBA" --analyze "$RESULTS/cba-capture-$i.bin" > "$RESULTS/cba-report-$i.txt" 2>&1; then
        echo "ClangBuildAnalyzer --analyze failed" >&2
        exit 5
    fi

    echo "run=$i build_rc=$BUILD_RC wall_s=$WALL" | tee -a "$RESULTS/timing.txt"

    cp "$RESULTS/cba-report-$i.txt" "$RESULTS/cba-report.txt"
    cp "$RESULTS/cba-capture-$i.bin" "$RESULTS/cba-capture.bin"

    if [[ $BUILD_RC -ne 0 ]]; then
        echo "NOTE: build failed (rc=$BUILD_RC), compilation metrics are still valid" >&2
        FAILED=1
    fi
done

# Keep one canonical capture; per-run text reports are enough for the rest.
rm -f "$RESULTS"/cba-capture-[0-9]*.bin

echo "Done. Results in $RESULTS"
exit "$FAILED"
