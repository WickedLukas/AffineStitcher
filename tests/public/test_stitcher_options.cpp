#include <catch2/catch_test_macros.hpp>

#include <affine_stitcher/stitcher_options.hpp>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>

using affine_stitcher::StitcherOptions;

static_assert(std::is_copy_constructible_v<StitcherOptions>);
static_assert(std::is_nothrow_move_constructible_v<StitcherOptions>);
static_assert(std::is_nothrow_move_assignable_v<StitcherOptions>);

TEST_CASE("StitcherOptions defaults") {
    const StitcherOptions options;
    CHECK(options.max_features() == 2000);
    CHECK(options.ransac_threshold() == 3.0);
}

TEST_CASE("StitcherOptions setters") {
    StitcherOptions options;
    options.set_max_features(1);
    options.set_ransac_threshold(0.5);
    CHECK(options.max_features() == 1);
    CHECK(options.ransac_threshold() == 0.5);
}

TEST_CASE("StitcherOptions rejects invalid values and keeps the old one") {
    StitcherOptions options;
    CHECK_THROWS_AS(options.set_max_features(0), std::invalid_argument);
    CHECK_THROWS_AS(options.set_max_features(-5), std::invalid_argument);
    CHECK(options.max_features() == 2000);

    CHECK_THROWS_AS(options.set_ransac_threshold(0.0), std::invalid_argument);
    CHECK_THROWS_AS(options.set_ransac_threshold(-1.0), std::invalid_argument);
    CHECK_THROWS_AS(options.set_ransac_threshold(std::numeric_limits<double>::quiet_NaN()),
                    std::invalid_argument);
    CHECK_THROWS_AS(options.set_ransac_threshold(std::numeric_limits<double>::infinity()),
                    std::invalid_argument);
    CHECK(options.ransac_threshold() == 3.0);
}

TEST_CASE("StitcherOptions copies are independent") {
    StitcherOptions original;
    original.set_max_features(100);

    StitcherOptions copy{original};
    copy.set_max_features(200);
    CHECK(original.max_features() == 100);

    StitcherOptions assigned;
    assigned = original;
    assigned.set_max_features(300);
    CHECK(original.max_features() == 100);
    CHECK(assigned.max_features() == 300);
}

TEST_CASE("StitcherOptions can be moved and a moved-from object reassigned") {
    StitcherOptions original;
    original.set_max_features(100);

    StitcherOptions moved{std::move(original)};
    CHECK(moved.max_features() == 100);

    original = StitcherOptions{}; // NOLINT(bugprone-use-after-move): assigning is allowed
    CHECK(original.max_features() == 2000);
}
