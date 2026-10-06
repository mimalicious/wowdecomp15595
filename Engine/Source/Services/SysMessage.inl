#ifndef ENGINE_SOURCE_SERVICES_SYSMESSAGE_INL
#define ENGINE_SOURCE_SERVICES_SYSMESSAGE_INL

// PROVEN: 7c65c78b IDENTICAL/NONE
// OFFSET: 0x005ABC70/0x14021CB00
inline int32_t SysMsgPrintf(STATUS_TYPE severity, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int32_t result = SysMsgVPrintf(severity, 1, format, args);
    va_end(args);

    return result;
}

#endif
