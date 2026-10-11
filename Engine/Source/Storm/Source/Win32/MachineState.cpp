#if defined(WOWDECOMP_ARCH_32)

#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include <dbghelp.h>
#include <intrin.h>
#include <tlhelp32.h>
#include <winternl.h>
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/MachineState.h"
#include "Engine/Source/Storm/Source/Win32/Float80.h"

#define CV_SIGNATURE_NB10 0x3031424E
#define CV_SIGNATURE_RSDS 0x53445352

typedef BOOL(WINAPI* SymInitializeProc)(HANDLE, PCSTR, BOOL);
typedef BOOL(WINAPI* SymInitializeWProc)(HANDLE, PCWSTR, BOOL);
typedef BOOL(WINAPI* StackWalk64Proc)(DWORD, HANDLE, HANDLE, LPSTACKFRAME64, PVOID, PREAD_PROCESS_MEMORY_ROUTINE64, PFUNCTION_TABLE_ACCESS_ROUTINE64, PGET_MODULE_BASE_ROUTINE64, PTRANSLATE_ADDRESS_ROUTINE64);
typedef PVOID(WINAPI* SymFunctionTableAccess64Proc)(HANDLE, DWORD64);
typedef BOOL(WINAPI* SymGetLineFromAddr64Proc)(HANDLE, DWORD64, PDWORD, PIMAGEHLP_LINE64);
typedef DWORD64(WINAPI* SymGetModuleBase64Proc)(HANDLE, DWORD64);
typedef BOOL(WINAPI* SymGetModuleInfo64Proc)(HANDLE, DWORD64, PIMAGEHLP_MODULE64);
typedef DWORD(WINAPI* SymGetOptionsProc)();
typedef BOOL(WINAPI* SymGetSymFromAddr64Proc)(HANDLE, DWORD64, PDWORD64, PIMAGEHLP_SYMBOL64);
typedef BOOL(WINAPI* SymCleanupProc)(HANDLE);
typedef DWORD(WINAPI* SymSetOptionsProc)(DWORD);
typedef BOOL(WINAPI* SymEnumerateModules64Proc)(HANDLE, PSYM_ENUMMODULES_CALLBACK64, PVOID);
typedef BOOL(WINAPI* SymEnumerateSymbols64Proc)(HANDLE, ULONG64, PSYM_ENUMSYMBOLS_CALLBACK64, PVOID);
typedef BOOL(WINAPI* MiniDumpWriteDumpProc)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE, PMINIDUMP_EXCEPTION_INFORMATION, PMINIDUMP_USER_STREAM_INFORMATION, PMINIDUMP_CALLBACK_INFORMATION);

class CDbgHelpDll {
    public:
    /* 0000 */ HINSTANCE hInstance;
    /* 0004 */ uint32_t loadCount;
    /* 0008 */ SymInitializeProc SymInitialize;
    /* 000C */ SymInitializeWProc SymInitializeW;
    /* 0010 */ StackWalk64Proc StackWalk64;
    /* 0014 */ SymFunctionTableAccess64Proc SymFunctionTableAccess64;
    /* 0018 */ SymGetLineFromAddr64Proc SymGetLineFromAddr64;
    /* 001C */ SymGetModuleBase64Proc SymGetModuleBase64;
    /* 0020 */ SymGetModuleInfo64Proc SymGetModuleInfo64;
    /* 0024 */ SymGetOptionsProc SymGetOptions;
    /* 0028 */ SymGetSymFromAddr64Proc SymGetSymFromAddr64;
    /* 002C */ SymCleanupProc SymCleanup;
    /* 0030 */ SymSetOptionsProc SymSetOptions;
    /* 0034 */ SymEnumerateModules64Proc SymEnumerateModules64;
    /* 0038 */ SymEnumerateSymbols64Proc SymEnumerateSymbols64;
    /* 003C */ MiniDumpWriteDumpProc MiniDumpWriteDump;

    int32_t Load();
    BOOL InitializeW(HANDLE process, PCWSTR path, BOOL invade);
    void Unload();
};

struct LDRMODULEENTRY {
    /* 0000 */ LIST_ENTRY inLoadOrderLinks;
    /* 0008 */ LIST_ENTRY inMemoryOrderLinks;
    /* 0010 */ LIST_ENTRY inInitializationOrderLinks;
    /* 0018 */ void* dllBase;
    /* 001C */ void* entryPoint;
    /* 0020 */ uint32_t sizeOfImage;
    /* 0024 */ UNICODE_STRING fullDllName;
};

struct LDRMODULELIST {
    /* 0000 */ uint32_t length;
    /* 0004 */ uint8_t initialized;
    /* 0005 */ uint8_t pad[3];
    /* 0008 */ void* ssHandle;
    /* 000C */ LIST_ENTRY inLoadOrderModuleList;
    /* 0014 */ LIST_ENTRY inMemoryOrderModuleList;
};

struct LOADEDMODULE {
    /* 0000 */ char name[256];
    /* 0100 */ void* base;
    /* 0104 */ uint32_t size;
};

struct MEMDUMP {
    /* 0000 */ void (*logLineProc)(void*, const char*, ...);
    /* 0004 */ void* logLineProcParam;
};

struct MiniDumpParam {
    /* 0000 */ void* logfile;
    /* 0004 */ EXCEPTION_POINTERS* exceptionPointers;
    /* 0008 */ uint32_t threadId;
    /* 000C */ int32_t result;
    /* 0010 */ uint32_t userStringCount;
    /* 0014 */ const char** userStrings;
};

struct MODULEDEBUGINFO {
    /* 0000 */ char pdbName[64];
    /* 0040 */ GUID guid;
    /* 0050 */ uint32_t age;
    /* 0054 */ uint32_t signature;
    /* 0058 */ uint32_t checksum;
    /* 005C */ uint32_t timeDateStamp;
};

