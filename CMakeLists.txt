cmake_minimum_required(VERSION 3.25)
project(AffineStitcher VERSION 0.1.0 LANGUAGES CXX)

# GCC 12 (Raspberry Pi OS Bookworm) fully supports C++20.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(AFFINE_STITCHER_BUILD_TESTS "Build unit tests" ON)
option(AFFINE_STITCHER_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs features2d calib3d stitching)
find_package(Eigen3 CONFIG REQUIRED)

add_library(project_warnings INTERFACE)
target_compile_options(project_warnings INTERFACE
    -Wall -Wextra -Wpedantic
    -Wshadow -Wconversion -Wsign-conversion -Wdouble-promotion
    -Wnon-virtual-dtor -Woverloaded-virtual -Wold-style-cast
    -Wnull-dereference -Wformat=2 -Wimplicit-fallthrough
    $<$<BOOL:${AFFINE_STITCHER_WARNINGS_AS_ERRORS}>:-Werror>
)

add_library(affine_stitcher_core
    src/affine.cpp
)
target_include_directories(affine_stitcher_core PUBLIC include)
target_link_libraries(affine_stitcher_core
    PUBLIC ${OpenCV_LIBS} Eigen3::Eigen
    PRIVATE project_warnings
)

add_executable(affine_stitcher src/main.cpp)
target_link_libraries(affine_stitcher PRIVATE affine_stitcher_core project_warnings)

if(AFFINE_STITCHER_BUILD_TESTS)
    enable_testing()
    find_package(Catch2 3 CONFIG REQUIRED)
    include(Catch)

    add_executable(affine_stitcher_tests tests/test_affine.cpp)
    target_link_libraries(affine_stitcher_tests
        PRIVATE affine_stitcher_core Catch2::Catch2WithMain project_warnings
    )
    catch_discover_tests(affine_stitcher_tests)
endif()
