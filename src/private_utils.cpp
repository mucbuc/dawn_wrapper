#include "private_utils.hpp"

namespace private_dawn_wrapper {
std::string apply_variables(std::string source, std::map<std::string, std::string> variables)
{
    while (true) {
        bool done = true;
        for (auto i : variables) {
            const auto name = i.first;
            const auto target = "{{" + name + "}}";
            while (true) {

                const auto p = source.find(target);

                if (p == std::string::npos) {
                    break;
                }

                source.replace(p, target.size(), i.second);
                done = false;
            }
        }

        if (done) {
            break;
        }
    }

    return source;
}
}
