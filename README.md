# cpp-concurrency-in-action

A C++23 project (library + app) built with CMake and Conan 2, tested with GoogleTest.

## Layout

| Path | Purpose |
|------|---------|
| `include/cpp_concurrency_in_action/` | Public library headers |
| `src/` | Library implementation (`cpp_concurrency_in_action` target) |
| `app/` | Executable that links the library (`cpp-concurrency-in-action` target) |
| `tests/` | GoogleTest suite (`cpp_concurrency_in_action_tests` target) |

## Prerequisites

- CMake >= 3.26 and Ninja
- Conan 2 (`pipx install conan`)
- A C++23 compiler (recent Clang or GCC)
- Conan profiles named `debug` and `release` (see [Conan profiles](#conan-profiles))

## Quick start (make)

A `Makefile` wraps the full workflow:

```bash
make              # configure (if needed) + build   [Debug]
make test         # build + run the test suite
make run          # build + run the app
make format       # clang-format every source in place
make format-check # clang-format dry-run (what CI enforces)
make clean        # remove the build tree + generated presets
```

Release builds: append `BUILD_TYPE=Release` to any target, e.g.
`make test BUILD_TYPE=Release`.

> **First build of a config may take a minute.** `conan install --build=missing`
> compiles any dependency (e.g. GoogleTest) that has no cached binary for your
> profile. It can look quiet, but it is building, not hung — later builds reuse
> the Conan cache and start instantly.

`make` selects the Conan host profile via `-pr`, named after the build type
(`-pr debug` / `-pr release`). See [Conan profiles](#conan-profiles).

### What each target runs

| `make` target | Equivalent commands |
|---------------|---------------------|
| `make` / `make build` | `conan install . --build=missing -pr debug`<br>`cmake --preset debug`<br>`cmake --build --preset debug` |
| `make test` | *(build, then)* `ctest --preset debug --output-on-failure` |
| `make run` | *(build, then)* `./build/Debug/bin/cpp-concurrency-in-action` |
| `make format` | `clang-format --style=file -i` over `app/ src/ tests/ include/` |
| `make format-check` | the same with `--dry-run --Werror` |
| `make clean` | `rm -rf build CMakeUserPresets.json` |

For `BUILD_TYPE=Release`, swap `-pr debug` -> `-pr release` and
`debug` -> `release` in the presets.

## Build & test (manual)

The same steps without `make`:

```bash
# 1. Resolve dependencies and generate the CMake toolchain.
conan install . --build=missing -pr debug   # see "Conan profiles" below

# 2. Configure, build, and test via the project presets (CMakePresets.json).
cmake --preset debug
cmake --build --preset debug
ctest --preset debug --output-on-failure

# 3. Run the app.
./build/Debug/bin/cpp-concurrency-in-action
```

For a release build, swap `-pr debug` -> `-pr release` and
`debug` -> `release` in the presets.

### Presets

`CMakePresets.json` defines `debug` and `release` configure/build/test presets:
Ninja, clang/clang++, `CMAKE_EXPORT_COMPILE_COMMANDS=ON` (for clangd), building
into `build/Debug` / `build/Release`. Each preset points at the Conan-generated
toolchain in `build/<Type>/generators/`, so `conan install` must run before the
first configure of a given type. Conan also writes its own `CMakeUserPresets.json`
(`conan-debug` / `conan-release`); those keep working, but the project presets
are the canonical interface (the `Makefile` and CI use them).

Build outputs:

| Artifact | Path |
|----------|------|
| App | `build/Debug/bin/cpp-concurrency-in-action` |
| Library | `build/Debug/src/libcpp_concurrency_in_action.a` |
| Tests | `build/Debug/bin/cpp_concurrency_in_action_tests` |

> **C++ standard note:** CMake pins this project to C++23. If a profile sets a
> lower `compiler.cppstd` (e.g. `gnu20`), dependencies still build fine; to align
> them, set `compiler.cppstd=gnu23` in the profile (or add `-s compiler.cppstd=gnu23`).

## Conan profiles

`make` and the manual commands pick a Conan **host profile** with `-pr`
(`-pr` is shorthand for `--profile:host`), named after the build type:
`-pr debug` for Debug, `-pr release` for Release. Create them once if you
don't already have them:

```bash
conan profile detect --force                              # ~/.conan2/profiles/default (Release)
cp ~/.conan2/profiles/default ~/.conan2/profiles/release
cp ~/.conan2/profiles/default ~/.conan2/profiles/debug
sed -i 's/^build_type=.*/build_type=Debug/' ~/.conan2/profiles/debug
```

The two profiles only need to differ in `build_type`.

## Formatting

Code style lives in `.clang-format` (Google base). Use `make format` /
`make format-check`, or call clang-format directly:

```bash
find app src tests include -name '*.cpp' -o -name '*.hpp' \
  | xargs clang-format --style=file -i
```

## Continuous integration

`.github/workflows/ci.yml` runs two jobs on every PR to `main` (and on pushes to
`main`): a **clang-format** check and a **build & test** job. To make them block
merges, enable branch protection on `main` (GitHub → Settings → Branches → add
rule for `main`) and mark `clang-format` and `build & test` as required status
checks.

CI deliberately uses `conan profile detect` + `-s build_type=Release` rather than
`-pr release`: a fresh runner has no named profiles, so it detects the runner's
own compiler. Local builds use the named `debug`/`release` profiles instead.
Note the presets pin clang/clang++ for the project itself (preinstalled on
`ubuntu-latest`), even if the detected Conan profile builds dependencies with GCC —
both use libstdc++, so linking is compatible.
