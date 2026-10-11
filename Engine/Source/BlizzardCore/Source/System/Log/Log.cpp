#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Log/Log.h"
#include "Engine/Source/BlizzardCore/Source/System/Process/Process.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"
#include "Engine/Source/BlizzardCore/Source/System/Unicode/Unicode.h"
#include "Engine/Source/Storm/H/storm.h"
#include <new>

static System_Log::UsingLog s_usingLog;

// STATIC: 0x00EBA62C
System_Log::Pref* System_Log::s_Prefs;

// STATIC: 0x00EBA630
System_Log::LogGlobals* System_Log::s_LogGlobals;

// INLINED
static System_Log::LogGlobals* GetLogGlobals() {
    if (!System_Log::s_LogGlobals) {
        void* memory = SMemAlloc(sizeof(System_Log::LogGlobals), __FILE__, __LINE__, 0);
        System_Log::s_LogGlobals = memory ? new (memory) System_Log::LogGlobals() : NULL;
    }

    return System_Log::s_LogGlobals;
}

// OFFSET: 0x007B55B0
System_Log::Pref& System_Log::Pref::operator=(const Pref& other) {
    if (&other != this) {
        this->m_mode = other.m_mode;
        this->m_preamble = other.m_preamble;
        this->m_dayHeader = other.m_dayHeader;
        this->m_disabled = other.m_disabled;
        this->m_noTimestamp = other.m_noTimestamp;
        this->m_maxOpenTime = other.m_maxOpenTime;
        this->m_maxIdleTime = other.m_maxIdleTime;
        this->m_directory = NULL;

        if (other.m_directory) {
            this->m_directory = static_cast<char*>(SMemAlloc(strlen(other.m_directory) + 1, __FILE__, __LINE__, 0));
            Blizzard::String::Copy(this->m_directory, other.m_directory, other.m_directory ? static_cast<uint32_t>(strlen(other.m_directory)) + 1 : 1);
        }
    }

    return *this;
}

// PROVEN: e10f01d8 IDENTICAL/NONE
// OFFSET: 0x007AD560
char* System_Log::GetPreamble(int32_t logClass) {
    char* (*preamble)() = s_Prefs[logClass].m_preamble;

    return preamble ? preamble() : s_Prefs[0].m_preamble();
}

// PROVEN: 2d5edc19 IDENTICAL/NONE
// OFFSET: 0x007AD590
char* System_Log::GetDayHeader(int32_t logClass) {
    char* (*dayHeader)() = s_Prefs[logClass].m_dayHeader;

    return dayHeader ? dayHeader() : s_Prefs[0].m_dayHeader();
}

// PROVEN: a980d848 IDENTICAL/IDENTICAL
// OFFSET: 0x007B5660/0x1404AB5C0
void System_Log::PrintTimestamp(FILE* file, Blizzard::Time::Timestamp time) {
    Blizzard::Time::TimeRec rec;
    Blizzard::Time::BreakTime(time, rec);
    fprintf(file, "%02d:%02d:%02d.%04d ", rec.hour, rec.minute, rec.second, rec.nanosecond / 100000);
}

// OFFSET: 0x007B56B0
int32_t System_Log::FindBest(const char* name, uint32_t now) {
    LogFile* logs = s_LogGlobals->m_logs;
    int32_t freeIndex = -1;
    int32_t oldestOpen = -1;
    int32_t oldestClosed = -1;

    for (int32_t i = 0; i < 16; i++) {
        LogFile& log = logs[i];

        if (!log.m_name) {
            if (freeIndex < 0) {
                freeIndex = i;
            }

            continue;
        }

        if (name && !strcmp(log.m_name, name)) {
            if (s_LogGlobals->m_logs[i].m_file || i >= 0) {
                return i;
            }

            break;
        }

        if (log.m_file) {
            if (oldestOpen < 0 || now - log.m_lastWrite > now - logs[oldestOpen].m_lastWrite) {
                oldestOpen = i;
            }
        } else if (oldestClosed < 0 || now - log.m_lastWrite > now - logs[oldestClosed].m_lastWrite) {
            oldestClosed = i;
        }
    }

    if (freeIndex >= 0) {
        return freeIndex;
    }

    int32_t index = oldestClosed >= 0 ? oldestClosed : oldestOpen;

    if (s_LogGlobals->m_logs[index].m_file) {
        fclose(s_LogGlobals->m_logs[index].m_file);
        s_LogGlobals->m_logs[index].m_file = NULL;
    }

    return index;
}