static char s_monthNames[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

int32_t g_memFullError;

// STATIC: 0x00DA6028
static CRITICAL_SECTION s_CrawlCritsect;

// STATIC: 0x00DA6040
static int32_t s_miniDumpWithDataSegs;

// STATIC: 0x00DA6044
static LONG sgRecursionLevel;

// STATIC: 0x00DA6048
static CDbgHelpDll sgDbgHelpDll;

// OFFSET: 0x004BEDD0/0x1400F8CA0
int32_t CDbgHelpDll::Load() {
    ++this->loadCount;

    if (this->hInstance) {
        return 1;
    }

    InitializeCriticalSection(&s_CrawlCritsect);
    EnterCriticalSection(&s_CrawlCritsect);

    HINSTANCE module = GetModuleHandleW(L"dbghelp.dll");

    if (!module) {
        module = LoadLibraryW(L"dbghelp.dll");
    }

    if (module
        && (this->StackWalk64 = reinterpret_cast<StackWalk64Proc>(GetProcAddress(module, "StackWalk64")))
        && (this->SymFunctionTableAccess64 = reinterpret_cast<SymFunctionTableAccess64Proc>(GetProcAddress(module, "SymFunctionTableAccess64")))
        && (this->SymGetLineFromAddr64 = reinterpret_cast<SymGetLineFromAddr64Proc>(GetProcAddress(module, "SymGetLineFromAddr64")))
        && (this->SymGetModuleBase64 = reinterpret_cast<SymGetModuleBase64Proc>(GetProcAddress(module, "SymGetModuleBase64")))
        && (this->SymGetModuleInfo64 = reinterpret_cast<SymGetModuleInfo64Proc>(GetProcAddress(module, "SymGetModuleInfo64")))
        && (this->SymGetOptions = reinterpret_cast<SymGetOptionsProc>(GetProcAddress(module, "SymGetOptions")))
        && (this->SymGetSymFromAddr64 = reinterpret_cast<SymGetSymFromAddr64Proc>(GetProcAddress(module, "SymGetSymFromAddr64")))
        && (this->SymInitialize = reinterpret_cast<SymInitializeProc>(GetProcAddress(module, "SymInitialize")))
        && ((this->SymInitializeW = reinterpret_cast<SymInitializeWProc>(GetProcAddress(module, "SymInitializeW"))), (this->SymCleanup = reinterpret_cast<SymCleanupProc>(GetProcAddress(module, "SymCleanup"))))
        && (this->SymSetOptions = reinterpret_cast<SymSetOptionsProc>(GetProcAddress(module, "SymSetOptions")))
        && (this->SymEnumerateModules64 = reinterpret_cast<SymEnumerateModules64Proc>(GetProcAddress(module, "SymEnumerateModules64")))
        && (this->SymEnumerateSymbols64 = reinterpret_cast<SymEnumerateSymbols64Proc>(GetProcAddress(module, "SymEnumerateSymbols64")))) {
        this->MiniDumpWriteDump = reinterpret_cast<MiniDumpWriteDumpProc>(GetProcAddress(module, "MiniDumpWriteDump"));
        this->hInstance = module;
        LeaveCriticalSection(&s_CrawlCritsect);

        return 1;
    }

    LeaveCriticalSection(&s_CrawlCritsect);
    DeleteCriticalSection(&s_CrawlCritsect);

    return 0;
}

// OFFSET: 0x004BEF40
BOOL CDbgHelpDll::InitializeW(HANDLE process, PCWSTR path, BOOL invade) {
    if (this->SymInitializeW) {
        return this->SymInitializeW(process, path, invade);
    }

    int32_t length = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS | WC_COMPOSITECHECK | WC_DEFAULTCHAR, path, -1, NULL, 0, NULL, NULL);
    char* narrow = static_cast<char*>(alloca(length));

    if (WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS | WC_COMPOSITECHECK | WC_DEFAULTCHAR, path, -1, narrow, length, NULL, NULL) != length) {
        return FALSE;
    }

    return this->SymInitialize(process, narrow, invade);
}

// INLINED
inline void CDbgHelpDll::Unload() {
    --this->loadCount;
}

// PROVEN: d0d70864 IDENTICAL/NONE
// OFFSET: 0x004BEFE0
static void sLogSeparatorLine(void (*logLineProc)(void*, const char*, ...), void* logLineProcParam, char character, int32_t longLine) {
    char line[80];
    uint32_t length = longLine ? 78 : 40;
    memset(line, character, length);
    line[length] = 0;
    logLineProc(logLineProcParam, "%s", line);
}

// PROVEN: 2c874ace IDENTICAL/NONE
// OFFSET: 0x004BF020/0x1400F8FC0
static void sLogHeader(void (*logLineProc)(void*, const char*, ...), void* logLineProcParam, const char* headerString) {
    logLineProc(logLineProcParam, "");
    sLogSeparatorLine(logLineProc, logLineProcParam, '-', 0);
    logLineProc(logLineProcParam, "    %s", headerString);
    sLogSeparatorLine(logLineProc, logLineProcParam, '-', 0);
    logLineProc(logLineProcParam, "");
}

// OFFSET: 0x004BF0B0
static void sLogMemoryHexDump(void (*logLineProc)(void*, const char*, ...), void* logLineProcParam, uint8_t* address, uint32_t numBytes, int32_t alignedLines) {
    char buffer[80];
    uint32_t numLines = numBytes >> 4;
    uint32_t offset = reinterpret_cast<uintptr_t>(address) & 0xF;

    if (alignedLines) {
        memset(buffer, ' ', 78);
        buffer[0] = '*';
        char* marker = &buffer[10 + 3 * offset + (offset >> 2)];
        buffer[78] = 0;
        buffer[2] = '=';
        memcpy(&buffer[4], "addr", 4);
        marker[0] = '*';
        marker[1] = '*';
        buffer[62 + offset] = '*';
        logLineProc(logLineProcParam, "%s", buffer);

        if (offset) {
            address -= offset;
            ++numLines;
        }
    }

    for (; numLines; --numLines) {
        char* text = buffer + sprintf(buffer, "%08X: ", reinterpret_cast<uintptr_t>(address));

        if (IsBadReadPtr(address, 16)) {
            strcpy(text, "<can't read from this address>");
            logLineProc(logLineProcParam, "%s", buffer);
            return;
        }

        text += sprintf(text, "%02X %02X %02X %02X  ", address[0], address[1], address[2], address[3]);
        text += sprintf(text, "%02X %02X %02X %02X  ", address[4], address[5], address[6], address[7]);
        text += sprintf(text, "%02X %02X %02X %02X  ", address[8], address[9], address[10], address[11]);
        text += sprintf(text, "%02X %02X %02X %02X  ", address[12], address[13], address[14], address[15]);

        for (uint32_t i = 0; i < 16; ++i) {
            text[i] = isprint(address[i]) ? address[i] : '.';
        }

        text[16] = 0;
        logLineProc(logLineProcParam, "%s", buffer);
        address += 16;
    }
}

// OFFSET: 0x004BF270
static const char* sGetPathLeaf(const char* path) {
    const char* leaf = NULL;
    const char* slash = strrchr(path, '/');

    if (slash) {
        leaf = slash;
    }

    const char* backslash = strrchr(path, '\\');

    if (backslash && backslash > leaf) {
        leaf = backslash;
    }

    const char* colon = strrchr(path, ':');

    if (colon && colon > leaf) {
        leaf = colon;
    }

    return leaf ? leaf + 1 : path;
}

// PROVEN: 03ee05e1 IDENTICAL/NONE
// OFFSET: 0x004BF2C0
static void sLogExeFile(void (*logLineProc)(void*, const char*, ...), void* logLineProcParam) {
    char exeFullPath[MAX_PATH];
    GetModuleFileNameA(NULL, exeFullPath, sizeof(exeFullPath));
    logLineProc(logLineProcParam, "%-10s%s", "Exe:", exeFullPath);
}

