#include <iostream>
#include <map>
#include <memory>

#include "compute_wrapper_impl.h"

struct GLFWwindow;

namespace dawn_wrapper {
compute_wrapper::compute_wrapper(ptr_type ptr)
    : m_pimpl(ptr)
{
}

void compute_wrapper::make_shader(std::string script, std::string entryPoint)
{
    m_pimpl->make_shader(script, entryPoint);
}

void compute_wrapper::make_pipeline(bindgroup_layout_wrapper layout)
{
    m_pimpl->make_pipeline(layout);
}

bool compute_wrapper::compute(bindgroup_wrapper bindGroup, unsigned width, unsigned height, encoder_wrapper encoder)
{
    return m_pimpl->compute(bindGroup, width, height, encoder);
}

bindgroup_layout_wrapper compute_wrapper::make_bindgroup_layout()
{
    return m_pimpl->make_bindgroup_layout();
}

bindgroup_wrapper compute_wrapper::make_bindgroup()
{
    return m_pimpl->make_bindgroup();
}

} // dawn_wrapper