#include <cmath>
#include <cfloat>
#include <cstdint>
#include <cstring>
#include <malloc.h>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/MD5.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lmemPool.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/lua-5.1/src/lualib.h"

// STATIC: 0x00EBEEF8/0x140DB0F70
double s_scriptTimeDivisor;

// STATIC: 0x00EBEF04/0x140DB0F80
FRAMESCRIPT_EVENTTIME* s_eventTimes;

// STATIC: 0x00EBEF08/0x140DB0F7C
uint32_t s_currentEventId;

// STATIC: 0x00D08398/0x140BB9760
int32_t s_eventTimeIndex = -1;

// STATIC: 0x00D0839C/0x140BB9764
static uint32_t s_nextEventId = 1;

// STATIC: 0x00EBEF24/0x140DB0FA8
static uint32_t s_scriptThreadId;

// STATIC: 0x00EBEF0C/0x140DB0F88
static void* s_mempool;

// STATIC: 0x00EBEF10/0x140DB0F90
lua_State* s_context;

// STATIC: 0x00EBEF18/0x140DB0F98
static int64_t s_scriptTimeUsed;

// STATIC: 0x00EBEF14/0x140DB0FA0
static int32_t s_scriptProfileEnabled;

// STATIC: 0x00EBEF20/0x140DB0FA4
int32_t s_inErrorHandler;

// STATIC: 0x00EBEF28/0x140DB0FAC
static PLURAL_RULE s_pluralRule;

// STATIC: 0x00EBEF30/0x140BB9780
TSHashTable<FrameScript_EventObject, HASHKEY_CONSTSTRI> s_scriptEventsHash;

// STATIC: 0x00EBEF5C/0x140DB0FB8
static TSFixedArray<FrameScript_EventObject*> s_scriptEvents;

// STATIC: 0x00EBEF68
static char s_printIndent[128];

// STATIC: 0x00EBEFE8
static char s_printBuffer[1024];

// STATIC: 0x00EBF3E8
static char s_packIndent[128];

// STATIC: 0x00D083A0/0x140BB9768
int32_t s_errorHandlerRef = -1;

// STATIC: 0x00D083A4/0x140BB976C
int32_t s_errorHandlerFunc = -1;

// STATIC: 0x00D083A8/0x140BB9770
static int32_t s_recursiveTableHash = -1;

// STATIC: 0x00D083B0/0x140BB97C8
static const char* s_savedVariablesTaint = "SavedVariables";

static const char* s_compatLua =
    "-------------------------------------------------------------------\n"
    "-- Table library\n"
    "local tab = table\n"
    "foreach = tab.foreach\n"
    "foreachi = tab.foreachi\n"
    "getn = tab.getn\n"
    "tinsert = tab.insert\n"
    "tremove = tab.remove\n"
    "sort = tab.sort\n"
    "wipe = tab.wipe\n"
    "\n"
    "-------------------------------------------------------------------\n"
    "-- math library\n"
    "local math = math\n"
    "abs = math.abs\n"
    "acos = function (x) return math.deg(math.acos(x)) end\n"
    "asin = function (x) return math.deg(math.asin(x)) end\n"
    "atan = function (x) return math.deg(math.atan(x)) end\n"
    "atan2 = function (x,y) return math.deg(math.atan2(x,y)) end\n"
    "ceil = math.ceil\n"
    "cos = function (x) return math.cos(math.rad(x)) end\n"
    "deg = math.deg\n"
    "exp = math.exp\n"
    "floor = math.floor\n"
    "frexp = math.frexp\n"
    "ldexp = math.ldexp\n"
    "log = math.log\n"
    "log10 = math.log10\n"
    "max = math.max\n"
    "min = math.min\n"
    "mod = math.fmod\n"
    "PI = math.pi\n"
    "--??? pow = math.pow\n"
    "rad = math.rad\n"
    "random = math.random\n"
    "--randomseed = math.randomseed\n"
    "sin = function (x) return math.sin(math.rad(x)) end\n"
    "sqrt = math.sqrt\n"
    "tan = function (x) return math.tan(math.rad(x)) end\n"
    "\n"
    "-------------------------------------------------------------------\n"
    "-- string library\n"
    "local str = string\n"
    "strbyte = str.byte\n"
    "strchar = str.char\n"
    "strfind = str.find\n"
    "format = str.format\n"
    "gmatch = str.gmatch\n"
    "gsub = str.gsub\n"
    "strlen = str.len\n"
    "strlower = str.lower\n"
    "strmatch = str.match\n"
    "strrep = str.rep\n"
    "strrev = str.reverse\n"
    "strsub = str.sub\n"
    "strupper = str.upper\n"
    "-------------------------------------------------------------------\n"
    "-- Add custom string functions to the string table\n"
    "str.trim = strtrim\n"
    "str.split = strsplit\n"
    "str.join = strjoin\n";

// PROVEN: c58de60e IDENTICAL/NONE
// OFFSET: 0x00839900
static int32_t IsNotFinite(double value) {
    return !std::isfinite(value);
}

// PROVEN: ce873ede IDENTICAL/DIFFERENT
// OFFSET: 0x0083A0C0/0x140564620
int32_t FrameScript_HookWrapper(lua_State* L) {
    int32_t top = lua_gettop(L);

    for (int32_t i = 0; i < top; i++) {
        lua_pushvalue(L, i + 1);
    }

    lua_pushvalue(L, lua_upvalueindex(1));
    lua_insert(L, top + 1);
    lua_call(L, top, LUA_MULTRET);

    int32_t results = lua_gettop(L) - top;

    for (int32_t i = 0; i < results; i++) {
        lua_insert(L, 1);
    }

    ++lua_taintexpected;
    const char* taint = lua_tainted;

    lua_pushvalue(L, lua_upvalueindex(2));
    lua_insert(L, results + 1);
    lua_rawgeti(L, LUA_REGISTRYINDEX, s_errorHandlerRef);
    lua_insert(L, results + 1);

    if (lua_pcall(L, top, 0, results + 1)) {
        lua_pop(L, 2);
    } else {
        lua_pop(L, 1);
    }

    if (lua_taintexpected && !lua_taintedclosure) {
        lua_tainted = taint;
    }

    if (--lua_taintexpected <= 0) {
        lua_taintexpected = 0;
    }

    return results;
}

// PROVEN: 0610bf15 IDENTICAL/IDENTICAL
// OFFSET: 0x0083AD30/0x140565440
void FrameScript_MemoryCleanup() {
    lua_gc(s_context, LUA_GCRESTART, 0);
}

