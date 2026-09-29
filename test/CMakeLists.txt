cmake_minimum_required(VERSION 3.27)

project(Example)

add_executable(Example src/example.cpp)

# NOT checked in: covers render_config and the compile callback, both of which
# driftype never exercises.
add_executable(RenderConfig src/render_config.cpp)
set_target_properties(RenderConfig PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
target_link_libraries(RenderConfig PRIVATE dawn_wrapper asserter)
target_include_directories(RenderConfig SYSTEM PUBLIC ${CMAKE_CURRENT_LIST_DIR}/lib)

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
        --use-port=emdawnwebgpu
        --closure=1
        --emrun
    )
endif()

target_link_libraries(Example PRIVATE
    dawn_wrapper
    asserter
)

target_include_directories(Example SYSTEM PUBLIC ${CMAKE_CURRENT_LIST_DIR}/lib)

# The particle target by hand: a compute pass, then a draw reading the particles
# in its vertex stage into an offscreen target, read back and checked pixel for
# pixel. Browser only; ./run_particles builds and runs it headless.
if (EMSCRIPTEN)
    add_executable(Particles src/particles_by_hand.cpp)
    set_target_properties(Particles PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        COMPILE_WARNING_AS_ERROR ON
        SUFFIX ".html"
    )
    target_link_options(Particles PRIVATE --use-port=emdawnwebgpu --emrun)
    target_link_libraries(Particles PRIVATE dawn_wrapper asserter)
    target_include_directories(Particles SYSTEM PUBLIC ${CMAKE_CURRENT_LIST_DIR}/lib)
endif()
