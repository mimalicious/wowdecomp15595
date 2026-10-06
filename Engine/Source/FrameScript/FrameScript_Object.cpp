#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: %0x00D8B760
int32_t FrameScript_Object::s_objectTypes;

// PROVEN: 79bbec07 IDENTICAL
// OFFSET: 0x00464DE0
const char* FrameScript_Object::GetDisplayName() {
    const char* name = this->GetName();

    return name ? name : "<unnamed>";
}

// PROVEN: fa4179ba IDENTICAL
// OFFSET: 0x00464E00
int32_t FrameScript_Object::GetLuaObjectRef() {
    if (!this->m_luaRegistered) {
        this->RegisterScriptObject(NULL);
    }

    return this->m_luaObjectRef;
}

// OFFSET: %0x0079FD80
int32_t FrameScript_Object::CreateScriptMetaTable(lua_State* L, void (*registerFunc)(lua_State*)) {
    lua_newtable(L);
    lua_pushstring(L, "__index");
    lua_newtable(L);
    registerFunc(L);
    lua_settable(L, -3);

    return luaL_ref(L, LUA_REGISTRYINDEX);
}

// OFFSET: %0x0079FDD0
void FrameScript_Object::FillScriptMethodTable(lua_State* L, FrameScript_Method* methods, int32_t count) {
    for (int32_t i = 0; i < count; i++) {
        lua_pushstring(L, methods[i].m_name);
        lua_pushcfunction(L, methods[i].m_method);
        lua_settable(L, -3);
    }
}

// OFFSET: %0x0079FD50
FrameScript_Object::FrameScript_Object() : m_scripts(NULL), m_scriptCapacity(0), m_scriptCount(0) {
    this->m_luaRegistered = 0;
    this->m_luaObjectRef = LUA_NOREF;
}

// OFFSET: %0x007A2D90
int32_t FrameScript_Object::SetScriptFunction(FrameScript_Object::ScriptIx& script, int32_t function, const char* taint) {
    int32_t previous = 0;

    if (script.m_index == 0xFF) {
        script.m_index = this->m_scriptCount++;

        if (this->m_scriptCount > this->m_scriptCapacity) {
            this->m_scriptCapacity += 4;
            this->m_scripts = static_cast<FrameScript_Object::ScriptFunction*>(SMemReAlloc(this->m_scripts, sizeof(FrameScript_Object::ScriptFunction) * this->m_scriptCapacity, __FILE__, __LINE__, 0));
        }
    } else {
        previous = this->m_scripts[script.m_index].m_luaRef;
    }

    this->m_scripts[script.m_index].m_luaRef = function;
    this->m_scripts[script.m_index].m_tainted = taint;

    return previous;
}

// OFFSET: %0x007A2E10
bool FrameScript_Object::GetScriptFunction(const FrameScript_Object::ScriptIx& script, int32_t& function, const char*& taint) {
    if (!this->HasScript(script)) {
        function = 0;
        taint = NULL;
        return false;
    }

    function = this->m_scripts[script.m_index].m_luaRef;
    taint = this->m_scripts[script.m_index].m_tainted;
    return true;
}

// OFFSET: [%0x007A2FF0, %0x007A40F0]
FrameScript_Object::~FrameScript_Object() {
    for (int32_t i = 0; i < this->m_scriptCount; i++) {
        if (this->m_scripts[i].m_luaRef) {
            luaL_unref(s_context, LUA_REGISTRYINDEX, this->m_scripts[i].m_luaRef);
        }
    }

    if (this->m_scripts) {
        SMemFree(this->m_scripts, __FILE__, __LINE__, 0);
    }
}

