#include "affine_stitcher/logging.hpp"

#include <memory>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <vector>

namespace affine_stitcher {

void setup_logging(const LogConfig& config) {
    if (!is_valid_log_level(config.level)) {
        throw std::invalid_argument("invalid log level '" + config.level + "'");
    }

    std::vector<spdlog::sink_ptr> sinks{std::make_shared<spdlog::sinks::stderr_color_sink_mt>()};
    if (!config.file.empty()) {
        sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            config.file.string(), config.max_file_size, config.max_files));
    }

    auto logger = std::make_shared<spdlog::logger>("affine_stitcher", sinks.begin(), sinks.end());
    logger->set_level(spdlog::level::from_str(config.level));
    logger->flush_on(spdlog::level::warn);
    spdlog::set_default_logger(std::move(logger));
}

} // namespace affine_stitcher
