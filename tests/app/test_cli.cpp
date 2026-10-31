#include "cli.hpp"
#include "temp_dir.hpp"

#include <catch2/catch_test_macros.hpp>

using affine_stitcher::app::add_options;
using affine_stitcher::app::CommandLineOptions;
using affine_stitcher::app::resolve_config;

namespace {

/// Parses `args` (without the program name) and returns the options.
CommandLineOptions parse(const std::string& args) {
    CommandLineOptions options;
    CLI::App app;
    add_options(app, options);
    app.parse(args, false);
    return options;
}

} // namespace

TEST_CASE("resolve_config without options uses the defaults") {
    const auto config = resolve_config(parse(""));
    CHECK(config.log.level == "info");
    CHECK(config.log.file.empty());
}

TEST_CASE("command-line options override the config file") {
    const TempDir dir;
    const auto file =
        dir.write("app.toml", "[log]\nlevel = \"warn\"\nfile = \"/tmp/from-config.log\"\n");

    SECTION("config file alone") {
        const auto config = resolve_config(parse("--config " + file.string()));
        CHECK(config.log.level == "warn");
        CHECK(config.log.file == "/tmp/from-config.log");
    }
    SECTION("options win over the file") {
        const auto config = resolve_config(
            parse("-c " + file.string() + " --log-level debug --log-file /tmp/from-cli.log"));
        CHECK(config.log.level == "debug");
        CHECK(config.log.file == "/tmp/from-cli.log");
    }
}

TEST_CASE("invalid command-line values are rejected while parsing") {
    REQUIRE_THROWS_AS(parse("--log-level verbose"), CLI::ValidationError);
    REQUIRE_THROWS_AS(parse("--config /nonexistent/app.toml"), CLI::ValidationError);
}
