#include <iostream>
#include <map>
#include <memory>

#include "dawn_wrapper.h"
#include "dawn_utils.hpp"

using namespace wgpu;

#include "bindgroup_layout_wrapper_impl.h"

struct GLFWwindow;

namespace dawn_wrapper {
bindgroup_layout_wrapper::bindgroup_layout_wrapper(ptr_type ptr)
    : m_pimpl(ptr)
{
}

bindgroup_layout_wrapper& bindgroup_layout_wrapper::add_buffer(unsigned binding)
{
    m_pimpl->add_buffer(binding);
    return *this;
}

bindgroup_layout_wrapper& bindgroup_layout_wrapper::add_read_only_buffer(unsigned binding)
{
    m_pimpl->add_read_only_buffer(binding);
    return *this;
}

bindgroup_layout_wrapper& bindgroup_layout_wrapper::add_uniform_buffer(unsigned binding)
{
    m_pimpl->add_uniform_buffer(binding);
    return *this;
}

bindgroup_layout_wrapper& bindgroup_layout_wrapper::add_texture_1d(unsigned binding)
{
    m_pimpl->add_texture_1d(binding);
    return *this;
}

bindgroup_layout_wrapper& bindgroup_layout_wrapper::add_texture_2d(unsigned binding)
{
    m_pimpl->add_texture_2d(binding);
    return *this;
}

bindgroup_layout_wrapper& bindgroup_layout_wrapper::add_storage_texture_2d(unsigned binding)
{
    m_pimpl->add_storage_texture_2d(binding);
    return *this;
}

bindgroup_layout_wrapper& bindgroup_layout_wrapper::add_sampler(unsigned binding)
{
    m_pimpl->add_sampler(binding);
    return *this;
}

} // dawn_wrapper
