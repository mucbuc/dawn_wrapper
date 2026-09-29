cmake_minimum_required(VERSION 3.27)

project(Pipeline)

# Browser only for now: the document reaches C++ through JS (pre.js, from_js.hpp).
# A native build needs another front end to fill the IR; see PIPELINE_DSL.md.
if (NOT EMSCRIPTEN)
    message(FATAL_ERROR "pipeline/ builds with emcmake only; see run")
endif()

add_compile_definitions(TARGET_HEADLESS)

add_subdirectory(lib/dawn_wrapper)
add_subdirectory(lib/asserter)

# Pipeline runs any document; Hatch checks the escape hatch against multiply.json;
# Particles is the particle target written by hand on dawn_wrapper, no loader.
foreach(page Pipeline Hatch Particles)
    string(TOLOWER ${page} source)
    if (page STREQUAL "Pipeline")
        set(source main)
    elseif (page STREQUAL "Particles")
        set(source particles_by_hand)
    endif()
    add_executable(${page} src/${source}.cpp)
    set_target_properties(${page} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        COMPILE_WARNING_AS_ERROR ON
        SUFFIX ".html"
    )
    target_link_options(${page} PRIVATE
        --use-port=emdawnwebgpu
        --emrun
        -lembind
        --pre-js ${CMAKE_CURRENT_LIST_DIR}/src/pre.js
        "-sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE=$addRunDependency,$removeRunDependency"
    )
    set_property(TARGET ${page} APPEND PROPERTY LINK_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/src/pre.js)
    target_link_libraries(${page} PRIVATE dawn_wrapper asserter)
    target_include_directories(${page} SYSTEM PUBLIC ${CMAKE_CURRENT_LIST_DIR}/lib)
endforeach()
