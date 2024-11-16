project(dawn_wrapper_lib)

add_library(dawn_wrapper STATIC src/dawn_wrapper.h)

set_target_properties(dawn_wrapper PROPERTIES
    CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON
)

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
