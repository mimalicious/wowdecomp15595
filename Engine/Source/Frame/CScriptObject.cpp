#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: %0x00CD12F0
int32_t CScriptObject::s_objectType;

// STATIC: %0x00BB8630
const char* CScriptObject::s_objectTypeName = "Object";

// OFFSET: %0x00472E50
const char* CScriptObject::GetName() {
    return this->m_name.m_str;
}

// OFFSET: %0x00472E60
bool CScriptObject::IsA(int32_t type) {
    return type == CScriptObject::s_objectType;
}

// OFFSET: %0x00472E80
bool CScriptObject::IsA(const char* typeName) {
    return SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR) == 0;
}

// OFFSET: %0x00520870
void CScriptObject::CreateName(const char* source, char* dest, uint32_t destSize) {
    if (SStrCmpI(source, "$parent", SStrLen("$parent"))) {
        SStrCopy(dest, source, destSize);
        return;
    }

    SStrCopy(dest, "Top", STORM_MAX_STR);

    for (CScriptObject* parent = this->GetScriptObjectParent(); parent; parent = parent->GetScriptObjectParent()) {
        const char* name = parent->GetName();

        if (name && *name) {
            SStrCopy(dest, name, destSize);
            break;
        }
    }

    SStrPack(dest, source + SStrLen("$parent"), destSize);
}

// OFFSET: %0x005207A0
CScriptObject* CScriptObject::GetScriptObjectByName(const char* name, int32_t type) {
    lua_State* L = FrameScript_GetContext();

    lua_pushstring(L, name);
    lua_rawget(L, LUA_GLOBALSINDEX);

    if (lua_type(L, -1) != LUA_TTABLE) {
        lua_settop(L, -2);
        return NULL;
    }

    lua_rawgeti(L, -1, 0);
    CScriptObject* object = static_cast<CScriptObject*>(lua_touserdata(L, -1));
    lua_settop(L, -3);

    if (object && object->IsA(type)) {
        return object;
    }

    return NULL;
}

// OFFSET: %0x00520830
CScriptObject::CScriptObject() {
}

// OFFSET: %0x00520860
const char* CScriptObject::GetObjectTypeName() {
    return CScriptObject::s_objectTypeName;
}

// OFFSET: %0x00520980
void CScriptObject::SetName(const char* name) {
    if (this->m_name.m_str) {
        this->UnregisterScriptObject(this->m_name.m_str);
        this->m_name.Copy(NULL);
    }

    if (name && *name) {
        char fullName[1024];
        this->CreateName(name, fullName, sizeof(fullName));
        this->m_name.Copy(fullName);
        this->RegisterScriptObject(fullName);
    }
}

// OFFSET: [%0x005209F0, %0x00520A20]
CScriptObject::~CScriptObject() {
    if (this->m_name.m_str) {
        this->UnregisterScriptObject(this->m_name.m_str);
        this->m_name.Copy(NULL);
    }
}
