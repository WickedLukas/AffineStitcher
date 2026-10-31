#pragma once

#include <opencv2/core.hpp>

/// Image stitching with affine transforms. Conventions are in docs/math.md.
namespace affine_stitcher {

/// Extends a 2x3 affine transform to its 3x3 homogeneous form.
[[nodiscard]] cv::Matx33d to_homogeneous(const cv::Matx23d& affine);

/// Returns the affine transform equivalent to applying `first`, then `second`.
[[nodiscard]] cv::Matx23d compose(const cv::Matx23d& second, const cv::Matx23d& first);

} // namespace affine_stitcher
