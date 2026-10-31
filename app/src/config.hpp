#pragma once

#include <affine_stitcher/stitcher_options.hpp>

#include <cstddef>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>

namespace affine_stitcher::app {

/// Logging settings, read from the `[log]` table of the config file.
struct LogConfig {
    /// One of `trace`, `debug`, `info`, `warn`, `error`, `critical`, `off`.
    std::string level = "info";
    /// Log file path. Empty means log to the console only.
    std::filesystem::path file;
    /// Size in bytes at which the log file is rotated.
    std::size_t max_file_size = std::size_t{5} * 1024 * 1024;
    /// Number of rotated files kept besides the current one.
    std::size_t max_files = 3;
};

/// All application settings. Every field has a default, so an empty config file is valid.
struct Config {
    /// Settings from the `[log]` table.
    LogConfig log;
    /// Settings from the `[stitcher]` table, passed on to the library.
    StitcherOptions stitcher;
};

/// Thrown when a config file can't be read, isn't valid TOML, or contains unknown keys or
/// invalid values. The message names the source and, where known, the line.
class ConfigError : public std::runtime_error {
  public:
    using std::runtime_error::runtime_error;
};

/// Returns true if `level` is one of the log level names accepted in LogConfig::level.
[[nodiscard]] bool is_valid_log_level(std::string_view level);

/// Parses a config from TOML text.
/// @param toml_text the TOML document
/// @param source_name name used in error messages, usually the file path
/// @throws ConfigError if the text is invalid; unknown keys are errors, so typos don't go unnoticed
[[nodiscard]] Config parse_config(std::string_view toml_text,
                                  std::string_view source_name = "config");

/// Reads and parses a TOML config file.
/// @throws ConfigError if the file can't be read or its contents are invalid
[[nodiscard]] Config load_config(const std::filesystem::path& path);

} // namespace affine_stitcher::app
