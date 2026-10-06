#include <cstring>

// INLINED
inline bool Blizzard::STL::CaseInsensitive::operator()(const std::string& a, const std::string& b) const {
    const char* str1 = a.c_str();
    const char* str2 = b.c_str();

    if (str1 == str2) {
        return false;
    }

    if (!str1) {
        return true;
    }

    if (str2) {
        return _stricmp(str1, str2) < 0;
    }

    return false;
}
