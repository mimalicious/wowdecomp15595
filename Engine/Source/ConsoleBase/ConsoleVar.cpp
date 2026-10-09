#include <cctype>
#include <cstdint>
#include <cstring>
#include <malloc.h>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00DD8A4C
static char* s_configFile;

// STATIC: 0x00E4D3A0
bool CVar::m_needsSave;

// STATIC: 0x00E4D3A1
bool CVar::m_initialized;

// STATIC: 0x00E4D3A2
bool CVar::s_archiveCodeRegisteredOnly;

// STATIC: 0x00E4D3A3
bool CVar::s_loading;

// STATIC: 0x00E4D3A8/0x140B90750
static TSHashTable<CVar, HASHKEY_STRI> s_registeredCVars;

// OFFSET: 0x00654D80/0x1402F5A50
static int32_t TryCreateDirectory(const char* path) {
    int32_t result = 1;

    if (!path || !*path) {
        return 0;
    }

    const char* separator = SStrChr(path + 1, '\\');

    while (separator) {
        if (separator - path >= 260) {
            return 0;
        }

        char directory[260];
        memcpy(directory, path, separator - path);
        directory[separator - path] = '\0';

        if (!OsCreateDirectory(directory, 0)) {
            uint32_t attributes = OsGetFileAttributes(directory);

            if (attributes == 0xFFFFFFFF || !(attributes & OS_FILE_ATTRIBUTE_DIRECTORY)) {
                return 0;
            }
        }

        separator = SStrChr(separator + 1, '\\');
    }

    return result;
}

// OFFSET: 0x00655A50/0x1402F6DF0
static int32_t CvarListCommandHandler(const char* command, const char* arguments) {
    for (CVar* var = s_registeredCVars.m_fulllist.Head(); var; var = s_registeredCVars.m_fulllist.Next(var)) {
        if (var->m_flags & 0x40) {
            continue;
        }

        char line[256];
        SStrPrintf(line, sizeof(line), "  \"%s\" is \"%s\"", var->m_key.m_str, var->m_stringValue.m_str);

        if (var->m_defaultValue.m_str && SStrCmp(var->m_stringValue.m_str, var->m_defaultValue.m_str, STORM_MAX_STR)) {
            char value[256];
            SStrPrintf(value, sizeof(value), " (default \"%s\")", var->m_defaultValue.m_str);
            SStrPack(line, value, sizeof(line));
        }

        if (var->m_resetValue.m_str && SStrCmp(var->m_stringValue.m_str, var->m_resetValue.m_str, STORM_MAX_STR)) {
            char value[256];
            SStrPrintf(value, sizeof(value), " (reset \"%s\")", var->m_resetValue.m_str);
            SStrPack(line, value, sizeof(line));
        }

        ConsoleWrite(line, DEFAULT_COLOR);
    }

    return 1;
}

// OFFSET: 0x00656010/0x1402F74F0
static int32_t CvarCommandHandler(const char* command, const char* arguments) {
    CVar* var = CVar::Lookup(command);

    if (var->m_flags & 0x40) {
        return 1;
    }

    const char* value = arguments;

    while (*value == ' ') {
        value++;
    }

    if (!*value) {
        const char* current = var->m_stringValue.m_str;

        if (current) {
            ConsolePrintf("CVar \"%s\" is \"%s\"", command, current);

            if (var->m_defaultValue.m_str && SStrCmp(current, var->m_defaultValue.m_str, STORM_MAX_STR)) {
                ConsolePrintf("  default value \"%s\"", var->m_defaultValue.m_str);
            }

            if (var->m_resetValue.m_str && SStrCmp(current, var->m_resetValue.m_str, STORM_MAX_STR)) {
                ConsolePrintf("  reset value \"%s\"", var->m_resetValue.m_str);
            }

            if (var->m_latchedValue.m_str && SStrCmp(current, var->m_latchedValue.m_str, STORM_MAX_STR)) {
                ConsolePrintf("  pending value \"%s\"", var->m_latchedValue.m_str);
            }
        }

        return 1;
    }

    if (var->m_flags & 0x100) {
        ConsolePrintf("%s is read only.", var->m_key.m_str);
        return 1;
    }

    var->Set(value, true, true, false, true);

    return 1;
}