// PROVEN: 9f6c1499 IDENTICAL/IDENTICAL
// OFFSET: 0x0083AD50/0x140565460
void FrameScript_SetPluralRule(PLURAL_RULE rule) {
    s_pluralRule = rule;
}

// OFFSET: 0x0083AD60/0x140565470
uint32_t FrameScript_GetPluralIndex(int32_t count) {
    switch (s_pluralRule) {
        case PLURAL_RULE_0:
            return count != 1;

        case PLURAL_RULE_1:
            return count > 1;

        case PLURAL_RULE_2:
            if (count % 100 >= 11 && count % 100 <= 14) {
                return 2;
            }

            if (count % 10 == 1) {
                return 0;
            }

            if (count % 10 >= 2 && count % 10 <= 4) {
                return 1;
            }

            return 2;

        default:
            return 0;
    }
}

// PROVEN: 4eeceae9 IDENTICAL/IDENTICAL
// OFFSET: 0x0083ADF0/0x140565500
void SetDecimalConversion(int32_t convert) {
    lua_setdecimalconversion(convert);
}

// PROVEN: d891cbfe IDENTICAL/IDENTICAL
// OFFSET: 0x0083AE00/0x140565510
lua_State* FrameScript_GetContext() {
    return s_context;
}

// PROVEN: a2414502 IDENTICAL/IDENTICAL
// OFFSET: 0x0083AE10/0x140565520
int32_t FrameScript_GetTotalMemoryUsed() {
    return lua_gc(s_context, LUA_GCCOUNT, 0);
}

// OFFSET: 0x0083AE30/0x140565540
const char* FrameScript_GetCurrentObject(lua_State* L, int32_t level) {
    lua_Debug ar;

    if (!lua_getstack(L, level, &ar)) {
        return NULL;
    }

    lua_getinfo(L, "Sln", &ar);

    if ((*ar.source != '*' && SStrCmp(ar.namewhat, "method", STORM_MAX_STR)) || !lua_getlocal(L, &ar, 1)) {
        return NULL;
    }

    if (lua_type(L, -1) == LUA_TTABLE) {
        lua_rawgeti(L, -1, 0);
        FrameScript_Object* object = static_cast<FrameScript_Object*>(lua_touserdata(L, -1));
        lua_pop(L, 1);

        if (object) {
            const char* name = object->GetName();
            lua_pop(L, 1);

            return name ? name : "<unnamed>";
        }
    }

    lua_pop(L, 1);

    return NULL;
}

// OFFSET: 0x0083AF50/0x140565660
char* FrameScript_GetCurrentFunction(lua_State* L, char* buffer, uint32_t bufferLen) {
    lua_Debug ar;
    memset(&ar, 0, sizeof(ar));

    if (lua_getstack(L, 0, &ar)) {
        lua_getinfo(L, "n", &ar);
    }

    if (ar.name) {
        const char* object = FrameScript_GetCurrentObject(L, 0);

        if (object) {
            SStrPrintf(buffer, bufferLen, "%s:%s", object, ar.name);
        } else {
            SStrPrintf(buffer, bufferLen, "%s", ar.name);
        }
    } else {
        SStrCopy(buffer, "UNKNOWN", bufferLen);
    }

    return buffer;
}

// PROVEN: 940ef983 IDENTICAL/IDENTICAL
// OFFSET: 0x0083B030/0x140565760
void FrameScript_RegisterFunction(const char* name, lua_CFunction function) {
    lua_State* L = s_context;
    lua_pushcfunction(L, function);
    lua_setglobal(L, name);
}

// PROVEN: 3c25bed1 IDENTICAL/IDENTICAL
// OFFSET: 0x0083B070/0x1405657B0
void FrameScript_UnregisterFunction(const char* name) {
    lua_State* L = s_context;
    lua_pushnil(L);
    lua_setglobal(L, name);
}

// PROVEN: 082ca460 IDENTICAL/IDENTICAL
// OFFSET: 0x0083B0B0/0x140565800
int32_t FrameScript_GetVariable(const char* name, const char*& value) {
    lua_State* L = s_context;
    int32_t result = 0;

    lua_pushstring(L, name);
    lua_rawget(L, LUA_GLOBALSINDEX);

    if (lua_isstring(L, -1)) {
        result = 1;
        value = lua_tostring(L, -1);
    }

    lua_pop(L, 1);

    return result;
}

// OFFSET: 0x0083B110/0x140565880
char* FrameScript_Sprintf(lua_State* L, int32_t index, char* buffer, uint32_t bufferLen) {
    size_t length;
    const char* format = luaL_checklstring(L, index, &length);
    const char* formatEnd = format + length;
    char* dest = buffer;
    int32_t space = bufferLen;
    int32_t arg = index;

    while (format < formatEnd && space > 1) {
        char c = *format++;

        if (c != '%') {
            *dest++ = c;
            space--;
            continue;
        }

        if (*format == '%') {
            *dest++ = '%';
            format++;
            space--;
            continue;
        }

        char spec[128];
        spec[0] = '%';

        if (*format >= '0' && *format <= '9' && format[1] == '$') {
            arg = *format + index - '1';
            format += 2;
        }

        arg++;

        const char* start = format;

        while (*format == '-' || *format == '+' || *format == ' ' || *format == '#' || *format == '0') {
            format++;
        }

        while (*format >= '0' && *format <= '9') {
            format++;
        }

        if (*format == '.') {
            format++;
        }

        if (*format == '-') {
            format++;
        }

        while (*format >= '0' && *format <= '9') {
            format++;
        }

        size_t specLength = format - start;

        if (specLength > 125) {
            luaL_error(L, "invalid format (width or precision too long)");
        }

        memcpy(spec + 1, start, specLength + 1);
        spec[specLength + 2] = '\0';

        int32_t written;

        switch (*format++) {
            case 'c':
                *dest++ = static_cast<char>(luaL_checknumber(L, arg));
                space--;
                break;

            case 'd':
            case 'i':
                written = SStrPrintf(dest, space, spec, static_cast<int32_t>(luaL_checknumber(L, arg)));

                if (written > 0) {
                    lua_convertdecimal(dest);
                    dest += written;
                    space -= written;
                }

                break;

            case 'o':
            case 'u':
            case 'x':
            case 'X':
                written = SStrPrintf(dest, space, spec, static_cast<int64_t>(luaL_checknumber(L, arg)));

                if (written > 0) {
                    dest += written;
                    space -= written;
                }

                break;

            case 'e':
            case 'E':
            case 'f':
            case 'g':
            case 'G':
                written = SStrPrintf(dest, space, spec, luaL_checknumber(L, arg));

                if (written > 0) {
                    dest += written;
                    space -= written;
                }

                break;

            case 'F':
                for (char* f = spec; *f; f++) {
                    if (*f == 'F') {
                        *f = 'f';
                    }
                }

                written = SStrPrintf(dest, space, spec, luaL_checknumber(L, arg));

                if (written > 0) {
                    lua_convertdecimal(dest);
                    dest += written;
                    space -= written;
                }

                break;

            case 's': {
                size_t stringLength;
                written = SStrPrintf(dest, space, spec, luaL_checklstring(L, arg, &stringLength));

                if (written > 0) {
                    dest += written;
                    space -= written;
                }

                break;
            }

            default:
                luaL_error(L, "invalid option in `format'");
                break;
        }
    }

    *dest = '\0';

    return buffer;
}

