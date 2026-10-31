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

include(GNUInstallDirs)

# All binaries in one place: build/<preset>/bin and build/<preset>/lib.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

add_subdirectory(lib)
add_subdirectory(app)

# Bundle the vcpkg shared libraries (OpenCV, spdlog, fmt, ...) with the install, so the install
# tree runs on a machine that doesn't have them, e.g. the Pi. System libraries (libc,
# libstdc++, ...) and our own library, which is installed as a target, are left out.
install(RUNTIME_DEPENDENCY_SET affine_stitcher_runtime_deps
    POST_INCLUDE_REGEXES "/vcpkg_installed/"
    POST_EXCLUDE_REGEXES ".*"
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# API reference from the /// comments in the public headers, plus the Markdown docs.
# Not part of ALL; build with `cmake --build --preset debug --target docs`.
find_package(Doxygen OPTIONAL_COMPONENTS dot)
if(DOXYGEN_FOUND)
    set(DOXYGEN_PROJECT_NAME AffineStitcher)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs)
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
    set(DOXYGEN_STRIP_FROM_INC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/include)
    set(DOXYGEN_MACRO_EXPANSION YES)
    set(DOXYGEN_EXPAND_ONLY_PREDEF YES)
    set(DOXYGEN_PREDEFINED AFFINE_STITCHER_EXPORT=)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
    set(DOXYGEN_WARN_AS_ERROR FAIL_ON_WARNINGS)
    doxygen_add_docs(docs lib/include README.md docs
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
    )
endif()

if(AFFINE_STITCHER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
