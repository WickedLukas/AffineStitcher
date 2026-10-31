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
ctest --preset debug -R "^public\."                      # one group: public., internal., app., abi.
cmake --preset asan && cmake --build --preset asan && ctest --preset asan   # sanitizers
cmake --preset tidy && cmake --build --preset tidy       # clang-tidy on every file
cmake --build --preset debug --target docs              # API docs → build/debug/docs/html (fails on undocumented API)
./build/debug/bin/affine_stitcher -c config/affine_stitcher.example.toml --log-level debug   # --help lists all options
cmake --install build/debug                              # self-contained install → build/install/debug
```

The first configure builds OpenCV through vcpkg, which takes a while. After that it comes from the binary cache.

## Layout

- `lib/`: the shared library `libaffine_stitcher.so` (target `affine_stitcher`).
  - `lib/include/affine_stitcher/`: public API, namespace `affine_stitcher`.
  - `lib/src/`: sources and private headers; internal code goes in `affine_stitcher::detail`.
- `app/src/`: the `affine_stitcher` executable, namespace `affine_stitcher::app`. Everything but `main.cpp` is in `affine_stitcher_app_core`.
- `tests/`: Catch2 v3 tests, registered with CTest via `catch_discover_tests`.
  - `public/`: links the `.so`, exported API only.
  - `internal/`: links the library's object files, can test `detail::`.
  - `app/`: tests the app code.
  - `abi/`: export check.
  - `support/`: test helpers.
  - `data/`: test images; rules in its README.
- Binaries end up in `build/<preset>/bin/` and `build/<preset>/lib/`.
- `config/affine_stitcher.example.toml`: documents every config key and its default. A test loads it, so keep it in sync with `Config`.
- `docs/`: design docs: `architecture.md` (components, pipeline), `math.md` (coordinate and transform conventions), `decisions/` (numbered decision records)

## Conventions

- snake_case for functions and variables, PascalCase for types, `[[nodiscard]]` on pure functions.
- Prefer `cv::Matx` for small fixed-size matrices and `cv::Mat` for images.
- Public library classes use pimpl and keep a stable ABI (see `docs/decisions/0004-library-app-tests-layout.md`):
  - Mark them `AFFINE_STITCHER_EXPORT`. Keep data in `struct Impl`, and define all special members in the `.cpp`.
  - Public headers include only `affine_stitcher/` headers, OpenCV and the standard library.
  - Everything else stays in `lib/src/` and is hidden.
  - Never remove or change an exported function without bumping the minor version (it's part of the SONAME while on 0.x).
- Log through `spdlog::info(...)` etc., never `std::cout`, which is reserved for program output. New stitching settings go in `StitcherOptions` (field in `Impl`, getter, validating setter), are read from `[stitcher]` in `app/src/config.cpp`, and get an entry in the example config. App-only settings go in `Config`.
- Every behavior change gets a Catch2 test. Build and run `ctest --preset debug` before saying a change is done.
- Docs grow with the code, in the same change:
  - Public API gets a `///` Doxygen comment in its header (purpose, and `@param`/`@return`/preconditions where they aren't obvious).
  - New or changed math conventions go in `docs/math.md`.
  - Pipeline stages or components that are added or changed go in `docs/architecture.md`.
  - Design choices with trade-offs (dependencies, build setup, algorithms) get a new record in `docs/decisions/`, plus a line in its index. Never rewrite an accepted record; supersede it.
- Keep the build warning-free (see `project_warnings` in `CMakeLists.txt`).
- A hook formats edited C++ files with clang-format, so don't hand-format.