// OFFSET: 0x00656190/0x1402F7670
static int32_t CvarResetCommandHandler(const char* command, const char* arguments) {
    char name[64];
    SStrTokenize(&arguments, name, sizeof(name), " ,;\t\"\r\n", NULL);

    if (name[0]) {
        CVar* var = CVar::Lookup(name);

        if (var && !(var->m_flags & 0x40)) {
            var->Reset();
        } else {
            ConsoleWriteA("No such cvar \"%s\"\n", ERROR_COLOR, name);
        }

        return 1;
    }

    ConsoleWrite("Resetting all cvars\n", DEFAULT_COLOR);

    for (CVar* var = s_registeredCVars.m_fulllist.Head(); var; var = s_registeredCVars.m_fulllist.Next(var)) {
        var->Reset();
    }

    return 1;
}

// OFFSET: 0x00656260/0x1402F7760
static int32_t CvarDefaultCommandHandler(const char* command, const char* arguments) {
    char name[64];
    SStrTokenize(&arguments, name, sizeof(name), " ,;\t\"\r\n", NULL);

    if (name[0]) {
        CVar* var = CVar::Lookup(name);

        if (var && !(var->m_flags & 0x40)) {
            var->Default();
        } else {
            ConsoleWriteA("No such cvar \"%s\"\n", ERROR_COLOR, name);
        }

        return 1;
    }

    ConsoleWrite("Restoring all cvars\n", DEFAULT_COLOR);

    for (CVar* var = s_registeredCVars.m_fulllist.Head(); var; var = s_registeredCVars.m_fulllist.Next(var)) {
        var->Default();
    }

    return 1;
}

// OFFSET: 0x00656DD0
static int32_t SetCommandHandler(const char* command, const char* arguments) {
    char name[64];
    char value[2048];
    SStrTokenize(&arguments, name, sizeof(name), " ,;\t\"\r\n", NULL);
    SStrTokenize(&arguments, value, sizeof(value), " ,;\t\"\r\n", NULL);

    CVar* var = CVar::Lookup(name);

    if (var) {
        if (CVar::s_loading || !(var->m_flags & 0x40) && !(var->m_flags & 0x100)) {
            var->Set(value, true, false, false, true);
        }
    } else {
        CVar::Register(name, "", 0, value, NULL, CATEGORY_DEFAULT, true, NULL, false);
    }

    return 1;
}

// PROVEN: d65a6a36 IDENTICAL/NONE
// OFFSET: 0x00654E50
void CVar::ArchiveCodeRegisteredOnly() {
    CVar::s_archiveCodeRegisteredOnly = true;
}

// OFFSET: 0x00654E60/0x1402F5B50
int32_t CVar::Load(HOSFILE file, bool (*sectionCallback)(const char* section, void* arg), void* arg) {
    int32_t result = 1;
    CVar::s_loading = true;
    uint32_t size = static_cast<uint32_t>(OsGetFileSize(file));
    bool active = true;

    if (!size) {
        CVar::s_loading = false;
        return result;
    }

    char* buffer;
    bool onStack;

    if (size >= 0x2000) {
        buffer = static_cast<char*>(SMemAlloc(size + 1, __FILE__, __LINE__, 0));
        onStack = false;
    } else {
        buffer = static_cast<char*>(alloca(size + 1));
        onStack = true;
    }

    uint32_t bytesRead;

    if (OsReadFile(file, buffer, size, &bytesRead)) {
        buffer[size] = '\0';
        const char* cur = buffer;

        if (cur && bytesRead >= 3 && static_cast<uint8_t>(cur[0]) == 0xEF && static_cast<uint8_t>(cur[1]) == 0xBB && static_cast<uint8_t>(cur[2]) == 0xBF) {
            cur += 3;
        }

        do {
            char line[2048];
            SStrTokenize(&cur, line, sizeof(line), "\r\n", NULL);

            if (line[0] == '[') {
                active = sectionCallback ? sectionCallback(line, arg) : false;
            } else if (active && !_strnicmp(line, "SET ", 4)) {
                ConsoleCommandExecute(line, 0);
            }
        } while (cur && *cur);
    } else {
        result = 0;
    }

    if (!onStack) {
        SMemFree(buffer, __FILE__, __LINE__, 0);
    }

    CVar::s_loading = false;

    return result;
}

