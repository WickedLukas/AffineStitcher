cmake_minimum_required(VERSION 3.25)

# Link vcpkg dependencies (OpenCV etc.) as shared libraries. Must be set before
# project(), where the vcpkg toolchain runs the manifest install.
if(NOT DEFINED VCPKG_TARGET_TRIPLET)
    cmake_host_system_information(RESULT _host_arch QUERY OS_PLATFORM)
    if(_host_arch MATCHES "^(aarch64|arm64)$")
        set(VCPKG_TARGET_TRIPLET arm64-linux-dynamic CACHE STRING "vcpkg target triplet")
    else()
        set(VCPKG_TARGET_TRIPLET x64-linux-dynamic CACHE STRING "vcpkg target triplet")
    endif()
endif()

project(AffineStitcher VERSION 0.1.0 LANGUAGES CXX)

# GCC 12 (Raspberry Pi OS Bookworm) fully supports C++20.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(AFFINE_STITCHER_BUILD_TESTS "Build unit tests" ON)
option(AFFINE_STITCHER_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs features2d calib3d stitching)
find_package(Eigen3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
find_package(tomlplusplus CONFIG REQUIRED)

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
    src/cli.cpp
    src/config.cpp
    src/logging.cpp
)
target_include_directories(affine_stitcher_core PUBLIC include)
target_link_libraries(affine_stitcher_core
    PUBLIC ${OpenCV_LIBS} Eigen3::Eigen spdlog::spdlog CLI11::CLI11
    PRIVATE tomlplusplus::tomlplusplus project_warnings
)

add_executable(affine_stitcher src/main.cpp)
target_link_libraries(affine_stitcher PRIVATE affine_stitcher_core project_warnings)
target_compile_definitions(affine_stitcher PRIVATE AFFINE_STITCHER_VERSION="${PROJECT_VERSION}")

# API reference from the /// comments in the public headers, plus the Markdown docs.
# Not part of ALL; build with `cmake --build --preset debug --target docs`.
find_package(Doxygen OPTIONAL_COMPONENTS dot)
if(DOXYGEN_FOUND)
    set(DOXYGEN_PROJECT_NAME AffineStitcher)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
    set(DOXYGEN_STRIP_FROM_INC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
    set(DOXYGEN_WARN_AS_ERROR FAIL_ON_WARNINGS)
    doxygen_add_docs(docs include README.md docs
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
    )
endif()

if(AFFINE_STITCHER_BUILD_TESTS)
    enable_testing()
    find_package(Catch2 3 CONFIG REQUIRED)
    include(Catch)

    add_executable(affine_stitcher_tests
        tests/test_affine.cpp
        tests/test_cli.cpp
        tests/test_config.cpp
        tests/test_logging.cpp
    )
    target_link_libraries(affine_stitcher_tests
        PRIVATE affine_stitcher_core Catch2::Catch2WithMain project_warnings
    )
    target_compile_definitions(affine_stitcher_tests
        PRIVATE AFFINE_STITCHER_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    )
    catch_discover_tests(affine_stitcher_tests)
endif()