// OFFSET: 0x0083B440/0x140565C40
static void PrintVariable(HOSFILE file, lua_State* L, int32_t index, int32_t depth) {
    uint32_t written;

    switch (lua_type(L, index)) {
        case LUA_TNIL:
            SStrPrintf(s_printBuffer, sizeof(s_printBuffer), "nil");
            break;

        case LUA_TBOOLEAN:
            SStrPrintf(s_printBuffer, sizeof(s_printBuffer), "%s", lua_toboolean(L, index) ? "true" : "false");
            break;

        case LUA_TNUMBER: {
            double value = lua_tonumber(L, index);

            if (std::isnan(value)) {
                SStrPrintf(s_printBuffer, sizeof(s_printBuffer), "nil --[[ nan ]]");
            } else if (IsNotFinite(value)) {
                SStrPrintf(s_printBuffer, sizeof(s_printBuffer), "nil --[[ inf ]]");
            } else {
                SStrPrintf(s_printBuffer, sizeof(s_printBuffer), "%.16g", lua_tonumber(L, index));
            }

            break;
        }

        case LUA_TSTRING: {
            size_t length;
            const char* string = luaL_checklstring(L, index, &length);
            s_printBuffer[0] = '"';
            uint32_t pos = 1;

            while (length--) {
                switch (*string) {
                    case '\0':
                        s_printBuffer[pos++] = '\\';
                        s_printBuffer[pos++] = '0';
                        s_printBuffer[pos++] = '0';
                        s_printBuffer[pos] = '0';
                        break;

                    case '\n':
                        s_printBuffer[pos++] = '\\';
                        s_printBuffer[pos] = 'n';
                        break;

                    case '\r':
                        s_printBuffer[pos++] = '\\';
                        s_printBuffer[pos] = 'r';
                        break;

                    case '"':
                    case '\\':
                        s_printBuffer[pos++] = '\\';

                    default:
                        s_printBuffer[pos] = *string;
                        break;
                }

                pos++;
                string++;

                if (pos >= sizeof(s_printBuffer) - 8) {
                    OsWriteFile(file, s_printBuffer, pos, &written);
                    pos = 0;
                }
            }

            s_printBuffer[pos] = '"';
            OsWriteFile(file, s_printBuffer, pos + 1, &written);

            return;
        }

        case LUA_TTABLE: {
            lua_checkstack(L, 3);
            lua_rawgeti(L, LUA_REGISTRYINDEX, s_recursiveTableHash);
            int32_t tableIndex = index - 1;
            lua_pushvalue(L, tableIndex);
            lua_rawget(L, -2);

            if (lua_type(L, -1) != LUA_TNIL) {
                lua_pop(L, 2);
                SStrPrintf(s_printBuffer, sizeof(s_printBuffer), "nil --[[ skipped recursive table ]]");
                OsWriteFile(file, s_printBuffer, SStrLen(s_printBuffer), &written);

                return;
            }

            lua_pop(L, 1);
            lua_pushvalue(L, tableIndex);
            lua_pushboolean(L, 1);
            lua_rawset(L, -3);
            lua_pop(L, 1);

            SStrPrintf(s_printBuffer, sizeof(s_printBuffer), "{\r\n");
            OsWriteFile(file, s_printBuffer, SStrLen(s_printBuffer), &written);

            if (depth < sizeof(s_printIndent)) {
                if (depth > 0) {
                    s_printIndent[depth - 1] = '\t';
                }

                s_printIndent[depth] = '\0';
            }

            int32_t count = lua_objlen(L, index);

            for (int32_t i = 1; i <= count; i++) {
                lua_rawgeti(L, index, i);
                SStrPrintf(s_printBuffer, sizeof(s_printBuffer), "%s", s_printIndent);
                OsWriteFile(file, s_printBuffer, SStrLen(s_printBuffer), &written);
                PrintVariable(file, L, -1, depth + 1);
                SStrPrintf(s_printBuffer, sizeof(s_printBuffer), ", -- [%d]\r\n", i);
                OsWriteFile(file, s_printBuffer, SStrLen(s_printBuffer), &written);
                lua_pop(L, 1);
            }

            lua_pushnil(L);

            while (lua_next(L, tableIndex)) {
                switch (lua_type(L, -2)) {
                    case LUA_TNUMBER: {
                        double key = lua_tonumber(L, -2);

                        if (std::isnan(key)) {
                            SStrPrintf(s_printBuffer, sizeof(s_printBuffer), "%s--[[ skipped entry with nan key ]]\r\n", s_printIndent);
                            OsWriteFile(file, s_printBuffer, SStrLen(s_printBuffer), &written);
                            lua_pop(L, 1);
                            continue;
                        }

                        if (!std::isfinite(key)) {
                            SStrPrintf(s_printBuffer, sizeof(s_printBuffer), "%s--[[ skipped entry with inf key ]]\r\n", s_printIndent);
                            OsWriteFile(file, s_printBuffer, SStrLen(s_printBuffer), &written);
                            lua_pop(L, 1);
                            continue;
                        }

                        if (count > 0) {
                            uint32_t n = static_cast<uint32_t>(key);

                            if (key > 0 && key <= count && n == key) {
                                lua_pop(L, 1);
                                continue;
                            }
                        }
                    }

                    case LUA_TNIL:
                    case LUA_TBOOLEAN:
                    case LUA_TSTRING:
                        SStrPrintf(s_printBuffer, sizeof(s_printBuffer), "%s[", s_printIndent);
                        OsWriteFile(file, s_printBuffer, SStrLen(s_printBuffer), &written);
                        PrintVariable(file, L, -2, depth + 1);
                        SStrPrintf(s_printBuffer, sizeof(s_printBuffer), "] = ");
                        OsWriteFile(file, s_printBuffer, SStrLen(s_printBuffer), &written);
                        PrintVariable(file, L, -1, depth + 1);
                        SStrPrintf(s_printBuffer, sizeof(s_printBuffer), ",\r\n");
                        OsWriteFile(file, s_printBuffer, SStrLen(s_printBuffer), &written);
                        lua_pop(L, 1);
                        break;

                    default:
                        SStrPrintf(s_printBuffer, sizeof(s_printBuffer), "%s--[[ skipped entry with %s key ]]\r\n", s_printIndent, lua_typename(L, lua_type(L, -2)));
                        OsWriteFile(file, s_printBuffer, SStrLen(s_printBuffer), &written);
                        lua_pop(L, 1);
                        break;
                }
            }

            if (depth < sizeof(s_printIndent) && depth > 0) {
                s_printIndent[depth - 1] = '\0';
            }

            SStrPrintf(s_printBuffer, sizeof(s_printBuffer), "%s}", s_printIndent);
            OsWriteFile(file, s_printBuffer, SStrLen(s_printBuffer), &written);

            lua_rawgeti(L, LUA_REGISTRYINDEX, s_recursiveTableHash);
            lua_pushvalue(L, tableIndex);
            lua_pushnil(L);
            lua_rawset(L, -3);
            lua_pop(L, 1);

            return;
        }

        case LUA_TFUNCTION: {
            lua_Debug ar;
            lua_checkstack(L, 1);
            lua_pushvalue(L, index);
            lua_getinfo(L, ">n", &ar);

            if (ar.name) {
                SStrPrintf(s_printBuffer, sizeof(s_printBuffer), "nil --[[ skipped function %s (may be aliased) ]]", ar.name);
            } else {
                SStrPrintf(s_printBuffer, sizeof(s_printBuffer), "nil --[[ skipped inline function ]]");
            }

            break;
        }

        default:
            SStrPrintf(s_printBuffer, sizeof(s_printBuffer), "nil --[[ skipped %s ]]", lua_typename(L, lua_type(L, index)));
            break;
    }

    OsWriteFile(file, s_printBuffer, SStrLen(s_printBuffer), &written);
}

