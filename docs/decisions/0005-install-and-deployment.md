# 5. Self-contained install tree with bundled vcpkg libraries

- Status: accepted
- Date: 2026-10-31

## Context

[0001](0001-dynamic-linking-via-vcpkg-triplets.md) left deployment open: with shared libraries, the binaries aren't self-contained. The Pi runs Raspberry Pi OS Bookworm, whose system OpenCV is 4.6, while the project builds OpenCV 4.14 with vcpkg. Other projects should also be able to use the library via `find_package`.

## Decision

- `cmake --install build/<preset>` writes a relocatable tree to `build/install/<preset>/` (the preset's `installDir`):
  ```
  bin/affine_stitcher                       RPATH $ORIGIN/../lib
  lib/libaffine_stitcher.so.0.1(.0)         RPATH $ORIGIN
  lib/<vcpkg .so files the app needs>       OpenCV, spdlog, fmt, CLI11, toml++, zlib, ...
  lib/cmake/AffineStitcher/                 AffineStitcherConfig.cmake, targets, version
  include/affine_stitcher/                  public headers + export.hpp
  share/affine_stitcher/                    example config
  ```
- The vcpkg libraries are bundled with `install(RUNTIME_DEPENDENCY_SET)`. It keeps every resolved dependency under `vcpkg_installed/` and leaves out system libraries (libc, libstdc++, …), which come from the target OS. Symlink chains (`.so.414` → `.so.4.14.0`) are installed as they are. vcpkg's dynamic triplets set `$ORIGIN` RPATHs on their libraries, so they find each other in `lib/`.
- The package is version-compatible by `SameMinorVersion`, matching the SONAME rule in [0004](0004-library-app-tests-layout.md). Its config calls `find_dependency(OpenCV)`, because OpenCV types are part of the API.

## Consequences

- The whole install folder can be copied to the target and run there without installing anything. This was checked on x86-64 by running a copy from an unrelated folder with an empty environment: every non-system library resolved inside the copy.
- It must be built on (or for) the target architecture. An x86-64 install doesn't run on the Pi. Cross-compiling for aarch64 is a separate step that hasn't been decided yet.
- The target's system libraries must be at least as new as the build machine's. The dev container uses Bookworm like the Pi, so they match.
- A Debug install bundles debug builds of OpenCV (around 60 MB for `opencv_core` alone). Use the `release` preset for deployment.
- Projects using the package need OpenCV's CMake files themselves (`find_dependency`). The bundled `.so` files are enough to run, not to build against.
