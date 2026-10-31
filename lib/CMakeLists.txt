# libaffine_stitcher.so: the public API is the pimpl classes in include/, everything else is
# hidden. See docs/decisions/0004-library-app-tests-layout.md.

include(GenerateExportHeader)
include(CMakePackageConfigHelpers)

# Compiled once, then linked into the shared library and directly into the internal tests,
# which need symbols the shared library doesn't export.
add_library(affine_stitcher_objects OBJECT
    src/affine.cpp
    src/stitcher.cpp
    src/stitcher_options.cpp
)
set_target_properties(affine_stitcher_objects PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)
target_include_directories(affine_stitcher_objects PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/generated
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(affine_stitcher_objects
    PUBLIC ${OpenCV_LIBS} spdlog::spdlog
    PRIVATE Eigen3::Eigen project_warnings
)

add_library(affine_stitcher SHARED $<TARGET_OBJECTS:affine_stitcher_objects>)
add_library(AffineStitcher::affine_stitcher ALIAS affine_stitcher)
generate_export_header(affine_stitcher
    BASE_NAME AFFINE_STITCHER
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/generated/affine_stitcher/export.hpp
)
# While the version is 0.x every minor release may break the ABI, so it's part of the SONAME:
# libaffine_stitcher.so.0.1
set_target_properties(affine_stitcher PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    INSTALL_RPATH "$ORIGIN"
)
target_include_directories(affine_stitcher PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(affine_stitcher PUBLIC cxx_std_17)
target_link_libraries(affine_stitcher
    PUBLIC ${OpenCV_LIBS}
    PRIVATE spdlog::spdlog
)

install(TARGETS affine_stitcher
    EXPORT AffineStitcherTargets
    RUNTIME_DEPENDENCY_SET affine_stitcher_runtime_deps
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/ ${CMAKE_CURRENT_BINARY_DIR}/generated/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# find_package(AffineStitcher) support for other projects.
set(config_dir ${CMAKE_INSTALL_LIBDIR}/cmake/AffineStitcher)
install(EXPORT AffineStitcherTargets
    NAMESPACE AffineStitcher::
    DESTINATION ${config_dir}
)
configure_package_config_file(cmake/AffineStitcherConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/AffineStitcherConfig.cmake
    INSTALL_DESTINATION ${config_dir}
)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/AffineStitcherConfigVersion.cmake
    COMPATIBILITY SameMinorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/AffineStitcherConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/AffineStitcherConfigVersion.cmake
    DESTINATION ${config_dir}
)
