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

void render_wrapper::compile_shader(std::string script, std::string entryPoint, std::map<std::string, std::string> variables)
{
    m_pimpl->compile_shader(script, entryPoint, variables);
}

bindgroup_layout_wrapper render_wrapper::make_bindgroup_layout()
{
    return m_pimpl->make_bindgroup_layout();
}

bindgroup_wrapper render_wrapper::make_bindgroup()
{
    return m_pimpl->make_bindgroup();
}

void render_wrapper::init_pipeline(bindgroup_layout_wrapper layout)
{
    m_pimpl->init_pipeline(layout);
}

void render_wrapper::init_pipeline()
{
    m_pimpl->init_pipeline();
}

void render_wrapper::render(bindgroup_wrapper bindGroup, encoder_wrapper encoder)
{
    m_pimpl->render(bindGroup, encoder);
}

void render_wrapper::render(encoder_wrapper encoder)
{
    m_pimpl->render(encoder);
}

render_wrapper::operator bool() const
{
    return m_pimpl ? true : false;
}

} // dawn_wrapper
