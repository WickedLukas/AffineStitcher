#include <catch2/catch_test_macros.hpp>

#include <affine_stitcher/stitcher.hpp>
#include <stdexcept>
#include <type_traits>
#include <utility>

using affine_stitcher::Stitcher;
using affine_stitcher::StitcherOptions;

static_assert(!std::is_copy_constructible_v<Stitcher>);
static_assert(!std::is_copy_assignable_v<Stitcher>);
static_assert(std::is_nothrow_move_constructible_v<Stitcher>);
static_assert(std::is_nothrow_move_assignable_v<Stitcher>);

TEST_CASE("Stitcher keeps a copy of its options") {
    StitcherOptions options;
    options.set_max_features(500);
    const Stitcher stitcher{options};

    options.set_max_features(9);
    CHECK(stitcher.options().max_features() == 500);
}

TEST_CASE("Stitcher uses default options when none are given") {
    const Stitcher stitcher;
    CHECK(stitcher.options().max_features() == StitcherOptions{}.max_features());
}

TEST_CASE("Stitcher can be moved") {
    StitcherOptions options;
    options.set_max_features(500);
    Stitcher original{options};

    Stitcher moved{std::move(original)};
    CHECK(moved.options().max_features() == 500);

    Stitcher assigned;
    assigned = std::move(moved);
    CHECK(assigned.options().max_features() == 500);
}

TEST_CASE("Stitcher::stitch is not implemented yet") {
    Stitcher stitcher;
    CHECK_THROWS_AS(stitcher.stitch({}), std::logic_error);
}
