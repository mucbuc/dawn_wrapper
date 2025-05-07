#include <iostream>
#include <map>
#include <memory>

#include "dawn_wrapper.hpp"
#include "surface_wrapper_impl.hpp"

struct GLFWwindow;

namespace dawn_wrapper {

surface_wrapper::surface_wrapper(ptr_type ptr)
    : m_pimpl(ptr)
{
}

void surface_wrapper::setup(GLFWwindow* window, unsigned width, unsigned height, bool opaque)
{
    m_pimpl->setup(window, width, height, opaque);
}

void surface_wrapper::setup(std::string selector, unsigned width, unsigned height)
{
    m_pimpl->setup(selector, width, height);
}

surface_wrapper::operator bool() const
{
    return m_pimpl ? true : false;
}

void surface_wrapper::present()
{
    m_pimpl->present();
}

} // dawn_wrapper
