#pragma once

#include <functional>
#include <map>
#include <string>

#include <dawn_wrapper/src/dawn_wrapper.hpp>

namespace typo {

struct Uniforms {
    float m_age;
};

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

template <typename T>
struct Array {
};

struct Uniform {
};

struct Shader {
    Shader(Context context)
        : m_context(context)
        , m_compute(context.dawn().make_compute())
        , m_layout(m_compute.make_bindgroup_layout())
        , m_bindgroup(m_layout.make_bindgroup())
        , m_buffers()
        , m_binding(0)
    {
    }

    void read(Uniform, std::string name, unsigned group)
    {
        m_layout.add_uniform_buffer(m_binding);

        // m_factory[m_binding] =

        const auto factory = [this](auto size) -> dawn_wrapper::buffer_wrapper {
            return m_context.dawn().make_dst_buffer(sizeof(Uniforms), dawn_wrapper::buffer_type::uniform);
        };

        // const auto buffer = m_context.dawn().make_buffer();
        // m_buffers[m_binding] = std::make_pair(name, buffer);
        // m_bindgroup.add_buffer(m_binding, buffer);

        ++m_binding;
    }

    template <typename T>
    void read(Array<T>, std::string name, unsigned group)
    {
        m_layout.add_read_only_buffer(m_binding);

        ++m_binding;
    }

    template <typename T>
    void write(Array<T>, std::string name, unsigned group)
    {
        m_layout.add_buffer(m_binding);
        ++m_binding;
    }

    void compile(std::string script, std::string entry, std::function<void(std::string)> cb)
    {
        m_compute.compile_shader(script, entry, cb);
    }

private:
    Context m_context;
    dawn_wrapper::compute_wrapper m_compute;
    dawn_wrapper::bindgroup_layout_wrapper m_layout;
    dawn_wrapper::bindgroup_wrapper m_bindgroup;
    std::map<unsigned, std::pair<std::string, dawn_wrapper::buffer_wrapper>> m_buffers;
    unsigned m_binding;
};

struct Pipeline {

    void compute(Shader);
};

struct Member {
};

}
