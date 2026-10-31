# Architecture

## Components

| Target | Sources | Role |
|---|---|---|
| `affine_stitcher_core` | `src/*.cpp`, `include/affine_stitcher/` | the library: all stitching logic |
| `affine_stitcher` | `src/main.cpp` | command-line executable built on the library |
| `affine_stitcher_tests` | `tests/` | Catch2 unit tests for the library |

## Implemented

- Affine transform helpers (`affine.hpp`): conversion to homogeneous form and composition. Conventions are in [math.md](math.md).

## Planned pipeline

None of these stages exists yet. Update this section as each one lands.

1. Feature detection and description per input image
2. Feature matching between overlapping image pairs
3. Robust affine estimation per pair (e.g. RANSAC)
4. Chaining the pairwise transforms into a common reference frame (using `compose`)
5. Warping each image into the output canvas
6. Blending the overlapping regions
