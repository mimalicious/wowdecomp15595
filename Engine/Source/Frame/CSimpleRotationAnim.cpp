#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleRotationAnim.h"
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

// STATIC: %0x00BB9A80
int32_t CSimpleRotationAnim::s_metatable = LUA_NOREF;

// STATIC: 0x00DD3CB4
int32_t CSimpleRotationAnim::s_objectType;

// STATIC: %0x00BB8BB0
const char* CSimpleRotationAnim::s_objectTypeName = "Rotation";

// PROVEN: 03507ea6 IDENTICAL/IDENTICAL
// OFFSET: 0x005284E0/0x140177A20
bool CSimpleRotationAnim::IsA(int32_t type) {
    return type == CSimpleRotationAnim::s_objectType || type == CSimpleAnim::s_objectType || type == CScriptObject::s_objectType;
}

// PROVEN: 25fa666a IDENTICAL/IDENTICAL
// OFFSET: 0x00528510/0x140177A40
bool CSimpleRotationAnim::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleRotationAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CSimpleAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// PROVEN: a77f96ee IDENTICAL/IDENTICAL
// OFFSET: 0x00528570/0x140177AA0
const char* CSimpleRotationAnim::GetObjectTypeName() {
    return CSimpleRotationAnim::s_objectTypeName;
}

// PROVEN: 2b67fd3a IDENTICAL/IDENTICAL
// OFFSET: 0x00528580/0x140177AB0
int32_t CSimpleRotationAnim::GetScriptMetaTable() {
    return CSimpleRotationAnim::s_metatable;
}

// OFFSET: %0x00537A20
void CSimpleRotationAnim::Apply(float progress) {
    CScriptRegion* region = this->GetRegionParent();

    if (region) {
        region->AddAnimRotation(this->m_target, this->m_origin, this->m_originOffset, this->m_radians * progress);
    }
}

// OFFSET: %0x00538F80
CSimpleRotationAnim::~CSimpleRotationAnim() {
}

// OFFSET: %0x00539650
CSimpleRotationAnim::CSimpleRotationAnim(CSimpleAnimGroup* group) : CSimpleAnim(group) {
    this->m_origin = FRAMEPOINT_CENTER;
    this->m_radians = 0.0f;
}

// PROVEN: 762dd731 IDENTICAL/DIFFERENT
// OFFSET: 0x0052B2E0/0x14017B8E0
void CSimpleRotationAnim::LoadXML(linkObject* node, CStatus* status) {
    VectorAnimation* animation = static_cast<VectorAnimation*>(node);

    if (animation->m_vectorFlags & 1) {
        this->m_radians = animation->m_x * 0.017453292f;
    }

    if (animation->m_vectorFlags & 2) {
        this->m_radians = animation->m_y;
    }

    CSimpleAnim::LoadXML(node, status);

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        if (child->m_element == XML_ORIGIN && !LoadXML_AnimOrigin(child, this->m_origin, this->m_originOffset, status)) {
            const char* name = this->GetName();
            status->Add(STATUS_WARNING, "%s %s: Error loading Origin element", this->GetObjectTypeName(), name ? name : "<unnamed>");
        }
    }
}
