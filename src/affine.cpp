#include "affine_stitcher/affine.hpp"

namespace affine_stitcher {

cv::Matx33d to_homogeneous(const cv::Matx23d& affine) {
    return {affine(0, 0), affine(0, 1), affine(0, 2),
            affine(1, 0), affine(1, 1), affine(1, 2),
            0.0,          0.0,          1.0};
}

cv::Matx23d compose(const cv::Matx23d& second, const cv::Matx23d& first) {
    return (to_homogeneous(second) * to_homogeneous(first)).get_minor<2, 3>(0, 0);
}

} // namespace affine_stitcher
