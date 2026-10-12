#ifndef ENGINE_SOURCE_SERVICES_PROFILE_INL
#define ENGINE_SOURCE_SERVICES_PROFILE_INL

// INLINED
inline int32_t ProfileGetValue(void* profile, const char* section, const char* key, char* buffer, uint32_t bufferSize, uint32_t index) {
    if (!section) {
        DebugValidate("section");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (!key) {
        DebugValidate("key");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (!buffer) {
        DebugValidate("value");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    *buffer = '\0';

    ProfileInternal::SECTION* sect = static_cast<ProfileInternal::PROFILE*>(profile)->m_sections.Ptr(section);

    if (!sect) {
        return 0;
    }

    ProfileInternal::KEYVALUE* keyValue = sect->m_keys.Ptr(key);

    if (!keyValue || index >= keyValue->m_values.Count()) {
        return 0;
    }

    char* str = keyValue->m_values[index];

    if (!str) {
        return 0;
    }

    SStrCopy(buffer, str, bufferSize);

    return 1;
}

#endif