// PROVEN: 23f38139 IDENTICAL/NONE
// OFFSET: 0x00654FE0/0x1402F5D10
int32_t CVar::Load(const char* fileName, bool (*sectionCallback)(const char* section, void* arg), void* arg) {
    HOSFILE file = OsCreateFile(fileName, OS_GENERIC_READ, OS_FILE_SHARE_READ, OS_OPEN_EXISTING, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (file == OS_INVALID_FILE) {
        char path[260];
        SStrPrintf(path, sizeof(path), "WTF\\%s", fileName);
        file = OsCreateFile(path, OS_GENERIC_READ, OS_FILE_SHARE_READ, OS_OPEN_EXISTING, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

        if (file == OS_INVALID_FILE) {
            return 0;
        }
    }

    int32_t result = CVar::Load(file, sectionCallback, arg);
    OsCloseFile(file);

    return result;
}

// PROVEN: 55cd9cf4 IDENTICAL/NONE
// OFFSET: 0x0060DAD0
int32_t ConsoleLoadClientCVar(HOSFILE file) {
    return CVar::Load(file, NULL, NULL);
}

// OFFSET: 0x00655080/0x1402F5DE0
int32_t CVar::Delete(const char* fileName) {
    const char* path = fileName;
    char buffer[260];

    if (!OsFileExists(fileName)) {
        SStrPrintf(buffer, sizeof(buffer), "WTF\\%s", fileName);

        if (!OsFileExists(buffer)) {
            return 0;
        }

        path = buffer;
    }

    OsDeleteFile(path);

    return 1;
}

// OFFSET: 0x006550F0/0x1402F5E60
int32_t CVar::SaveCvar(const char* name, const char* value, void* arg) {
    char line[260];
    SStrPrintf(line, sizeof(line), "SET %s \"%s\"\n", name, value);

    uint32_t written = 0;
    OsWriteFile(static_cast<HOSFILE>(arg), line, SStrLen(line), &written);

    return written != 0;
}

// OFFSET: 0x00655160/0x1402F5EF0
int32_t CVar::LogCvar(const char* name, const char* value, void* arg) {
    char line[260];
    SStrPrintf(line, sizeof(line), "SET %s \"%s\"\n", name, value);

    CVarLogBuffer* log = static_cast<CVarLogBuffer*>(arg);
    SStrPack(log->m_buffer, line, log->m_size);

    return 1;
}

// PROVEN: 3b0a414c IDENTICAL/IDENTICAL
// OFFSET: 0x006551F0/0x1402F5F80
void CVar::SetReadOnly(bool readOnly) {
    if (readOnly) {
        this->m_flags |= 0x4;
    } else {
        this->m_flags &= ~0x4;
    }
}

// PROVEN: 2e13077c IDENTICAL/DIFFERENT
// OFFSET: 0x00655320/0x1402F6140
void CVarIntListWrapper::AddNextValue(uint32_t value) {
    this->m_state = 2;

    if (value <= 93 * 93 * 93 - 1 && this->m_pos < 0xFD) {
        this->m_buffer[this->m_pos++] = static_cast<char>(value / (93 * 93) + 35);
        int32_t remainder = value % (93 * 93);
        this->m_buffer[this->m_pos++] = static_cast<char>(remainder / 93 + 35);
        this->m_buffer[this->m_pos++] = static_cast<char>(remainder % 93 + 35);
    }
}

// OFFSET: 0x00655210/0x1402F5F90
void CVar::InternalSet(const char* value, bool setValue, bool setReset, bool setDefault, bool setNeedsSave) {
    bool changed = false;

    if ((this->m_flags & 0x4) || !value) {
        return;
    }

    if (setValue) {
        changed = !this->m_stringValue.m_str || SStrCmpI(value, this->m_stringValue.m_str, STORM_MAX_STR) != 0;

        if (changed) {
            this->m_stringValue.Copy(value);
            this->m_intValue = SStrToInt(value);
            this->m_floatValue = SStrToFloat(value);
        }
    }

    if (setReset && !this->m_resetValue.m_str) {
        changed = true;
        this->m_resetValue.Copy(value);
    }

    if (setDefault && !this->m_defaultValue.m_str) {
        this->m_defaultValue.Copy(value);
    } else if (!changed) {
        return;
    }

    if (setNeedsSave) {
        CVar::m_needsSave = true;
    }
}

// OFFSET: 0x006552C0/0x1402F6080
bool CVarArrayWrapper::RawGetValue(uint32_t index) const {
    const char* str = this->m_cvar->m_stringValue.m_str;

    return *str && index < 2 * (3 * strlen(str) - 3) && (str[index / 6 + 1] & (1 << (index % 6))) != 0;
}

// OFFSET: 0x006553B0/0x1402F61D0
bool CVar::Set(const char* value, bool setValue, bool setReset, bool setDefault, bool setNeedsSave) {
    bool postCallback = false;
    char* oldValue = NULL;

    if (setValue) {
        if (this->m_callback) {
            if (this->m_flags & 0x200) {
                if (this->m_stringValue.m_str) {
                    oldValue = static_cast<char*>(SMemAlloc(SStrLen(this->m_stringValue.m_str) + 1, __FILE__, __LINE__, 0));
                    strcpy(oldValue, this->m_stringValue.m_str);
                }

                postCallback = true;
            } else {
                OsValidateFunctionPointer(reinterpret_cast<const void*>(this->m_callback));

                if (!this->m_callback(this, this->m_stringValue.m_str, value, this->m_arg)) {
                    return true;
                }
            }
        }

        this->m_modified++;

        if (this->m_flags & 0x2) {
            this->m_latchedValue.Copy(value);
            CVar::m_needsSave = true;

            return true;
        }
    }

    this->InternalSet(value, setValue, setReset, setDefault, setNeedsSave);

    if (postCallback) {
        const char* newValue = this->m_stringValue.m_str;
        OsValidateFunctionPointer(reinterpret_cast<const void*>(this->m_callback));
        this->m_callback(this, oldValue, newValue, this->m_arg);

        if (oldValue) {
            SMemFree(oldValue, __FILE__, __LINE__, 0);
        }
    }

    return true;
}

// PROVEN: d63d05b9 IDENTICAL/UNPROVEN
// OFFSET: 0x006554C0/0x1402F6330
bool CVar::Set(int32_t value, bool setValue, bool setReset, bool setDefault, bool setNeedsSave) {
    char buffer[32];
    SStrPrintf(buffer, sizeof(buffer), "%d", value);

    return this->Set(buffer, setValue, setReset, setDefault, setNeedsSave);
}

// OFFSET: 0x00655510/0x1402F63B0
void CVar::Reset() {
    const char* value = this->m_resetValue.m_str;

    if (!value) {
        value = this->m_defaultValue.m_str;
    }

    if (!value) {
        return;
    }

    if (this->m_callback) {
        OsValidateFunctionPointer(reinterpret_cast<const void*>(this->m_callback));

        if (!this->m_callback(this, this->m_stringValue.m_str, value, this->m_arg)) {
            return;
        }
    }

    this->InternalSet(value, true, false, false, true);
}

// OFFSET: 0x006555A0/0x1402F6470
void CVar::Default() {
    const char* value = this->m_defaultValue.m_str;

    if (!value) {
        value = this->m_resetValue.m_str;
    }

    if (!value) {
        return;
    }

    if (this->m_callback) {
        OsValidateFunctionPointer(reinterpret_cast<const void*>(this->m_callback));

        if (!this->m_callback(this, this->m_stringValue.m_str, value, this->m_arg)) {
            return;
        }
    }

    this->InternalSet(value, true, false, false, true);
}

// OFFSET: 0x00655630/0x1402F6530
bool CVar::Update() {
    if (!(this->m_flags & 0x2) || !this->m_latchedValue.m_str) {
        return false;
    }

    this->InternalSet(this->m_latchedValue.m_str, true, false, false, true);
    this->m_latchedValue.Copy(NULL);

    return true;
}

// OFFSET: 0x006556A0/0x1402F65D0
void CVarArrayWrapper::UpdateFromVersion(int32_t version) {
    if (version != 1) {
        return;
    }

    CVar* cvar = this->m_cvar;
    char buffer[256];
    uint32_t out = 1;
    int32_t bits = 0;
    int32_t used = 0;
    const char* in = cvar->m_stringValue.m_str + 1;
    buffer[0] = 2;

    while (*in) {
        if (out >= 0xFF) {
            break;
        }

        if (!bits) {
            buffer[out] = 0;
        }

        buffer[out] = static_cast<char>(((buffer[out] | ((*in & 0x7F) << (bits - used))) & 0x3F) | 0x40);

        if (used > bits + 1) {
            in++;
            bits = bits - used + 7;
            used = 0;
        } else {
            if (used < bits + 1) {
                used += 6 - bits;
            } else {
                in++;
                used = 0;
            }

            out++;
            bits = 0;
        }
    }

    if (bits) {
        buffer[out + 1] = 0;
    } else {
        buffer[out] = 0;
    }

    cvar->Set(buffer, true, false, false, true);
}

// OFFSET: 0x00655770
bool CVarArrayWrapper::RawSetValue(uint32_t index, bool value) {
    const char* str = this->m_cvar->m_stringValue.m_str;
    size_t length = strlen(str);

    if (length >= 0xFF) {
        return false;
    }

    char buffer[256];
    strncpy(buffer, str, sizeof(buffer));

    uint32_t pos = index / 6 + 1;
    buffer[length] = 0;
    buffer[0] = 2;

    if (index / 6 + 2 >= sizeof(buffer)) {
        return false;
    }

    if (value) {
        if (length < pos) {
            memset(&buffer[length], '@', pos - length + 1);
            buffer[pos + 1] = 0;
        }

        buffer[pos] |= 1 << (index % 6);
    } else {
        if (pos > length) {
            return true;
        }

        buffer[pos] &= ~(1 << (index % 6));

        for (size_t i = length - 1; i; i--) {
            if (buffer[i] != '@') {
                break;
            }

            buffer[i] = 0;
        }
    }

    this->m_cvar->Set(buffer, true, false, false, true);

    return true;
}

// OFFSET: 0x006559D0
CVar::CVar() {
}

// OFFSET: 0x006558B0/0x1402F6BF0
void CVarIntListWrapper::UpdateFromVersion(int32_t version) {
    if (version) {
        return;
    }

    const uint8_t* str = reinterpret_cast<const uint8_t*>(this->m_cvar->m_stringValue.m_str);
    int32_t weights[3] = { 220 * 220, 220, 1 };
    int8_t digit = 0;
    uint32_t value = 0;

    while (*str) {
        value += weights[digit++] * (*str - 35);

        if (digit > 2) {
            this->AddNextValue(value);
            value = 0;
            digit = 0;
        }

        str++;
    }

    this->WriteVersionTag();
}

// OFFSET: 0x00655940/0x1402F6CA0
uint32_t CVarIntListWrapper::GetNextValue() {
    if (this->m_pos == 2) {
        uint8_t version = 0;

        if (this->m_cvar && this->m_cvar->m_stringValue.m_str[0] == 'v') {
            version = this->m_cvar->m_stringValue.m_str[1];
        }

        if (version != 1) {
            this->UpdateFromVersion(version);
            this->m_pos = 2;
        }

        this->m_state = 1;
    }

    const char* str = this->m_cvar->m_stringValue.m_str + this->m_pos;

    if (!str[0] || !str[1] || !str[2]) {
        return 0;
    }

    this->m_pos += 3;

    return 93 * (93 * (str[0] - 35) + str[1] - 35) + str[2] - 35;
}

// OFFSET: 0x00655C30/0x1402F7000
int32_t CVar::IterateForArchive(uint32_t category, uint32_t excludeFlags, int32_t (*callback)(const char* name, const char* value, void* arg), void* arg) {
    if (CStringManager::IsDestroyed()) {
        return 1;
    }

    for (CVar* var = s_registeredCVars.m_fulllist.Head(); var; var = s_registeredCVars.m_fulllist.Next(var)) {
        if (!(var->m_flags & 0x1) || (var->m_flags & 0x80) || (var->m_flags & 0x30) != category || (var->m_flags & excludeFlags)) {
            continue;
        }

        const char* value = var->m_latchedValue.m_str;

        if (!value) {
            value = var->m_stringValue.m_str;
        }

        if (!value) {
            value = var->m_defaultValue.m_str;
        }

        if (!value) {
            continue;
        }

        if (((var->m_flags & 0x800) || !var->m_defaultValue.m_str || SStrCmpI(value, var->m_defaultValue.m_str, STORM_MAX_STR)) && !callback(var->m_key.m_str, value, arg)) {
            return 0;
        }
    }

    return 1;
}

// PROVEN: 7265b043 IDENTICAL/NONE
// OFFSET: 0x00655D00
int32_t CVar::SaveAll(HOSFILE file, uint32_t category, uint32_t excludeFlags) {
    return CVar::IterateForArchive(category, excludeFlags, CVar::SaveCvar, file);
}

// OFFSET: 0x00655D20/0x1402F7130
int32_t CVar::SaveAll(const char* fileName) {
    if (!CVar::m_needsSave) {
        return 1;
    }

    if (!fileName || !*fileName) {
        return 0;
    }

    CVar::m_needsSave = false;
    int32_t result = 0;

    char path[260];
    SStrCopy(path, "WTF\\", sizeof(path));
    SStrPack(path, fileName, sizeof(path));

    HOSFILE file = OsCreateFile(path, OS_GENERIC_WRITE, 0, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (file != OS_INVALID_FILE) {
        result = CVar::IterateForArchive(0, 0, CVar::SaveCvar, file);
        OsCloseFile(file);
    }

    return result;
}

// PROVEN: 90586ca4 IDENTICAL/IDENTICAL
// OFFSET: 0x00655E20/0x1402F7290
int32_t CVar::LogAll(char* buffer, uint32_t size, uint32_t category, uint32_t excludeFlags) {
    CVarLogBuffer log;
    log.m_buffer = buffer;
    log.m_size = size;

    return CVar::IterateForArchive(category, excludeFlags, CVar::LogCvar, &log);
}

// OFFSET: 0x00655E50/0x1402F72C0
CVar* CVar::Lookup(const char* name) {
    return name ? s_registeredCVars.Ptr(name) : NULL;
}

// OFFSET: 0x00655E70/0x1402F72E0
CVar* CVar::LookupRegistered(const char* name) {
    if (!name) {
        return NULL;
    }

    CVar* var = s_registeredCVars.Ptr(name);

    if (!var) {
        return NULL;
    }

    if (!(var->m_flags & 0x80000000) && !(var->m_flags & 0x80)) {
        return NULL;
    }

    return var;
}

// OFFSET: 0x00656330
void CVar::DeleteCharacterCVars() {
    CVar* var = s_registeredCVars.m_fulllist.Head();

    while (var) {
        CVar* next = s_registeredCVars.Next(var);

        if ((var->m_flags & 0x1) && (var->m_flags & 0x30) == 0x20) {
            s_registeredCVars.Delete(var);
        }

        var = next;
    }
}

// OFFSET: 0x006563A0
void CVar::DeleteAccountCVars() {
    CVar* var = s_registeredCVars.m_fulllist.Head();

    while (var) {
        CVar* next = s_registeredCVars.Next(var);

        if ((var->m_flags & 0x1) && (var->m_flags & 0x30) == 0x10) {
            s_registeredCVars.Delete(var);
        }

        var = next;
    }
}

// OFFSET: 0x00656410/0x1402F7970
void CVar::Destroy() {
    CVar::m_initialized = false;
    ConsoleCommandUnregister("set");
    ConsoleCommandUnregister("cvar_reset");
    ConsoleCommandUnregister("cvar_default");
    ConsoleCommandUnregister("cvarlist");
    s_registeredCVars.Clear();
}

// OFFSET: 0x00656BF0
CVar* CVar::Register(const char* name, const char* help, uint32_t flags, const char* value, bool (*callback)(CVar*, const char*, const char*, void*), uint32_t category, bool setCommand, void* arg, bool unk) {
    if (!name || !*name || !value) {
        return NULL;
    }

    bool saveValue = (flags >> 11) & 1;
    CVar* var = s_registeredCVars.Ptr(name);

    if (var) {
        bool setReset = var->m_resetValue.m_str == NULL;
        var->m_flags = flags | (var->m_flags & ~0x30);
        bool setDefault = var->m_defaultValue.m_str == NULL;
        var->m_callback = callback;
        var->m_arg = arg;

        bool setValue = false;

        if (callback && !callback(var, var->m_stringValue.m_str, var->m_stringValue.m_str, arg)) {
            setValue = true;
        }

        var->Set(value, setValue, setReset, setDefault, saveValue && setValue);

        if (!setCommand) {
            var->m_flags |= 0x80000000;
        }

        if (unk && !(var->m_flags & 0x80000000)) {
            var->m_flags |= 0x80;
        }

        return var;
    }

    var = s_registeredCVars.New(name, 0, 0);
    var->m_stringValue.Copy(NULL);
    var->m_floatValue = 0.0f;
    var->m_intValue = 0;
    var->m_modified = 0;
    var->m_category = category;
    var->m_defaultValue.Copy(NULL);
    var->m_resetValue.Copy(NULL);
    var->m_latchedValue.Copy(NULL);
    var->m_callback = callback;
    var->m_flags = flags & 0x200;
    var->m_arg = arg;
    var->m_help.Copy(help);

    if (setCommand) {
        var->Set(value, true, true, false, saveValue);
    } else {
        var->Set(value, true, false, true, saveValue);
    }

    var->m_flags |= (flags & 0x400) ? flags : flags | 0x1;

    if (!setCommand) {
        var->m_flags |= 0x80000000;
    }

    if (unk && !(var->m_flags & 0x80000000)) {
        var->m_flags |= 0x80;
    }

    ConsoleCommandRegister(var->m_key.m_str, CvarCommandHandler, static_cast<CATEGORY>(category), help, NULL);

    return var;
}

// OFFSET: 0x00656E90/0x1402F89C0
void CVar::Initialize() {
    CVar::m_initialized = true;

    char path[260];
    SFile::GetBasePath(path, sizeof(path));
    SStrPack(path, "WTF\\", sizeof(path));
    TryCreateDirectory(path);

    ConsoleCommandRegister("set", SetCommandHandler, CATEGORY_DEFAULT, "Set the value of a CVar", NULL);
    ConsoleCommandRegister("cvar_reset", CvarResetCommandHandler, CATEGORY_DEFAULT, "Set the value of a CVar to it's startup value", NULL);
    ConsoleCommandRegister("cvar_default", CvarDefaultCommandHandler, CATEGORY_DEFAULT, "Set the value of a CVar to it's coded default value", NULL);
    ConsoleCommandRegister("cvarlist", CvarListCommandHandler, CATEGORY_DEFAULT, "List cvars", NULL);
}

// PROVEN: 459a1ab2 IDENTICAL/NONE
// OFFSET: 0x0060DA50
int32_t ConsoleInitializeClientCVar(const char* fileName) {
    CVar::Initialize();
    s_configFile = SStrDupA(fileName, __FILE__, __LINE__);
    return CVar::Load(fileName, NULL, NULL);
}

// PROVEN: 8d336341 IDENTICAL/NONE
// OFFSET: 0x0060DA90/0x1402976D0
int32_t ConsoleLoadClientCVar(const char* fileName) {
    return CVar::Load(fileName, NULL, NULL);
}

// PROVEN: 0584b250 IDENTICAL/NONE
// OFFSET: 0x0060DAB0
int32_t ConsoleLoadClientCVarAndSave(const char* fileName) {
    CVar::m_needsSave = true;
    return CVar::Load(fileName, NULL, NULL);
}

// OFFSET: 0x0060DAF0/0x140297710
int32_t ConsoleSaveAllClientCVar() {
    return CVar::SaveAll(s_configFile);
}

// OFFSET: 0x0060DB00/0x140297720
void ConsoleDestroyClientCVar() {
    CVar::SaveAll(s_configFile);
    CVar::Destroy();
    SMemFree(s_configFile, __FILE__, __LINE__, 0);
    s_configFile = NULL;
}

// OFFSET: #0x005300D0
void CVarIntListWrapper::WriteVersionTag() {
    this->m_buffer[1] = 1;
    this->m_buffer[0] = 'v';
    this->m_buffer[this->m_pos] = 0;
    this->m_cvar->Set(this->m_buffer, true, false, false, true);
}

// OFFSET: 0x008C6590
CVarArrayWrapper::CVarArrayWrapper(CVar* cvar, bool defaultValue) {
    this->m_cvar = cvar;
    this->m_defaultValue = defaultValue;

    if (cvar) {
        if (*cvar->m_stringValue.m_str != 2) {
            this->UpdateFromVersion(static_cast<uint8_t>(*cvar->m_stringValue.m_str));
        }
    } else {
        this->UpdateFromVersion(0);
    }
}

// OFFSET: 0x008C1470
bool CVarArrayWrapper::GetValue(uint32_t index) const {
    return this->RawGetValue(index) != static_cast<bool>(this->m_defaultValue);
}

// OFFSET: 0x008C1450
bool CVarArrayWrapper::SetValue(uint32_t index, bool value) {
    return this->RawSetValue(index, value != static_cast<bool>(this->m_defaultValue));
}