// OFFSET: %0x007A3050
void FrameScript_Object::RegisterScriptObject(const char* name) {
    lua_State* L = s_context;

    if (!this->m_luaRegistered) {
        const char* taint = lua_tainted;

        if (++lua_taintexpected && !lua_taintedclosure) {
            lua_tainted = NULL;
        }

        lua_newtable(L);
        lua_pushnumber(L, 0.0);
        lua_pushlightuserdata(L, this);
        lua_rawset(L, -3);
        lua_rawgeti(L, LUA_REGISTRYINDEX, this->GetScriptMetaTable());
        lua_setmetatable(L, -2);
        this->m_luaObjectRef = luaL_ref(L, LUA_REGISTRYINDEX);

        if (lua_taintexpected && !lua_taintedclosure) {
            lua_tainted = taint;
        }

        if (--lua_taintexpected <= 0) {
            lua_taintexpected = 0;
        }
    }

    this->m_luaRegistered++;

    if (name) {
        bool exists = false;

        lua_pushstring(L, name);
        lua_rawget(L, LUA_GLOBALSINDEX);

        if (lua_type(L, -1) != LUA_TNIL) {
            exists = true;
        }

        lua_pop(L, 1);

        if (!exists) {
            lua_rawgeti(L, LUA_REGISTRYINDEX, this->m_luaObjectRef);
            lua_setglobal(L, name);
        }
    }
}

// OFFSET: %0x007A3190
void FrameScript_Object::UnregisterScriptObject(const char* name) {
    lua_State* L = s_context;

    if (this->m_luaRegistered > 0 && --this->m_luaRegistered == 0) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, this->m_luaObjectRef);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        lua_rawset(L, -3);
        lua_pop(L, 1);
        luaL_unref(L, LUA_REGISTRYINDEX, this->m_luaObjectRef);
        this->m_luaObjectRef = LUA_NOREF;
    }

    if (name) {
        lua_pushnil(L);
        lua_setglobal(L, name);
    }
}

// PROVEN: 9ea68b8e IDENTICAL
// OFFSET: 0x0083CE90
FrameScript_Object::ScriptIx* FrameScript_Object::GetScriptByName(const char* name, FrameScript_Object::ScriptData& data) {
    char lower[128];
    SStrCopy(lower, name, sizeof(lower));
    SStrLower(lower);

    if (!SStrCmp(name, "onevent", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,event,...) %s end";
        return &this->m_onEvent;
    }

    return NULL;
}

// PROVEN: 818d87f4 IDENTICAL
// OFFSET: 0x0083CF30
FrameScript_Object::ScriptIx* FrameScript_Object::GetScriptByNameCamelCase(const char* name, FrameScript_Object::ScriptData& data) {
    char lower[128];
    SStrCopy(lower, name, sizeof(lower));
    SStrLower(lower);

    return this->GetScriptByName(lower, data);
}

// OFFSET: %0x007A3330
void FrameScript_Object::GetScriptTime(lua_State* L, const FrameScript_Object::ScriptIx& script, double* time, uint32_t* calls) {
    if (!this->HasScript(script)) {
        return;
    }

    int32_t function;
    const char* taint;
    this->GetScriptFunction(script, function, taint);

    lua_rawgeti(L, LUA_REGISTRYINDEX, function);

    double functionTime;
    double totalTime;
    int functionCalls;
    lua_gcgetfunctiontime(L, &functionTime, &totalTime, &functionCalls);
    lua_pop(L, 1);

    *time += totalTime;
    *calls += functionCalls;
}

// OFFSET: %0x007A3A80
int32_t FrameScript_Object::GetScript(lua_State* L) {
    if (!lua_isstring(L, 2)) {
        luaL_error(L, "Usage: %s:GetScript(\"type\")", this->GetDisplayName());
    }

    FrameScript_Object::ScriptData data;
    data.m_wrapper = "return function(self) %s end";
    data.m_event = -1;

    FrameScript_Object::ScriptIx* script = this->GetScriptByNameCamelCase(lua_tostring(L, 2), data);

    if (!script) {
        luaL_error(L, "%s doesn't have a \"%s\" script", this->GetDisplayName(), lua_tostring(L, 2));
    }

    int32_t function;
    const char* taint;

    if (this->GetScriptFunction(*script, function, taint)) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, function);
    } else {
        lua_pushnil(L);
    }

    if (taint && lua_taintexpected && !lua_taintedclosure) {
        lua_tainted = taint;
    }

    return 1;
}