// OFFSET: 0x004BF300
static void sLogDateTimeString(void (*logLineProc)(void*, const char*, ...), void* logLineProcParam, const SYSTEMTIME* time) {
    uint16_t hour = time->wHour;
    char ampm = hour < 12 ? 'A' : 'P';

    if (hour > 12) {
        hour -= 12;
    } else if (!hour) {
        hour = 12;
    }

    logLineProc(logLineProcParam, "%-10s%3s %2d, %4d %2d:%02d:%02d.%03d %cM", "Time:", s_monthNames[time->wMonth - 1], time->wDay, time->wYear, hour, time->wMinute, time->wSecond, time->wMilliseconds, ampm);
}

// PROVEN: 00a12d9d IDENTICAL/NONE
// OFFSET: 0x004BF370
static void sLogUserName(void (*logLineProc)(void*, const char*, ...), void* logLineProcParam) {
    char userName[MAX_PATH];
    DWORD size = 257;

    if (!GetUserNameA(userName, &size)) {
        strcpy(userName, "<unknown>");
    }

    logLineProc(logLineProcParam, "%-10s%s", "User:", userName);
}

// PROVEN: 8953dba4 IDENTICAL/NONE
// OFFSET: 0x004BF3E0
static void sLogComputerName(void (*logLineProc)(void*, const char*, ...), void* logLineProcParam) {
    char computerName[16];
    DWORD size = sizeof(computerName);

    if (!GetComputerNameA(computerName, &size)) {
        strcpy(computerName, "<unknown>");
    }

    logLineProc(logLineProcParam, "%-10s%s", "Computer:", computerName);
}

// PROVEN: 66eaa072 IDENTICAL/NONE
// OFFSET: 0x004BF440
static void sLogMemory(uint32_t logOptions, void (*logLineProc)(void*, const char*, ...), void* logLineProcParam, uint8_t* instructionPointer, uint8_t* stackPointer) {
    sLogHeader(logLineProc, logLineProcParam, "Memory Dump");

    if (logOptions & LOG_OPTION_CODE) {
        logLineProc(logLineProcParam, "Code: %d bytes starting at (%s = %08X)", 16, "EIP", reinterpret_cast<uintptr_t>(instructionPointer));
        logLineProc(logLineProcParam, "");
        sLogMemoryHexDump(logLineProc, logLineProcParam, instructionPointer, 16, 0);
        logLineProc(logLineProcParam, "");
        logLineProc(logLineProcParam, "");
    }

    if (logOptions & LOG_OPTION_STACK) {
        logLineProc(logLineProcParam, "Stack: %d bytes starting at (%s = %08X)", 1024, "ESP", reinterpret_cast<uintptr_t>(stackPointer));
        logLineProc(logLineProcParam, "");
        sLogMemoryHexDump(logLineProc, logLineProcParam, stackPointer, 1024, 1);
        logLineProc(logLineProcParam, "");
        logLineProc(logLineProcParam, "");
    }
}

// PROVEN: b826b12b IDENTICAL/NONE
// OFFSET: 0x004BF4F0/0x1400F95C0
static int __cdecl sModuleCompareProc(const void* elem1, const void* elem2) {
    const LOADEDMODULE* module1 = static_cast<const LOADEDMODULE*>(elem1);
    const LOADEDMODULE* module2 = static_cast<const LOADEDMODULE*>(elem2);

    if (module1->base > module2->base) {
        return 1;
    }

    return module1->base < module2->base ? -1 : 0;
}

// PROVEN: e482edb6 IDENTICAL/NONE
// OFFSET: 0x004BF520/0x1407B59DC
static const char* GetFileNameFromPath(const char* path, size_t maxchars) {
    if (!memchr(path, 0, maxchars)) {
        return "";
    }

    char* leaf = SStrChrR(const_cast<char*>(path), '/');
    char* backslash = SStrChrR(const_cast<char*>(path), '\\');

    if (!leaf || (backslash && leaf < backslash)) {
        leaf = backslash;
    }

    return leaf ? leaf + 1 : path;
}

// OFFSET: 0x004C0050
static void sReadCodeViewInfo(MODULEDEBUGINFO* info, uint32_t size, uint32_t type, const uint8_t* data) {
    if (size < 4) {
        return;
    }

    const uint32_t* header = reinterpret_cast<const uint32_t*>(data);

    if (header[0] == CV_SIGNATURE_NB10) {
        if (size >= 0x14) {
            info->signature = header[2];
            SStrCopy(info->pdbName, GetFileNameFromPath(reinterpret_cast<const char*>(data + 0x10), 64), sizeof(info->pdbName));
            info->age = header[3];
            memset(&info->guid, 0, sizeof(info->guid));
        }
    } else if (header[0] == CV_SIGNATURE_RSDS && size >= 0x1C) {
        SStrCopy(info->pdbName, GetFileNameFromPath(reinterpret_cast<const char*>(data + 0x18), MAX_PATH), sizeof(info->pdbName));
        info->age = header[5];
        memcpy(&info->guid, data + 4, sizeof(info->guid));
    }
}

// PROVEN: 674825ca IDENTICAL/NONE
// OFFSET: 0x004BF570
static void sGetLogicalAddress(void* addr, char* moduleName, uint32_t moduleNameSize, uint32_t* section, uint32_t* offset) {
    lstrcpynA(moduleName, "<unknown>", moduleNameSize);
    *offset = 0;
    *section = 0;

    MEMORY_BASIC_INFORMATION memInfo;

    if (!VirtualQuery(addr, &memInfo, sizeof(memInfo))) {
        return;
    }

    HMODULE module = static_cast<HMODULE>(memInfo.AllocationBase);

    if (!module) {
        module = GetModuleHandleA(NULL);
    }

    if (!GetModuleFileNameA(module, moduleName, moduleNameSize)) {
        lstrcpynA(moduleName, "<unknown>", moduleNameSize);
        return;
    }

    if (!module) {
        return;
    }

    const IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(module);

    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE || !dosHeader->e_lfanew) {
        return;
    }

    const IMAGE_NT_HEADERS* ntHeaders = reinterpret_cast<const IMAGE_NT_HEADERS*>(reinterpret_cast<const uint8_t*>(module) + dosHeader->e_lfanew);

    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
        return;
    }

    const IMAGE_SECTION_HEADER* sectionHeader = IMAGE_FIRST_SECTION(ntHeaders);
    uint32_t rva = static_cast<uint32_t>(static_cast<uint8_t*>(addr) - reinterpret_cast<uint8_t*>(module));

    for (uint32_t i = 0; i < ntHeaders->FileHeader.NumberOfSections; ++i, ++sectionHeader) {
        uint32_t sectionStart = sectionHeader->VirtualAddress;
        uint32_t sectionLength = sectionHeader->SizeOfRawData > sectionHeader->Misc.VirtualSize ? sectionHeader->SizeOfRawData : sectionHeader->Misc.VirtualSize;

        if (rva >= sectionStart && rva <= sectionStart + sectionLength) {
            *section = i + 1;
            *offset = rva - sectionStart;
            return;
        }
    }
}

