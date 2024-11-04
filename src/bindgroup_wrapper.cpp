#include <iostream>
#include <map>
#include <memory>

#include "bindgroup_wrapper_impl.h"

namespace dawn_wrapper {
bindgroup_wrapper::bindgroup_wrapper(ptr_type ptr)
    : m_pimpl(ptr)
{
}

bindgroup_wrapper& bindgroup_wrapper::add_buffer(unsigned binding, buffer_wrapper buffer)
{
    ASSERT(buffer.m_pimpl);
    m_pimpl->add_buffer(binding, buffer);
    return *this;
}

bindgroup_wrapper& bindgroup_wrapper::addTexture(unsigned binding, texture_wrapper texture)
{
    ASSERT(texture.m_pimpl);
    m_pimpl->addTexture(binding, texture);
}

bindgroup_wrapper& bindgroup_wrapper::addTexture(unsigned binding, texture_output_wrapper texture)
{
    ASSERT(texture.m_pimpl);
    m_pimpl->addTexture(binding, texture);
    return *this;
}

bindgroup_wrapper& bindgroup_wrapper::add_sampler(unsigned binding, texture_wrapper texture)
{
    ASSERT(texture.m_pimpl);
    m_pimpl->add_sampler(binding, texture);
    return *this;
}

bindgroup_wrapper& bindgroup_wrapper::add_sampler(unsigned binding, texture_output_wrapper texture)
{
    ASSERT(texture.m_pimpl);
    m_pimpl->add_sampler(binding, texture);
    return *this;
}

bindgroup_wrapper::operator bool() const
{
    return m_pimpl ? true : false;
}

} // dawn_wrapper
