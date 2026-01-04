cmake_minimum_required(VERSION 3.27)

project(Example)

add_executable(Example src/example.cpp)

set_target_properties(Example PROPERTIES
    CXX_STANDARD 20
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF
	COMPILE_WARNING_AS_ERROR ON
)

add_compile_definitions(TARGET_HEADLESS)
add_subdirectory(lib/dawn_wrapper)
add_subdirectory(lib/asserter)

if (EMSCRIPTEN)
    set_target_properties(Example PROPERTIES SUFFIX ".html")
    target_link_options(Example PRIVATE 
        -sUSE_WEBGPU=1
        --emrun
    )
endif()

target_link_libraries(Example PRIVATE
    dawn_wrapper
    asserter
)

target_include_directories(Example SYSTEM PUBLIC ${CMAKE_CURRENT_LIST_DIR}/lib)
