#include "cli.hpp"
#include "logging.hpp"

#include <opencv2/core/version.hpp>

#include <CLI/CLI.hpp>
#include <affine_stitcher/stitcher.hpp>
#include <exception>
#include <iostream>
#include <spdlog/spdlog.h>

// main's signature is fixed by the standard, so argv can't become const char* const*.
int main(int argc, char** argv) { // NOLINT(misc-const-correctness)
    CLI::App app{"AffineStitcher: image stitching with affine transforms"};
    app.set_version_flag("--version", "AffineStitcher " AFFINE_STITCHER_VERSION);
    affine_stitcher::app::CommandLineOptions options;
    affine_stitcher::app::add_options(app, options);
    CLI11_PARSE(app, argc, argv);

    try {
        const affine_stitcher::app::Config config = affine_stitcher::app::resolve_config(options);
        affine_stitcher::app::setup_logging(config.log);

        spdlog::info("AffineStitcher {} (OpenCV {})", AFFINE_STITCHER_VERSION, CV_VERSION);
        const affine_stitcher::Stitcher stitcher{config.stitcher};
        spdlog::debug("stitcher options: max_features={}, ransac_threshold={}",
                      stitcher.options().max_features(), stitcher.options().ransac_threshold());
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
