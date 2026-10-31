#pragma once

#include "affine_stitcher/export.hpp"

#include <memory>

namespace affine_stitcher {

/// Settings for a Stitcher.
///
/// The fields live behind a pointer (pimpl), so new settings can be added without changing
/// the size or layout of this class, and therefore without breaking the ABI. Every setting has
/// a default. A moved-from StitcherOptions may only be assigned to or destroyed.
class AFFINE_STITCHER_EXPORT StitcherOptions {
  public:
    /// Creates options with all defaults.
    StitcherOptions();
    ~StitcherOptions();
    /// Copies all settings; the copy is independent of `other`.
    StitcherOptions(const StitcherOptions& other);
    /// Replaces all settings with those of `other`.
    StitcherOptions& operator=(const StitcherOptions& other);
    /// Takes over the settings of `other`, which is left moved-from.
    StitcherOptions(StitcherOptions&& other) noexcept;
    /// Takes over the settings of `other`, which is left moved-from.
    StitcherOptions& operator=(StitcherOptions&& other) noexcept;

    /// Maximum number of features detected per image. Default: 2000.
    [[nodiscard]] int max_features() const;
    /// @throws std::invalid_argument if `value` is less than 1
    void set_max_features(int value);

    /// Maximum reprojection error in pixels for a match to count as an inlier when estimating
    /// the affine transform between two images (RANSAC). Default: 3.0.
    [[nodiscard]] double ransac_threshold() const;
    /// @throws std::invalid_argument if `value` isn't a finite number greater than 0
    void set_ransac_threshold(double value);

  private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace affine_stitcher