// OFFSET: 0x004C0130
static int32_t sDbgHelpGetStackFrameInfo(char* moduleName, DWORD64 address, char* symbolName, DWORD64* symbolDisplacement, char* fileName, uint32_t* lineNumber) {
    int32_t err = 0;

    IMAGEHLP_MODULE64 module;
    memset(&module, 0, sizeof(module));
    module.SizeOfStruct = sizeof(module);

    if (sgDbgHelpDll.SymGetModuleInfo64(GetCurrentProcess(), address, &module)) {
        strcpy(moduleName, sGetPathLeaf(module.ImageName));
    } else {
        strcpy(moduleName, "<unknown module>");
        err = 1;
    }

    *fileName = 0;
    *lineNumber = 0;

    uint8_t buffer[sizeof(IMAGEHLP_SYMBOL64) + 256];
    memset(buffer, 0, sizeof(buffer));

    IMAGEHLP_SYMBOL64* symbol = reinterpret_cast<IMAGEHLP_SYMBOL64*>(buffer);
    symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
    symbol->Address = address;
    symbol->MaxNameLength = 256;

    DWORD64 displacement;

    if (!sgDbgHelpDll.SymGetSymFromAddr64(GetCurrentProcess(), address, &displacement, symbol)) {
        strcpy(symbolName, "<unknown symbol>");
        *symbolDisplacement = 0;

        return err | 4;
    }

    SStrCopy(symbolName, symbol->Name, 256);
    *symbolDisplacement = displacement;

    IMAGEHLP_LINE64 line;
    memset(&line, 0, sizeof(line));
    line.SizeOfStruct = sizeof(line);

    DWORD lineDisplacement;

    if (!sgDbgHelpDll.SymGetLineFromAddr64(GetCurrentProcess(), address, &lineDisplacement, &line)) {
        return err | 2;
    }

    displacement = lineDisplacement;
    strncpy(fileName, sGetPathLeaf(line.FileName), 259);
    fileName[259] = 0;
    *lineNumber = line.LineNumber;

    return err;
}

// INLINED
inline void sLogVerboseMessage(uint32_t logOptions, void (*logLineProc)(void*, const char*, ...), void* logLineProcParam, const char* format, uint32_t error) {
    if (logOptions & LOG_OPTION_VERBOSE) {
        logLineProc(logLineProcParam, format, error);
    }
}

// OFFSET: 0x004C0350
static void sLogDbgHelpStackFrame(uint32_t logOptions, void (*logLineProc)(void*, const char*, ...), void* logLineProcParam, const STACKFRAME64* stackFrame) {
    char symbolName[256];
    char fileName[MAX_PATH];
    char moduleName[32];
    uint32_t lineNumber;
    DWORD64 symbolDisplacement;
    DWORD_PTR address = static_cast<DWORD_PTR>(stackFrame->AddrPC.Offset);
    int32_t err = sDbgHelpGetStackFrameInfo(moduleName, address, symbolName, &symbolDisplacement, fileName, &lineNumber);

    if (err & 1) {
        sLogVerboseMessage(logOptions, logLineProc, logLineProcParam, "**** SymGetModuleInfo() failed, error: %d", GetLastError());
    }

    if (err & 2) {
        sLogVerboseMessage(logOptions, logLineProc, logLineProcParam, "**** SymGetLineFromAddr() failed, error: %d", GetLastError());
    }

    if (err & 4) {
        sLogVerboseMessage(logOptions, logLineProc, logLineProcParam, "**** SymGetSymFromAddr() failed, error: %d", GetLastError());
    }

    if (logOptions & LOG_OPTION_FRAMEPARAMS) {
        const char* format = fileName[0] ? "%08X %-12s %s+%d (%08X,%08X,%08X,%08X) (%s,%d)" : "%08X %-12s %s+%d (%08X,%08X,%08X,%08X)";
        logLineProc(logLineProcParam, format, address, moduleName, symbolName, static_cast<uint32_t>(symbolDisplacement), static_cast<DWORD>(stackFrame->Params[0]), static_cast<DWORD>(stackFrame->Params[1]), static_cast<DWORD>(stackFrame->Params[2]), static_cast<DWORD>(stackFrame->Params[3]), fileName, lineNumber);
    } else {
        const char* format = fileName[0] ? "%08X %-12s %s+%d (%s,%d)" : "%08X %-12s %s+%d";
        logLineProc(logLineProcParam, format, address, moduleName, symbolName, static_cast<uint32_t>(symbolDisplacement), fileName, lineNumber);
    }
}

// OFFSET: 0x004BF670
static BOOL sSymInitialize(HANDLE process) {
    wchar_t path[MAX_PATH];
    path[0] = 0;
    GetModuleFileNameW(NULL, path, MAX_PATH);

    wchar_t* leaf = wcsrchr(path, L'/');
    wchar_t* backslash = wcsrchr(path, L'\\');

    if (!leaf || (backslash && leaf < backslash)) {
        leaf = backslash;
    }

    if (leaf) {
        *leaf = 0;
    }

    return sgDbgHelpDll.InitializeW(process, path, TRUE);
}

// PROVEN: 7fc2c220 IDENTICAL/NONE
// OFFSET: 0x004BF6F0/0x1400F96F0
static void STORMAPI CmdMemOutput(void* hOutput, const MEMREPORTENTRY* entry) {
    MEMDUMP* dump = static_cast<MEMDUMP*>(hOutput);
    dump->logLineProc(dump->logLineProcParam, "%5d %5d %s(%d)", entry->unk_0010, entry->unk_0014, entry->unk_0024, entry->unk_0020 >> 1);
}

// PROVEN: e56e3960 IDENTICAL/NONE
// OFFSET: 0x004BF720
static void sShowOutOfMemory(void (*logLineProc)(void*, const char*, ...), void* logLineProcParam) {
    if (!g_memFullError) {
        return;
    }

    MEMDUMP dump;
    dump.logLineProc = logLineProc;
    dump.logLineProcParam = logLineProcParam;
    SMemGenerateReport(SMEM_REPORT_BY_CALLER, CmdMemOutput, &dump);
    sLogSeparatorLine(logLineProc, logLineProcParam, '-', 1);
}

// INLINED: [0x0077D6CF->0x0077D6E7]
void SMemGenerateReport(SMEMREPORTTYPE type, void (STORMAPI* callback)(void*, const MEMREPORTENTRY*), void* context) {
    StormCallService(STORM_SERVICE_GENERATEREPORT, type, callback, context);
}

// PROVEN: 58205597 IDENTICAL/NONE
// OFFSET: 0x004BF820
static void sEnumerateThreads(DWORD* threads, uint32_t maxThreads, uint32_t* numThreads, uint32_t* totalThreads) {
    *numThreads = 0;
    *totalThreads = 0;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

    if (snapshot != INVALID_HANDLE_VALUE) {
        THREADENTRY32 entry;
        entry.dwSize = sizeof(entry);

        if (Thread32First(snapshot, &entry)) {
            DWORD processId = GetCurrentProcessId();

            while (Thread32Next(snapshot, &entry)) {
                if (entry.th32ThreadID && entry.th32OwnerProcessID == processId) {
                    if (*numThreads < maxThreads) {
                        threads[(*numThreads)++] = entry.th32ThreadID;
                    }

                    ++*totalThreads;
                }
            }
        }

        CloseHandle(snapshot);
    }

    if (!*numThreads) {
        threads[0] = GetCurrentThreadId();
        *numThreads = 1;
        *totalThreads = 1;
    }
}

