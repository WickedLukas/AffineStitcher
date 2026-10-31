#include <catch2/catch_test_macros.hpp>
#include <opencv2/imgcodecs.hpp>

#include <cstdint>
#include <filesystem>
#include <string>

// Checks the rules in tests/data/README.md for every image there, so a broken or oversized
// file fails here and not in some unrelated test. Passes trivially while there are no images.
TEST_CASE("test data images are readable and small") {
    constexpr std::uintmax_t max_file_size = std::uintmax_t{500} * 1024;

    for (const auto& entry :
         std::filesystem::recursive_directory_iterator{AFFINE_STITCHER_TEST_DATA_DIR}) {
        const std::string extension = entry.path().extension().string();
        if (!entry.is_regular_file() ||
            (extension != ".png" && extension != ".jpg" && extension != ".jpeg")) {
            continue;
        }
        INFO(entry.path().string());
        CHECK(entry.file_size() <= max_file_size);
        CHECK(!cv::imread(entry.path().string()).empty());
    }
}
