# 3. spdlog, toml++ and CLI11 for logging, config and command line

- Status: accepted
- Date: 2026-10-31

## Context

The application needs log files, a config file for settings, and command-line arguments. On the Pi it will run for long periods, so the log files must not grow without limit.

## Decision

| Need | Library | Linked |
|---|---|---|
| Logging | spdlog (with fmt) | shared (`libspdlog`, `libfmt`) |
| Config files | toml++, TOML format | header-only, private to `affine_stitcher_core` |
| Command line | CLI11 | header-only, public (it appears in `cli.hpp`) |

- **spdlog** writes to stderr and, when configured, to a rotating file sink. It flushes the file on `warn` and above. GCC 12 has no `std::format`, so fmt also gives the code modern formatting.
- **TOML** supports comments and nested tables and is safe to edit by hand. Unknown keys are errors, so a typo in a setting fails loudly instead of being ignored.
- **CLI11** parses the options and checks their values. Its own config-file support was not used, because it only covers flat values that are also command-line options, and stitching settings will need nested data.
- Settings are resolved in a fixed order: built-in defaults, then the config file, then command-line options. `resolve_config` implements this and is tested.

Alternatives not chosen: yaml-cpp (YAML pitfalls, heavier), nlohmann-json (no comments in config files), Boost.Program_options and Boost.Log (they pull in a large part of Boost).

## Consequences

- Every new setting needs a field in `Config`, parsing and validation in `parse_config`, an entry in `config/affine_stitcher.example.toml` and a test. A test loads the example file, so it can't drift into invalid syntax.
- A relative `log.file` in a config file is resolved against the config file's directory. A relative `--log-file` is resolved against the working directory.
- Code that logs just calls `spdlog::info(...)` etc. `setup_logging` installs the default logger once at startup.