// OFFSET: 0x004C0490
static void sLogDbgHelpStackFrames(STACKFRAME64* stackFrame, HANDLE process, HANDLE thread, uint32_t logOptions, void (*logLineProc)(void*, const char*, ...), void* logLineProcParam, CONTEXT* context, uint32_t stackFramesToSkip) {
    for (uint32_t frame = 0; frame < 100; ++frame) {
        if (!sgDbgHelpDll.StackWalk64(IMAGE_FILE_MACHINE_I386, process, thread, stackFrame, context, NULL, sgDbgHelpDll.SymFunctionTableAccess64, sgDbgHelpDll.SymGetModuleBase64, NULL)) {
            sLogVerboseMessage(logOptions, logLineProc, logLineProcParam, "**** StackWalk() returned FALSE, error: %d", GetLastError());
            return;
        }

        if (!stackFrame->AddrPC.Offset) {
            sLogVerboseMessage(logOptions, logLineProc, logLineProcParam, "**** StackWalk() returned zero address - skipping stack frame", static_cast<uint32_t>(stackFrame->AddrPC.Offset) | static_cast<uint32_t>(stackFrame->AddrPC.Offset >> 32));
        } else if (frame >= stackFramesToSkip) {
            sLogDbgHelpStackFrame(logOptions, logLineProc, logLineProcParam, stackFrame);
        }
    }
}

// OFFSET: 0x004C0530
static void sLogDbgHelpStackTrace(uint32_t logOptions, void (*logLineProc)(void*, const char*, ...), void* logLineProcParam, const CONTEXT* threadContext, uint32_t stackFramesToSkip) {
    sLogHeader(logLineProc, logLineProcParam, "Stack Trace (Using DBGHELP.DLL)");

    if (!sgDbgHelpDll.Load()) {
        logLineProc(logLineProcParam, "****  Couldn't load DBGHELP.DLL, error: %d", GetLastError());
    } else {
        sgDbgHelpDll.SymSetOptions(sgDbgHelpDll.SymGetOptions() | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
        HANDLE process = GetCurrentProcess();

        if (!sSymInitialize(process)) {
            logLineProc(logLineProcParam, "****  Couldn't initialize Debug Help library, error: %d", GetLastError());
        } else if (logOptions & LOG_OPTION_DBGHELPSTACK) {
            DWORD currentThreadId = GetCurrentThreadId();
            DWORD threads[128];
            uint32_t numThreads = 0;
            uint32_t totalThreads = 0;
            sEnumerateThreads(threads, 128, &numThreads, &totalThreads);
            logLineProc(logLineProcParam, "Showing %d/%d threads...", numThreads, totalThreads);

            for (uint32_t i = 0; i < numThreads; ++i) {
                DWORD threadId = threads[i];
                HANDLE thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, threadId);
                logLineProc(logLineProcParam, "");
                logLineProc(logLineProcParam, threadId == currentThreadId ? "--- Thread ID: %d [Current Thread] ---" : "--- Thread ID: %d ---", threadId);

                if (!thread && (numThreads != 1 || !(thread = GetCurrentThread()))) {
                    logLineProc(logLineProcParam, "**** Unable to gain access to the thread, error: ", GetLastError());
                    continue;
                }

                STACKFRAME64 stackFrame;
                CONTEXT context;
                memset(&stackFrame, 0, sizeof(stackFrame));
                memset(&context, 0, sizeof(context));

                if (threadId == currentThreadId) {
                    memcpy(&context, threadContext, sizeof(context));
                } else {
                    SuspendThread(thread);
                    context.ContextFlags = CONTEXT_FULL;
                    BOOL result = GetThreadContext(thread, &context);
                    ResumeThread(thread);

                    if (!result) {
                        logLineProc(logLineProcParam, "**** Unable to retrieve thread context, error: %d", GetLastError());
                        continue;
                    }
                }

                stackFrame.AddrPC.Offset = context.Eip;
                stackFrame.AddrPC.Mode = AddrModeFlat;
                stackFrame.AddrFrame.Offset = context.Ebp;
                stackFrame.AddrFrame.Mode = AddrModeFlat;
                stackFrame.AddrStack.Offset = context.Esp;
                stackFrame.AddrStack.Mode = AddrModeFlat;

                sLogDbgHelpStackFrames(&stackFrame, process, thread, logOptions, logLineProc, logLineProcParam, &context, threadId != currentThreadId ? 1 : stackFramesToSkip);
                CloseHandle(thread);
            }
        }
    }

    sgDbgHelpDll.Unload();
    logLineProc(logLineProcParam, "");
}

// OFFSET: 0x004BF8D0
static void sLogX86ManualStackTrace(void (*logLineProc)(void*, const char*, ...), void* logLineProcParam, const CONTEXT* threadContext, uint32_t stackFramesToSkip) {
    sLogHeader(logLineProc, logLineProcParam, "Stack Trace (Manual)");
    logLineProc(logLineProcParam, "%s", "Address  Frame    Logical addr  Module");
    logLineProc(logLineProcParam, "");

    DWORD currentThreadId = GetCurrentThreadId();
    DWORD threads[128];
    uint32_t numThreads = 0;
    uint32_t totalThreads = 0;
    sEnumerateThreads(threads, 128, &numThreads, &totalThreads);
    logLineProc(logLineProcParam, "Showing %d/%d threads...", numThreads, totalThreads);

    for (uint32_t i = 0; i < numThreads; ++i) {
        DWORD threadId = threads[i];
        HANDLE thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION, FALSE, threadId);
        logLineProc(logLineProcParam, "");
        logLineProc(logLineProcParam, threadId == currentThreadId ? "--- Thread ID: %d [Current Thread] ---" : "--- Thread ID: %d ---", threadId);

        if (!thread && numThreads == 1) {
            thread = GetCurrentThread();
        }

        DWORD_PTR eip;
        DWORD_PTR* ebp;

        if (threadId == currentThreadId) {
            eip = threadContext->Eip;
            ebp = reinterpret_cast<DWORD_PTR*>(threadContext->Ebp);
        } else {
            SuspendThread(thread);

            CONTEXT context;
            memset(&context, 0, sizeof(context));
            context.ContextFlags = CONTEXT_FULL;
            BOOL result = GetThreadContext(thread, &context);
            ResumeThread(thread);

            if (!result) {
                logLineProc(logLineProcParam, "**** Unable to retrieve thread context, error: %d", GetLastError());
                continue;
            }

            eip = context.Eip;
            ebp = reinterpret_cast<DWORD_PTR*>(context.Ebp);
        }

        for (uint32_t frame = 0; frame < 100; ++frame) {
            if (frame >= (threadId != currentThreadId ? 1 : stackFramesToSkip)) {
                char modulePath[MAX_PATH];
                uint32_t section;
                uint32_t offset;
                sGetLogicalAddress(reinterpret_cast<void*>(eip), modulePath, sizeof(modulePath), &section, &offset);
                logLineProc(logLineProcParam, "%08X %08X %04X:%08X %s", eip, reinterpret_cast<DWORD_PTR>(ebp), section, offset, modulePath);
            }

            if (IsBadWritePtr(ebp, 2 * sizeof(DWORD_PTR))) {
                break;
            }

            eip = ebp[1];
            DWORD_PTR* previous = ebp;
            ebp = reinterpret_cast<DWORD_PTR*>(ebp[0]);

            if (reinterpret_cast<DWORD_PTR>(ebp) & 3) {
                break;
            }

            if (ebp <= previous) {
                break;
            }

            if (IsBadWritePtr(ebp, 2 * sizeof(DWORD_PTR))) {
                break;
            }
        }

        CloseHandle(thread);
    }
}

