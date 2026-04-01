#pragma once

#include <dawn_wrapper/src/dawn_wrapper.hpp>

namespace dawn_wrapper {

template <typename Base>
struct Dawn_Module : public Base {

    template <typename... U>
    Dawn_Module(U... init)
        : Base(init...)
    {
    }

    void init(auto done)
    {
        m_dawn.on_load([done]() mutable {
            done([]() {});
        });
    }

    void run(auto& gs, float ft, auto cancel)
    {
#ifndef __EMSCRIPTEN__
        m_dawn.run();
#endif
    }

    dawn_wrapper::dawn_plugin m_dawn;
};

}