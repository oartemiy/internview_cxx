#!/usr/bin/env bash
# Locate or build ClangBuildAnalyzer locally in .tools/.
#
# Prints shell assignments for eval:
#   eval "$(scripts/bootstrap_cba.sh)"
#
# Set CBA=/path/to/ClangBuildAnalyzer to use an existing binary.
# Set CBA_SRC=/path/to/ClangBuildAnalyzer to use another source checkout.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

if [[ -n "${CBA:-}" ]]; then
    if [[ -x "$CBA" ]]; then
        printf "export CBA='%s'\n" "$CBA"
        exit 0
    fi
    echo "CBA is set but is not executable: $CBA" >&2
    exit 1
fi

CBA="$ROOT/.tools/ClangBuildAnalyzer/build/ClangBuildAnalyzer"
if [[ -x "$CBA" ]]; then
    printf "export CBA='%s'\n" "$CBA"
    exit 0
fi

SRC="${CBA_SRC:-$ROOT/.tools/ClangBuildAnalyzer}"
JOBS="${JOBS:-4}"

if [[ ! -d "$SRC" ]]; then
    mkdir -p "$(dirname "$SRC")"
    echo "Cloning ClangBuildAnalyzer into $SRC" >&2
    git clone --depth 1 https://github.com/aras-p/ClangBuildAnalyzer.git "$SRC" >&2
elif [[ ! -d "$SRC/.git" ]]; then
    echo "Using existing ClangBuildAnalyzer source at $SRC" >&2
fi

cmake -S "$SRC" -B "$SRC/build" -DCMAKE_BUILD_TYPE=Release >&2
cmake --build "$SRC/build" -j "$JOBS" >&2

CBA="$SRC/build/ClangBuildAnalyzer"
if [[ ! -x "$CBA" ]]; then
    echo "ClangBuildAnalyzer build failed: $CBA not found" >&2
    exit 1
fi

printf "export CBA='%s'\n" "$CBA"
