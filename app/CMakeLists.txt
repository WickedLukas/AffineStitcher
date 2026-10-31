# The affine_stitcher executable. Everything except main() is in a static library, so the
# app tests can link it.

add_library(affine_stitcher_app_core STATIC
    src/cli.cpp
    src/config.cpp
    src/logging.cpp
)
target_include_directories(affine_stitcher_app_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(affine_stitcher_app_core
    PUBLIC AffineStitcher::affine_stitcher spdlog::spdlog CLI11::CLI11
    PRIVATE tomlplusplus::tomlplusplus project_warnings
)

add_executable(affine_stitcher_app src/main.cpp)
set_target_properties(affine_stitcher_app PROPERTIES
    OUTPUT_NAME affine_stitcher
    INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}"
)
target_link_libraries(affine_stitcher_app PRIVATE affine_stitcher_app_core project_warnings)
target_compile_definitions(affine_stitcher_app PRIVATE
    AFFINE_STITCHER_VERSION="${PROJECT_VERSION}"
)

install(TARGETS affine_stitcher_app
    RUNTIME_DEPENDENCY_SET affine_stitcher_runtime_deps
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(FILES ${PROJECT_SOURCE_DIR}/config/affine_stitcher.example.toml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/affine_stitcher
)
