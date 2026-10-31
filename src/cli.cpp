#include "affine_stitcher/cli.hpp"

#include <vector>

namespace affine_stitcher {

void add_options(CLI::App& app, CommandLineOptions& options) {
    app.add_option("-c,--config", options.config_file, "TOML config file")
        ->check(CLI::ExistingFile);
    app.add_option("--log-level", options.log_level, "Log level (overrides the config file)")
        ->check(CLI::IsMember(std::vector<std::string>{"trace", "debug", "info", "warn", "error",
                                                       "critical", "off"}));
    app.add_option("--log-file", options.log_file, "Rotating log file (overrides the config file)");
}

Config resolve_config(const CommandLineOptions& options) {
    Config config = options.config_file.empty() ? Config{} : load_config(options.config_file);
    if (options.log_level) {
        config.log.level = *options.log_level;
    }
    if (options.log_file) {
        config.log.file = *options.log_file;
    }
    return config;
}

} // namespace affine_stitcher