// PROVEN: 05fa48bd IDENTICAL/IDENTICAL
// OFFSET: 0x0083BCB0/0x1405664C0
int32_t FrameScript_PrintVariable(HOSFILE file, const char* name) {
    lua_State* L = s_context;

    lua_pushstring(L, name);
    lua_rawget(L, LUA_GLOBALSINDEX);
    lua_checkstack(L, 4);
    lua_rawgeti(L, LUA_REGISTRYINDEX, s_recursiveTableHash);
    lua_pushnil(L);

    while (lua_next(L, -2)) {
        lua_pop(L, 1);
        lua_pushvalue(L, -1);
        lua_pushnil(L);
        lua_rawset(L, -4);
    }

    lua_pop(L, 1);

    uint32_t written;
    OsWriteFile(file, "\r\n", 2, &written);
    OsWriteFile(file, name, SStrLen(name), &written);
    OsWriteFile(file, " = ", 3, &written);
    PrintVariable(file, L, -1, 1);
    lua_pop(L, 1);

    return 1;
}

// PROVEN: d9a7fd5b IDENTICAL/IDENTICAL
// OFFSET: 0x0083BDA0/0x140566610
int32_t FrameScript_LoadVariables(const char* filename, CStatus* status) {
    lua_State* L = s_context;

    char chunkname[261];
    SStrPrintf(chunkname, sizeof(chunkname), "@%s", filename);

    void* data;
    uint32_t bytes;

    if (!SFile::ReadFile(NULL, filename, &data, &bytes, 0, 1)) {
        if (status) {
            status->Add(STATUS_ERROR, "Error loading %s", filename);
        }

        return 0;
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, s_errorHandlerRef);

    if (luaL_loadbuffer(L, static_cast<const char*>(data), bytes, chunkname)) {
        if (status) {
            status->Add(STATUS_ERROR, "%s", lua_tostring(L, -1));
        }

        if (lua_pcall(L, 1, 0, 0)) {
            lua_pop(L, 1);
        }

        SFile::Unload(data);

        return 0;
    }

    SFile::Unload(data);

    lua_newtable(L);
    lua_insert(L, -3);
    lua_pushvalue(L, -3);
    lua_setfenv(L, -2);

    lua_closuretaint = s_savedVariablesTaint;
    int32_t error = lua_pcall(L, 0, 0, -2);
    lua_closuretaint = NULL;

    if (error) {
        if (status) {
            status->Add(STATUS_ERROR, "%s", lua_tostring(L, -1));
        }

        lua_pop(L, 3);

        return 0;
    }

    lua_pop(L, 1);
    lua_pushnil(L);

    while (lua_next(L, -2)) {
        if (lua_type(L, -2) == LUA_TSTRING) {
            lua_pushstring(L, lua_tostring(L, -2));
            lua_insert(L, -2);
            lua_rawset(L, LUA_GLOBALSINDEX);
        } else {
            lua_pop(L, 1);
        }
    }

    lua_pop(L, 1);

    return 1;
}

// PROVEN: 47d8f14d IDENTICAL/NONE
// OFFSET: 0x0083BFC0/0x1405668A0
int32_t FrameScript_ExecuteBuffer(const char* buffer, size_t bytes, const char* bufferName, CStatus* status, const char* wrapper) {
    lua_State* L = s_context;

    lua_rawgeti(L, LUA_REGISTRYINDEX, s_errorHandlerRef);

    if (luaL_loadbuffer(L, buffer, bytes, bufferName)) {
        if (status) {
            status->Add(STATUS_ERROR, "%s", lua_tostring(L, -1));
        }

        if (lua_pcall(L, 1, 0, 0)) {
            lua_pop(L, 1);
        }

        return 0;
    }

    int32_t args = 0;

    if (wrapper) {
        lua_pushstring(L, wrapper);
        lua_pushvalue(L, -4);
        args = 2;
    }

    if (lua_pcall(L, args, 0, -2 - args)) {
        if (status) {
            status->Add(STATUS_ERROR, "%s", lua_tostring(L, -1));
        }

        lua_pop(L, 2);

        return 0;
    }

    lua_pop(L, 1);

    return 1;
}

