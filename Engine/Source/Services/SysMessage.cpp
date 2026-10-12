#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00CA48C0/0x140B42B10
static const SYSMSGSEVERITY s_severities[4] = { { 0xFF, 0xFF, 0xFF, 0, 10.0f }, { 0xFF, 0xFF, 0x7F, 0, 15.0f }, { 0xFF, 0xA5, 0x00, 0, 20.0f }, { 0xFF, 0x7F, 0x7F, 0, 30.0f } };

// STATIC: 0x00CA48E0/0x140B42B30
static int32_t s_enabled = 1;

// STATIC: 0x00CA48E4/0x140B42B34
static int32_t s_maxSeverity = 3;

// STATIC: 0x00CA48E8/0x140B42B38
static uint32_t s_categoryFilter = 0xFFFFFFFF;

// STATIC: 0x00DD3CFC/0x140CB9AB8
static int32_t s_minSeverity;

// OFFSET: 0x0053E840/0x140193030
void SysMsgGetSeverityColor(STATUS_TYPE severity, float& red, float& green, float& blue) {
    red = s_severities[severity].m_red * 0.0039215689f;
    green = s_severities[severity].m_green * 0.0039215689f;
    blue = s_severities[severity].m_blue * 0.0039215689f;
}

// OFFSET: 0x0053E8D0/0x1401930C0
int32_t SysMsgAdd(const char* text, STATUS_TYPE severity) {
    if (!text || severity >= STATUS_NUMTYPES) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return 1;
}

// OFFSET: 0x0053E900/0x1401930F0
int32_t SysMsgAdd(CStatus* status) {
    if (status->IsEmpty()) {
        return 1;
    }

    char* text = status->GetErrorStrAlloc(STATUS_INFO, "\n");
    int32_t result = SysMsgAdd(text, status->m_highestSeverity);
    SMemFree(text, __FILE__, __LINE__, 0);

    return result;
}

// OFFSET: 0x0053E960/0x140193170
int32_t SysMsgVPrintf(STATUS_TYPE severity, uint32_t flags, const char* format, va_list args) {
    char buffer[256];
    _vsnprintf(buffer, sizeof(buffer), format, args);
    buffer[255] = '\0';

    if (severity < STATUS_NUMTYPES) {
        return 1;
    }

    SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);

    return 0;
}

// OFFSET: 0x0053E9B0/0x1401931C0
int32_t SysMsgPrintf(STATUS_TYPE severity, uint32_t flags, const char* format, ...) {
    va_list args;
    va_start(args, format);

    if (format) {
        char buffer[256];
        _vsnprintf(buffer, sizeof(buffer), format, args);
        buffer[255] = '\0';

        if (severity < STATUS_NUMTYPES) {
            return 1;
        }
    }

    SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);

    return 0;
}

// OFFSET: 0x0053E7B0/0x140192FA0
void SysMsgEnable(int32_t enable) {
    s_enabled = enable;
}

// OFFSET: 0x0053E7C0/0x140192FB0
void SysMsgSetMinDisplayLevel(int32_t level) {
    s_minSeverity = level;

    if (s_maxSeverity < level) {
        s_maxSeverity = level;
    }
}

// OFFSET: 0x0053E7E0/0x140192FD0
void SysMsgSetMaxDisplayLevel(int32_t level) {
    s_maxSeverity = level;

    if (s_minSeverity > level) {
        s_minSeverity = level;
    }
}

// OFFSET: 0x0053E800/0x140192FF0
int32_t SysMsgGetMinDisplayLevel() {
    return s_minSeverity;
}

// OFFSET: 0x0053E810/0x140193000
int32_t SysMsgGetMaxDisplayLevel() {
    return s_maxSeverity;
}

// OFFSET: 0x0053E820/0x140193010
void SysMsgSetFilter(uint32_t filter) {
    s_categoryFilter = filter;
}

// OFFSET: 0x0053E830/0x140193020
uint32_t SysMsgGetFilter() {
    return s_categoryFilter;
}

// OFFSET: 0x0053E8A0/0x140193090
void SysMsgInitialize() {
    s_enabled = 1;
    s_minSeverity = 0;
    s_maxSeverity = 3;
    s_categoryFilter = 0xFFFFFFFF;
}

// OFFSET: 0x00401000/0x1407D7E80
void SysMsgSetCallback(void (*callback)(const char* text, STATUS_TYPE severity)) {
}

// INLINED
void SysMsgShutdown() {
    s_enabled = 0;
}
