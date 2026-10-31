#pragma once

#include "affine_stitcher/export.hpp"
#include "affine_stitcher/stitcher_options.hpp"

#include <opencv2/core/mat.hpp>

#include <memory>
#include <vector>

/// Image stitching with affine transforms. Conventions are in docs/math.md.
namespace affine_stitcher {

/// Stitches overlapping images into one panorama, assuming the images are related by affine
/// transforms.
///
/// The state lives behind a pointer (pimpl), so it can change without breaking the ABI.
/// Stitcher is movable but not copyable. A moved-from Stitcher may only be assigned to or
/// destroyed.
class AFFINE_STITCHER_EXPORT Stitcher {
  public:
    /// Creates a stitcher with the given settings, which are copied.
    explicit Stitcher(const StitcherOptions& options = {});
    ~Stitcher();
    Stitcher(const Stitcher&) = delete;
    Stitcher& operator=(const Stitcher&) = delete;
    /// Takes over the state of `other`, which is left moved-from.
    Stitcher(Stitcher&& other) noexcept;
    /// Takes over the state of `other`, which is left moved-from.
    Stitcher& operator=(Stitcher&& other) noexcept;

    /// The settings this stitcher was created with.
    [[nodiscard]] const StitcherOptions& options() const;

    /// Stitches `images` into one panorama.
    /// @param images overlapping input images, all of the same type
    /// @return the panorama
    /// @throws std::logic_error always for now: the pipeline isn't implemented yet
    [[nodiscard]] cv::Mat stitch(const std::vector<cv::Mat>& images);

  private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace affine_stitcher