// OFFSET: 0x0083D5D0/0x140568520
int32_t FrameScript_ExecuteFile(const char* filename, const char* wrapper, MD5_CTX* md5, CStatus* status) {
    const char* path = filename;
    char normalized[260];

    if (SStrStr(filename, "..")) {
        SStrCopy(normalized, filename, sizeof(normalized));

        for (char* dots = SStrStr(normalized, ".."); dots; dots = SStrStr(normalized, "..")) {
            if (dots[-1] != '\\' && dots[-1] != '/') {
                break;
            }

            if (dots[2] != '\\' && dots[2] != '/') {
                break;
            }

            char* slash;

            for (slash = dots - 2; slash >= normalized; slash--) {
                if (*slash == '\\' || *slash == '/') {
                    break;
                }
            }

            SStrCopy(slash + 1, dots + 3, STORM_MAX_STR);
        }

        path = normalized;
    }

    char chunkname[261];
    SStrPrintf(chunkname, sizeof(chunkname), "@%s", path);

    void* data;
    uint32_t bytes;

    if (!SFile::ReadFile(NULL, path, &data, &bytes, 0, 1)) {
        if (status) {
            status->Add(STATUS_ERROR, "Error loading %s", path);
        }

        return 0;
    }

    if (md5) {
        MD5Update(md5, static_cast<const uint8_t*>(data), bytes);
    }

    int32_t result = FrameScript_ExecuteBuffer(static_cast<const char*>(data), bytes, chunkname, status, wrapper);
    SFile::Unload(data);

    return result;
}

// PROVEN: ae667a40 IDENTICAL/DIFFERENT
// OFFSET: 0x0083C0D0/0x140566A00
int32_t FrameScript_CompileFunction(const char* name, const char* wrapper, const char* body, CStatus* status) {
    lua_State* L = s_context;

    size_t size = SStrLen(wrapper) + SStrLen(body) + 1;
    char* source = static_cast<char*>(alloca(size));
    SStrPrintf(source, size, wrapper, body);

    lua_rawgeti(L, LUA_REGISTRYINDEX, s_errorHandlerRef);

    if (luaL_loadbuffer(L, source, SStrLen(source), name)) {
        if (status) {
            status->Add(STATUS_ERROR, "%s", lua_tostring(L, -1));
        }

        if (lua_pcall(L, 1, 0, 0)) {
            lua_pop(L, 1);
        }

        return -1;
    }

    if (lua_pcall(L, 0, 1, -2)) {
        if (status) {
            status->Add(STATUS_ERROR, "%s", lua_tostring(L, -1));
        }

        lua_pop(L, 2);

        return -1;
    }

    int32_t function = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pop(L, 1);

    return function;
}

// PROVEN: 987adb75 IDENTICAL/IDENTICAL
// OFFSET: 0x0083C210/0x140566BA0
void FrameScript_ReleaseFunction(int32_t function) {
    luaL_unref(s_context, LUA_REGISTRYINDEX, function);
}

// PROVEN: 17bf5c05 IDENTICAL/DIFFERENT
// OFFSET: 0x0083C230/0x140566BC0
void FrameScript_Execute(const char* source, const char* filename, const char* taint) {
    const char* savedTaint = lua_tainted;

    if (++lua_taintexpected && !lua_taintedclosure) {
        lua_tainted = taint;
    }

    size_t length = SStrLen(source);
    lua_State* L = s_context;

    lua_rawgeti(L, LUA_REGISTRYINDEX, s_errorHandlerRef);

    if (luaL_loadbuffer(L, source, length, filename)) {
        if (lua_pcall(L, 1, 0, 0)) {
            lua_pop(L, 1);
        }
    } else if (lua_pcall(L, 0, 0, -2)) {
        lua_pop(L, 2);
    } else {
        lua_pop(L, 1);
    }

    if (lua_taintexpected && !lua_taintedclosure) {
        lua_tainted = savedTaint;
    }

    if (--lua_taintexpected <= 0) {
        lua_taintexpected = 0;
    }
}

// PROVEN: 71bfa199 IDENTICAL/IDENTICAL
// OFFSET: 0x0083C310/0x140566CD0
double FrameScript_GetScriptCPUUsage() {
    return s_scriptTimeUsed * s_scriptTimeDivisor;
}

// PROVEN: dc3886b2 IDENTICAL/IDENTICAL
// OFFSET: 0x0083C320/0x140566CF0
int32_t FrameScript_GetErrorHandlerReference() {
    return s_errorHandlerRef;
}

// PROVEN: ad9edd7e IDENTICAL/NONE
// OFFSET: 0x0083C330
static bool IsIntegerNumber(double value) {
    return _finite(value) && value >= -2147483648.0 && value <= 2147483647.0;
}

