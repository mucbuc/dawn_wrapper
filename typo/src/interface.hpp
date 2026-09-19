#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <sstream>
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

struct Index {

    Index(std::string name)
        : m_name(name)
    {
    }

    std::string wgsl()
    {
        return m_name;
    }

private:
    std::string m_name;
};

struct Expression {

    // template <typename T>
    Expression(std::string name)
        : m_wgsl(name)
    {
    }

    Expression& operator[](Index i)
    {
        m_wgsl += "[" + i.wgsl() + "] ";
        return *this;
    }

    Expression& operator+(Expression e)
    {
        m_wgsl += " + " + e.wgsl();
        return *this;
    }

    Expression& operator+=(Expression e)
    {
        m_wgsl += " += " + e.wgsl();
        return *this;
    }

    std::string wgsl()
    {
        return m_wgsl;
    }

private:
    std::string m_wgsl;
};

template <typename T>
struct Array {
    Array(std::string name)
        : m_name(name)
    {
    }

private:
    std::string m_name;
};

struct Uniform {
};

struct WGSL {
    template <typename T>
    WGSL(T expression)
        : m_wgsl()
    {
    }

    std::string wgsl()
    {
        return m_wgsl;
    }

private:
    std::string m_wgsl;
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

    std::string wgsl_bindings()
    {
        return "";
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

    void compile(WGSL script, std::string entry, std::function<void(std::string)> cb)
    {
        std::stringstream s;
        s << wgsl_bindings();
        s << script.wgsl();

        std::cout << script.wgsl() << std::endl;

        m_compute.compile_shader(s.str(), entry, cb);
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

template <typename T>
struct Member {
    Member(Uniform u, std::string name);

    void set(T);
};

}
