project(dawn_wrapper_lib)

set(CMAKE_CXX_STANDARD 20)
add_library(dawn_wrapper STATIC src/dawn_wrapper.h)
target_sources(dawn_wrapper PRIVATE
    src/bindgroup_layout_wrapper.cpp
    src/bindgroup_wrapper.cpp
    src/buffer_wrapper.cpp
    src/compute_wrapper.cpp
    src/dawn_wrapper.cpp
    src/encoder_wrapper.cpp
    src/render_wrapper.cpp
    src/texture_wrapper.cpp
    src/texture_output_wrapper.cpp
)

if (NOT EMSCRIPTEN)
    set(DAWN_FETCH_DEPENDENCIES ON)
    add_subdirectory("lib/dawn" EXCLUDE_FROM_ALL)
    target_link_libraries(dawn_wrapper PUBLIC
        webgpu_dawn
        webgpu_glfw
    )
endif()