// OFFSET: 0x0083C370/0x140566D00
void FrameScript_PackVariable(lua_State* L, const char* name, int32_t depth, char* buffer, uint32_t bufferLen) {
    char line[1024];

    if (depth < sizeof(s_packIndent)) {
        if (depth > 0) {
            s_packIndent[depth - 1] = ' ';
        }

        s_packIndent[depth] = '\0';
    }

    switch (lua_type(L, -1)) {
        case LUA_TNIL:
            SStrPrintf(line, sizeof(line), "%s%s = nil\n", s_packIndent, name);
            SStrPack(buffer, line, bufferLen);
            break;

        case LUA_TBOOLEAN:
            SStrPrintf(line, sizeof(line), "%s%s = %s\n", s_packIndent, name, lua_toboolean(L, -1) ? "true" : "false");
            SStrPack(buffer, line, bufferLen);
            break;

        case LUA_TLIGHTUSERDATA:
        case LUA_TUSERDATA:
            SStrPrintf(line, sizeof(line), "%s%s = <userdata>\n", s_packIndent, name);
            SStrPack(buffer, line, bufferLen);
            break;

        case LUA_TNUMBER:
            SStrPrintf(line, sizeof(line), "%s%s = %s\n", s_packIndent, name, lua_tostring(L, -1));
            SStrPack(buffer, line, bufferLen);
            break;

        case LUA_TSTRING:
            SStrPrintf(line, sizeof(line), "%s%s = \"%s\"\n", s_packIndent, name, lua_tostring(L, -1));
            SStrPack(buffer, line, bufferLen);
            break;

        case LUA_TTABLE: {
            FrameScript_Object* object = NULL;
            const char* displayName = "<table>";

            lua_rawgeti(L, -1, 0);

            if (lua_type(L, -1) == LUA_TLIGHTUSERDATA) {
                object = static_cast<FrameScript_Object*>(lua_touserdata(L, -1));
            }

            lua_pop(L, 1);

            if (object) {
                displayName = object->GetDisplayName();
            }

            SStrPrintf(line, sizeof(line), "%s%s = %s {\n", s_packIndent, name, displayName);
            SStrPack(buffer, line, bufferLen);

            if (!depth) {
                lua_pushnil(L);

                while (lua_next(L, -2)) {
                    lua_Number number;

                    if (lua_isnumber(L, -2) && IsIntegerNumber(number = lua_tonumber(L, -2))) {
                        char key[32];
                        SStrPrintf(key, sizeof(key), "%d", static_cast<int32_t>(number));
                        FrameScript_PackVariable(L, key, 1, buffer, bufferLen);
                    } else {
                        FrameScript_PackVariable(L, lua_tostring(L, -2), 1, buffer, bufferLen);
                    }

                    lua_pop(L, 1);
                }
            }

            SStrPrintf(line, sizeof(line), "%s}\n", s_packIndent);
            SStrPack(buffer, line, bufferLen);
            break;
        }

        case LUA_TFUNCTION: {
            lua_Debug ar;
            lua_pushvalue(L, -1);
            lua_getinfo(L, ">nS", &ar);

            if (ar.name) {
                SStrPrintf(line, sizeof(line), "%s%s = %s() defined %s:%d\n", s_packIndent, name, ar.name, ar.source, ar.linedefined);
            } else {
                SStrPrintf(line, sizeof(line), "%s%s = <function> defined %s:%d\n", s_packIndent, name, ar.source, ar.linedefined);
            }

            SStrPack(buffer, line, bufferLen);
            break;
        }

        default:
            SStrPrintf(line, sizeof(line), "%s%s = <%s>\n", s_packIndent, name, lua_typename(L, -1));
            SStrPack(buffer, line, bufferLen);
            break;
    }

    if (depth < sizeof(s_packIndent) && depth > 0) {
        s_packIndent[depth - 1] = '\0';
    }
}

// PROVEN: 9d2f43e4 IDENTICAL/IDENTICAL
// OFFSET: 0x0083CB70/0x1405677B0
static int32_t FrameScript_HandleError(lua_State* L) {
    if (!lua_isstring(L, -1)) {
        lua_pushstring(L, "UNKNOWN ERROR");
        lua_insert(L, -1);
    }

    const char* message = lua_tostring(L, -1);
    const char* marker = SStrStr(message, "*:");
    const char* object = FrameScript_GetCurrentObject(L, 1);

    if (marker && object) {
        lua_pushlstring(L, message, marker - message);
        lua_pushstring(L, object);
        lua_pushstring(L, marker + 1);
        lua_concat(L, 3);
        lua_replace(L, -2);
    }

    if (s_errorHandlerFunc != -1) {
        s_inErrorHandler = 1;
        lua_rawgeti(L, LUA_REGISTRYINDEX, s_errorHandlerFunc);
        lua_insert(L, -2);
        lua_call(L, 1, 1);
        s_inErrorHandler = 0;
    }

    return 1;
}

// OFFSET: 0x0083D150/0x140567F20
int32_t FrameScript_Initialize(int32_t profile) {
    s_scriptThreadId = SGetCurrentThreadId();
    s_mempool = luaM_initPool();
    s_context = lua_newstate(luaM_reallocPool, s_mempool, profile);
    s_scriptTimeUsed = 0;
    s_scriptProfileEnabled = profile;
    s_errorHandlerFunc = -1;
    s_scriptTimeDivisor = 1000.0 / OsGetAsyncClocksPerSecond();

    if (profile) {
        s_eventTimes = static_cast<FRAMESCRIPT_EVENTTIME*>(SMemAlloc(sizeof(FRAMESCRIPT_EVENTTIME) * 1000, __FILE__, __LINE__, 0x8));
    }

    lua_pushcfunction(s_context, FrameScript_HandleError);
    s_errorHandlerRef = luaL_ref(s_context, LUA_REGISTRYINDEX);

    lua_newtable(s_context);
    s_recursiveTableHash = luaL_ref(s_context, LUA_REGISTRYINDEX);

    lua_gc(s_context, LUA_GCSETPAUSE, 110);

    luaopen_base(s_context);
    lua_pop(s_context, 2);
    luaopen_string(s_context);
    lua_pop(s_context, 1);
    luaopen_table(s_context);
    lua_pop(s_context, 1);
    luaopen_math(s_context);
    lua_pop(s_context, 1);
    luaopen_bit(s_context);
    lua_pop(s_context, 1);

    luaL_register(s_context, "_G", s_extraFuncs);
    lua_pop(s_context, 1);

    FrameScript_Execute(s_compatLua, "compat.lua", NULL);

    return 1;
}

// OFFSET: 0x00464D70
bool FrameScript_GetText(const char* token, char* buffer, uint32_t size, int32_t count, FRAMESCRIPT_GENDER gender) {
    const char* text = FrameScript_GetText(token, count, gender);
    SStrCopy(buffer, text, size);
    return *text != '\0';
}

// OFFSET: 0x0083D310/0x140568110
const char* FrameScript_GetText(const char* token, int32_t count, FRAMESCRIPT_GENDER gender) {
    // STATIC: 0x00EBF468/0x140DB14D0
    static const char* text;

    text = "";

    uint32_t plural = FrameScript_GetPluralIndex(count);
    char pluralSuffix[4];
    memset(pluralSuffix, 0, sizeof(pluralSuffix));

    if (plural >= 1 && plural <= 9) {
        pluralSuffix[0] = '_';
        pluralSuffix[1] = 'P';
        pluralSuffix[2] = '0' + plural;
        pluralSuffix[3] = '\0';
    }

    const char* genderSuffix = "";

    if (gender == GENDER_FEMALE) {
        genderSuffix = "_FEMALE";
    }

    size_t size = SStrLen(genderSuffix) + SStrLen(token) + SStrLen(pluralSuffix) + 1;
    char* name = static_cast<char*>(alloca(size));
    name[0] = '\0';

    SStrPrintf(name, size, "%s%s%s", token, pluralSuffix, genderSuffix);

    if (!FrameScript_GetVariable(name, text)) {
        SStrPrintf(name, size, "%s%s", token, pluralSuffix);

        if (!FrameScript_GetVariable(name, text)) {
            SStrPrintf(name, size, "%s%s", token, genderSuffix);

            if (!FrameScript_GetVariable(name, text)) {
                FrameScript_GetVariable(token, text);
            }
        }
    }

    return text;
}

