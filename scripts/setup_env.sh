#!/usr/bin/env bash
# Resolve a Clang compiler that supports -ftime-trace.
#
# Prints shell assignments for eval:
#   eval "$(scripts/setup_env.sh)"
#
# Set CXX=/path/to/clang++ to force a compiler. Homebrew LLVM is preferred on
# macOS because Apple Clang may not support -ftime-trace.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

find_cxx() {
    local candidate
    for candidate in "${CXX:-}" clang++-18 clang++-17 clang++-16 clang++; do
        [[ -n "$candidate" ]] || continue
        if command -v "$candidate" >/dev/null 2>&1; then
            command -v "$candidate"
            return 0
        fi
    done

    if command -v brew >/dev/null 2>&1; then
        local prefix
        prefix="$(brew --prefix llvm 2>/dev/null || true)"
        if [[ -n "$prefix" && -x "$prefix/bin/clang++" ]]; then
            printf '%s\n' "$prefix/bin/clang++"
            return 0
        fi
    fi

    return 1
}

CXX="$(find_cxx || true)"
if [[ -z "$CXX" ]]; then
    echo "No Clang C++ compiler found." >&2
    echo "Install LLVM (for example: brew install llvm) or set CXX=/path/to/clang++." >&2
    exit 1
fi

if [[ -z "${CC:-}" ]]; then
    CC="${CXX%++}"
    if [[ "$CC" == "$CXX" ]]; then
        CC="${CXX%clang++}clang"
    fi
else
    CC="$(command -v "$CC" 2>/dev/null || printf '%s' "$CC")"
fi

if [[ ! -x "$CC" ]]; then
    echo "C compiler not found at $CC; set CC explicitly." >&2
    exit 1
fi

printf "export CC='%s' CXX='%s'\n" "$CC" "$CXX"
