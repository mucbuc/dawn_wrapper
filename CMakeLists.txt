cmake_minimum_required(VERSION 3.27)

project(dawn_wrapper_lib)

add_library(dawn_wrapper STATIC src/dawn_wrapper.hpp)

set_target_properties(dawn_wrapper PROPERTIES
    CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON
)

target_sources(dawn_wrapper PRIVATE
    src/bindgroup_layout_wrapper.cpp
    src/bindgroup_layout_wrapper_impl.hpp
    src/bindgroup_wrapper.cpp
    src/bindgroup_wrapper_impl.hpp
    src/buffer_wrapper.cpp
    src/buffer_wrapper_impl.hpp
    src/compute_wrapper.cpp
    src/compute_wrapper_impl.hpp
    src/dawn_wrapper.cpp
    src/encoder_wrapper.cpp
    src/encoder_wrapper_impl.hpp
    src/render_wrapper.cpp
    src/render_wrapper_impl.hpp
    src/surface_wrapper.cpp
    src/surface_wrapper_impl.hpp
    src/texture_output_wrapper.cpp
    src/texture_output_wrapper_impl.hpp
    src/texture_wrapper.cpp
    src/texture_wrapper_impl.hpp
)

if (NOT EMSCRIPTEN)
    set(DAWN_FETCH_DEPENDENCIES ON)
    set(TINT_BUILD_TESTS FALSE)
    set(BUILD_SAMPLES OFF)
    set(TINT_BUILD_SPV_READER OFF)
    set(BUILD_TESTS OFF)

    add_subdirectory("lib/dawn" EXCLUDE_FROM_ALL)
    target_link_libraries(dawn_wrapper PUBLIC
        webgpu_dawn
    )
endif()
