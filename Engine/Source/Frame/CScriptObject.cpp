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

// PROVEN: 543c32af IDENTICAL/IDENTICAL
// OFFSET: 0x00469400/0x14007D630
const char* CScriptObject::GetName() {
    return this->m_name.m_str;
}

// PROVEN: ceb63d24 IDENTICAL/IDENTICAL
// OFFSET: 0x00469410/0x14007D640
bool CScriptObject::IsA(int32_t type) {
    return type == CScriptObject::s_objectType;
}

// PROVEN: 93faf8f9 IDENTICAL/IDENTICAL
// OFFSET: 0x00469430/0x14007D650
bool CScriptObject::IsA(const char* typeName) {
    return SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR) == 0;
}

// OFFSET: 0x005104C0/0x140159BE0
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

// PROVEN: 7e707b6f IDENTICAL/UNPROVEN
// OFFSET: 0x005103F0/0x140159AD0
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

// OFFSET: 0x00510480/0x140159B90
CScriptObject::CScriptObject() {
}

// PROVEN: c28fced4 IDENTICAL/IDENTICAL
// OFFSET: 0x005104B0/0x140159BD0
const char* CScriptObject::GetObjectTypeName() {
    return CScriptObject::s_objectTypeName;
}

// PROVEN: da12d2fa IDENTICAL/IDENTICAL
// OFFSET: 0x005105D0/0x140159D30
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