// OFFSET: 0x007BC930/0x1404B4A40
bool Blizzard::Log::Expand(int32_t logClass, const char* name, char* dst, int32_t count, bool createDirectory) {
    Blizzard::Time::TimeRec rec;
    Blizzard::Time::BreakTime(System_Log::s_LogGlobals->m_startTime, rec);

    char path[1024];
    Blizzard::String::MakeUnivPath(name, path, sizeof(path));

    char* directory = path;
    char* filename;
    char* separator = strrchr(path, '/');

    if (separator) {
        *separator = '\0';
        filename = separator + 1;
    } else {
        filename = path;
        directory = NULL;
    }

    const char* defaultDirectory = System_Log::s_Prefs[0].m_directory;
    const char* classDirectory = NULL;

    if (logClass) {
        classDirectory = System_Log::s_Prefs[logClass].m_directory ? System_Log::s_Prefs[logClass].m_directory : System_Log::s_Prefs[0].m_directory;
    }

    char base[512];

    if (directory && Blizzard::File::IsAbsolutePath(directory)) {
        base[0] = '\0';
    } else if (!defaultDirectory) {
        if (classDirectory) {
            Blizzard::String::Copy(base, classDirectory, sizeof(base));
        } else {
            base[0] = '\0';
        }
    } else if (classDirectory) {
        if (Blizzard::File::IsAbsolutePath(defaultDirectory) && !Blizzard::File::IsAbsolutePath(classDirectory)) {
            Blizzard::String::Copy(base, defaultDirectory, sizeof(base));
            Blizzard::String::Append(base, classDirectory, sizeof(base));
        } else {
            Blizzard::String::Copy(base, classDirectory, sizeof(base));
        }
    } else {
        Blizzard::String::Copy(base, defaultDirectory, sizeof(base));
    }

    char* out = dst + Blizzard::String::Copy(dst, base, count);

    if (directory) {
        out += Blizzard::String::Copy(out, directory, static_cast<uint32_t>(count + dst - out));
        *out++ = '/';
    }

    *out = '\0';

    if (createDirectory) {
        Blizzard::File::CreateDirectory(dst, true);
    }

    const char* source = filename;

    if (*source) {
        int32_t year = rec.year;

        do {
            if (out >= dst + count - 20) {
                break;
            }

            char c = *source;

            if (c != '%' || (c = *++source) == '%') {
                *out++ = c;
            } else if (c == 'T') {
                Blizzard::String::Format(out, static_cast<uint32_t>(count + dst - out), "%02d%02d%02d-%02d%02d%02d", year % 100, rec.month, rec.day, rec.hour, rec.minute, rec.second);
                out += out ? strlen(out) : 0;
            } else if (c == 'Y') {
                Blizzard::String::Format(out, static_cast<uint32_t>(count + dst - out), "%02d%02d%02d", year % 100, rec.month, rec.day);
                out += out ? strlen(out) : 0;
            }

            source++;
        } while (*source);
    }

    *out = '\0';
    Blizzard::String::MakeBackslashPath(dst, dst, count);

    return true;
}