// OFFSET: %0x007A3B90
int32_t FrameScript_Object::SetScript(lua_State* L) {
    if (!lua_isstring(L, 2) || (lua_type(L, 3) != LUA_TFUNCTION && lua_type(L, 3) != LUA_TNIL)) {
        luaL_error(L, "Usage: %s:SetScript(\"type\", function)", this->GetDisplayName());
    }

    FrameScript_Object::ScriptData data;
    data.m_wrapper = "return function(self) %s end";
    data.m_event = -1;

    FrameScript_Object::ScriptIx* script = this->GetScriptByNameCamelCase(lua_tostring(L, 2), data);

    if (!script) {
        luaL_error(L, "%s doesn't have a \"%s\" script", this->GetDisplayName(), lua_tostring(L, 2));
    }

    int32_t function = luaL_ref(L, LUA_REGISTRYINDEX);

    if (function < 0) {
        function = 0;
    }

    int32_t previous = this->SetScriptFunction(*script, function, lua_tainted);

    if (previous) {
        luaL_unref(s_context, LUA_REGISTRYINDEX, previous);
    }

    if (function && data.m_event != -1 && this->IsA(CSimpleFrame::GetObjectType())) {
        static_cast<CSimpleFrame*>(this)->EnableEvent(static_cast<CSimpleEventType>(data.m_event), -1);
    }

    return 0;
}

// OFFSET: %0x007A3CE0
int32_t FrameScript_Object::HookScript(lua_State* L) {
    if (!lua_isstring(L, 2) || lua_type(L, 3) != LUA_TFUNCTION) {
        luaL_error(L, "Usage: %s:HookScript(\"type\", function)", this->GetDisplayName());
    }

    FrameScript_Object::ScriptData data;
    data.m_wrapper = "return function(self) %s end";
    data.m_event = -1;

    FrameScript_Object::ScriptIx* script = this->GetScriptByNameCamelCase(lua_tostring(L, 2), data);

    if (!script) {
        luaL_error(L, "%s doesn't have a \"%s\" script", this->GetDisplayName(), lua_tostring(L, 2));
    }

    if (!this->HasScript(*script)) {
        this->SetScriptFunction(*script, luaL_ref(L, LUA_REGISTRYINDEX), lua_tainted);
        return 0;
    }

    lua_remove(L, 2);

    const char* hookTaint = lua_tainted;

    if (++lua_taintexpected && !lua_taintedclosure) {
        lua_tainted = NULL;
    }

    int32_t function;
    const char* scriptTaint;
    this->GetScriptFunction(*script, function, scriptTaint);
    this->SetScriptFunction(*script, 0, scriptTaint);

    lua_rawgeti(L, LUA_REGISTRYINDEX, function);
    luaL_unref(L, LUA_REGISTRYINDEX, function);

    const char* taint = lua_tainted;

    if (++lua_taintexpected && !lua_taintedclosure) {
        lua_tainted = scriptTaint;
    }

    lua_insert(L, 2);
    lua_settaint(L, 2, scriptTaint);
    lua_settaint(L, 3, taint);
    lua_pushcclosure(L, &FrameScript_HookWrapper, 2);
    lua_settaint(L, 2, scriptTaint);

    if (lua_taintexpected && !lua_taintedclosure) {
        lua_tainted = taint;
    }

    if (--lua_taintexpected <= 0) {
        lua_taintexpected = 0;
    }

    this->SetScriptFunction(*script, luaL_ref(L, LUA_REGISTRYINDEX), scriptTaint);

    if (lua_taintexpected && !lua_taintedclosure) {
        lua_tainted = hookTaint;
    }

    if (--lua_taintexpected <= 0) {
        lua_taintexpected = 0;
    }

    return 0;
}

