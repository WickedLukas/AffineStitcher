# Run as: cmake -DLIBRARY=<path to .so> -DNM=<nm> -P check_exports.cmake
# Fails if internal symbols are exported or public API symbols are missing.

execute_process(
    COMMAND ${NM} --dynamic --defined-only --demangle ${LIBRARY}
    OUTPUT_VARIABLE symbols
    RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "${NM} failed on ${LIBRARY}")
endif()

string(REGEX MATCHALL "[^\n]*affine_stitcher::detail::[^\n]*" leaked "${symbols}")
if(leaked)
    list(JOIN leaked "\n" leaked)
    message(FATAL_ERROR "Internal symbols are exported:\n${leaked}")
endif()

foreach(required
    "affine_stitcher::Stitcher::Stitcher("
    "affine_stitcher::Stitcher::stitch("
    "affine_stitcher::StitcherOptions::StitcherOptions("
    "affine_stitcher::StitcherOptions::set_max_features("
)
    string(FIND "${symbols}" "${required}" position)
    if(position EQUAL -1)
        message(FATAL_ERROR "Public symbol is not exported: ${required}")
    endif()
endforeach()
