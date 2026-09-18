#pragma once

#include <functional>
#include <memory>
#include <sstream>
#include <string>

namespace dawn_wrapper {
struct shader_base {
protected:
    // What GetCompilationInfo answers with, delivered to whoever asked for the
    // compile. ASYNCHRONOUS: Dawn calls back after compile_shader has returned,
    // so there is no way to hand these back as a return value — a string returned
    // from compile_shader is always empty, measured 2026-09-17.
    //
    // The whole message text, or an empty string when the shader compiled clean.
    // A caller that passes nothing gets silence, which is why every caller that
    // cares passes one: nothing here prints, and nothing here aborts.
    //
    // It used to ASSERT(!errorCount) instead, which aborted the module. Under
    // Emscripten that takes the whole page down — every element sharing the
    // runtime — over one bad shader, and the abort discarded the diagnostic that
    // said what was wrong.
    using compile_callback = std::function<void(std::string)>;

    // Heap-owned, one per compile, deleted when the callback runs. The request
    // has to outlive the call that made it, and Dawn's userdata is the only thing
    // carried through — so the callback cannot reach for a member of a wrapper
    // that may be gone by then.
    struct compile_request {
        compile_callback on_messages;
    };

    static void compilation_callback(CompilationInfoRequestStatus status,
        CompilationInfo const* compilationInfo, void* userdata)
    {
        const std::unique_ptr<compile_request> request(
            static_cast<compile_request*>(userdata));

        std::stringstream messages;
        if (compilationInfo != nullptr) {
            for (auto i = 0u; i < compilationInfo->messageCount; ++i) {
                const auto message = compilationInfo->messages[i];
                if (message.type == CompilationMessageType::Error) {
                    messages << "Error(" << i << "): ";
                } else if (message.type == CompilationMessageType::Warning) {
                    messages << "Warning(" << i << "): ";
                } else if (message.type == CompilationMessageType::Info) {
                    messages << "Info(" << i << "): ";
                }
                // Length-aware, like notify_page in dawn_wrapper.cpp: a
                // StringView is not necessarily null-terminated, and SIZE_MAX is
                // the sentinel that says it is. Printing .data alone read past
                // the end — one message came out as its own text twice over.
                const auto text = message.message.data == nullptr
                    ? std::string()
                    : (message.message.length == SIZE_MAX
                            ? std::string(message.message.data)
                            : std::string(message.message.data, message.message.length));
                messages << text << std::endl;
            }
        }

        if (request && request->on_messages) {
            request->on_messages(messages.str());
        }
    }

    // The userdata for one compile. Ownership passes to Dawn and comes back in
    // the callback above.
    static void* make_request(compile_callback on_messages)
    {
        return new compile_request { std::move(on_messages) };
    }
};
}
