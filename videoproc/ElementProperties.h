#pragma once

#include <string>
#include <unordered_map>

namespace videoproc {

struct ElementProperties {
    std::string name;
    std::unordered_map<std::string, std::string> arguments;
};

}
