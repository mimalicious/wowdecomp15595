#include <cstdarg>
#include <cstdint>
#include <cstring>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00C0EA04
static CStatus s_errorList;

// INLINED
CStatus::STATUSENTRY::~STATUSENTRY() {
    if (this->m_text) {
        SMemFree(this->m_text, __FILE__, __LINE__, 0);
    }
}

// INLINED
void CStatus::Clear() {
    this->m_statusList.Clear();
    this->m_highestSeverity = STATUS_INFO;
}

// PROVEN: b83ebdf0 IDENTICAL/IDENTICAL
// OFFSET: 0x00872470/0x1405A8870
CStatus& GetGlobalStatusObj() {
    return s_errorList;
}

// OFFSET: 0x00872480/0x1405A8880
void CStatus::Add(const CStatus& source) {
    CStatus& src = const_cast<CStatus&>(source);

    for (STATUSENTRY* entry = src.m_statusList.Head(); entry; entry = src.m_statusList.Next(entry)) {
        this->Add(entry->m_severity, entry->m_text);
    }
}

// PROVEN: 564bdf23 IDENTICAL/DIFFERENT
// OFFSET: 0x008724D0/0x1405A88E0
int32_t CStatus::IsEmpty() {
    return this->m_statusList.Head() == NULL;
}

// OFFSET: 0x008724F0/0x1405A8900
char* CStatus::GetErrorStr(char* buffer, size_t bufchars, STATUS_TYPE minSeverity, const char* separator) {
    char* curr = buffer;
    *curr = 0;

    size_t separatorchars = separator && *separator ? SStrLen(separator) : 0;

    for (STATUSENTRY* entry = this->m_statusList.Head(); entry; entry = this->m_statusList.Next(entry)) {
        if (entry->m_severity < minSeverity) {
            continue;
        }

        size_t textchars = SStrLen(entry->m_text);

        if (textchars + separatorchars >= bufchars) {
            return buffer;
        }

        SStrCopy(curr, entry->m_text, bufchars);
        curr += textchars;
        bufchars -= textchars;

        if (separatorchars) {
            SStrPack(curr, separator, bufchars);
            bufchars -= separatorchars;
            curr += separatorchars;
        }
    }

    return buffer;
}

// OFFSET: #0x0047C1D0
uint32_t CStatus::GetErrorStrLen(STATUS_TYPE minSeverity, const char* separator) {
    size_t separatorchars = 0;
    uint32_t length = 0;

    if (separator && *separator) {
        separatorchars = SStrLen(separator);
    }

    for (STATUSENTRY* entry = this->m_statusList.Head(); entry; entry = this->m_statusList.Next(entry)) {
        if (entry->m_severity >= minSeverity) {
            length += SStrLen(entry->m_text) + separatorchars;
        }
    }

    return length;
}

// PROVEN: 21318ce1 IDENTICAL/DIFFERENT
// OFFSET: 0x008726B0/0x1405A8AF0
char* CStatus::GetErrorStrAlloc(STATUS_TYPE minSeverity, const char* separator) {
    size_t bufchars = this->GetErrorStrLen(minSeverity, separator) + 1;
    char* buffer = static_cast<char*>(SMemAlloc(bufchars, __FILE__, __LINE__, 0));

    if (buffer) {
        this->GetErrorStr(buffer, bufchars, minSeverity, separator);
    }

    return buffer;
}

// OFFSET: #0x0047C290
void CStatus::Prepend(STATUS_TYPE severity, const char* format, ...) {
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));

    if (!format) {
        DebugValidate("format");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    va_list args;
    va_start(args, format);
    SStrVPrintf(buffer, sizeof(buffer), format, args);

    if (!buffer[0]) {
        return;
    }

    STATUSENTRY* entry = this->m_statusList.NewNode(1, 0, 0);
    entry->m_text = SStrDupA(buffer, __FILE__, __LINE__);
    entry->m_severity = severity;

    if (severity > this->m_highestSeverity) {
        this->m_highestSeverity = severity;
    }
}

// OFFSET: #0x0047C360
void CStatus::Add(STATUS_TYPE severity, const char* format, ...) {
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));

    if (!format) {
        DebugValidate("format");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    va_list args;
    va_start(args, format);
    SStrVPrintf(buffer, sizeof(buffer), format, args);

    if (!buffer[0]) {
        return;
    }

    STATUSENTRY* entry = this->m_statusList.NewNode(2, 0, 0);
    entry->m_text = SStrDupA(buffer, __FILE__, __LINE__);
    entry->m_severity = severity;

    if (severity > this->m_highestSeverity) {
        this->m_highestSeverity = severity;
    }
}

// OFFSET: [%0x007D7560, %0x004CB750]
CStatus::~CStatus() {
    this->Clear();
}

void CStatus::Display() {
}
