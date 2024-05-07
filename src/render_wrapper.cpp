#include <iostream>
#include <map>
#include <memory>

#include "render_wrapper_impl.h"

struct GLFWwindow;

namespace dawn_wrapper {

render_wrapper::render_wrapper(ptr_type ptr)
    : m_pimpl(ptr)
{
}

void render_wrapper::setup_surface(GLFWwindow* window, unsigned width, unsigned height, bool opaque)
{
    m_pimpl->setup_surface(window, width, height, opaque);
}

void render_wrapper::make_shader(std::string script, std::string entryPoint)
{
    m_pimpl->make_fragmentShader(script, entryPoint);
}

bindgroup_layout_wrapper render_wrapper::make_bindgroup_layout()
{
    return m_pimpl->make_bindgroup_layout();
}

bindgroup_wrapper render_wrapper::make_bindgroup()
{
    return m_pimpl->make_bindgroup();
}

void render_wrapper::make_pipeline(bindgroup_layout_wrapper layout)
{
    m_pimpl->make_pipeline(layout);
}

void render_wrapper::make_pipeline()
{
    m_pimpl->make_pipeline();
}

void render_wrapper::render(bindgroup_wrapper bindGroup, encoder_wrapper encoder)
{
    m_pimpl->render(bindGroup, encoder);
}

void render_wrapper::render(encoder_wrapper encoder)
{
    m_pimpl->render(encoder);
}

} // dawn_wrapper
