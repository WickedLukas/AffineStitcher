#include "affine_stitcher/config.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <toml++/toml.hpp>

namespace affine_stitcher {

namespace {

[[noreturn]] void fail(std::string_view source, const toml::source_region& where,
                       std::string_view what) {
    std::string message{source};
    if (where.begin.line != 0) {
        message += ':';
        message += std::to_string(where.begin.line);
    }
    message += ": ";
    message += what;
    throw ConfigError(message);
}

std::string read_string(const toml::node& node, std::string_view key, std::string_view source) {
    if (const auto* value = node.as_string()) {
        return value->get();
    }
    fail(source, node.source(), "'" + std::string{key} + "' must be a string");
}

std::size_t read_positive(const toml::node& node, std::string_view key, std::string_view source) {
    const auto* value = node.as_integer();
    if (value == nullptr || value->get() < 1) {
        fail(source, node.source(), "'" + std::string{key} + "' must be a positive integer");
    }
    return static_cast<std::size_t>(value->get());
}

void read_log(const toml::table& table, LogConfig& log, std::string_view source) {
    for (const auto& [key, node] : table) {
        const std::string_view name = key.str();
        if (name == "level") {
            log.level = read_string(node, "log.level", source);
            if (!is_valid_log_level(log.level)) {
                fail(source, node.source(),
                     "'log.level' must be one of trace, debug, info, warn, error, critical, off");
            }
        } else if (name == "file") {
            log.file = read_string(node, "log.file", source);
        } else if (name == "max_file_size") {
            log.max_file_size = read_positive(node, "log.max_file_size", source);
        } else if (name == "max_files") {
            log.max_files = read_positive(node, "log.max_files", source);
        } else {
            fail(source, key.source(), "unknown key 'log." + std::string{name} + "'");
        }
    }
}

} // namespace

bool is_valid_log_level(std::string_view level) {
    constexpr std::array<std::string_view, 7> levels{"trace", "debug",    "info", "warn",
                                                     "error", "critical", "off"};
    return std::ranges::find(levels, level) != levels.end();
}

Config parse_config(std::string_view toml_text, std::string_view source_name) {
    toml::table root;
    try {
        root = toml::parse(toml_text, source_name);
    } catch (const toml::parse_error& error) {
        fail(source_name, error.source(), error.description());
    }

    Config config;
    for (const auto& [key, node] : root) {
        if (key.str() == "log") {
            const auto* table = node.as_table();
            if (table == nullptr) {
                fail(source_name, node.source(), "'log' must be a table");
            }
            read_log(*table, config.log, source_name);
        } else {
            fail(source_name, key.source(), "unknown key '" + std::string{key.str()} + "'");
        }
    }
    return config;
}

Config load_config(const std::filesystem::path& path) {
    const std::ifstream file{path};
    if (!file) {
        throw ConfigError(path.string() + ": cannot open config file");
    }
    std::ostringstream text;
    text << file.rdbuf();

    Config config = parse_config(text.str(), path.string());
    // A relative log path is relative to the config file, not to the working directory.
    if (!config.log.file.empty() && config.log.file.is_relative()) {
        config.log.file = path.parent_path() / config.log.file;
    }
    return config;
}

} // namespace affine_stitcher
