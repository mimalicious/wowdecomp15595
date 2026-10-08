#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_LOG_LOG_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_LOG_LOG_H

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/Thread.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Types.h"
#include "Engine/Source/BlizzardCore/Source/System/Using/Using.h"

namespace Blizzard {
namespace Log {

bool Expand(int32_t logClass, const char* name, char* dst, int32_t count, bool createDirectory);
void Flush(int32_t logClass);
void Printf(int32_t logClass, const char* name, const char* format, ...);
void VPrintf(int32_t logClass, const char* name, const char* format, va_list args);

}
}

namespace System_Log {

class UsingLog : public Blizzard::Using::Use {
    public:
    UsingLog();
};

struct Pref {
    /* 0000 */ const char* m_mode;
    /* 0004 */ char* (*m_preamble)();
    /* 0008 */ char* (*m_dayHeader)();
    /* 000C */ bool m_disabled;
    /* 000D */ bool m_noTimestamp;
    /* 000E */ uint8_t unk_000E[2];
    /* 0010 */ uint32_t m_maxOpenTime;
    /* 0014 */ uint32_t m_maxIdleTime;
    /* 0018 */ char* m_directory;

    Pref& operator=(const Pref& other);
};

struct LogFile {
    /* 0000 */ const char* m_name;
    /* 0004 */ const char* m_path;
    /* 0008 */ FILE* m_file;
    /* 000C */ int32_t m_logClass;
    /* 0010 */ uint32_t m_openTime;
    /* 0014 */ uint32_t m_lastWrite;
    /* 0018 */ bool m_headerWritten;
    /* 0019 */ bool m_binary;
    /* 001A */ uint8_t unk_001A[2];
};

struct LogGlobals {
    /* 0000 */ int32_t m_state;
    /* 0004 */ uint8_t unk_0004[4];
    /* 0008 */ Blizzard::Time::Timestamp m_startTime;
    /* 0010 */ LogFile m_logs[16];
    /* 01D0 */ char* m_stringPool;
    /* 01D4 */ uint32_t m_stringPoolUsed;
    /* 01D8 */ uint32_t m_stringPoolSize;
    /* 01DC */ int32_t m_prefCount;
    /* 01E0 */ Blizzard::Lock::Mutex m_mutex;
    /* 01F8 */ Blizzard::Thread::ThreadRecord* m_thread;
    /* 01FC */ bool m_threadRunning;
    /* 01FD */ bool m_threadDone;
    /* 01FE */ uint8_t unk_01FE[2];
};

extern Pref* s_Prefs;
extern LogGlobals* s_LogGlobals;

bool Acquire(int32_t logClass);
char* DefaultDayHeader();
char* DefaultPreamble();
int32_t FindBest(const char* name, uint32_t now);
void FlushLogs(bool force);
char* GetDayHeader(int32_t logClass);
char* GetPreamble(int32_t logClass);
void Init(int32_t logClass);
const char* Intern(const char* str);
void IVPrintf(bool locked, int32_t logClass, const char* name, const char* format, va_list args);
uint32_t LogThread(void* param);
void PrintTimestamp(FILE* file, Blizzard::Time::Timestamp time);
bool Reopen(int32_t logClass, const char* name, FILE*& file, int32_t& index, bool binary);
void ShutdownProc();

}

#endif