// OFFSET: 0x007BCC30/0x1404B4F70
const char* System_Log::Intern(const char* str) {
    LogGlobals* globals = GetLogGlobals();

    if (!globals->m_stringPool) {
        s_LogGlobals->m_stringPool = static_cast<char*>(SMemAlloc(4096, __FILE__, __LINE__, 0));
        s_LogGlobals->m_stringPoolUsed = 0;
        s_LogGlobals->m_stringPoolSize = 4096;
        globals = s_LogGlobals;
    }

    uint32_t length = str ? static_cast<uint32_t>(strlen(str)) : 0;

    if (globals->m_stringPoolUsed + length + 1 > 4096) {
        char* newPool = NULL;

        for (int32_t pass = 2; pass; pass--) {
            globals->m_stringPoolUsed = 0;

            for (int32_t i = 0; i < 16; i++) {
                const char* name = s_LogGlobals->m_logs[i].m_name;
                const char* moved = NULL;

                if (name) {
                    moved = name;
                    uint32_t bytes = static_cast<uint32_t>(strlen(name)) + 1;

                    if (newPool) {
                        moved = newPool + s_LogGlobals->m_stringPoolUsed;
                        Blizzard::String::Copy(newPool + s_LogGlobals->m_stringPoolUsed, name, bytes);
                    }

                    s_LogGlobals->m_stringPoolUsed += bytes;
                }

                s_LogGlobals->m_logs[i].m_name = moved;

                const char* path = s_LogGlobals->m_logs[i].m_path;
                moved = NULL;

                if (path) {
                    moved = path;
                    uint32_t bytes = static_cast<uint32_t>(strlen(path)) + 1;

                    if (newPool) {
                        moved = newPool + s_LogGlobals->m_stringPoolUsed;
                        Blizzard::String::Copy(newPool + s_LogGlobals->m_stringPoolUsed, path, bytes);
                    }

                    s_LogGlobals->m_stringPoolUsed += bytes;
                }

                s_LogGlobals->m_logs[i].m_path = moved;
            }

            for (globals = s_LogGlobals; globals->m_stringPoolUsed > globals->m_stringPoolSize; globals = s_LogGlobals) {
                globals->m_stringPoolSize *= 2;
            }

            if (!newPool) {
                newPool = static_cast<char*>(SMemAlloc(globals->m_stringPoolSize, __FILE__, __LINE__, 0));
                globals = s_LogGlobals;
            }
        }

        SMemFree(globals->m_stringPool, __FILE__, __LINE__, 0);
        s_LogGlobals->m_stringPool = newPool;
        globals = s_LogGlobals;
    }

    char* pool = globals->m_stringPool;

    if (!str) {
        return NULL;
    }

    const char* result = str;
    uint32_t bytes = static_cast<uint32_t>(strlen(str)) + 1;

    if (pool) {
        result = pool + globals->m_stringPoolUsed;
        Blizzard::String::Copy(pool + globals->m_stringPoolUsed, str, bytes);
    }

    globals->m_stringPoolUsed += bytes;

    return result;
}

// OFFSET: 0x007C42C0/0x1404BEC00
bool System_Log::Reopen(int32_t logClass, const char* name, FILE*& file, int32_t& index, bool binary) {
    GetLogGlobals();
    uint32_t now = static_cast<uint32_t>(Blizzard::Time::Milliseconds());
    index = FindBest(name, now);
    FILE* stream = s_LogGlobals->m_logs[index].m_file;

    if (stream) {
        if (s_LogGlobals->m_logs[index].m_binary != binary) {
            fclose(stream);
            stream = NULL;
            s_LogGlobals->m_logs[index].m_binary = binary;
        }

        if (stream) {
            file = s_LogGlobals->m_logs[index].m_file;
            return true;
        }
    }

    char path[1024];

    if (!Blizzard::Log::Expand(logClass, name, path, sizeof(path), true)) {
        return false;
    }

    s_LogGlobals->m_logs[index].m_name = Intern(name);
    s_LogGlobals->m_logs[index].m_path = Intern(path);
    s_LogGlobals->m_logs[index].m_openTime = now;
    s_LogGlobals->m_logs[index].m_lastWrite = now;
    s_LogGlobals->m_logs[index].m_logClass = logClass;

    const char* mode = s_Prefs[logClass].m_mode ? s_Prefs[logClass].m_mode : s_Prefs[0].m_mode;

    if (binary) {
        mode = "ab";
    }

    bool created = true;
    bool exists = *mode == 'a' && !_wfopen_s(&stream, reinterpret_cast<const wchar_t*>(Blizzard::StringConversion_CodepageToUTF8::ToNative(s_LogGlobals->m_logs[index].m_path).m_data), reinterpret_cast<const wchar_t*>(Blizzard::StringConversion_CodepageToUTF8::ToNative("r").m_data));

    if (exists) {
        fclose(stream);
        created = false;
    }

    _wfopen_s(&s_LogGlobals->m_logs[index].m_file, reinterpret_cast<const wchar_t*>(Blizzard::StringConversion_CodepageToUTF8::ToNative(s_LogGlobals->m_logs[index].m_path).m_data), reinterpret_cast<const wchar_t*>(Blizzard::StringConversion_CodepageToUTF8::ToNative(mode).m_data));

    if (!s_LogGlobals->m_logs[index].m_file) {
        file = NULL;
        return false;
    }

    if (s_LogGlobals->m_logs[index].m_headerWritten) {
        if (created) {
            char* header = GetDayHeader(logClass);
            fprintf(s_LogGlobals->m_logs[index].m_file, "%s", header);
            SMemFree(header, __FILE__, __LINE__, 0);
        }
    } else {
        char* header = GetPreamble(logClass);
        fprintf(s_LogGlobals->m_logs[index].m_file, "%s", header);
        s_LogGlobals->m_logs[index].m_headerWritten = true;
        SMemFree(header, __FILE__, __LINE__, 0);
    }

    file = s_LogGlobals->m_logs[index].m_file;

    return true;
}