// OFFSET: 0x004BFB20/0x1400F9E00
static void sLogX86XmmRegisters(void (*logLineProc)(void*, const char*, ...), void* logLineProcParam, const uint8_t* extendedRegisters) {
    for (uint32_t i = 0; i < 8; i += 2) {
        const uint64_t* xmm = reinterpret_cast<const uint64_t*>(extendedRegisters + 0xA0 + 16 * i);
        logLineProc(logLineProcParam, "XMM%-2u=%016llX%016llX  XMM%-2u=%016llX%016llX", i, xmm[1], xmm[0], i + 1, xmm[3], xmm[2]);
    }

    const uint32_t* mxcsr = reinterpret_cast<const uint32_t*>(extendedRegisters + 0x18);
    logLineProc(logLineProcParam, "CSR=%08X  MSK=%08X", mxcsr[0], mxcsr[1]);
    logLineProc(logLineProcParam, "");
}

// OFFSET: 0x004BFBA0
static void sLogX86ContextRegisters(void (*logLineProc)(void*, const char*, ...), void* logLineProcParam, const CONTEXT* context, bool extended) {
    sLogHeader(logLineProc, logLineProcParam, "x86 Registers");
    logLineProc(logLineProcParam, "EAX=%08X  EBX=%08X  ECX=%08X  EDX=%08X  ESI=%08X", context->Eax, context->Ebx, context->Ecx, context->Edx, context->Esi);
    logLineProc(logLineProcParam, "EDI=%08X  EBP=%08X  ESP=%08X  EIP=%08X  FLG=%08X", context->Edi, context->Ebp, context->Esp, context->Eip, context->EFlags);
    logLineProc(logLineProcParam, "CS =%04X      DS =%04X      ES =%04X      SS =%04X      FS =%04X      GS =%04X", context->SegCs, context->SegDs, context->SegEs, context->SegSs, context->SegFs, context->SegGs);
    logLineProc(logLineProcParam, "");

    if (!extended) {
        return;
    }

    if ((context->ContextFlags & CONTEXT_FLOATING_POINT) == CONTEXT_FLOATING_POINT) {
        const Float80* reg = reinterpret_cast<const Float80*>(context->FloatSave.RegisterArea);

        for (uint32_t i = 0; i < 8; ++i) {
            char text[50];
            reg->ToString(text, sizeof(text), 19);
            logLineProc(logLineProcParam, "ST%u=%04X:%016llX  (%s)", i, reg->m_exp & 0x7FFF, *reinterpret_cast<const uint64_t*>(reg), text);
            reg = reinterpret_cast<const Float80*>(reinterpret_cast<const uint8_t*>(reg) + 10);
        }

        logLineProc(logLineProcParam, "CW =%04X      SW =%04X      TAG=%04X", context->FloatSave.ControlWord, context->FloatSave.StatusWord, context->FloatSave.TagWord);
        logLineProc(logLineProcParam, "");
    }

    if ((context->ContextFlags & CONTEXT_EXTENDED_REGISTERS) == CONTEXT_EXTENDED_REGISTERS) {
        sLogX86XmmRegisters(logLineProc, logLineProcParam, context->ExtendedRegisters);
    }
}

// PROVEN: 0083ff3b IDENTICAL/NONE
// OFFSET: 0x004BFD00
void LoadMachineStateSymbols() {
    sgDbgHelpDll.Load();
}

// PROVEN: cb396cd5 IDENTICAL/NONE
// OFFSET: 0x004BFD10
void UnloadMachineStateSymbols() {
    sgDbgHelpDll.Unload();
}

// PROVEN: 801c4405 IDENTICAL/NONE
// OFFSET: 0x004BFD20
void LogComputerInfoHeader(uint32_t logOptions, void (*logLineProc)(void*, const char*, ...), void* logLineProcParam, const char* headerTitleLine, const SYSTEMTIME* time) {
    if (InterlockedIncrement(&sgRecursionLevel) == 1) {
        if (!logOptions || (logOptions & LOG_OPTION_DEFAULT)) {
            logOptions |= LOG_OPTION_EXEFILE | LOG_OPTION_DATETIME | LOG_OPTION_COMPUTERNAME | LOG_OPTION_USERNAME;

            if (headerTitleLine && *headerTitleLine) {
                logOptions |= LOG_OPTION_TITLE;
            }
        }

        sLogSeparatorLine(logLineProc, logLineProcParam, '=', 1);

        if ((logOptions & LOG_OPTION_TITLE) && headerTitleLine && *headerTitleLine) {
            logLineProc(logLineProcParam, "%s", headerTitleLine);

            switch (SErrGetAppPhase()) {
                case 2:
                    logLineProc(logLineProcParam, "[startup]");
                    break;
                case 3:
                    logLineProc(logLineProcParam, "[shutdown]");
                    break;
            }

            logLineProc(logLineProcParam, "");
        }

        if (logOptions & LOG_OPTION_EXEFILE) {
            sLogExeFile(logLineProc, logLineProcParam);
        }

        if (logOptions & LOG_OPTION_DATETIME) {
            SYSTEMTIME currentTime;

            if (!time) {
                GetLocalTime(&currentTime);
                time = &currentTime;
            }

            sLogDateTimeString(logLineProc, logLineProcParam, time);
        }

        if (logOptions & LOG_OPTION_USERNAME) {
            sLogUserName(logLineProc, logLineProcParam);
        }

        if (logOptions & LOG_OPTION_COMPUTERNAME) {
            sLogComputerName(logLineProc, logLineProcParam);
        }

        sLogSeparatorLine(logLineProc, logLineProcParam, '-', 1);
    }

    InterlockedDecrement(&sgRecursionLevel);
}