// OFFSET: 0x0083D440/0x1405683C0
int32_t vFrameScript_PushEventArgs(const char* format, va_list args) {
    lua_State* L = s_context;
    int32_t needed = 0;

    for (const char* c = format; *c; c++) {
        if (*c == '%') {
            needed++;
        }
    }

    lua_checkstack(L, needed);
    int32_t count = 0;

    for (; *format; format++) {
        if (*format != '%') {
            continue;
        }

        switch (format[1]) {
            case 'b':
                lua_pushboolean(L, va_arg(args, int32_t));
                count++;
                break;

            case 'd':
                lua_pushnumber(L, va_arg(args, int32_t));
                count++;
                break;

            case 'f':
                lua_pushnumber(L, va_arg(args, double));
                count++;
                break;

            case 's':
                lua_pushstring(L, va_arg(args, const char*));
                count++;
                break;

            case 'u':
                lua_pushnumber(L, va_arg(args, uint32_t));
                count++;
                break;
        }
    }

    return count;
}

// OFFSET: 0x0083D590/0x1405684D0
void FrameScript_PushEventName(uint32_t index) {
    lua_pushstring(s_context, s_scriptEvents[index]->m_key.m_str);
}

// OFFSET: 0x0083DDF0/0x1405690E0
bool FrameScript_ShouldSignalEvent(uint32_t index) {
    if (!s_scriptEvents.Count()) {
        return false;
    }

    FrameScript_EventObject* event = s_scriptEvents[index];

    return event && event->m_listeners.Head();
}

// PROVEN: 4faa3e58 IDENTICAL/IDENTICAL
// OFFSET: 0x0083DE40/0x140569140
void FrameScript_Execute(int32_t function, FrameScript_Object* object, int32_t argCount, const char* taint, FrameScript_EventObject* event) {
    lua_State* L = s_context;
    int64_t start = 0;

    if (s_scriptProfileEnabled) {
        start = OsGetAsyncTimeClocks();
    }

    int32_t argBase = lua_gettop(L) - argCount + 1;
    int32_t callArgs = argCount;

    const char* savedTaint = lua_tainted;

    if (++lua_taintexpected && !lua_taintedclosure) {
        lua_tainted = taint;
    }

    lua_checkstack(L, argCount + 3);
    lua_rawgeti(L, LUA_REGISTRYINDEX, s_errorHandlerRef);
    lua_rawgeti(L, LUA_REGISTRYINDEX, function);

    if (object) {
        if (!object->m_luaRegistered) {
            object->RegisterScriptObject(NULL);
        }

        lua_rawgeti(L, LUA_REGISTRYINDEX, object->m_luaObjectRef);
        callArgs++;
    }

    for (int32_t i = 0; i < argCount; i++) {
        lua_pushvalue(L, i + argBase);
    }

    if (lua_pcall(L, callArgs, 0, -2 - callArgs)) {
        lua_pop(L, 1);
    }

    lua_pop(L, 1);

    if (lua_taintexpected && !lua_taintedclosure) {
        lua_tainted = savedTaint;
    }

    if (--lua_taintexpected <= 0) {
        lua_taintexpected = 0;
    }

    lua_pop(L, argCount);

    if (s_scriptProfileEnabled) {
        int64_t elapsed = OsGetAsyncTimeClocks() - start;

        if (event) {
            event->m_timeUsed += elapsed;
        }

        s_scriptTimeUsed += elapsed;
    }
}

// OFFSET: 0x0083E090/0x1405693F0
void FrameScript_GetEventCPUUsage(const char* name, double* time, uint32_t* count) {
    int64_t timeUsed = 0;
    uint32_t signalCount = 0;

    for (FrameScript_EventObject* event = s_scriptEventsHash.m_fulllist.Head(); event; event = s_scriptEventsHash.m_fulllist.Next(event)) {
        timeUsed += event->m_timeUsed;
        signalCount += event->m_signalCount;
    }

    *time = timeUsed * s_scriptTimeDivisor;
    *count = signalCount;
}

// OFFSET: 0x0083E110/0x140569470
void FrameScript_ResetCPUUsage() {
    s_scriptTimeUsed = 0;

    for (FrameScript_EventObject* event = s_scriptEventsHash.m_fulllist.Head(); event; event = s_scriptEventsHash.m_fulllist.Next(event)) {
        event->m_timeUsed = 0;
        event->m_signalCount = 0;
    }

    lua_gcresettimes(s_context);
}

// OFFSET: 0x0083EF30/0x14056A780
FrameScript_EventObject::~FrameScript_EventObject() {
    this->m_listeners.Clear();
}

// OFFSET: 0x0083E2E0/0x1405696A0
void FrameScript_RegisterScriptEvent(FrameScript_Object* object, FrameScript_EventObject* event) {
    if (event->m_pendingSignalCount) {
        for (EVENTLISTENERNODE* node = event->m_registerListeners.Head(); !event->m_registerListeners.IsEnd(node); node = event->m_registerListeners.RawNext(node)) {
            if (node->m_listener == object) {
                return;
            }
        }

        EVENTLISTENERNODE* node = event->m_registerListeners.NewNode(2, 0, 0);
        node->m_listener = object;
    } else {
        EVENTLISTENERNODE* node = event->m_listeners.NewNode(2, 0, 0);
        node->m_listener = object;
    }
}

// OFFSET: 0x0083E600/0x140569B20
void FrameScript_UnregisterScriptEvent(FrameScript_Object* object, FrameScript_EventObject* event) {
    if (event->m_pendingSignalCount) {
        for (EVENTLISTENERNODE* node = event->m_unregisterListeners.Head(); !event->m_unregisterListeners.IsEnd(node); node = event->m_unregisterListeners.RawNext(node)) {
            if (node->m_listener == object) {
                return;
            }
        }

        EVENTLISTENERNODE* node = event->m_unregisterListeners.NewNode(2, 0, 0);
        node->m_listener = object;
    } else {
        for (EVENTLISTENERNODE* node = event->m_listeners.Head(); !event->m_listeners.IsEnd(node); node = event->m_listeners.RawNext(node)) {
            if (node->m_listener == object) {
                event->m_listeners.DeleteNode(node);
                break;
            }
        }
    }
}

// OFFSET: 0x0083E410/0x140569840
static void DestroyEvents() {
    s_scriptEventsHash.Clear();
    s_scriptEvents.Clear();
}

