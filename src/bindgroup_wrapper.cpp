#include <iostream>
#include <map>
#include <memory>

#include "bindgroup_wrapper_impl.h"

namespace dawn_wrapper {
bindgroup_wrapper::bindgroup_wrapper(ptr_type ptr)
    : m_pimpl(ptr)
{
}

void bindgroup_wrapper::addBuffer(unsigned binding, buffer_wrapper buffer)
{
    ASSERT(buffer.m_pimpl);
    m_pimpl->addBuffer(binding, buffer);
}

void bindgroup_wrapper::addTexture(unsigned binding, texture_wrapper texture)
{
    ASSERT(texture.m_pimpl);
    m_pimpl->addTexture(binding, texture);
}

void bindgroup_wrapper::addTexture(unsigned binding, texture_output_wrapper texture)
{
    ASSERT(texture.m_pimpl);
    m_pimpl->addTexture(binding, texture);
}

void bindgroup_wrapper::addSampler(unsigned binding, texture_wrapper texture)
{
    ASSERT(texture.m_pimpl);
    m_pimpl->addSampler(binding, texture);
}

void bindgroup_wrapper::addSampler(unsigned binding, texture_output_wrapper texture)
{
    ASSERT(texture.m_pimpl);
    m_pimpl->addSampler(binding, texture);
}

bindgroup_wrapper::operator bool() const
{
    return m_pimpl ? true : false;
}

} // dawn_wrapper
