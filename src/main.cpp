#include "affine_stitcher/affine.hpp"
#include "affine_stitcher/cli.hpp"
#include "affine_stitcher/logging.hpp"

#include <opencv2/core/version.hpp>

#include <CLI/CLI.hpp>
#include <exception>
#include <iostream>
#include <spdlog/spdlog.h>

// main's signature is fixed by the standard, so argv can't become const char* const*.
int main(int argc, char** argv) { // NOLINT(misc-const-correctness)
    CLI::App app{"AffineStitcher: image stitching with affine transforms"};
    app.set_version_flag("--version", "AffineStitcher " AFFINE_STITCHER_VERSION);
    affine_stitcher::CommandLineOptions options;
    affine_stitcher::add_options(app, options);
    CLI11_PARSE(app, argc, argv);

    try {
        const affine_stitcher::Config config = affine_stitcher::resolve_config(options);
        affine_stitcher::setup_logging(config.log);
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }

    spdlog::info("AffineStitcher {} (OpenCV {})", AFFINE_STITCHER_VERSION, CV_VERSION);

    const cv::Matx23d shift{1.0, 0.0, 10.0, 0.0, 1.0, 5.0};
    std::cout << "shift applied twice:\n" << affine_stitcher::compose(shift, shift) << '\n';
    return 0;
}
