#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00BB9BF4
static const SYSMSGSEVERITY s_severities[4] = { { 0xFF, 0xFF, 0xFF, 0, 10.0f }, { 0xFF, 0xFF, 0x7F, 0, 15.0f }, { 0xFF, 0x7F, 0x7F, 0, 15.0f }, { 0x7F, 0xFF, 0xFF, 0, 20.0f } };

// STATIC: %0x00BB9C14
static int32_t s_enabled = 1;

// STATIC: %0x00BB9C18
static int32_t s_maxSeverity = 3;

// STATIC: %0x00BB9C1C
static uint32_t s_categoryFilter = 0xFFFFFFFF;

// STATIC: %0x00CD285C
static int32_t s_minSeverity;

// PROVEN: 31ecab71 IDENTICAL
// OFFSET: 0x0053E840
void SysMsgGetSeverityColor(STATUS_TYPE severity, float& red, float& green, float& blue) {
    red = s_severities[severity].m_red * 0.0039215689f;
    green = s_severities[severity].m_green * 0.0039215689f;
    blue = s_severities[severity].m_blue * 0.0039215689f;
}

// OFFSET: %0x0054F240
int32_t SysMsgAdd(const char* text, STATUS_TYPE severity) {
    if (!text || severity >= STATUS_NUMTYPES) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    return 1;
}

// OFFSET: %0x0054F270
int32_t SysMsgAdd(CStatus* status) {
    if (status->IsEmpty()) {
        return 1;
    }

    char* text = status->GetErrorStrAlloc(STATUS_INFO, "\n");
    int32_t result = SysMsgAdd(text, status->m_highestSeverity);
    SMemFree(text, __FILE__, __LINE__, 0);

    return result;
}

// OFFSET: %0x0054F2D0
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

// OFFSET: %0x0054F320
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

// PROVEN: 910dc464 IDENTICAL
// OFFSET: 0x0053E7B0
void SysMsgEnable(int32_t enable) {
    s_enabled = enable;
}

// PROVEN: 7ac02aa2 IDENTICAL
// OFFSET: 0x0053E7C0
void SysMsgSetMinDisplayLevel(int32_t level) {
    s_minSeverity = level;

    if (s_maxSeverity < level) {
        s_maxSeverity = level;
    }
}

// PROVEN: b3910be8 IDENTICAL
// OFFSET: 0x0053E7E0
void SysMsgSetMaxDisplayLevel(int32_t level) {
    s_maxSeverity = level;

    if (s_minSeverity > level) {
        s_minSeverity = level;
    }
}

// PROVEN: b0437edb IDENTICAL
// OFFSET: 0x0053E800
int32_t SysMsgGetMinDisplayLevel() {
    return s_minSeverity;
}

// PROVEN: abf5a958 IDENTICAL
// OFFSET: 0x0053E810
int32_t SysMsgGetMaxDisplayLevel() {
    return s_maxSeverity;
}

// PROVEN: 95b1318a IDENTICAL
// OFFSET: 0x0053E820
void SysMsgSetFilter(uint32_t filter) {
    s_categoryFilter = filter;
}

// PROVEN: 6017db40 IDENTICAL
// OFFSET: 0x0053E830
uint32_t SysMsgGetFilter() {
    return s_categoryFilter;
}

// PROVEN: 76051731 IDENTICAL
// OFFSET: 0x0053E8A0
void SysMsgInitialize() {
    s_enabled = 1;
    s_minSeverity = 0;
    s_maxSeverity = 3;
    s_categoryFilter = 0xFFFFFFFF;
}

// PROVEN: f92a5b55 IDENTICAL
// OFFSET: 0x00401000
void SysMsgSetCallback(void (*callback)(const char* text, STATUS_TYPE severity)) {
}

// OFFSET: %0x0054F370
void SysMsgShutdown() {
    s_enabled = 0;
}