// OFFSET: 0x004BFE60/0x1400FA330
static DWORD WINAPI MiniDumpThreadProc(void* param) {
    MiniDumpParam* miniDumpParam = static_cast<MiniDumpParam*>(param);

    if (!sgDbgHelpDll.Load()) {
        miniDumpParam->result = 0;
        return 0;
    }

    int32_t result = 0;

    if (sgDbgHelpDll.MiniDumpWriteDump) {
        MINIDUMP_EXCEPTION_INFORMATION miniDumpExceptionInfo;
        miniDumpExceptionInfo.ThreadId = miniDumpParam->threadId;
        miniDumpExceptionInfo.ExceptionPointers = miniDumpParam->exceptionPointers;
        miniDumpExceptionInfo.ClientPointers = FALSE;

        MINIDUMP_USER_STREAM miniDumpUserStreamArray[16];
        uint32_t count = 0;

        for (uint32_t i = 0; i < miniDumpParam->userStringCount; ++i) {
            const char* string = miniDumpParam->userStrings[i];

            if (string && count < 16) {
                miniDumpUserStreamArray[count].Type = count + 0x1000;
                miniDumpUserStreamArray[count].BufferSize = static_cast<ULONG>(strlen(string) + 1);
                miniDumpUserStreamArray[count].Buffer = const_cast<char*>(string);
                ++count;
            }
        }

        MINIDUMP_USER_STREAM_INFORMATION miniDumpUserStreamInfo;
        miniDumpUserStreamInfo.UserStreamCount = count;
        miniDumpUserStreamInfo.UserStreamArray = miniDumpUserStreamArray;
        result = sgDbgHelpDll.MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), miniDumpParam->logfile, s_miniDumpWithDataSegs ? MiniDumpWithDataSegs : MiniDumpWithIndirectlyReferencedMemory, miniDumpParam->exceptionPointers ? &miniDumpExceptionInfo : NULL, &miniDumpUserStreamInfo, NULL);
    }

    sgDbgHelpDll.Unload();
    miniDumpParam->result = result;

    return 0;
}

// PROVEN: a4a1432f IDENTICAL/NONE
// OFFSET: 0x004BFF60/0x1400FA490
int32_t LogMiniDump(void* logfile, EXCEPTION_POINTERS* exceptionpointers, uint32_t count, const char** strings) {
    int32_t result = 0;
    DWORD threadId = 0;

    if (InterlockedIncrement(&sgRecursionLevel) == 1) {
        MiniDumpParam miniDumpParam;
        miniDumpParam.logfile = logfile;
        miniDumpParam.threadId = GetCurrentThreadId();
        miniDumpParam.userStringCount = count;
        miniDumpParam.userStrings = strings;
        miniDumpParam.exceptionPointers = exceptionpointers;

        if (!GetModuleHandleA("psapi.dll")) {
            LoadLibraryA("psapi.dll");
        }

        HANDLE thread = CreateThread(NULL, 0, MiniDumpThreadProc, &miniDumpParam, 0, &threadId);

        if (thread) {
            WaitForSingleObject(thread, INFINITE);
            CloseHandle(thread);
            result = miniDumpParam.result;
        }
    }

    InterlockedDecrement(&sgRecursionLevel);

    return result;
}

// OFFSET: 0x004C0000
int32_t LogMiniDumpIsAvailable() {
    if (!sgDbgHelpDll.Load()) {
        return 0;
    }

    int32_t result = sgDbgHelpDll.MiniDumpWriteDump != NULL;
    sgDbgHelpDll.Unload();

    return result;
}

// OFFSET: [0x004C07F0<IMAGE_NT_HEADERS32>, 0x004C0870<IMAGE_NT_HEADERS64>]
template <class T>
static void sReadDebugDirectory(const T* ntHeaders, MODULEDEBUGINFO* info, const uint8_t* base) {
    info->timeDateStamp = ntHeaders->FileHeader.TimeDateStamp;

    if (ntHeaders->OptionalHeader.NumberOfRvaAndSizes < IMAGE_DIRECTORY_ENTRY_DEBUG + 1) {
        return;
    }

    const IMAGE_DATA_DIRECTORY* directory = &ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];

    if (!directory->VirtualAddress || directory->Size < sizeof(IMAGE_DEBUG_DIRECTORY)) {
        return;
    }

    const IMAGE_DEBUG_DIRECTORY* debugDirectory = reinterpret_cast<const IMAGE_DEBUG_DIRECTORY*>(base + directory->VirtualAddress);
    uint32_t count = directory->Size / sizeof(IMAGE_DEBUG_DIRECTORY);

    for (uint32_t i = 0; i < count; ++i, ++debugDirectory) {
        if (debugDirectory->Type == IMAGE_DEBUG_TYPE_CODEVIEW) {
            sReadCodeViewInfo(info, debugDirectory->SizeOfData, IMAGE_DEBUG_TYPE_CODEVIEW, base + debugDirectory->AddressOfRawData);
            return;
        }
    }
}

// PROVEN: 43405277 IDENTICAL/NONE
// OFFSET: 0x004C08F0
static void sReadModuleDebugInfo(MODULEDEBUGINFO* info, const uint8_t* base) {
    memset(info, 0, sizeof(*info));

    const IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);

    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        return;
    }

    if (dosHeader->e_res2[8] == 0xB74F && dosHeader->e_res2[9] == 0x2D98) {
        info->checksum = dosHeader->e_res2[6] + (dosHeader->e_res2[7] << 16);

        if (info->checksum) {
            info->checksum *= 0xFDCA75BB;
        }
    }

    const IMAGE_NT_HEADERS32* ntHeaders = reinterpret_cast<const IMAGE_NT_HEADERS32*>(base + dosHeader->e_lfanew);

    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
        return;
    }

    switch (ntHeaders->OptionalHeader.Magic) {
        case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
            sReadDebugDirectory(ntHeaders, info, base);
            break;
        case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
            sReadDebugDirectory(reinterpret_cast<const IMAGE_NT_HEADERS64*>(ntHeaders), info, base);
            break;
    }
}

