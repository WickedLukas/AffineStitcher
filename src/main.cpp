#include "affine_stitcher/affine.hpp"

#include <opencv2/core/version.hpp>

#include <iostream>

int main() {
    const cv::Matx23d shift{1.0, 0.0, 10.0, 0.0, 1.0, 5.0};
    std::cout << "AffineStitcher (OpenCV " << CV_VERSION << ")\n"
              << "shift applied twice:\n"
              << affine_stitcher::compose(shift, shift) << '\n';
    return 0;
}
