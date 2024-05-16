#include <iostream>
#include <map>
#include <memory>

#include "bindgroup_layout_wrapper_impl.h"

struct GLFWwindow;

namespace dawn_wrapper {
bindgroup_layout_wrapper::bindgroup_layout_wrapper(ptr_type ptr)
    : m_pimpl(ptr)
{
}

void bindgroup_layout_wrapper::addBuffer(unsigned binding)
{
    m_pimpl->addBuffer(binding);
}

void bindgroup_layout_wrapper::addReadOnlyBuffer(unsigned binding)
{
    m_pimpl->addReadOnlyBuffer(binding);
}

void bindgroup_layout_wrapper::addUniformBuffer(unsigned binding)
{
    m_pimpl->addUniformBuffer(binding);
}

void bindgroup_layout_wrapper::addTexture_1d(unsigned binding)
{
    m_pimpl->addTexture_1d(binding);
}

void bindgroup_layout_wrapper::addTexture_2d(unsigned binding)
{
    m_pimpl->addTexture_2d(binding);
}

void bindgroup_layout_wrapper::addStorageTexture_2d(unsigned binding)
{
    m_pimpl->addStorageTexture_2d(binding);
}

void bindgroup_layout_wrapper::addSampler(unsigned binding)
{
    m_pimpl->addSampler(binding);
}

} // dawn_wrapper
