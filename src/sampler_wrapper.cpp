#include "sampler_wrapper_impl.hpp"

namespace dawn_wrapper {
sampler_wrapper::sampler_wrapper(ptr_type ptr)
    : m_pimpl(ptr)
{
}

bool sampler_wrapper::is_valid() const
{
    return m_pimpl ? true : false;
}

} // dawn_wrapper