// PROVEN: 98724317 IDENTICAL/DIFFERENT
// OFFSET: 0x0083E810/0x140569EA0
void FrameScript_Destroy() {
    if (s_eventTimes) {
        SMemFree(s_eventTimes, __FILE__, __LINE__, 0);
        s_eventTimes = NULL;
        s_eventTimeIndex = -1;
    }

    lua_close(s_context);
    s_context = NULL;
    luaM_freePool(s_mempool);
    s_mempool = NULL;
    DestroyEvents();
}

// OFFSET: 0x0083E870/0x140569F10
void FrameScript_SignalEvent(uint32_t index, lua_State* L, int32_t argCount) {
    if (!s_scriptEvents.Count()) {
        return;
    }

    FrameScript_EventObject* event = s_scriptEvents[index];

    if (!event) {
        return;
    }

    uint32_t previousEventId = s_currentEventId;
    event->m_signalCount++;
    event->m_pendingSignalCount++;
    s_currentEventId = s_nextEventId++;
    int64_t startTime = event->m_timeUsed;
    int32_t handlerCount = 0;
    FRAMESCRIPT_EVENTTIME* record = NULL;

    if (s_scriptProfileEnabled) {
        s_eventTimeIndex = (s_eventTimeIndex + 1) % 1000;
        record = &s_eventTimes[s_eventTimeIndex];
        record->m_eventId = s_currentEventId;
        record->m_slowestTime = 0.0f;
    }

    lua_checkstack(L, argCount);

    EVENTLISTENERNODE* node = event->m_listeners.Head();

    while (!event->m_listeners.IsEnd(node)) {
        EVENTLISTENERNODE* next = event->m_listeners.RawNext(node);
        int32_t unregistered = 0;

        for (EVENTLISTENERNODE* other = event->m_unregisterListeners.Head(); !event->m_unregisterListeners.IsEnd(other); other = event->m_unregisterListeners.RawNext(other)) {
            if (node->m_listener == other->m_listener) {
                unregistered = 1;
                break;
            }
        }

        if (!unregistered) {
            FrameScript_Object::ScriptIx& script = node->m_listener->m_onEvent;

            if (node->m_listener->HasScript(script)) {
                for (int32_t i = argCount; i > 0; i--) {
                    lua_pushvalue(L, -argCount);
                }

                int64_t handlerStart = 0;
                const char* handlerName = NULL;

                if (s_scriptProfileEnabled) {
                    handlerStart = event->m_timeUsed;
                    handlerName = node->m_listener->GetName();

                    if (!handlerName) {
                        handlerName = "<unnamed>";
                    }
                }

                int32_t function;
                const char* taint;
                node->m_listener->GetScriptFunction(script, function, taint);
                FrameScript_Execute(function, node->m_listener, argCount, taint, event);

                if (s_scriptProfileEnabled) {
                    float time = static_cast<float>((event->m_timeUsed - handlerStart) * s_scriptTimeDivisor);

                    if (record && time > record->m_slowestTime) {
                        record->m_slowestTime = time;
                        record->m_slowestHandler = handlerName;
                    }
                }

                handlerCount++;
            }
        }

        node = next;
    }

    if (s_scriptProfileEnabled) {
        record->m_handlerCount = handlerCount;
        record->m_time = static_cast<float>((event->m_timeUsed - startTime) * s_scriptTimeDivisor);
    }

    event->m_pendingSignalCount--;
    s_currentEventId = previousEventId;

    for (node = event->m_unregisterListeners.Head(); !event->m_unregisterListeners.IsEnd(node); node = event->m_unregisterListeners.DeleteNode(node)) {
        FrameScript_UnregisterScriptEvent(node->m_listener, event);
    }

    for (node = event->m_registerListeners.Head(); !event->m_registerListeners.IsEnd(node); node = event->m_registerListeners.DeleteNode(node)) {
        FrameScript_RegisterScriptEvent(node->m_listener, event);
    }
}

// PROVEN: 3d3e8f5d IDENTICAL/IDENTICAL
// OFFSET: 0x0083EC60/0x14056A450
void FrameScript_Flush() {
    if (s_context) {
        FrameScript_Destroy();
        FrameScript_Initialize(s_scriptProfileEnabled);
    }
}

// OFFSET: 0x0083EC80/0x14056A480
void vFrameScript_SignalEvent(uint32_t index, const char* format, va_list args) {
    if (!FrameScript_ShouldSignalEvent(index)) {
        return;
    }

    int32_t argCount = 1;

    const char* taint = lua_tainted;

    if (++lua_taintexpected && !lua_taintedclosure) {
        lua_tainted = NULL;
    }

    lua_State* L = s_context;
    FrameScript_PushEventName(index);

    if (format) {
        argCount = vFrameScript_PushEventArgs(format, args) + 1;
    }

    FrameScript_SignalEvent(index, L, argCount);
    lua_pop(L, argCount);

    if (lua_taintexpected && !lua_taintedclosure) {
        lua_tainted = taint;
    }

    if (--lua_taintexpected <= 0) {
        lua_taintexpected = 0;
    }
}

// OFFSET: 0x0083EE00
FrameScript_EventObject::FrameScript_EventObject() {
    this->m_key.m_str = NULL;
    this->m_timeUsed = 0;
    this->m_signalCount = 0;
    this->m_pendingSignalCount = 0;
}

// OFFSET: 0x0083E070/0x1405693C0
TSList<EVENTLISTENERNODE>* FrameScript_GetEventListeners(const char* name) {
    FrameScript_EventObject* event = s_scriptEventsHash.Ptr(name);

    return event ? &event->m_listeners : NULL;
}

// PROVEN: 4eab4056 IDENTICAL/IDENTICAL
// OFFSET: 0x0083F3A0/0x14056AE10
void FrameScript_SignalEvent(uint32_t index, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vFrameScript_SignalEvent(index, format, args);
    va_end(args);
}

// OFFSET: 0x0083F450/0x14056AE40
void FrameScript_CreateEvents(const char** names, uint32_t count) {
    s_scriptEvents.SetCount(count);

    for (uint32_t i = 0; i < count; i++) {
        if (names[i]) {
            FrameScript_EventObject* event = s_scriptEventsHash.Ptr(names[i]);

            if (!event) {
                event = s_scriptEventsHash.New(names[i], 0, 0);
            }

            s_scriptEvents[i] = event;
        } else {
            s_scriptEvents[i] = NULL;
        }
    }
}
