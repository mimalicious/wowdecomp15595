#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CSimpleAlphaAnim.h"
#include "Engine/Source/Frame/CSimpleAnimGroup.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: %0x00CD12EC
int32_t CScriptRegion::s_objectType;

// STATIC: %0x00BB860C
const char* CScriptRegion::s_objectTypeName = "Region";

// PROVEN: e23865aa IDENTICAL/DIFFERENT
// OFFSET: 0x00469450/0x14013FFC0
CScriptObject* CScriptRegion::GetScriptObjectParent() {
    return this->m_parent;
}

// PROVEN: 70c65224 IDENTICAL/IDENTICAL
// OFFSET: 0x00469460/0x14007D680
bool CScriptRegion::IsA(int32_t type) {
    return type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// PROVEN: b3363a79 IDENTICAL/DIFFERENT
// OFFSET: 0x00469490/0x1401531B0
bool CScriptRegion::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CScriptRegion::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// PROVEN: ba11daed IDENTICAL/NONE
// OFFSET: 0x005025F0
const char* CScriptRegion::GetObjectTypeName() {
    return CScriptRegion::s_objectTypeName;
}

// OFFSET: %0x0051FF30
CScriptRegion::CScriptRegion() {
    this->m_parent = NULL;
    this->m_animGroups = NULL;
    this->m_playingAnimGroups = NULL;
}

// PROVEN: 17d84e01 IDENTICAL/IDENTICAL
// OFFSET: 0x0050FBA0/0x140158F50
bool CScriptRegion::IsDragging() {
    return this->m_parent ? this->m_parent->IsDragging() : false;
}

// PROVEN: 95f2f5a3 IDENTICAL/NONE
// OFFSET: 0x005025E0
void CScriptRegion::SetParent(CSimpleFrame* parent) {
    this->m_parent = parent;
}

// OFFSET: %0x0051FFA0
void CScriptRegion::LoadXML_Animations(linkObject* node, CStatus* status) {
    linkObject* animations = node->GetChild(XML_ANIMATIONS);

    if (!animations) {
        return;
    }

    for (linkObject* child = animations->m_child; child; child = child->m_next) {
        if (child->m_element == XML_ANIMATIONGROUP) {
            LoadXML_AnimGroup(child, this, status);
        } else {
            const char* name = this->GetName();
            status->Add(STATUS_WARNING, "%s %s: Unknown child node in %s element: %s", this->GetObjectTypeName(), name ? name : "<unnamed>", animations->m_type.m_string, child->m_type.m_string);
        }
    }
}

// PROVEN: 92f1b045 IDENTICAL/DIFFERENT
// OFFSET: 0x0050FCA0/0x140159090
CLayoutFrame* CScriptRegion::GetLayoutParent() {
    CLayoutFrame* parent = this->m_parent;
    return parent ? parent : CSimpleTop::s_instance;
}

// PROVEN: d8e36992 IDENTICAL/IDENTICAL
// OFFSET: 0x0050FCC0/0x1401590B0
CLayoutFrame* CScriptRegion::GetLayoutFrameByName(const char* name) {
    char fullName[1024];
    this->CreateName(name, fullName, sizeof(fullName));

    CScriptRegion* region = static_cast<CScriptRegion*>(CScriptObject::GetScriptObjectByName(fullName, CScriptRegion::GetObjectType()));
    return region;
}

// OFFSET: %0x005200F0
bool CScriptRegion::ProtectedFunctionsAllowed() {
    return !this->IsProtected() || !lua_tainted || CSimpleTop::s_instance->m_allowProtectedActions != 0;
}

// OFFSET: %0x00520120
bool CScriptRegion::IsMouseOver(float top, float bottom, float left, float right) {
    CSimpleTop* simpleTop = CSimpleTop::s_instance;

    if (!simpleTop) {
        return false;
    }

    if (this->IsResizePending()) {
        this->Resize(1);
    }

    CRect rect = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (!this->GetRect(&rect)) {
        return false;
    }

    float ddcX = 0.0f;
    float ddcY = 0.0f;
    NDCToDDC(simpleTop->m_mousePosition.m_x, simpleTop->m_mousePosition.m_y, &ddcX, &ddcY);

    float x = 1.0f / this->m_layoutScale * ddcX;
    float y = 1.0f / this->m_layoutScale * ddcY;
    float scale = 1.0f / this->m_layoutScale;

    return rect.minX * scale + left < x && rect.maxX * scale + right > x && scale * rect.minY + bottom < y && scale * rect.maxY + top > y;
}

// OFFSET: %0x00520240
void CScriptRegion::LoadXML(linkObject* node, CStatus* status) {
    CLayoutFrame::LoadXML(node, status);

    Region* region = static_cast<Region*>(node);

    if (region->m_regionFlags & 4) {
        const char* parentKey = region->m_parentKey.m_string;
        lua_State* L = FrameScript_GetContext();
        CScriptObject* parent = this->GetScriptObjectParent();

        if (parent) {
            if (!parent->m_luaRegistered) {
                parent->RegisterScriptObject(NULL);
            }

            lua_rawgeti(L, LUA_REGISTRYINDEX, parent->m_luaObjectRef);
            lua_pushstring(L, parentKey);

            if (!this->m_luaRegistered) {
                this->RegisterScriptObject(NULL);
            }

            lua_rawgeti(L, LUA_REGISTRYINDEX, this->m_luaObjectRef);
            lua_settable(L, -3);
            lua_settop(L, -2);
        }
    }

    this->LoadXML_Animations(node, status);
}

// PROVEN: 89deea3a IDENTICAL/IDENTICAL
// OFFSET: 0x0050FF30/0x1401593D0
int32_t CScriptRegion::HasActiveAnimGroup() {
    for (CScriptRegion* region = this; region; region = region->m_parent) {
        if (region->m_playingAnimGroups && region->m_playingAnimGroups->Head()) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x0050FFA0/0x140159480
int32_t CScriptRegion::HasActiveAlphaAnim() {
    for (CScriptRegion* region = this; region; region = region->m_parent) {
        if (region->m_playingAnimGroups && region->m_playingAnimGroups->Head()) {
            for (CSimpleAnimGroup* group = region->m_playingAnimGroups->Head(); !region->m_playingAnimGroups->IsEnd(group); group = region->m_playingAnimGroups->RawNext(group)) {
                if (group->HasAlphaAnim()) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

// OFFSET: 0x00510010/0x140159510
int32_t CScriptRegion::HasActivePositionAnim() {
    for (CScriptRegion* region = this; region; region = region->m_parent) {
        if (region->m_playingAnimGroups && region->m_playingAnimGroups->Head()) {
            for (CSimpleAnimGroup* group = region->m_playingAnimGroups->Head(); !region->m_playingAnimGroups->IsEnd(group); group = region->m_playingAnimGroups->RawNext(group)) {
                if (group->HasPositionAnim()) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

// OFFSET: %0x00520490
void CScriptRegion::OnLayerUpdate(float elapsedSec) {
    if (!this->m_animGroups) {
        return;
    }

    for (CSimpleAnimGroup* group = this->m_animGroups->Head(); group; group = this->m_animGroups->Next(group)) {
        group->OnUpdate(elapsedSec);
    }
}

// OFFSET: %0x005204F0
void CScriptRegion::StopAnimating() {
    if (!this->m_playingAnimGroups) {
        return;
    }

    for (CSimpleAnimGroup* group = this->m_playingAnimGroups->Head(); group; group = this->m_playingAnimGroups->Next(group)) {
        group->Stop(true);
    }
}

// OFFSET: 0x00510140
void CScriptRegion::NotifyAnimEnd(CSimpleAnimGroup* group) {
    if (!group || !this->m_playingAnimGroups) {
        return;
    }

    this->m_playingAnimGroups->UnlinkNode(group);
    int32_t alpha = this->AnimData_HasActiveAlpha() && !this->HasActiveAlphaAnim();
    int32_t position = this->AnimData_HasActivePosition() && !this->HasActivePositionAnim();
    this->AnimDeactivated(group, alpha, position);
}

// OFFSET: 0x005101C0
void CScriptRegion::NotifyAnimBegin(CSimpleAnimGroup* group) {
    if (!group) {
        return;
    }

    if (!this->m_playingAnimGroups) {
        this->m_playingAnimGroups = new (SMemAlloc(sizeof(*this->m_playingAnimGroups), __FILE__, __LINE__, 0)) TSExplicitList<CSimpleAnimGroup, offsetof(CSimpleAnimGroup, m_playingLink)>;

        if (!this->m_playingAnimGroups) {
            return;
        }
    }

    int32_t alpha = !this->AnimData_HasActiveAlpha() && group->HasAlphaAnim();
    int32_t position = !this->AnimData_HasActivePosition() && group->HasPositionAnim();
    this->AnimActivated(group, alpha, position);
    this->m_playingAnimGroups->LinkToTail(group);
}

// OFFSET: %0x005206A0
void CScriptRegion::RegisterAnimGroup(CSimpleAnimGroup* group) {
    if (!group) {
        return;
    }

    if (!this->m_animGroups) {
        this->m_animGroups = new (SMemAlloc(sizeof(*this->m_animGroups), __FILE__, __LINE__, 0)) TSExplicitList<CSimpleAnimGroup, offsetof(CSimpleAnimGroup, m_regionLink)>;
    }

    if (this->m_animGroups) {
        this->m_animGroups->LinkToTail(group);
    }
}

// OFFSET: [%0x0051FF90, %0x00520710, %0x00520770]
CScriptRegion::~CScriptRegion() {
    if (this->m_animGroups) {
        this->m_animGroups->Clear();
        delete this->m_animGroups;
    }

    if (this->m_playingAnimGroups) {
        delete this->m_playingAnimGroups;
    }
}

// PROVEN: 4d845176 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CScriptRegion::PreOnAnimUpdate() {
}

// PROVEN: 624067b4 IDENTICAL/DIFFERENT
// OFFSET: 0x00627990/0x140693B10
void CScriptRegion::AnimActivated(CSimpleAnimGroup* group, int32_t alpha, int32_t position) {
}

// PROVEN: d026b161 IDENTICAL/DIFFERENT
// OFFSET: 0x00841AE0/0x140211B90
void CScriptRegion::AnimDeactivated(CSimpleAnimGroup* group, int32_t alpha, int32_t position) {
}

// PROVEN: 520f3671 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CScriptRegion::AddAnimTranslation(CScriptRegion* region, const C2Vector& translation) {
}

// PROVEN: 026f6a8a IDENTICAL/DIFFERENT
// OFFSET: 0x00627990/0x140693B10
void CScriptRegion::AddAnimRotation(CScriptRegion* region, FRAMEPOINT origin, const C2Vector& offset, float radians) {
}

// PROVEN: 4414fe0c IDENTICAL/DIFFERENT
// OFFSET: 0x00627990/0x140693B10
void CScriptRegion::AddAnimScale(CScriptRegion* region, FRAMEPOINT origin, const C2Vector& offset, const C2Vector& scale) {
}

// PROVEN: 7108781d IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CScriptRegion::AddAnimAlpha(CScriptRegion* region, int16_t alpha) {
}
