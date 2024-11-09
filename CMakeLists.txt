
set(CMAKE_CXX_STANDARD 20)
set(DAWN_FETCH_DEPENDENCIES ON)
set(CMAKE_XCODE_ATTRIBUTE_ARCHS "$(ARCHS_STANDARD)")
set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD)")

project(dawn_wrapper_lib)
add_subdirectory("lib/dawn" EXCLUDE_FROM_ALL)

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

target_link_libraries(dawn_wrapper PUBLIC
    dawn_internal_config
    dawncpp
    dawn_common
    dawn_glfw
    dawn_native
    dawn_wire
    dawn_utils
)
