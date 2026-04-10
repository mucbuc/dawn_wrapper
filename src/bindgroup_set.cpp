#include <iostream>
#include <map>
#include <memory>

#include "bindgroup_set_impl.hpp"

namespace dawn_wrapper {
bindgroup_set::bindgroup_set()
    : m_pimpl(std::make_shared<bindgroup_set::pimpl>())
{
}

bindgroup_set& bindgroup_set::add_bindgroup(bindgroup_wrapper bg, unsigned group)
{
    m_pimpl->add_bindgroup(bg, group);
    return *this;
}

} // dawn_wrapper