// OFFSET: 0x007C46E0/0x1404BF100
void System_Log::FlushLogs(bool force) {
    GetLogGlobals();
    uint32_t now = static_cast<uint32_t>(Blizzard::Time::Milliseconds());

    if (force) {
        now -= 86400000;
    }

    for (int32_t i = 0; i < 16; i++) {
        LogFile& log = s_LogGlobals->m_logs[i];

        if (!log.m_file) {
            continue;
        }

        uint32_t maxOpenTime = s_Prefs[log.m_logClass].m_maxOpenTime;

        if (!maxOpenTime) {
            maxOpenTime = s_Prefs[0].m_maxOpenTime;
        }

        if (now - log.m_openTime <= maxOpenTime) {
            uint32_t maxIdleTime = s_Prefs[log.m_logClass].m_maxIdleTime;

            if (!maxIdleTime) {
                maxIdleTime = s_Prefs[0].m_maxIdleTime;
            }

            if (now - log.m_lastWrite <= maxIdleTime) {
                continue;
            }
        }

        fclose(log.m_file);
        s_LogGlobals->m_logs[i].m_file = NULL;
    }
}

// OFFSET: 0x007C6410/0x1404C1650
void System_Log::IVPrintf(bool locked, int32_t logClass, const char* name, const char* format, va_list args) {
    FILE* file;
    int32_t index;

    if (!Reopen(logClass, name, file, index, false)) {
        return;
    }

    Blizzard::Time::Timestamp now = Blizzard::Time::GetTimestamp();

    if (!s_Prefs[logClass].m_noTimestamp && !s_Prefs[0].m_noTimestamp) {
        PrintTimestamp(file, now);
    }

    vfprintf(file, format, args);
    s_LogGlobals->m_logs[index].m_lastWrite = static_cast<uint32_t>(Blizzard::Time::Milliseconds());
}

// OFFSET: 0x007C48C0/0x1404BF260
void System_Log::Init(int32_t logClass) {
    if (s_LogGlobals->m_prefCount > logClass) {
        return;
    }

    if (logClass < 4) {
        logClass = 4;
    }

    Pref* old = s_Prefs;
    uint32_t bytes = sizeof(Pref) * (logClass + 1);
    s_Prefs = static_cast<Pref*>(SMemAlloc(bytes, __FILE__, __LINE__, 0));
    memset(s_Prefs, 0, bytes);

    if (old) {
        Blizzard::String::MemCopy(s_Prefs, old, sizeof(Pref) * s_LogGlobals->m_prefCount);
        SMemFree(old, __FILE__, __LINE__, 0);
    }

    if (!s_LogGlobals->m_prefCount) {
        s_Prefs[0].m_mode = "a";
        s_Prefs[0].m_preamble = DefaultPreamble;
        s_Prefs[0].m_dayHeader = DefaultDayHeader;
        s_Prefs[0].m_disabled = false;
        s_Prefs[0].m_noTimestamp = false;
        s_Prefs[0].m_maxOpenTime = 30000;
        s_Prefs[0].m_maxIdleTime = 15000;

        char directory[MAX_PATH];
        Blizzard::Process::GetProcessDirectory(directory, sizeof(directory));
        s_Prefs[0].m_directory = static_cast<char*>(SMemAlloc(strlen(directory) + 1, __FILE__, __LINE__, 0));
        Blizzard::String::MakeUnivPath(directory, s_Prefs[0].m_directory, static_cast<int32_t>(strlen(directory) + 1));
        s_LogGlobals->m_prefCount++;
    }

    if (s_LogGlobals->m_prefCount == 1) {
        s_Prefs[1] = s_Prefs[0];
        s_Prefs[1].m_maxOpenTime = 1;
        s_LogGlobals->m_prefCount++;
    }

    if (s_LogGlobals->m_prefCount == 2) {
        s_Prefs[2] = s_Prefs[0];
        s_Prefs[2].m_maxOpenTime = 1;
        s_LogGlobals->m_prefCount++;
    }

    if (s_LogGlobals->m_prefCount == 3) {
        s_Prefs[3] = s_Prefs[0];
        s_Prefs[3].m_disabled = true;
        s_LogGlobals->m_prefCount++;
    }

    s_LogGlobals->m_prefCount = logClass + 1;
}

