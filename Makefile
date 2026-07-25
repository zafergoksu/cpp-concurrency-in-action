# Convenience wrapper around the Conan 2 + CMake preset workflow.
#
#   make              # configure (if needed) + build  [Debug]
#   make test         # build + run the test suite
#   make run          # build + run the app
#   make format       # clang-format every source in place
#   make format-check # clang-format dry-run (what CI enforces)
#   make clean        # remove the build tree + generated presets
#
# Release build: append BUILD_TYPE=Release to any target, e.g.
#   make test BUILD_TYPE=Release
#
# Builds use the Conan profile named after the build type via `-pr` (the host
# profile): BUILD_TYPE=Debug -> `-pr debug`, Release -> `-pr release`. Profiles
# named `debug` and `release` must therefore exist (see README "Conan profiles").
#
# Note: the first build of a given BUILD_TYPE runs `conan install --build=missing`,
# which compiles any uncached dependency (e.g. GoogleTest) from source. That step
# can look quiet for a minute but is building, not hung; later builds use the cache.

BUILD_TYPE ?= Debug
PROFILE    := $(shell printf '%s' '$(BUILD_TYPE)' | tr '[:upper:]' '[:lower:]')
PRESET     := $(PROFILE)
BUILD_DIR  := build/$(BUILD_TYPE)
STAMP      := $(BUILD_DIR)/.configured
FIND_SRC   := find app src tests include -type f \( -name '*.cpp' -o -name '*.hpp' \)

.PHONY: all build test run format format-check clean help

all: build

# Resolve dependencies and configure once; re-runs only after `make clean`.
$(STAMP):
	conan install . --build=missing -pr $(PROFILE)
	cmake --preset $(PRESET)
	@touch $(STAMP)

build: $(STAMP)
	cmake --build --preset $(PRESET)

test: build
	ctest --preset $(PRESET) --output-on-failure

run: build
	./$(BUILD_DIR)/bin/cpp-concurrency-in-action

format:
	$(FIND_SRC) -print0 | xargs -0 --no-run-if-empty clang-format --style=file -i

format-check:
	$(FIND_SRC) -print0 | xargs -0 --no-run-if-empty clang-format --style=file --dry-run --Werror

clean:
	rm -rf build CMakeUserPresets.json

help:
	@echo "targets: build (default) | test | run | format | format-check | clean"
	@echo "release: make <target> BUILD_TYPE=Release"
