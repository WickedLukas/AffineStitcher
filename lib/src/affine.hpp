#pragma once

#include <opencv2/core.hpp>

// Internal: not part of the public API and not exported from the shared library.
// Conventions are in docs/math.md.
namespace affine_stitcher::detail {

/// Extends a 2x3 affine transform to its 3x3 homogeneous form.
[[nodiscard]] cv::Matx33d to_homogeneous(const cv::Matx23d& affine);

/// Returns the affine transform equivalent to applying `first`, then `second`.
[[nodiscard]] cv::Matx23d compose(const cv::Matx23d& second, const cv::Matx23d& first);

} // namespace affine_stitcher::detail
