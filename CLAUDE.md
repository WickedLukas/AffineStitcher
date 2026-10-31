# AffineStitcher

C++20 image stitching with affine transforms, built on OpenCV 4 and Eigen. Targets x86-64 Linux now and a Raspberry Pi 5 (Raspberry Pi OS Bookworm, aarch64) later.

## Environment

- Runs in a dev container (Debian Bookworm, GCC 12, matching the Pi). The compiler is GCC only. clangd, clang-tidy and clang-format (LLVM 23) are used as tooling.
- Outbound network is limited to an allowlist (`.devcontainer/allowed-domains.txt`). `sudo` and `apt` are not available. If a new system package or domain is needed, say so instead of working around it.
- WebFetch and `curl` only reach the domains in `.devcontainer/allowed-domains.txt`. Other hosts fail immediately with a connection error from the firewall. Still use WebFetch whenever a page would help, since the user approves each new domain. If a fetch is blocked, name the domain so the user can decide whether to allowlist it.
- There's no display. Write debug images to `build/out/` (e.g. with `cv::imwrite`) instead of using `cv::imshow`, then look at the PNG.
- Dependencies come from vcpkg in manifest mode (`vcpkg.json`). Don't change `builtin-baseline` without asking.

## Commands

```sh
cmake --preset debug && cmake --build --preset debug     # configure + build
ctest --preset debug                                     # run all tests
ctest --preset debug -R "compose"                        # run matching tests
cmake --preset asan && cmake --build --preset asan && ctest --preset asan   # sanitizers
cmake --preset tidy && cmake --build --preset tidy       # clang-tidy on every file
cmake --build --preset debug --target docs              # API docs → build/debug/docs/html (fails on undocumented API)
./build/debug/affine_stitcher
```

The first configure builds OpenCV through vcpkg, which takes a while. After that it comes from the binary cache.

## Layout

- `include/affine_stitcher/`: public headers, namespace `affine_stitcher`
- `src/`: library sources (`affine_stitcher_core`) plus `main.cpp` (`affine_stitcher` executable)
- `tests/`: Catch2 v3 tests, registered with CTest via `catch_discover_tests`
- `docs/`: design docs: `architecture.md` (components, pipeline), `math.md` (coordinate and transform conventions), `decisions/` (numbered decision records)

## Conventions

- snake_case for functions and variables, PascalCase for types, `[[nodiscard]]` on pure functions.
- Prefer `cv::Matx` for small fixed-size matrices and `cv::Mat` for images.
- Every behavior change gets a Catch2 test. Build and run `ctest --preset debug` before saying a change is done.
- Docs grow with the code, in the same change:
  - Public API gets a `///` Doxygen comment in its header (purpose, and `@param`/`@return`/preconditions where they aren't obvious).
  - New or changed math conventions go in `docs/math.md`.
  - Pipeline stages or components that are added or changed go in `docs/architecture.md`.
  - Design choices with trade-offs (dependencies, build setup, algorithms) get a new record in `docs/decisions/`, plus a line in its index. Never rewrite an accepted record; supersede it.
- Keep the build warning-free (see `project_warnings` in `CMakeLists.txt`).
- A hook formats edited C++ files with clang-format, so don't hand-format.
