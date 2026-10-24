# internview

Template of a C++ service that uses [userver framework](https://github.com/userver-framework/userver).


## Download and Build

To create your own userver-based service follow the following steps:

1. Press the "Use this template button" at the top right of this GitHub page
2. Clone the service `git clone your-service-repo && cd your-service-repo && git submodule update --init`
3. Give a proper name to your service and replace all the occurrences of "internview" string with that name
4. Feel free to tweak, adjust or fully rewrite the source code of your service.


## Makefile

`PRESET` is either `debug`, `release`, or if you've added custom presets in `CMakeUserPresets.json`, it
can also be `debug-custom`, `release-custom`.

* `make cmake-PRESET` - run cmake configure, update cmake options and source file lists
* `make build-PRESET` - build the service
* `make test-PRESET` - build the service and run all tests
* `make start-PRESET` - build the service, start it in testsuite environment and leave it running
* `make install-PRESET` - build the service and install it in directory set in environment `PREFIX`
* `make` or `make all` - build and run all tests in `debug` and `release` modes
* `make format` - reformat all C++ and Python sources
* `make dist-clean` - clean build files and cmake cache
* `make docker-COMMAND` - run `make COMMAND` in docker environment
* `make docker-clean-data` - stop docker containers


## Build-time profiling (ClangBuildAnalyzer)

The `scripts/` directory contains a reproducible build-time profiler. It builds
the `internview` target from scratch with `-ftime-trace`, captures the trace with
ClangBuildAnalyzer, and writes per-run and aggregated reports.

Requirements: Clang with `-ftime-trace` support (Homebrew LLVM is recommended on
macOS; Apple Clang may not support it), Ninja, CMake, and Python 3.

```bash
make cba             # download/build ClangBuildAnalyzer into .tools/
make measure-quick   # 1 clean Release build, writes results-quick/
make measure         # 5 clean Release builds, writes results/
make results         # print results/baseline/comparison.md
make results-quick   # print results-quick/baseline/comparison.md
```

The first `make cba` needs network access: it clones
[ClangBuildAnalyzer](https://github.com/aras-p/ClangBuildAnalyzer) into
`.tools/ClangBuildAnalyzer` and builds it locally. Nothing is installed
system-wide. The build is done in `build-cba/`, so `build-debug/` is untouched.

Useful environment variables:

* `REPS` — number of clean builds (default 5);
* `JOBS` — parallel build jobs (default 4);
* `CMAKE_OPTS` — extra CMake arguments;
* `CXX` / `CC` — force a specific compiler;
* `CBA` — use an existing ClangBuildAnalyzer binary;
* `CBA_SRC` — use another ClangBuildAnalyzer source checkout.

Raw data and the report are written under `results/baseline/`:

* `cba-report-N.txt` — full CBA report for run N;
* `tu-times-N.csv` — per-TU frontend/backend times;
* `phases-N.csv` — compile/link phases from `.ninja_log`;
* `timing.txt` — wall-clock time for every run;
* `comparison.md`, `summary.json` — aggregated medians and machine-readable data.

`make measure-quick` writes the same layout under `results-quick/baseline/`, so
it does not overwrite the canonical `results/`.


## License

The original template is distributed under the [Apache-2.0 License](https://github.com/userver-framework/userver/blob/develop/LICENSE)
and [CLA](https://github.com/userver-framework/userver/blob/develop/CONTRIBUTING.md). Services based on the template may change
the license and CLA.