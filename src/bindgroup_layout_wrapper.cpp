#include <iostream>
#include <map>
#include <memory>

#include "bindgroup_layout_wrapper_impl.hpp"

struct GLFWwindow;

namespace dawn_wrapper {
bindgroup_layout_wrapper::bindgroup_layout_wrapper(ptr_type ptr)
    : m_pimpl(ptr)
{
}

bindgroup_layout_wrapper& bindgroup_layout_wrapper::add_buffer(unsigned binding, bool enable)
{
    m_pimpl->add_buffer(binding, enable);
    return *this;
}

bindgroup_layout_wrapper& bindgroup_layout_wrapper::add_read_only_buffer(unsigned binding, bool enable)
{
    m_pimpl->add_read_only_buffer(binding, enable);
    return *this;
}

bindgroup_layout_wrapper& bindgroup_layout_wrapper::add_uniform_buffer(unsigned binding, bool enable)
{
    m_pimpl->add_uniform_buffer(binding, enable);
    return *this;
}

bindgroup_layout_wrapper& bindgroup_layout_wrapper::add_texture_1d(unsigned binding, bool enable)
{
    m_pimpl->add_texture_1d(binding, enable);
    return *this;
}

bindgroup_layout_wrapper& bindgroup_layout_wrapper::add_texture_2d(unsigned binding, bool enable)
{
    m_pimpl->add_texture_2d(binding, enable);
    return *this;
}

bindgroup_layout_wrapper& bindgroup_layout_wrapper::add_storage_texture_2d(unsigned binding, bool enable)
{
    m_pimpl->add_storage_texture_2d(binding, enable);
    return *this;
}

bindgroup_layout_wrapper& bindgroup_layout_wrapper::add_sampler(unsigned binding, bool enable)
{
    m_pimpl->add_sampler(binding, enable);
    return *this;
}

} // dawn_wrapper
