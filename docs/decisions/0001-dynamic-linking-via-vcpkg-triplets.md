# 1. Link vcpkg dependencies dynamically

- Status: accepted
- Date: 2026-10-31

## Context

vcpkg builds static libraries by default on Linux (`x64-linux`, `arm64-linux`). OpenCV is by far the largest dependency, and the project owner prefers linking it dynamically. The build has to work both in the x86-64 dev container and on the Raspberry Pi 5 (aarch64).

## Decision

Before `project()`, `CMakeLists.txt` sets `VCPKG_TARGET_TRIPLET` from the host architecture when it isn't already defined:

- `aarch64` / `arm64` → `arm64-linux-dynamic`
- anything else → `x64-linux-dynamic`

Both are vcpkg community triplets. They set `VCPKG_FIXUP_ELF_RPATH`, so the installed shared libraries find each other. Passing `-DVCPKG_TARGET_TRIPLET=...` still overrides the default.

## Consequences

- OpenCV, Catch2 and the other vcpkg libraries are shared objects under `build/vcpkg_installed/<triplet>/`. Binaries in the build tree find them through the RPATH that CMake sets.
- The binaries are not self-contained. Running them outside the build tree, for example after copying them to the Pi, means shipping the `.so` files along or setting up an install RPATH. That needs its own decision once deployment comes up.
- Community triplets are tested less by vcpkg than the built-in ones. They built without problems for the current dependency set.
- Build directories configured before this change keep their cached triplet and need `cmake --preset <name> --fresh`.