// OFFSET: %0x007A3F40
void FrameScript_Object::LoadFunction(lua_State* L, Function* function, CStatus* status) {
    const char* body = function->m_body.m_string;
    const char* name = function->m_type.m_string;

    FrameScript_Object::ScriptData data;
    data.m_wrapper = "return function(self) %s end";
    data.m_event = -1;

    FrameScript_Object::ScriptIx* script = this->GetScriptByNameCamelCase(name, data);

    if (!script) {
        status->Add(STATUS_WARNING, "Frame %s: Unknown script element %s", this->GetDisplayName(), name);
        return;
    }

    int32_t ref = 0;

    if (!function->m_ref && body && *body) {
        char chunkname[1024];
        SStrPrintf(chunkname, sizeof(chunkname), "*:%s", name);
        function->m_ref = new (__FILE__, __LINE__) int32_t(FrameScript_CompileFunction(chunkname, data.m_wrapper, body, status));
    }

    if (function->m_functionFlags & 1) {
        lua_pushstring(L, function->m_function.m_string);
        lua_rawget(L, LUA_GLOBALSINDEX);

        int32_t global = luaL_ref(L, LUA_REGISTRYINDEX);

        if (global == LUA_REFNIL) {
            status->Add(STATUS_WARNING, "Frame %s: Unknown function %s in element %s", this->GetDisplayName(), static_cast<const char*>(NULL), name);
        } else {
            ref = global;
        }
    } else if (function->m_ref) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, *function->m_ref);
        ref = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    int32_t previous = this->SetScriptFunction(*script, ref, NULL);

    if (previous) {
        luaL_unref(s_context, LUA_REGISTRYINDEX, previous);
    }
}

// OFFSET: %0x007A42E0
void FrameScript_Object::RunScript(const FrameScript_Object::ScriptIx& script, int32_t argCount, const char* taint) {
    int32_t function;
    const char* scriptTaint;
    this->GetScriptFunction(script, function, scriptTaint);

    if (!taint) {
        taint = scriptTaint;
    }

    FrameScript_Execute(function, this, argCount, taint, NULL);
}

// OFFSET: %0x007A53A0
int32_t FrameScript_Object::IsScriptEventRegistered(const char* name) {
    FrameScript_EventObject* event = s_scriptEventsHash.Ptr(name);

    if (event) {
        for (EVENTLISTENERNODE* node = event->m_listeners.Head(); !event->m_listeners.IsEnd(node); node = event->m_listeners.RawNext(node)) {
            if (node->m_listener == this) {
                return 1;
            }
        }
    }

    return 0;
}

// OFFSET: %0x007A53F0
int32_t FrameScript_Object::RegisterScriptEvent(const char* name) {
    FrameScript_EventObject* event = s_scriptEventsHash.Ptr(name);

    if (!event) {
        return 0;
    }

    if (event->m_pendingSignalCount) {
        for (EVENTLISTENERNODE* node = event->m_unregisterListeners.Head(); !event->m_unregisterListeners.IsEnd(node); node = event->m_unregisterListeners.RawNext(node)) {
            if (node->m_listener == this) {
                event->m_unregisterListeners.DeleteNode(node);
                break;
            }
        }
    }

    for (EVENTLISTENERNODE* node = event->m_listeners.Head(); !event->m_listeners.IsEnd(node); node = event->m_listeners.RawNext(node)) {
        if (node->m_listener == this) {
            return 1;
        }
    }

    FrameScript_RegisterScriptEvent(this, event);

    return 1;
}

// OFFSET: %0x007A5480
void FrameScript_Object::UnregisterScriptEvent(const char* name) {
    FrameScript_EventObject* event = s_scriptEventsHash.Ptr(name);

    if (!event) {
        return;
    }

    if (event->m_pendingSignalCount) {
        for (EVENTLISTENERNODE* node = event->m_registerListeners.Head(); !event->m_registerListeners.IsEnd(node); node = event->m_registerListeners.RawNext(node)) {
            if (node->m_listener == this) {
                event->m_registerListeners.DeleteNode(node);
                break;
            }
        }
    }

    FrameScript_UnregisterScriptEvent(this, event);
}

// OFFSET: %0x007A54E0
void FrameScript_Object::RegisterAllScriptEvents() {
    for (FrameScript_EventObject* event = s_scriptEventsHash.m_fulllist.Head(); event; event = s_scriptEventsHash.m_fulllist.Next(event)) {
        this->RegisterScriptEvent(event->m_key.m_str);
    }
}

// OFFSET: %0x007A5530
void FrameScript_Object::UnregisterAllScriptEvents() {
    for (FrameScript_EventObject* event = s_scriptEventsHash.m_fulllist.Head(); event; event = s_scriptEventsHash.m_fulllist.Next(event)) {
        this->UnregisterScriptEvent(event->m_key.m_str);
    }
}
