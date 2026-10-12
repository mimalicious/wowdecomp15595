#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/System/Debug/Debug.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Using/Using.h"

static System_Debug::UsingDebug s_usingDebug;

Blizzard::Debug::AssertCallback System_Debug::s_AssertCallback = Blizzard::Debug::DefaultCallback;

char System_Debug::s_CanonBuffer[1024];

char* System_Debug::s_CanonHead = System_Debug::s_CanonBuffer;

// STATIC: 0x00D07414/0x140BB7F10
char* System_Debug::s_CanonTail = System_Debug::s_CanonBuffer;

// INLINED
void Blizzard::Debug::SetAssertHandler(AssertCallback handler) {
    System_Debug::s_AssertCallback = handler;
}

// OFFSET: 0x007BD220/0x1404B58A0
char* Blizzard::Debug::CanonicalStr(const char* src, char* dst, uint32_t maxchars) {
    char buffer[512];
    Blizzard::String::MakeUnivPath(src, buffer, sizeof(buffer));
    Blizzard::String::Copy(dst, buffer, maxchars);

    for (const char* prefix = System_Debug::s_CanonHead; prefix < System_Debug::s_CanonTail;) {
        const char* current = prefix;
        uint32_t length = prefix ? static_cast<uint32_t>(strlen(prefix)) : 0;
        prefix += length + 1;

        if (current && !_strnicmp(buffer, current, length)) {
            Blizzard::String::Copy(dst, buffer + length, maxchars);
            break;
        }
    }

    return dst;
}

// OFFSET: 0x007AC350
void Blizzard::Debug::StderrOutput(const char* text) {
    if (text) {
        fputs(text, stderr);
    }
}

// INLINED
void Blizzard::Debug::Printf(void (*output)(const char*), const char* format, ...) {
    char buffer[4096];

    if (format) {
        va_list args;
        va_start(args, format);
        vsnoprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        buffer[sizeof(buffer) - 1] = '\0';
    } else {
        buffer[0] = '\0';
    }

    output(buffer);
}

// INLINED
System_Debug::UsingDebug::UsingDebug() : Blizzard::Using::Use("Blizzard::Debug") {
    System_Using::Intern(this->m_name)->shutdownproc = System_Debug::ShutdownProc;
    Blizzard::Using::DependsOn("Blizzard::Debug", "Blizzard::Memory");
}
