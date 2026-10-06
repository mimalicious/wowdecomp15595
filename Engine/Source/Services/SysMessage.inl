#ifndef ENGINE_SOURCE_SERVICES_SYSMESSAGE_INL
#define ENGINE_SOURCE_SERVICES_SYSMESSAGE_INL

// OFFSET: %0x005B3CE0
inline int32_t SysMsgPrintf(STATUS_TYPE severity, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int32_t result = SysMsgVPrintf(severity, 1, format, args);
    va_end(args);

    return result;
}

#endif
