cmake_minimum_required(VERSION 3.27)

project(Example)

add_executable(Example src/example.cpp)
set_target_properties(Example PROPERTIES
    CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON
)
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
add_compile_definitions(TARGET_HEADLESS)
add_subdirectory(lib/dawn_wrapper)

if (EMSCRIPTEN)
    set_target_properties(Example PROPERTIES SUFFIX ".html")
    target_link_options(Example PRIVATE "-sUSE_WEBGPU=1" "-sUSE_GLFW=3")
endif()

target_link_libraries(Example PRIVATE
    dawn_wrapper
)

target_include_directories(Example SYSTEM PUBLIC ${CMAKE_SOURCE_DIR})
