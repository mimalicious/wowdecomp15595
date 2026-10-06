#ifndef ENGINE_SOURCE_FRAMESCRIPT_FRAMESCRIPT_OBJECT_H
#define ENGINE_SOURCE_FRAMESCRIPT_FRAMESCRIPT_OBJECT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/lua-5.1/src/lua.h"

class CStatus;
class Function;

struct FrameScript_Method {
    /* 0000 */ const char* m_name;
    /* 0004 */ lua_CFunction m_method;
};

class FrameScript_Object {
    public:
    struct ScriptData {
        /* 0000 */ const char* m_wrapper;
        /* 0004 */ int32_t m_event;
    };

    struct ScriptFunction {
        /* 0000 */ int32_t m_luaRef;
        /* 0004 */ const char* m_tainted;
    };

    class ScriptIx {
        public:
        /* 0000 */ uint8_t m_index;

        ScriptIx();
    };

    static int32_t s_objectTypes;

    /* 0004 */ FrameScript_Object::ScriptFunction* m_scripts;
    /* 0008 */ uint8_t m_scriptCapacity;
    /* 0009 */ uint8_t m_scriptCount;
    /* 000A */ FrameScript_Object::ScriptIx m_onEvent;
    /* 000C */ int32_t m_luaRegistered;
    /* 0010 */ int32_t m_luaObjectRef;

    static int32_t CreateScriptMetaTable(lua_State* L, void (*registerFunc)(lua_State*));
    static void FillScriptMethodTable(lua_State* L, FrameScript_Method* methods, int32_t count);

    virtual ~FrameScript_Object();
    virtual const char* GetName() = 0;
    virtual int32_t GetScriptMetaTable() = 0;
    virtual FrameScript_Object::ScriptIx* GetScriptByName(const char* name, FrameScript_Object::ScriptData& data);
    virtual bool IsA(int32_t type) = 0;

    FrameScript_Object();
    const char* GetDisplayName();
    int32_t GetLuaObjectRef();
    int32_t GetScript(lua_State* L);
    FrameScript_Object::ScriptIx* GetScriptByNameCamelCase(const char* name, FrameScript_Object::ScriptData& data);
    bool GetScriptFunction(const FrameScript_Object::ScriptIx& script, int32_t& function, const char*& taint);
    void GetScriptTime(lua_State* L, const FrameScript_Object::ScriptIx& script, double* time, uint32_t* calls);
    bool HasScript(const FrameScript_Object::ScriptIx& script);
    int32_t HookScript(lua_State* L);
    int32_t IsScriptEventRegistered(const char* name);
    void LoadFunction(lua_State* L, Function* function, CStatus* status);
    void RegisterAllScriptEvents();
    int32_t RegisterScriptEvent(const char* name);
    void RegisterScriptObject(const char* name);
    void RunScript(const FrameScript_Object::ScriptIx& script, int32_t argCount, const char* taint);
    int32_t SetScript(lua_State* L);
    int32_t SetScriptFunction(FrameScript_Object::ScriptIx& script, int32_t function, const char* taint);
    void UnregisterAllScriptEvents();
    void UnregisterScriptEvent(const char* name);
    void UnregisterScriptObject(const char* name);
};

#include "Engine/Source/FrameScript/FrameScript_Object.inl"

#endif
