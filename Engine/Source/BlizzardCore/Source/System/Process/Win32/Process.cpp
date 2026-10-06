#if defined(WOWDECOMP_ARCH_32)

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <set>
#include <string>
#include <windows.h>
#include <tlhelp32.h>
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/TempBuffer.h"
#include "Engine/Source/BlizzardCore/Source/System/Process/Process.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/BlizzardCore/Source/System/Unicode/Unicode.h"

struct ChildProcess {
    /* 0000 */ HANDLE m_process;
    /* 0004 */ void (*m_callback)(void*);
    /* 0008 */ void* m_param;
};

class ToolhelpFunction {
    public:
    /* 0000 */ bool m_loaded;
    /* 0004 */ Blizzard::Lock::Atomic m_count;
    /* 0008 */ FARPROC m_proc;

    void Load(const char* name);
    bool CallProcessW(HANDLE snapshot, uint32_t* pid, char* name, uint32_t namesize);
    bool CallModuleW(HANDLE snapshot, char* path, uint32_t pathsize);
    bool ProcessFirst(HANDLE snapshot, uint32_t* pid, char* name, uint32_t namesize);
    bool ProcessNext(HANDLE snapshot, uint32_t* pid, char* name, uint32_t namesize);
    bool ModuleFirst(HANDLE snapshot, char* path, uint32_t pathsize);
    bool ModuleNext(HANDLE snapshot, char* path, uint32_t pathsize);
};

// STATIC: #0x00B33FF4
static UINT_PTR s_timer;

// STATIC: #0x00D871B0
static ToolhelpFunction s_process32First;

// STATIC: #0x00D871BC
static ToolhelpFunction s_process32Next;

// STATIC: #0x00D871C8
static ToolhelpFunction s_module32First;

// STATIC: #0x00D871D4
static ToolhelpFunction s_module32Next;

// INLINED
inline void ToolhelpFunction::Load(const char* name) {
    if (!this->m_loaded) {
        if (this->m_count.Increment() == 1) {
            this->m_proc = GetProcAddress(GetModuleHandleA("kernel32.dll"), name);
            this->m_loaded = true;
        } else {
            while (!this->m_loaded) {
                Sleep(1);
            }
        }
    }
}

// OFFSET: %0x0071EB00
bool ToolhelpFunction::CallProcessW(HANDLE snapshot, uint32_t* pid, char* name, uint32_t namesize) {
    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(entry);

    if (!reinterpret_cast<BOOL (WINAPI*)(HANDLE, PROCESSENTRY32W*)>(this->m_proc)(snapshot, &entry)) {
        return false;
    }

    *pid = entry.th32ProcessID;
    Blizzard::Unicode::ConvertUTF16to8(name, namesize, reinterpret_cast<const uint16_t*>(entry.szExeFile), -1, NULL, NULL);
    return true;
}

// OFFSET: %0x0071EB60
bool ToolhelpFunction::CallModuleW(HANDLE snapshot, char* path, uint32_t pathsize) {
    MODULEENTRY32W entry;
    entry.dwSize = sizeof(entry);

    if (!reinterpret_cast<BOOL (WINAPI*)(HANDLE, MODULEENTRY32W*)>(this->m_proc)(snapshot, &entry)) {
        return false;
    }

    Blizzard::Unicode::ConvertUTF16to8(path, pathsize, reinterpret_cast<const uint16_t*>(entry.szExePath), -1, NULL, NULL);
    return true;
}

// OFFSET: %0x0071FEC0
static bool WINAPI Process32FirstA(HANDLE snapshot, uint32_t* pid, char* name, uint32_t namesize) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(entry);

    if (!Process32First(snapshot, &entry)) {
        return false;
    }

    *pid = entry.th32ProcessID;
    System_File::FromNativeName(name, namesize, entry.szExeFile);
    return true;
}

