#if defined(WOWDECOMP_ARCH_64)

#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <windows.h>
#include "Engine/Source/Storm/Source/SLog.h"
#include "Engine/Source/Storm/Source/SOption.h"
#include "Engine/Source/Storm/H/storm.h"

static int32_t s_logsysteminit;

static uintptr_t s_sequence;

static char s_defaultdir[MAX_PATH];

static CRITICAL_SECTION s_defaultdir_critsect;

static _LOG* s_loghead[4];

static CRITICAL_SECTION s_critsect[4];

// PROVEN: 22299fbb IDENTICAL/DIFFERENT
// OFFSET: 0x004BDCE0/0x1400F7780
static uint32_t PathGetRootChars(const char* path) {
    if (*path == '/') {
        return 1;
    }

    uint32_t length = SStrLen(path);

    if (length < 2) {
        return 0;
    }

    if (path[1] == ':') {
        return (path[2] == '\\') + 2;
    }

    if (path[0] != '\\' || path[1] != '\\') {
        return 0;
    }

    const char* curr = path + 2;

    for (int32_t i = 0; i < 2; i++) {
        if (curr) {
            curr = SStrChr(curr, '\\') + 1;
        }
    }

    return curr ? static_cast<uint32_t>(curr - path) : length;
}

// PROVEN: 2c36920b IDENTICAL/IDENTICAL
// OFFSET: 0x004BDD70/0x1400F7850
static void FlushLog(_LOG* logptr) {
    if (logptr->bufferused) {
        DWORD byteswritten;
        WriteFile(logptr->file, logptr->buffer, logptr->bufferused, &byteswritten, NULL);
        FlushFileBuffers(logptr->file);
        logptr->bufferused = 0;
        logptr->pendpoint = 0;
    }
}

// OFFSET: 0x004BDDC0/0x1400F78B0
static _LOG* LockLog(HSLOG log, HLOCKEDLOG* lockedhandle, int32_t createifnecessary) {
    if (!log) {
        *lockedhandle = reinterpret_cast<HLOCKEDLOG>(-1);
        return NULL;
    }

    uintptr_t index = reinterpret_cast<uintptr_t>(log) & 3;
    EnterCriticalSection(&s_critsect[index]);
    *lockedhandle = reinterpret_cast<HLOCKEDLOG>(index);

    _LOG** prevptr = &s_loghead[index];

    for (_LOG* logptr = s_loghead[index]; logptr; logptr = logptr->next) {
        if (logptr->log == log) {
            return logptr;
        }

        prevptr = &logptr->next;
    }

    if (!createifnecessary) {
        LeaveCriticalSection(&s_critsect[index]);
        *lockedhandle = reinterpret_cast<HLOCKEDLOG>(-1);
        return NULL;
    }

    _LOG* logptr = static_cast<_LOG*>(VirtualAlloc(NULL, sizeof(_LOG), MEM_COMMIT, PAGE_READWRITE));
    *prevptr = logptr;

    if (!logptr) {
        LeaveCriticalSection(&s_critsect[index]);
        *lockedhandle = reinterpret_cast<HLOCKEDLOG>(-1);
        return NULL;
    }

    logptr->log = log;
    logptr->next = NULL;
    logptr->filename[0] = 0;
    logptr->file = INVALID_HANDLE_VALUE;
    logptr->bufferused = 0;
    logptr->pendpoint = 0;

    return logptr;
}

// PROVEN: c5a12319 IDENTICAL/IDENTICAL
// OFFSET: 0x004BDE80/0x1400F79C0
static void OutputIndent(_LOG* logptr) {
    int32_t indent = logptr->indent;

    if (indent > 0) {
        if (indent >= 128) {
            indent = 128;
        }

        memset(&logptr->buffer[logptr->bufferused], ' ', indent);
        logptr->buffer[logptr->bufferused + indent] = 0;
        logptr->bufferused += indent;
    }
}

// INLINED: [0x00775B2B->0x00775B54]
static void OutputReturn(_LOG* logptr) {
    strcpy(&logptr->buffer[logptr->bufferused], "\r\n");
    logptr->bufferused += 2;
}

// PROVEN: f5a964f6 IDENTICAL/IDENTICAL
// OFFSET: 0x004BDED0/0x1400F7A20
static void OutputTime(_LOG* logptr, int32_t show) {
    static uint32_t lasttime;
    static char timestr[64];
    static uint32_t timestrlen;

    if (!logptr->timeStamp) {
        return;
    }

    DWORD time = GetTickCount();

    if (time != lasttime) {
        lasttime = time;

        SYSTEMTIME systime;
        GetLocalTime(&systime);
        wsprintfA(timestr, "%u/%u %02u:%02u:%02u.%03u  ", systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);
        timestrlen = SStrLen(timestr);
    }

    if (show) {
        memcpy(&logptr->buffer[logptr->bufferused], timestr, timestrlen + 1);
    } else {
        memset(&logptr->buffer[logptr->bufferused], ' ', timestrlen);
        logptr->buffer[logptr->bufferused + timestrlen] = 0;
    }

    logptr->bufferused += timestrlen;
}

