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

#if !defined(TARGET_HEADLESS) && !defined(__EMSCRIPTEN__)
void surface_wrapper::setup(GLFWwindow* window, unsigned width, unsigned height, bool opaque)
{
    m_pimpl->setup(window, width, height, opaque);
}
#endif

#if !defined(TARGET_HEADLESS) && defined(__EMSCRIPTEN__)
void surface_wrapper::setup(std::string selector, unsigned width, unsigned height)
{
    m_pimpl->setup(selector, width, height);
}
#endif

bool surface_wrapper::is_valid() const
{
    return m_pimpl ? true : false;
}

void surface_wrapper::present()
{
    m_pimpl->present();
}

} // dawn_wrapper