// OFFSET: %0x0071FF20
static bool WINAPI Process32NextA(HANDLE snapshot, uint32_t* pid, char* name, uint32_t namesize) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(entry);

    if (!Process32Next(snapshot, &entry)) {
        return false;
    }

    *pid = entry.th32ProcessID;
    System_File::FromNativeName(name, namesize, entry.szExeFile);
    return true;
}

// OFFSET: %0x0071FF80
static bool WINAPI Module32FirstA(HANDLE snapshot, char* path, uint32_t pathsize) {
    MODULEENTRY32 entry;
    entry.dwSize = sizeof(entry);

    if (!Module32First(snapshot, &entry)) {
        return false;
    }

    System_File::FromNativeName(path, pathsize, entry.szExePath);
    return true;
}

// OFFSET: %0x0071FFD0
static bool WINAPI Module32NextA(HANDLE snapshot, char* path, uint32_t pathsize) {
    MODULEENTRY32 entry;
    entry.dwSize = sizeof(entry);

    if (!Module32Next(snapshot, &entry)) {
        return false;
    }

    System_File::FromNativeName(path, pathsize, entry.szExePath);
    return true;
}

// OFFSET: %0x00722D80
bool ToolhelpFunction::ProcessFirst(HANDLE snapshot, uint32_t* pid, char* name, uint32_t namesize) {
    this->Load("Process32FirstW");
    return this->m_proc ? this->CallProcessW(snapshot, pid, name, namesize) : Process32FirstA(snapshot, pid, name, namesize);
}

// OFFSET: %0x00722E10
bool ToolhelpFunction::ProcessNext(HANDLE snapshot, uint32_t* pid, char* name, uint32_t namesize) {
    this->Load("Process32NextW");
    return this->m_proc ? this->CallProcessW(snapshot, pid, name, namesize) : Process32NextA(snapshot, pid, name, namesize);
}

// OFFSET: %0x00722EA0
bool ToolhelpFunction::ModuleFirst(HANDLE snapshot, char* path, uint32_t pathsize) {
    this->Load("Module32FirstW");
    return this->m_proc ? this->CallModuleW(snapshot, path, pathsize) : Module32FirstA(snapshot, path, pathsize);
}

// OFFSET: %0x00722F20
bool ToolhelpFunction::ModuleNext(HANDLE snapshot, char* path, uint32_t pathsize) {
    this->Load("Module32NextW");
    return this->m_proc ? this->CallModuleW(snapshot, path, pathsize) : Module32NextA(snapshot, path, pathsize);
}

// STATIC: #0x00B38310
static std::set<ChildProcess*> s_childProcesses;

// OFFSET: #0x0043DEC0
static void CALLBACK TimerFunc(HWND hwnd, UINT message, UINT_PTR id, DWORD time) {
    for (std::set<ChildProcess*>::iterator it = s_childProcesses.begin(); it != s_childProcesses.end();) {
        std::set<ChildProcess*>::iterator current = it++;
        ChildProcess* child = *current;

        if (!WaitForSingleObjectEx(child->m_process, 0, FALSE)) {
            child->m_callback(child->m_param);
            s_childProcesses.erase(current);
            delete child;
        }
    }

    if (!s_childProcesses.size()) {
        KillTimer(NULL, s_timer);
        s_timer = 0;
    }
}

// OFFSET: #0x00435C20
bool Blizzard::Process::GetProcessPath(char* buffer, int32_t buffersize) {
    if (!buffer || buffersize <= 0) {
        return false;
    }

    uint16_t filename[MAX_PATH];
    filename[0] = 0;
    GetModuleFileNameW(GetModuleHandleW(NULL), reinterpret_cast<LPWSTR>(filename), MAX_PATH);
    Blizzard::Unicode::ConvertUTF16to8(buffer, buffersize, filename, -1, NULL, NULL);

    return true;
}

