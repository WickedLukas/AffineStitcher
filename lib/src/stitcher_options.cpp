#include "affine_stitcher/stitcher_options.hpp"

#include <cmath>
#include <stdexcept>

namespace affine_stitcher {

struct StitcherOptions::Impl {
    int max_features = 2000;
    double ransac_threshold = 3.0;
};

StitcherOptions::StitcherOptions() : impl_(std::make_unique<Impl>()) {}
StitcherOptions::~StitcherOptions() = default;
StitcherOptions::StitcherOptions(const StitcherOptions& other)
    : impl_(std::make_unique<Impl>(*other.impl_)) {}
StitcherOptions& StitcherOptions::operator=(const StitcherOptions& other) {
    if (this != &other) {
        impl_ = std::make_unique<Impl>(*other.impl_);
    }
    return *this;
}
StitcherOptions::StitcherOptions(StitcherOptions&& other) noexcept = default;
StitcherOptions& StitcherOptions::operator=(StitcherOptions&& other) noexcept = default;

int StitcherOptions::max_features() const {
    return impl_->max_features;
}

void StitcherOptions::set_max_features(int value) {
    if (value < 1) {
        throw std::invalid_argument("max_features must be at least 1");
    }
    impl_->max_features = value;
}

double StitcherOptions::ransac_threshold() const {
    return impl_->ransac_threshold;
}

void StitcherOptions::set_ransac_threshold(double value) {
    if (!std::isfinite(value) || value <= 0.0) {
        throw std::invalid_argument("ransac_threshold must be a finite number greater than 0");
    }
    impl_->ransac_threshold = value;
}

} // namespace affine_stitcher
