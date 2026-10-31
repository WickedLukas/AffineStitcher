#include "affine_stitcher/config.hpp"
#include "temp_dir.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

using affine_stitcher::ConfigError;
using affine_stitcher::load_config;
using affine_stitcher::parse_config;
using Catch::Matchers::ContainsSubstring;

TEST_CASE("parse_config uses defaults for an empty document") {
    const auto config = parse_config("");
    CHECK(config.log.level == "info");
    CHECK(config.log.file.empty());
    CHECK(config.log.max_file_size == 5U * 1024U * 1024U);
    CHECK(config.log.max_files == 3);
}

TEST_CASE("parse_config reads the [log] table") {
    const auto config = parse_config(R"(
        [log]
        level = "debug"
        file = "/var/log/stitcher.log"
        max_file_size = 1024
        max_files = 7
    )");
    CHECK(config.log.level == "debug");
    CHECK(config.log.file == "/var/log/stitcher.log");
    CHECK(config.log.max_file_size == 1024);
    CHECK(config.log.max_files == 7);
}

TEST_CASE("parse_config rejects invalid documents with source and line") {
    SECTION("TOML syntax error") {
        REQUIRE_THROWS_WITH(parse_config("[log\n", "my.toml"), ContainsSubstring("my.toml:1:"));
    }
    SECTION("unknown top-level key") {
        REQUIRE_THROWS_WITH(parse_config("\nlogg = 1\n", "my.toml"),
                            ContainsSubstring("my.toml:2:") &&
                                ContainsSubstring("unknown key 'logg'"));
    }
    SECTION("unknown key in [log]") {
        REQUIRE_THROWS_WITH(parse_config("[log]\nlevle = \"info\"\n"),
                            ContainsSubstring("unknown key 'log.levle'"));
    }
    SECTION("log is not a table") {
        REQUIRE_THROWS_WITH(parse_config("log = 1\n"), ContainsSubstring("'log' must be a table"));
    }
    SECTION("invalid level name") {
        REQUIRE_THROWS_WITH(parse_config("[log]\nlevel = \"verbose\"\n"),
                            ContainsSubstring("'log.level' must be one of"));
    }
    SECTION("wrong value type") {
        REQUIRE_THROWS_WITH(parse_config("[log]\nlevel = 3\n"),
                            ContainsSubstring("'log.level' must be a string"));
    }
    SECTION("non-positive count") {
        REQUIRE_THROWS_WITH(parse_config("[log]\nmax_files = 0\n"),
                            ContainsSubstring("'log.max_files' must be a positive integer"));
        REQUIRE_THROWS_WITH(parse_config("[log]\nmax_file_size = -1\n"),
                            ContainsSubstring("'log.max_file_size' must be a positive integer"));
    }
    SECTION("errors are ConfigError") {
        REQUIRE_THROWS_AS(parse_config("x = 1\n"), ConfigError);
    }
}

TEST_CASE("load_config") {
    const TempDir dir;

    SECTION("missing file") {
        REQUIRE_THROWS_AS(load_config(dir.path() / "missing.toml"), ConfigError);
    }
    SECTION("errors name the file") {
        const auto file = dir.write("bad.toml", "nope = 1\n");
        REQUIRE_THROWS_WITH(load_config(file), ContainsSubstring(file.string() + ":1:"));
    }
    SECTION("a relative log file is resolved against the config file's directory") {
        const auto file = dir.write("app.toml", "[log]\nfile = \"logs/app.log\"\n");
        CHECK(load_config(file).log.file == dir.path() / "logs/app.log");
    }
    SECTION("an absolute log file is kept") {
        const auto file = dir.write("app.toml", "[log]\nfile = \"/tmp/app.log\"\n");
        CHECK(load_config(file).log.file == "/tmp/app.log");
    }
}

TEST_CASE("the example config in the repository is valid") {
    const auto config =
        load_config(AFFINE_STITCHER_SOURCE_DIR "/config/affine_stitcher.example.toml");
    CHECK(affine_stitcher::is_valid_log_level(config.log.level));
}
