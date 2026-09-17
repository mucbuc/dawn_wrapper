#pragma once

#include <iostream>

namespace dawn_wrapper {
struct shader_base {
protected:
    static void compilation_callback(CompilationInfoRequestStatus status, CompilationInfo const* compilationInfo, void* userdata)
    {
        shader_base* base = reinterpret_cast<shader_base*>(userdata);

        size_t errorCount = 0;
        for (auto i = 0; i < compilationInfo->messageCount; ++i) {
            const auto message = compilationInfo->messages[i];
            if (message.type == CompilationMessageType::Error) {
                base->m_messages << "Error(" << i << "): ";
                ++errorCount;
            } else if (message.type == CompilationMessageType::Warning) {
                base->m_messages << "Warning(" << i << "): ";
            } else if (message.type == CompilationMessageType::Info) {
                base->m_messages << "Info(" << i << "): ";
            }
            base->m_messages << message.message.data << std::endl;
        }
    }

protected:
    std::stringstream m_messages;
};
}