// OFFSET: #0x004389C0
bool Blizzard::Process::GetProcessDirectory(char* buffer, int32_t buffersize) {
    Blizzard::Memory::TempBuffer<char, 1024> path(buffersize);

    if (!GetProcessPath(path.m_data, buffersize)) {
        return false;
    }

    if (path.m_data) {
        if (buffer) {
            int32_t length = static_cast<int32_t>(Blizzard::String::FindFilename(path.m_data) - path.m_data + 1);
            Blizzard::String::Copy(buffer, path.m_data, std::min(length, buffersize));
        }
    } else if (buffer && buffersize > 0) {
        *buffer = '\0';
    }

    return true;
}

// OFFSET: #0x00440D80
bool Blizzard::Process::LaunchChildProcessWithArguments(const char* path, const char* arguments, void (*callback)(void*), void* param) {
    if (!arguments) {
        arguments = "";
    }

    std::string command;
    command.push_back('"');
    command.append(path);
    command.push_back('"');

    while (*arguments) {
        size_t length = strcspn(arguments, "\n");
        command.push_back(' ');

        bool quote = Blizzard::String::FindFirstOf(arguments, "\t ", static_cast<int32_t>(length)) != NULL;

        if (quote) {
            command.push_back('"');
        }

        command.append(std::string(arguments, length));

        if (quote) {
            command.push_back('"');
        }

        arguments += length;
        arguments += strspn(arguments, "\n");
    }

    STARTUPINFOW startup;
    PROCESS_INFORMATION info;
    memset(&startup, 0, sizeof(startup));
    memset(&info, 0, sizeof(info));
    startup.cb = sizeof(startup);

    BOOL created;

    {
        Blizzard::StringConversion_CodepageToUTF8::ToNative nativeCommand(command.c_str());
        Blizzard::StringConversion_CodepageToUTF8::ToNative nativePath(path);
        created = CreateProcessW(reinterpret_cast<LPCWSTR>(nativePath.m_data), reinterpret_cast<LPWSTR>(nativeCommand.m_data), NULL, NULL, FALSE, 0, NULL, NULL, &startup, &info);
    }

    if (!created) {
        return false;
    }

    if (!callback) {
        return true;
    }

    if (WaitForInputIdle(info.hProcess, 10000)) {
        return false;
    }

    ChildProcess* child = new ChildProcess;
    child->m_process = info.hProcess;
    child->m_callback = callback;
    child->m_param = param;
    s_childProcesses.insert(child);

    if (!s_timer) {
        s_timer = SetTimer(NULL, 0, 250, TimerFunc);
    }

    return true;
}

// OFFSET: %0x00725BA0
bool Blizzard::Process::EnumProcessesByName(const char* name, void (*callback)(const char*, uint32_t, void*), void* param) {
    if (!name || !callback) {
        return false;
    }

    HANDLE modules = INVALID_HANDLE_VALUE;
    char path[MAX_PATH];
    path[0] = 0;

    HANDLE processes = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (processes == INVALID_HANDLE_VALUE) {
        return false;
    }

    uint32_t pid;

    if (!s_process32First.ProcessFirst(processes, &pid, path, sizeof(path))) {
        CloseHandle(processes);
        return false;
    }

    do {
        if (_stricmp(path, name)) {
            continue;
        }

        modules = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);

        if (modules == INVALID_HANDLE_VALUE || !s_module32First.ModuleFirst(modules, path, sizeof(path))) {
            if (modules != INVALID_HANDLE_VALUE) {
                CloseHandle(modules);
            }

            CloseHandle(processes);
            return false;
        }

        do {
            const char* leaf = path;

            for (const char* curr = path; *curr; curr++) {
                if (*curr == '/' || *curr == '\\') {
                    leaf = curr + 1;
                }
            }

            if (leaf && !_stricmp(leaf, name)) {
                callback(path, pid, param);
            }
        } while (s_module32Next.ModuleNext(modules, path, sizeof(path)));
    } while (s_process32Next.ProcessNext(processes, &pid, path, sizeof(path)));

    if (modules != INVALID_HANDLE_VALUE) {
        CloseHandle(modules);
    }

    CloseHandle(processes);
    return true;
}

// INLINED
void Blizzard::Process::Sleep(uint32_t milliseconds) {
    ::Sleep(milliseconds);
}

#endif
