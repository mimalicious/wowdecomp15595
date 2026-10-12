#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleScaleAnim.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleAnimGroup.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/FrameXML/LoadXML.h"

// STATIC: 0x00CA476C/0x140B427DC
int32_t CSimpleScaleAnim::s_metatable = LUA_NOREF;

// STATIC: 0x00DD3CC0/0x140CB9A60
int32_t CSimpleScaleAnim::s_objectType;

// STATIC: 0x00CA382C/0x140B40AA8
const char* CSimpleScaleAnim::s_objectTypeName = "Scale";

// OFFSET: 0x00528590/0x140177AC0
bool CSimpleScaleAnim::IsA(int32_t type) {
    return type == CSimpleScaleAnim::s_objectType || type == CSimpleAnim::s_objectType || type == CScriptObject::s_objectType;
}

// OFFSET: 0x005285C0/0x140177AE0
bool CSimpleScaleAnim::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleScaleAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CSimpleAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// OFFSET: 0x00528620/0x140177B40
const char* CSimpleScaleAnim::GetObjectTypeName() {
    return CSimpleScaleAnim::s_objectTypeName;
}

// OFFSET: 0x00528630/0x140177B50
int32_t CSimpleScaleAnim::GetScriptMetaTable() {
    return CSimpleScaleAnim::s_metatable;
}

// OFFSET: 0x00527060/0x1401763A0
void CSimpleScaleAnim::SetScale(const C2Vector& scale) {
    this->m_scaleChange.x = 1.0f - (scale.x >= 0.001f ? scale.x : 0.001f);
    this->m_scaleChange.y = 1.0f - (scale.y >= 0.001f ? scale.y : 0.001f);
}

// OFFSET: 0x005279F0/0x140176D90
void CSimpleScaleAnim::GetScale(C2Vector& scale) {
    scale.x = 1.0f - this->m_scaleChange.x;
    scale.y = 1.0f - this->m_scaleChange.y;
}

// OFFSET: 0x00527A20/0x140176DC0
void CSimpleScaleAnim::Apply(float progress) {
    CScriptRegion* region = this->GetRegionParent();

    if (region) {
        C2Vector scale;
        scale.x = this->m_scaleChange.x * progress;
        scale.y = this->m_scaleChange.y * progress;
        region->AddAnimScale(this->m_target, this->m_origin, this->m_originOffset, scale);
    }
}

// OFFSET: 0x00529380/0x140178D00
CSimpleScaleAnim::~CSimpleScaleAnim() {
}

// OFFSET: 0x0052A6B0/0x14017A750
CSimpleScaleAnim::CSimpleScaleAnim(CSimpleAnimGroup* group) : CSimpleAnim(group) {
    this->m_origin = FRAMEPOINT_CENTER;
}

// OFFSET: 0x0052B3A0/0x14017B9E0
void CSimpleScaleAnim::LoadXML(linkObject* node, CStatus* status) {
    VectorAnimation* animation = static_cast<VectorAnimation*>(node);

    C2Vector scale;

    if (animation->m_vectorFlags & 1) {
        scale.x = animation->m_x;

        if (scale.x < 0.001f) {
            const char* name = this->GetName();
            status->Add(STATUS_WARNING, "%s: Invalid scaleX value: %f. Value must be at least %d.", name ? name : "<unnamed>", scale.x, 0.001f);
        }
    }

    if (animation->m_vectorFlags & 2) {
        scale.y = animation->m_y;

        if (scale.y < 0.001f) {
            const char* name = this->GetName();
            status->Add(STATUS_WARNING, "%s: Invalid scaleY value: %f. Value must be at least %d.", name ? name : "<unnamed>", scale.y, 0.001f);
        }
    }

    this->SetScale(scale);

    CSimpleAnim::LoadXML(node, status);

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        if (child->m_element == XML_ORIGIN && !LoadXML_AnimOrigin(child, this->m_origin, this->m_originOffset, status)) {
            const char* name = this->GetName();
            status->Add(STATUS_WARNING, "%s %s: Error loading Origin element", this->GetObjectTypeName(), name ? name : "<unnamed>");
        }
    }
}
