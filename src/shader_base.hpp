#pragma once

#include <iostream>

namespace dawn_wrapper {
struct shader_base {
protected:
    static void compilation_callback(CompilationInfoRequestStatus status, CompilationInfo const* compilationInfo, void* userdata)
    {
        std::stringstream messages;
        size_t errorCount = 0;
        for (auto i = 0; i < compilationInfo->messageCount; ++i) {
            const auto message = compilationInfo->messages[i];
            if (message.type == CompilationMessageType::Error) {
                messages << "Error(" << i << "): ";
                ++errorCount;
            } else if (message.type == CompilationMessageType::Warning) {
                messages << "Warning(" << i << "): ";
            } else if (message.type == CompilationMessageType::Info) {
                messages << "Info(" << i << "): ";
            }
            messages << message.message.data << std::endl;
        }

        // PRINTED BEFORE THE ASSERT, and the order is the whole point.
        //
        // It was the other way round, so every WGSL compile error this project
        // has ever had aborted while still holding the message that explained
        // it. What reached the console was "Assertion failed: (!errorCount)" and
        // a line number in this file — which says a shader failed and nothing
        // whatever about why. A failure that discards its own diagnostic is
        // worse than no check, because it looks like it reported.
        // cerr, not cout. Under Emscripten cout reaches console.log and cerr
        // reaches console.error, and the headless capture harness collects
        // errors — so on cout the diagnostic printed into a stream nothing was
        // reading, which is the same failure as not printing it.
        if (!messages.str().empty()) {
            std::cerr << messages.str() << std::endl;
        }

        ASSERT(!errorCount);
        // instance->m_shaderCompileCallback(messages.str());

        //        if (!errorCount) {
        //            instance->setFragmentShaderUser();
        //        }
    }
};
}
