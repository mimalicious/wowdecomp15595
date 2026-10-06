#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleTranslationAnim.h"
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

// STATIC: %0x00BB9A4C
int32_t CSimpleTranslationAnim::s_metatable = LUA_NOREF;

// STATIC: %0x00CD2810
int32_t CSimpleTranslationAnim::s_objectType;

// STATIC: %0x00BB8BAC
const char* CSimpleTranslationAnim::s_objectTypeName = "Translation";

// PROVEN: 16876655 IDENTICAL/IDENTICAL
// OFFSET: 0x00528430/0x140177980
bool CSimpleTranslationAnim::IsA(int32_t type) {
    return type == CSimpleTranslationAnim::s_objectType || type == CSimpleAnim::s_objectType || type == CScriptObject::s_objectType;
}

// PROVEN: bd46feb1 IDENTICAL/IDENTICAL
// OFFSET: 0x00528460/0x1401779A0
bool CSimpleTranslationAnim::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleTranslationAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CSimpleAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// PROVEN: eff1da0f IDENTICAL/IDENTICAL
// OFFSET: 0x005284C0/0x140177A00
const char* CSimpleTranslationAnim::GetObjectTypeName() {
    return CSimpleTranslationAnim::s_objectTypeName;
}

// PROVEN: 1b7686a6 IDENTICAL/IDENTICAL
// OFFSET: 0x005284D0/0x140177A10
int32_t CSimpleTranslationAnim::GetScriptMetaTable() {
    return CSimpleTranslationAnim::s_metatable;
}

// OFFSET: %0x00537960
void CSimpleTranslationAnim::SetOffset(const C2Vector& offset) {
    if (offset.x * offset.x + offset.y * offset.y <= 0.00000023841858f) {
        this->m_offset.x = 0.0f;
        this->m_offset.y = 0.0f;
    } else {
        this->m_offset = offset;
    }
}

// OFFSET: %0x005379C0
void CSimpleTranslationAnim::Apply(float progress) {
    CScriptRegion* region = this->GetRegionParent();

    if (region) {
        C2Vector translation;
        translation.x = this->m_offset.x * progress;
        translation.y = this->m_offset.y * progress;
        region->AddAnimTranslation(this->m_target, translation);
    }
}

// OFFSET: %0x00538F50
CSimpleTranslationAnim::~CSimpleTranslationAnim() {
}

// OFFSET: %0x00539620
CSimpleTranslationAnim::CSimpleTranslationAnim(CSimpleAnimGroup* group) : CSimpleAnim(group) {
}

// OFFSET: %0x0053B280
void CSimpleTranslationAnim::LoadXML(linkObject* node, CStatus* status) {
    VectorAnimation* animation = static_cast<VectorAnimation*>(node);

    C2Vector offset;

    if (animation->m_vectorFlags & 1) {
        offset.x = NDCToDDCWidth(animation->m_x / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    if (animation->m_vectorFlags & 2) {
        offset.y = NDCToDDCWidth(animation->m_y / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    this->SetOffset(offset);

    CSimpleAnim::LoadXML(node, status);
}