// OFFSET: 0x007E1A00/0x1404E7EB0
void System_Log::ShutdownProc() {
    if (!s_LogGlobals || s_LogGlobals->m_state <= 0) {
        return;
    }

    s_LogGlobals->m_state = -1;
    Blizzard::Thread::ThreadRecord* thread = s_LogGlobals->m_thread;

    if (thread && thread->m_state >= 0 && s_LogGlobals->m_threadRunning) {
        Blizzard::Thread::WaitForever(s_LogGlobals->m_thread);
        Blizzard::Thread::Close(s_LogGlobals->m_thread);
    }

    Blizzard::Lock::MutexEnter(s_LogGlobals->m_mutex);
    Blizzard::Lock::MutexLeave(s_LogGlobals->m_mutex);
    FlushLogs(true);
    Blizzard::Lock::MutexDestroy(s_LogGlobals->m_mutex);

    for (int32_t i = 0; i < s_LogGlobals->m_prefCount; i++) {
        SMemFree(s_Prefs[i].m_directory, __FILE__, __LINE__, 0);
        s_Prefs[i].m_directory = NULL;
    }

    SMemFree(s_Prefs, __FILE__, __LINE__, 0);
    SMemFree(s_LogGlobals->m_stringPool, __FILE__, __LINE__, 0);
    SMemFree(s_LogGlobals, __FILE__, __LINE__, 0);
}

// OFFSET: 0x007EBFA0/0x1404EF270
bool System_Log::Acquire(int32_t logClass) {
    LogGlobals* globals = GetLogGlobals();

    if (globals->m_state < 0) {
        return false;
    }

    if (!globals->m_state) {
        s_LogGlobals->m_startTime = Blizzard::Time::GetTimestamp();
        Blizzard::Lock::MutexCreate(s_LogGlobals->m_mutex);
        s_LogGlobals->m_state = 1;
        s_LogGlobals->m_threadRunning = true;
        Blizzard::Thread::Create(LogThread, NULL, &s_LogGlobals->m_thread, "", 0, 0x10000);
        globals = s_LogGlobals;
    }

    Blizzard::Lock::MutexEnter(globals->m_mutex);
    Init(logClass);

    return true;
}

// OFFSET: 0x007EDEC0/0x1404F5910
void Blizzard::Log::Printf(int32_t logClass, const char* name, const char* format, ...) {
    va_list args;
    va_start(args, format);

    if (System_Log::Acquire(logClass)) {
        if (!System_Log::s_Prefs[logClass].m_disabled && !System_Log::s_Prefs[0].m_disabled) {
            System_Log::IVPrintf(true, logClass, name, format, args);
        }

        Blizzard::Lock::MutexLeave(System_Log::s_LogGlobals->m_mutex);
    }

    va_end(args);
}

// OFFSET: 0x007EDF30
void Blizzard::Log::VPrintf(int32_t logClass, const char* name, const char* format, va_list args) {
    if (System_Log::Acquire(logClass)) {
        if (!System_Log::s_Prefs[logClass].m_disabled && !System_Log::s_Prefs[0].m_disabled) {
            System_Log::IVPrintf(true, logClass, name, format, args);
        }

        Blizzard::Lock::MutexLeave(System_Log::s_LogGlobals->m_mutex);
    }
}

// OFFSET: 0x007EDFA0/0x1404F5990
void Blizzard::Log::Flush(int32_t logClass) {
    if (System_Log::Acquire(logClass)) {
        System_Log::FlushLogs(true);
        Blizzard::Lock::MutexLeave(System_Log::s_LogGlobals->m_mutex);
    }
}

// INLINED
System_Log::UsingLog::UsingLog() : Blizzard::Using::Use("Blizzard::Log") {
    System_Using::Intern(this->m_name)->shutdownproc = System_Log::ShutdownProc;
    Blizzard::Using::DependsOn("Blizzard::Log", "Blizzard::File");
}
