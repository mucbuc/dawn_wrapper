#pragma once

#include <functional>
#include <string>

#include <dawn_wrapper/src/dawn_wrapper.hpp>

namespace typo {

struct Context {

    Context(dawn_wrapper::dawn_plugin d)
        : m_dawn(d)
    {
    }

    dawn_wrapper::dawn_plugin dawn()
    {
        return m_dawn;
    }

private:
    dawn_wrapper::dawn_plugin m_dawn;
};

struct Shader {
    Shader(Context context)
        : m_compute(context.dawn().make_compute())
    {
    }

    void compile(std::string script, std::string entry, std::function<void(std::string)> cb)
    {
        m_compute.compile_shader(script, entry, cb);
    }

private:
    dawn_wrapper::compute_wrapper m_compute;
};

struct Pipeline {
    void compute(Shader);
};

struct Array {
};

struct Uniform {
};

struct Member {
};

}
