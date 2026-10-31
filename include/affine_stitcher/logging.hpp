#pragma once

#include "affine_stitcher/config.hpp"

namespace affine_stitcher {

/// Installs the default spdlog logger, which the `spdlog::info(...)` etc. functions write to.
/// It logs to stderr and, if LogConfig::file is set, also to a rotating log file. Warnings
/// and errors are flushed to the file immediately.
/// @throws std::invalid_argument if LogConfig::level isn't a valid level name
/// @throws spdlog::spdlog_ex if the log file can't be opened
void setup_logging(const LogConfig& config);

} // namespace affine_stitcher