// OFFSET: 0x004BDFC0
static void UnlockDeleteLog(_LOG* logptr, HLOCKEDLOG lockedhandle) {
    uintptr_t index = reinterpret_cast<uintptr_t>(lockedhandle);
    _LOG** prevptr = &s_loghead[index];

    for (_LOG* curr = s_loghead[index]; curr; curr = curr->next) {
        if (curr == logptr) {
            *prevptr = curr->next;
            VirtualFree(curr, 0, MEM_RELEASE);
            break;
        }

        prevptr = &curr->next;
    }

    LeaveCriticalSection(&s_critsect[index]);
}

// INLINED
static void UnlockLog(HLOCKEDLOG lockedhandle) {
    LeaveCriticalSection(&s_critsect[reinterpret_cast<uintptr_t>(lockedhandle)]);
}

// OFFSET: 0x004BE1B0/0x1400F7E00
static const char* PrependDefaultDir(char* newfilename, uint32_t newfilenamesize, const char* filename) {
    if (!filename || !*filename || filename[1] == ':' || SStrChr(filename, '\\')) {
        return filename;
    }

    EnterCriticalSection(&s_defaultdir_critsect);

    if (s_defaultdir[0]) {
        size_t chars = SStrCopy(newfilename, s_defaultdir, newfilenamesize);
        SStrCopy(&newfilename[chars], filename, newfilenamesize - chars);
    } else {
        GetModuleFileNameA(GetModuleHandleA(NULL), newfilename, newfilenamesize);

        char* slash = SStrChrR(newfilename, '\\');

        if (slash) {
            *slash = 0;
        }

        SStrPack(newfilename, "\\", newfilenamesize);
        SStrPack(newfilename, filename, newfilenamesize);
    }

    LeaveCriticalSection(&s_defaultdir_critsect);

    return newfilename;
}

// PROVEN: bf8499cd IDENTICAL/DIFFERENT
// OFFSET: 0x004BE020/0x1400F7B30
void STORMAPI SLogClose(HSLOG log) {
    if (!s_logsysteminit) {
        return;
    }

    HLOCKEDLOG lockedhandle;
    _LOG* logptr = LockLog(log, &lockedhandle, 0);

    if (!logptr) {
        return;
    }

    if (logptr->file != INVALID_HANDLE_VALUE) {
        FlushLog(logptr);
        CloseHandle(logptr->file);
    }

    UnlockDeleteLog(logptr, lockedhandle);
}

// OFFSET: 0x004BE080/0x1403E6CA0
void STORMAPI SLogFlush(HSLOG log) {
    HLOCKEDLOG lockedhandle;
    _LOG* logptr = LockLog(log, &lockedhandle, 0);

    if (logptr) {
        if (logptr->file != INVALID_HANDLE_VALUE) {
            FlushLog(logptr);
        }

        UnlockLog(lockedhandle);
    }
}

// PROVEN: 29e66796 DIFFERENT/IDENTICAL
// OFFSET: 0x004BE0D0/0x1400F7CE0
void STORMAPI SLogFlushAll() {
    for (int32_t i = 0; i < 4; i++) {
        EnterCriticalSection(&s_critsect[i]);

        for (_LOG* logptr = s_loghead[i]; logptr; logptr = logptr->next) {
            if (logptr->file != INVALID_HANDLE_VALUE) {
                FlushLog(logptr);
            }
        }

        LeaveCriticalSection(&s_critsect[i]);
    }
}

// INLINED
void STORMAPI SLogInitialize() {
    if (s_logsysteminit) {
        return;
    }

    for (int32_t i = 0; i < 4; i++) {
        InitializeCriticalSection(&s_critsect[i]);
    }

    InitializeCriticalSection(&s_defaultdir_critsect);
    s_logsysteminit = 1;
}

// INLINED: [0x0077565E->0x007756AA]
static void PathStripFilename(char* path) {
    char* backslash = SStrChrR(path, '\\');
    char* slash = SStrChrR(path, '/');
    char* last = backslash > slash ? backslash : slash;

    if (last) {
        char* root = &path[PathGetRootChars(path)];

        if (last >= root) {
            last[1] = 0;
        } else {
            *root = 0;
        }
    }
}

// INLINED: [0x007756BC->0x007756E5]
static void PathConvertSlashes(char* path) {
    for (char* curr = SStrChr(path, '/'); curr; curr = SStrChr(curr + 1, '/')) {
        *curr = '\\';
    }
}

