#include "affine_stitcher/logging.hpp"
#include "temp_dir.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <spdlog/spdlog.h>
#include <stdexcept>

using affine_stitcher::LogConfig;
using affine_stitcher::setup_logging;
using Catch::Matchers::ContainsSubstring;

TEST_CASE("setup_logging writes messages at or above the level to the log file") {
    const TempDir dir;
    LogConfig config;
    config.level = "info";
    config.file = dir.path() / "nested/dir/app.log";

    setup_logging(config);
    spdlog::debug("hidden debug message");
    spdlog::info("visible info message {}", 42);
    spdlog::default_logger()->flush();

    const std::string content = read_file(config.file);
    CHECK_THAT(content, ContainsSubstring("visible info message 42"));
    CHECK_THAT(content, !ContainsSubstring("hidden debug message"));

    // Close the file before the temp directory is removed.
    setup_logging(LogConfig{});
}

TEST_CASE("setup_logging rejects an invalid level") {
    LogConfig config;
    config.level = "verbose";
    REQUIRE_THROWS_AS(setup_logging(config), std::invalid_argument);
}
