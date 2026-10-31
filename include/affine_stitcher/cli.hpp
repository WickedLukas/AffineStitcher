#pragma once

#include "affine_stitcher/config.hpp"

#include <CLI/CLI.hpp>
#include <filesystem>
#include <optional>
#include <string>

namespace affine_stitcher {

/// Values given on the command line. Unset options leave the config file value in place.
struct CommandLineOptions {
    /// `-c, --config`: TOML config file. Empty means no file, so all defaults apply.
    std::filesystem::path config_file;
    /// `--log-level`: overrides LogConfig::level.
    std::optional<std::string> log_level;
    /// `--log-file`: overrides LogConfig::file.
    std::optional<std::filesystem::path> log_file;
};

/// Registers the command-line options on `app`, bound to the fields of `options`.
/// `options` must outlive `app`, because parsing writes into it.
void add_options(CLI::App& app, CommandLineOptions& options);

/// Builds the effective config. Defaults are overridden by the config file, and the config
/// file is overridden by command-line options.
/// @throws ConfigError if the config file can't be loaded
[[nodiscard]] Config resolve_config(const CommandLineOptions& options);

} // namespace affine_stitcher