// OFFSET: 0x004C0970/0x1400FAFD0
static void sLogModuleLine(void (*logLineProc)(void*, const char*, ...), void* logLineProcParam, void* base, const char* name, uint32_t size) {
    MODULEDEBUGINFO info;

    __try {
        sReadModuleDebugInfo(&info, static_cast<const uint8_t*>(base));
    } __except (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION || GetExceptionCode() == EXCEPTION_IN_PAGE_ERROR ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {
        memset(&info, 0, sizeof(info));
    }

    char checksum[10];

    if (info.checksum) {
        sprintf_s(checksum, " %08lX", info.checksum);
    } else {
        checksum[0] = 0;
    }

    logLineProc(logLineProcParam, "DBG-MODULE<%08X %08lX \"%s\" \"%s\" %lu {%08lx-%04hx-%04hx-%02x%02x%02x%02x%02x%02x%02x%02x} %lu %lu%s>", reinterpret_cast<uintptr_t>(base), size, name, info.pdbName, info.signature, info.guid.Data1, info.guid.Data2, info.guid.Data3, info.guid.Data4[0], info.guid.Data4[1], info.guid.Data4[2], info.guid.Data4[3], info.guid.Data4[4], info.guid.Data4[5], info.guid.Data4[6], info.guid.Data4[7], info.age, info.timeDateStamp, checksum);
}

// OFFSET: 0x004C0A80/0x1400FB210
static void sLogLoadedModules(void (*logLineProc)(void*, const char*, ...), void* logLineProcParam) {
    LOADEDMODULE modules[256];

    logLineProc(logLineProcParam, "");
    sLogHeader(logLineProc, logLineProcParam, "Loaded Modules");

    size_t numModules = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);

    if (snapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 entry;
        entry.dwSize = sizeof(entry);

        if (Module32First(snapshot, &entry)) {
            do {
                if (numModules == 256) {
                    break;
                }

                LOADEDMODULE* module = &modules[numModules];
                module->base = entry.modBaseAddr;
                module->size = entry.modBaseSize;
                SStrCopy(module->name, entry.szModule, sizeof(module->name));
                ++numModules;
                entry.dwSize = sizeof(entry);
            } while (Module32Next(snapshot, &entry));
        }

        CloseHandle(snapshot);
    } else if (GetModuleHandleW(L"ntdll.dll")) {
        __try {
            uint8_t* peb = static_cast<uint8_t*>(reinterpret_cast<void**>(NtCurrentTeb())[12]);
            LDRMODULELIST* ldr = *reinterpret_cast<LDRMODULELIST**>(peb + 0x0C);
            LIST_ENTRY* head = &ldr->inMemoryOrderModuleList;

            for (LIST_ENTRY* link = head->Flink; link != head && numModules != 256; link = link->Flink) {
                LDRMODULEENTRY* ldrEntry = CONTAINING_RECORD(link, LDRMODULEENTRY, inMemoryOrderLinks);
                LOADEDMODULE* module = &modules[numModules];
                module->base = ldrEntry->dllBase;
                module->size = ldrEntry->sizeOfImage;

                uint32_t length;

                if (SUniConvertUTF16to8(module->name, 255, reinterpret_cast<const uint16_t*>(ldrEntry->fullDllName.Buffer), ldrEntry->fullDllName.Length, &length, NULL) > 0) {
                    length = 0;
                }

                module->name[length] = 0;

                char* leaf = SStrChrR(module->name, '/');
                char* backslash = SStrChrR(module->name, '\\');

                if (backslash && (!leaf || leaf < backslash)) {
                    leaf = backslash;
                }

                if (leaf) {
                    ++leaf;
                }

                memmove(module->name, leaf, module->name + length - leaf + 1);
                ++numModules;
            }
        } __except (EXCEPTION_EXECUTE_HANDLER) {
        }
    }

    qsort(modules, numModules, sizeof(LOADEDMODULE), sModuleCompareProc);

    for (size_t i = 0; i < numModules; ++i) {
        sLogModuleLine(logLineProc, logLineProcParam, modules[i].base, modules[i].name, modules[i].size);
    }

    if (!numModules) {
        logLineProc(logLineProcParam, "****  CreateToolhelp32Snapshot couldn't enumerate modules, error: %d", GetLastError());
    }
}

// PROVEN: e783bbd4 IDENTICAL/NONE
// OFFSET: 0x004BF770/0x1400F97E0
static void sLogMemoryStatus(void (*logLineProc)(void*, const char*, ...), void* logLineProcParam) {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);

    logLineProc(logLineProcParam, "Percent memory used:    %u", status.dwMemoryLoad);
    logLineProc(logLineProcParam, "Total physical memory:  %llu", status.ullTotalPhys);
    logLineProc(logLineProcParam, "Free physical memory:   %llu", status.ullAvailPhys);
    logLineProc(logLineProcParam, "Page file:              %llu", status.ullTotalPageFile);
    logLineProc(logLineProcParam, "Total virtual memory:   %llu", status.ullTotalVirtual);
    logLineProc(logLineProcParam, "Free virtual memory:    %llu", status.ullAvailVirtual);
    sLogSeparatorLine(logLineProc, logLineProcParam, '-', 1);
}

// OFFSET: 0x004C0D30
void LogMachineState(uint32_t logOptions, void (*logLineProc)(void*, const char*, ...), void* logLineProcParam, uint32_t stackFramesToSkip, CONTEXT* context) {
    if (InterlockedIncrement(&sgRecursionLevel) == 1) {
        if (!logOptions || (logOptions & LOG_OPTION_DEFAULT)) {
            logOptions |= LOG_OPTION_DBGHELPSTACK | LOG_OPTION_MANUALSTACK | LOG_OPTION_STACK | LOG_OPTION_MODULES;

            if (context) {
                logOptions |= LOG_OPTION_REGISTERS | LOG_OPTION_CODE;
            }
        }

        CONTEXT localContext;

        if (context) {
            stackFramesToSkip = 0;
        } else {
            memset(&localContext, 0, sizeof(localContext));
            localContext.ContextFlags = CONTEXT_CONTROL | CONTEXT_INTEGER;

            DWORD registerEip;
            DWORD registerEbp;
            DWORD registerEsp;

            __asm {
                call next
            next:
                pop registerEip
                mov registerEbp, ebp
                mov registerEsp, esp
            }

            localContext.Eip = registerEip;
            localContext.Ebp = registerEbp;
            localContext.Esp = registerEsp;
            context = &localContext;
            logOptions &= ~static_cast<uint32_t>(LOG_OPTION_REGISTERS | LOG_OPTION_CODE);
            stackFramesToSkip = stackFramesToSkip + 1;
        }

        if (!(logOptions & LOG_OPTION_REGISTERS)) {
            logOptions &= ~static_cast<uint32_t>(LOG_OPTION_EXTREGISTERS);
        }

        sLogSeparatorLine(logLineProc, logLineProcParam, '-', 1);

        if ((logOptions & LOG_OPTION_REGISTERS) && context) {
            sLogX86ContextRegisters(logLineProc, logLineProcParam, context, (logOptions & LOG_OPTION_EXTREGISTERS) != 0);
        }

        if (logOptions & LOG_OPTION_MANUALSTACK) {
            sLogX86ManualStackTrace(logLineProc, logLineProcParam, context, stackFramesToSkip);
        }

        if (logOptions & LOG_OPTION_DBGHELPSTACK) {
            sLogDbgHelpStackTrace(logOptions, logLineProc, logLineProcParam, context, stackFramesToSkip);
        }

        if (logOptions & LOG_OPTION_MODULES) {
            sLogLoadedModules(logLineProc, logLineProcParam);
        }

        if (logOptions & (LOG_OPTION_CODE | LOG_OPTION_STACK)) {
            sLogMemory(logOptions, logLineProc, logLineProcParam, reinterpret_cast<uint8_t*>(context->Eip), reinterpret_cast<uint8_t*>(context->Esp));
        }

        sLogSeparatorLine(logLineProc, logLineProcParam, '-', 1);
        sShowOutOfMemory(logLineProc, logLineProcParam);
        sLogMemoryStatus(logLineProc, logLineProcParam);
    }

    InterlockedDecrement(&sgRecursionLevel);
}

#endif
