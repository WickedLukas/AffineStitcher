# Three test executables, one per level. Test names get a prefix, so `ctest -R "^public\."`
# runs one group. See docs/decisions/0004-library-app-tests-layout.md.

find_package(Catch2 3 CONFIG REQUIRED)
include(Catch)

# Public API, linked against the real shared library like any other user of it.
add_executable(affine_stitcher_public_tests
    public/test_data.cpp
    public/test_stitcher.cpp
    public/test_stitcher_options.cpp
)
target_link_libraries(affine_stitcher_public_tests
    PRIVATE AffineStitcher::affine_stitcher Catch2::Catch2WithMain project_warnings
)
target_compile_definitions(affine_stitcher_public_tests
    PRIVATE AFFINE_STITCHER_TEST_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
)
catch_discover_tests(affine_stitcher_public_tests TEST_PREFIX "public.")

# Library internals, linked against the object files instead of the shared library, because
# the shared library doesn't export them.
add_executable(affine_stitcher_internal_tests
    internal/test_affine.cpp
)
target_link_libraries(affine_stitcher_internal_tests
    PRIVATE affine_stitcher_objects Catch2::Catch2WithMain project_warnings
)
catch_discover_tests(affine_stitcher_internal_tests TEST_PREFIX "internal.")

# Application code: command line, config file, logging.
add_executable(affine_stitcher_app_tests
    app/test_cli.cpp
    app/test_config.cpp
    app/test_logging.cpp
)
target_include_directories(affine_stitcher_app_tests PRIVATE support)
target_link_libraries(affine_stitcher_app_tests
    PRIVATE affine_stitcher_app_core Catch2::Catch2WithMain project_warnings
)
target_compile_definitions(affine_stitcher_app_tests
    PRIVATE AFFINE_STITCHER_SOURCE_DIR="${PROJECT_SOURCE_DIR}"
)
catch_discover_tests(affine_stitcher_app_tests TEST_PREFIX "app.")

# Checks the shared library's exported symbols: public API in, internals out.
add_test(NAME abi.exported_symbols
    COMMAND ${CMAKE_COMMAND}
        -DLIBRARY=$<TARGET_FILE:affine_stitcher>
        -DNM=${CMAKE_NM}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/abi/check_exports.cmake
)
