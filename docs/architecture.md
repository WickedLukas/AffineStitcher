# Architecture

## Components

| Target | Sources | Role |
|---|---|---|
| `affine_stitcher_core` | `src/*.cpp`, `include/affine_stitcher/` | the library: all stitching logic |
| `affine_stitcher` | `src/main.cpp` | command-line executable built on the library |
| `affine_stitcher_tests` | `tests/` | Catch2 unit tests for the library |

## Implemented

- Affine transform helpers (`affine.hpp`): conversion to homogeneous form and composition. Conventions are in [math.md](math.md).
- Configuration (`config.hpp`): the `Config` struct, loaded from a TOML file. See `config/affine_stitcher.example.toml` for every key.
- Command line (`cli.hpp`): CLI11 options and `resolve_config`, which combines defaults, the config file and the options.
- Logging (`logging.hpp`): `setup_logging` installs spdlog's default logger, writing to stderr and optionally to a rotating file.

## Startup

`main` parses the command line, resolves the config (defaults, then the config file, then command-line options), sets up logging and then runs. Errors in the config file or options stop the program with a message naming the file and line. See [decision 0003](decisions/0003-logging-config-and-command-line-libraries.md).

## Planned pipeline

None of these stages exists yet. Update this section as each one lands.

1. Feature detection and description per input image
2. Feature matching between overlapping image pairs
3. Robust affine estimation per pair (e.g. RANSAC)
4. Chaining the pairwise transforms into a common reference frame (using `compose`)
5. Warping each image into the output canvas
6. Blending the overlapping regions
