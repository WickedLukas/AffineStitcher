# Architecture

## Components

| Target | Sources | Role |
|---|---|---|
| `affine_stitcher` (`libaffine_stitcher.so`) | `lib/` | the library: all stitching logic. Public API: `Stitcher`, `StitcherOptions` |
| `affine_stitcher_objects` | `lib/src/` | the library's object files, shared by the `.so` and the internal tests |
| `affine_stitcher_app_core` | `app/src/` (all but `main.cpp`) | command line, config file, logging setup |
| `affine_stitcher_app` (binary `affine_stitcher`) | `app/src/main.cpp` | the command-line application |
| `affine_stitcher_{public,internal,app}_tests` | `tests/` | Catch2 tests, plus the `abi.exported_symbols` check |

How the parts depend on each other, and why, is in [decision 0004](decisions/0004-library-app-tests-layout.md). Installing and deployment are covered in [decision 0005](decisions/0005-install-and-deployment.md).

## Implemented

Library (`lib/`):

- `Stitcher` (`stitcher.hpp`): pimpl class, move-only. `stitch()` throws `std::logic_error` until the pipeline exists.
- `StitcherOptions` (`stitcher_options.hpp`): pimpl settings with validating setters: `max_features`, `ransac_threshold`.
- Affine helpers (`lib/src/affine.hpp`, internal, `affine_stitcher::detail`): conversion to homogeneous form and composition. Conventions are in [math.md](math.md).

Application (`app/`, namespace `affine_stitcher::app`):

- Configuration (`config.hpp`): the `Config` struct, loaded from a TOML file. The `[stitcher]` table fills a `StitcherOptions`. See `config/affine_stitcher.example.toml` for every key.
- Command line (`cli.hpp`): CLI11 options and `resolve_config`, which combines defaults, the config file and the options.
- Logging (`logging.hpp`): `setup_logging` installs spdlog's default logger, writing to stderr and optionally to a rotating file. The library's log messages go there as well.

## Startup

`main` parses the command line, resolves the config (defaults, then the config file, then command-line options), sets up logging, and creates a `Stitcher` from the `[stitcher]` settings. Errors in the config file or options stop the program with a message naming the file and line. See [decision 0003](decisions/0003-logging-config-and-command-line-libraries.md).

## Planned pipeline

None of these stages exists yet. Update this section as each one lands.

1. Feature detection and description per input image
2. Feature matching between overlapping image pairs
3. Robust affine estimation per pair (e.g. RANSAC)
4. Chaining the pairwise transforms into a common reference frame (using `compose`)
5. Warping each image into the output canvas
6. Blending the overlapping regions
