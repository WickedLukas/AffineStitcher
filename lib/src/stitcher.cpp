#include "affine_stitcher/stitcher.hpp"

#include <spdlog/spdlog.h>
#include <stdexcept>

namespace affine_stitcher {

struct Stitcher::Impl {
    StitcherOptions options;
};

Stitcher::Stitcher(const StitcherOptions& options) : impl_(std::make_unique<Impl>(Impl{options})) {}
Stitcher::~Stitcher() = default;
Stitcher::Stitcher(Stitcher&& other) noexcept = default;
Stitcher& Stitcher::operator=(Stitcher&& other) noexcept = default;

const StitcherOptions& Stitcher::options() const {
    return impl_->options;
}

cv::Mat Stitcher::stitch(const std::vector<cv::Mat>& images) {
    spdlog::debug("stitch() called with {} images, max_features={}", images.size(),
                  impl_->options.max_features());
    throw std::logic_error("Stitcher::stitch() is not implemented yet");
}

} // namespace affine_stitcher
