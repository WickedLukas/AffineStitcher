#include "affine_stitcher/affine.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using affine_stitcher::compose;
using affine_stitcher::to_homogeneous;
using Catch::Matchers::WithinAbs;

namespace {

void require_near(const cv::Matx23d& actual, const cv::Matx23d& expected) {
    for (int r = 0; r < 2; ++r) {
        for (int c = 0; c < 3; ++c) {
            REQUIRE_THAT(actual(r, c), WithinAbs(expected(r, c), 1e-12));
        }
    }
}

} // namespace

TEST_CASE("to_homogeneous appends the [0 0 1] row") {
    const cv::Matx23d a{1, 2, 3, 4, 5, 6};
    const cv::Matx33d h = to_homogeneous(a);
    CHECK(h(2, 0) == 0.0);
    CHECK(h(2, 1) == 0.0);
    CHECK(h(2, 2) == 1.0);
    CHECK(h(1, 2) == 6.0);
}

TEST_CASE("compose") {
    const cv::Matx23d identity{1, 0, 0, 0, 1, 0};
    const cv::Matx23d shift{1, 0, 10, 0, 1, 5};
    const cv::Matx23d scale{2, 0, 0, 0, 2, 0};

    SECTION("identity is neutral") {
        require_near(compose(identity, shift), shift);
        require_near(compose(shift, identity), shift);
    }
    SECTION("translations add up") {
        require_near(compose(shift, shift), cv::Matx23d{1, 0, 20, 0, 1, 10});
    }
    SECTION("order matters: scale after shift scales the translation") {
        require_near(compose(scale, shift), cv::Matx23d{2, 0, 20, 0, 2, 10});
        require_near(compose(shift, scale), cv::Matx23d{2, 0, 10, 0, 2, 5});
    }
}
