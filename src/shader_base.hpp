#pragma once

#include <iostream>

namespace dawn_wrapper {
struct shader_base {
protected:
    static void compilation_callback(CompilationInfoRequestStatus status, CompilationInfo const * compilationInfo, void* userdata)
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

        ASSERT(!errorCount);

        if (!messages.str().empty()) {
            std::cout << messages.str() << std::endl;
        }
        // instance->m_shaderCompileCallback(messages.str());

        //        if (!errorCount) {
        //            instance->setFragmentShaderUser();
        //        }
    }
};
}
