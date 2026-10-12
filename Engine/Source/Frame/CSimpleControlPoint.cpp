#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleControlPoint.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include <new>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CSimplePathAnim.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/FrameXML.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: 0x00CA4798/0x140B428D0
int32_t CSimpleControlPoint::s_metatable = LUA_NOREF;

// STATIC: 0x00DD3CBC/0x140CB9A5C
int32_t CSimpleControlPoint::s_objectType;

// STATIC: 0x00CA3830/0x140B40AB0
const char* CSimpleControlPoint::s_objectTypeName = "ControlPoint";

// OFFSET: 0x00527DB0/0x140177140
bool CSimpleControlPoint::IsA(int32_t type) {
    return type == CSimpleControlPoint::s_objectType || type == CScriptObject::s_objectType;
}

// OFFSET: 0x00527DE0/0x140177160
bool CSimpleControlPoint::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleControlPoint::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// OFFSET: 0x00527E20/0x1401771A0
const char* CSimpleControlPoint::GetObjectTypeName() {
    return CSimpleControlPoint::s_objectTypeName;
}

// OFFSET: 0x00527E30/0x1401771B0
int32_t CSimpleControlPoint::GetScriptMetaTable() {
    return CSimpleControlPoint::s_metatable;
}

// OFFSET: 0x00527A90/0x140176E30
void CSimpleControlPoint::SetOffset(const C2Vector& offset) {
    if (offset.x * offset.x + offset.y * offset.y <= 0.00000023841858f) {
        this->m_offset.x = 0.0f;
        this->m_offset.y = 0.0f;
    } else {
        this->m_offset = offset;
    }
}

// OFFSET: 0x00527E40/0x1401771C0
void CSimpleControlPoint::LoadXML(linkObject* node, CStatus* status) {
    ControlPoint* point = static_cast<ControlPoint*>(node);

    if (node->m_flags & 0x2000000) {
        XMLHashNodeRef ref;

        if (!ref.Acquire(node->m_inherits.m_string)) {
            status->Add(STATUS_WARNING, "Couldn't find inherited node: %s", ref.m_name);
        } else if (ref.m_locked) {
            status->Add(STATUS_WARNING, "Recursively inherited node: %s", ref.m_name);
        } else {
            this->LoadXML(ref.m_node, status);
        }
    }

    if (point->m_pointFlags & 4) {
        lua_State* L = FrameScript_GetContext();
        CScriptObject* parent = this->GetScriptObjectParent();

        if (parent) {
            if (!parent->m_luaRegistered) {
                parent->RegisterScriptObject(NULL);
            }

            lua_rawgeti(L, LUA_REGISTRYINDEX, parent->m_luaObjectRef);
            lua_pushstring(L, point->m_parentKey.m_string);

            if (!this->m_luaRegistered) {
                this->RegisterScriptObject(NULL);
            }

            lua_rawgeti(L, LUA_REGISTRYINDEX, this->m_luaObjectRef);
            lua_settable(L, -3);
            lua_settop(L, -2);
        }
    }

    C2Vector offset;

    if (point->m_pointFlags & 1) {
        offset.x = NDCToDDCWidth(point->m_offsetX / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    if (point->m_pointFlags & 2) {
        offset.y = NDCToDDCWidth(point->m_offsetY / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    this->SetOffset(offset);
}

// OFFSET: 0x00528640/0x140177B60
CSimpleControlPoint::~CSimpleControlPoint() {
}

// OFFSET: 0x005293B0/0x140178D50
void CSimpleControlPoint::SetOrder(int32_t order, bool notify, bool wrap) {
    int32_t oldOrder = this->m_order;

    if (wrap) {
        this->m_order = order < 0 || order > 99 ? 0 : order;
    } else {
        this->m_order = order < 0 ? 0 : (order > 99 ? 99 : order);
    }

    if (this->m_parent && notify && oldOrder != this->m_order) {
        this->m_parent->NotifyOrderChanged(this);
    }
}

// OFFSET: 0x00529A70/0x1401796E0
CSimpleControlPoint::CSimpleControlPoint(CSimplePathAnim* parent) {
    this->m_position = 0.0f;
    this->m_order = -1;
    this->m_parent = parent;

    if (parent) {
        parent->m_controlPoints.LinkToTail(this);
    }
}

// OFFSET: 0x00529F30/0x140179D70
bool CSimpleControlPoint::ChangeParent(CSimplePathAnim* parent) {
    if (!parent || this->m_parent == parent) {
        return false;
    }

    parent->m_controlPoints.LinkToTail(this);

    if (this->m_parent) {
        this->m_parent->NotifyControlPointRemoved(this);
    }

    this->m_parent = parent;
    parent->NotifyControlPointAdded(this);

    return true;
}

// OFFSET: 0x007C3BC0/0x140177130
CScriptObject* CSimpleControlPoint::GetScriptObjectParent() {
    return this->m_parent;
}
