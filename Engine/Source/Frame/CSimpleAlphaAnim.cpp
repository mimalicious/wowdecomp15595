#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleAlphaAnim.h"
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

// STATIC: %0x00BB9B10
int32_t CSimpleAlphaAnim::s_metatable = LUA_NOREF;

// STATIC: %0x00CD27FC
int32_t CSimpleAlphaAnim::s_objectType;

// STATIC: %0x00BB8BC0
const char* CSimpleAlphaAnim::s_objectTypeName = "Alpha";

// PROVEN: 224a22a3 IDENTICAL/IDENTICAL
// OFFSET: 0x00528D50/0x140178490
bool CSimpleAlphaAnim::IsA(int32_t type) {
    return type == CSimpleAlphaAnim::s_objectType || type == CSimpleAnim::s_objectType || type == CScriptObject::s_objectType;
}

// PROVEN: 18f43c30 IDENTICAL/IDENTICAL
// OFFSET: 0x00528D80/0x1401784B0
bool CSimpleAlphaAnim::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleAlphaAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CSimpleAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// PROVEN: 960bec46 IDENTICAL/IDENTICAL
// OFFSET: 0x00528DE0/0x140178510
const char* CSimpleAlphaAnim::GetObjectTypeName() {
    return CSimpleAlphaAnim::s_objectTypeName;
}

// PROVEN: 1743e0a6 IDENTICAL/IDENTICAL
// OFFSET: 0x00528DF0/0x140178520
int32_t CSimpleAlphaAnim::GetScriptMetaTable() {
    return CSimpleAlphaAnim::s_metatable;
}

// OFFSET: %0x00537080
void CSimpleAlphaAnim::SetChange(float change) {
    float value = change < -1.0f ? -1.0f : (change >= 1.0f ? 1.0f : change);
    this->m_change = static_cast<int16_t>(value * 255.0f);
}

// OFFSET: %0x00537C10
void CSimpleAlphaAnim::Apply(float progress) {
    CScriptRegion* region = this->GetRegionParent();

    if (region) {
        region->AddAnimAlpha(this->m_target, static_cast<int16_t>(this->m_change * progress));
    }
}

// OFFSET: %0x005393B0
CSimpleAlphaAnim::~CSimpleAlphaAnim() {
}

// OFFSET: %0x005399A0
CSimpleAlphaAnim::CSimpleAlphaAnim(CSimpleAnimGroup* group) : CSimpleAnim(group) {
    this->m_change = 0;
}

// OFFSET: %0x0053B6C0
void CSimpleAlphaAnim::LoadXML(linkObject* node, CStatus* status) {
    CSimpleAnim::LoadXML(node, status);

    AlphaAnimation* animation = static_cast<AlphaAnimation*>(node);
    float change = 0.0f;

    if (animation->m_alphaFlags & 1) {
        change = animation->m_change;

        if (change < -1.0f || change > 1.0f) {
            const char* name = this->GetName();
            status->Add(STATUS_WARNING, "%s: Invalid change value: %f. Value must be between %d and %d, inclusive.", name ? name : "<unnamed>", animation->m_change, -1.0f, 1.0f);
            change = 0.0f;
        }
    }

    this->SetChange(change);
}