// OFFSET: 0x004BE320/0x1400F8010
static int32_t CreateFileDirectory(const char* path) {
    if (!path) {
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    char buffer[MAX_PATH];
    SStrCopy(buffer, path, sizeof(buffer));
    PathStripFilename(buffer);

    char* dirs = &buffer[PathGetRootChars(buffer)];
    PathConvertSlashes(dirs);

    for (char* curr = SStrChr(dirs, '\\'); curr; curr = SStrChr(curr + 1, '\\')) {
        *curr = 0;
        CreateDirectoryA(buffer, NULL);
        *curr = '\\';
    }

    return CreateDirectoryA(buffer, NULL);
}

// PROVEN: cd4b6f6e DIFFERENT/IDENTICAL
// OFFSET: 0x004BE440/0x1400F8180
static int32_t OpenLogFile(const char* filename, void** file, uint32_t flags) {
    if (!filename || !*filename) {
        *file = INVALID_HANDLE_VALUE;
        return 0;
    }

    char newfilename[MAX_PATH];
    const char* path = PrependDefaultDir(newfilename, sizeof(newfilename), filename);
    int32_t append = (flags & SLOG_FLAG_APPEND) != 0;
    CreateFileDirectory(path);

    *file = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, append ? OPEN_ALWAYS : CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    int32_t result = *file != INVALID_HANDLE_VALUE;

    if (result && append) {
        SetFilePointer(*file, 0, NULL, FILE_END);
    }

    return result;
}

// INLINED
int32_t STORMAPI SLogCreate(const char* filename, uint32_t flags, HSLOG* log) {
    if (!filename || !*filename || !log) {
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    *log = NULL;

    if (flags & SLOG_FLAG_NOFILE) {
        filename = "";
        flags &= ~SLOG_FLAG_OPENFILE;
    }

    void* file = INVALID_HANDLE_VALUE;

    if ((flags & SLOG_FLAG_OPENFILE) && !OpenLogFile(filename, &file, flags)) {
        return 0;
    }

    *log = reinterpret_cast<HSLOG>(++s_sequence);

    HLOCKEDLOG lockedhandle;
    _LOG* logptr = LockLog(*log, &lockedhandle, 1);

    if (!logptr) {
        *log = NULL;
        return 0;
    }

    logptr->file = file;
    SStrCopy(logptr->filename, filename, sizeof(logptr->filename));
    logptr->flags = flags;
    logptr->timeStamp = 1;
    logptr->indent = 0;
    UnlockLog(lockedhandle);

    return 1;
}

// INLINED
void STORMAPI SLogDestroy() {
    SLogFlushAll();

    for (int32_t i = 0; i < 4; i++) {
        EnterCriticalSection(&s_critsect[i]);

        while (s_loghead[i]) {
            char filename[MAX_PATH];
            SStrCopy(filename, s_loghead[i]->filename, sizeof(filename));
            SLogClose(s_loghead[i]->log);
            SErrReportNamedResourceLeak("HSLOG", filename);
        }

        LeaveCriticalSection(&s_critsect[i]);
    }

    s_logsysteminit = 0;

    for (int32_t i = 0; i < 4; i++) {
        DeleteCriticalSection(&s_critsect[i]);
    }

    DeleteCriticalSection(&s_defaultdir_critsect);
}

// INLINED: [0x00775AB0->0x00775ADE]
static int32_t PrepareLog(_LOG* logptr) {
    if (logptr->file != INVALID_HANDLE_VALUE) {
        return 1;
    }

    if (OpenLogFile(logptr->filename, &logptr->file, logptr->flags)) {
        return 1;
    }

    logptr->filename[0] = 0;
    return 0;
}

// INLINED
void STORMAPI SLogVWrite(HSLOG log, const char* format, va_list arglist) {
    HLOCKEDLOG lockedhandle;
    _LOG* logptr = LockLog(log, &lockedhandle, 0);

    if (!logptr) {
        return;
    }

    if (PrepareLog(logptr)) {
        OutputTime(logptr, 1);
        OutputIndent(logptr);
        SStrVPrintf(&logptr->buffer[logptr->bufferused], sizeof(logptr->buffer) - logptr->bufferused, format, arglist);
        logptr->bufferused += SStrLen(&logptr->buffer[logptr->bufferused]);
        OutputReturn(logptr);

        if (g_opt.echotooutputdebugstring) {
            OutputDebugStringA(&logptr->buffer[logptr->pendpoint]);
        }

        logptr->pendpoint = logptr->bufferused;

        if (logptr->bufferused >= 0xC000) {
            FlushLog(logptr);
        }
    }

    UnlockLog(lockedhandle);
}

// PROVEN: 1e7f5ed8 IDENTICAL/IDENTICAL
// OFFSET: 0x004BE8C0/0x1400F8690
void SLogWrite(HSLOG log, const char* format, ...) {
    va_list arglist;
    va_start(arglist, format);
    SLogVWrite(log, format, arglist);
    va_end(arglist);
}

#endif
