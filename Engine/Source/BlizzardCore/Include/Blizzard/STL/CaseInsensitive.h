#ifndef ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_STL_CASEINSENSITIVE_H
#define ENGINE_SOURCE_BLIZZARDCORE_INCLUDE_BLIZZARD_STL_CASEINSENSITIVE_H

#include <string>

namespace Blizzard {
namespace STL {

struct CaseInsensitive {
    bool operator()(const std::string& a, const std::string& b) const;
};

}
}

#include "Engine/Source/BlizzardCore/Include/Blizzard/STL/CaseInsensitive.inl"

#endif
