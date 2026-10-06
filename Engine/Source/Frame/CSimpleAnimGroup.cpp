#include <cstddef>
#include <cstdint>
#include <new>
#include "Engine/Source/Frame/CSimpleAnimGroup.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleAlphaAnim.h"
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimplePathAnim.h"
#include "Engine/Source/Frame/CSimpleRotationAnim.h"
#include "Engine/Source/Frame/CSimpleScaleAnim.h"
#include "Engine/Source/Frame/CSimpleTranslationAnim.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/FrameXML/FrameXML.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: %0x00BB9B14
int32_t CSimpleAnimGroup::s_metatable = LUA_NOREF;

// STATIC: %0x00CD27F8
int32_t CSimpleAnimGroup::s_objectType;

// STATIC: %0x00BB8BC4
const char* CSimpleAnimGroup::s_objectTypeName = "AnimationGroup";

// OFFSET: %0x005370C0
void CSimpleAnimGroup::PreLoadXML(linkObject* node, CStatus* status) {
    this->m_flags |= 1;

    if (node->m_flags & 0x1000000) {
        this->SetName(node->m_name.m_string);
    }
}

// INLINED
void CSimpleAnimGroup::LoadXML_Anim(linkObject* node, CStatus* status, CSimpleAnim* anim) {
    if (!anim) {
        return;
    }

    anim->m_flags |= 1;

    if (node->m_flags & 0x1000000) {
        anim->SetName(node->m_name.m_string);
    }

    anim->LoadXML(node, status);
}

// OFFSET: %0x005370F0
void CSimpleAnimGroup::LoadXML_Scripts(linkObject* node, CStatus* status) {
    lua_State* L = FrameScript_GetContext();

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        if (child->m_flags & 0x20) {
            this->LoadFunction(L, static_cast<Function*>(child), status);
        }
    }
}

// OFFSET: %0x00537140
FrameScript_Object::ScriptIx* CSimpleAnimGroup::GetScriptByName(const char* name, FrameScript_Object::ScriptData& data) {
    FrameScript_Object::ScriptIx* script = this->FrameScript_Object::GetScriptByName(name, data);

    if (script) {
        return script;
    }

    if (!SStrCmp(name, "onload", STORM_MAX_STR)) {
        return &this->m_onLoad;
    }

    if (!SStrCmp(name, "onplay", STORM_MAX_STR)) {
        return &this->m_onPlay;
    }

    if (!SStrCmp(name, "onpause", STORM_MAX_STR)) {
        return &this->m_onPause;
    }

    if (!SStrCmp(name, "onstop", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,requested) %s end";
        return &this->m_onStop;
    }

    if (!SStrCmp(name, "onfinished", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,requested) %s end";
        return &this->m_onFinished;
    }

    if (!SStrCmp(name, "onupdate", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,elapsed) %s end";
        return &this->m_onUpdate;
    }

    if (!SStrCmp(name, "onloop", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,loopState) %s end";
        return &this->m_onLoop;
    }

    return NULL;
}

// OFFSET: %0x00537330
bool CSimpleAnimGroup::IsDone() {
    if (this->m_duration <= 0.0001f) {
        return true;
    }

    if (this->m_loopType == ANIM_LOOPTYPE_REPEAT || this->m_loopType == ANIM_LOOPTYPE_BOUNCE) {
        return false;
    }

    if (this->m_loopState == ANIM_LOOPSTATE_FORWARD || this->m_loopState == ANIM_LOOPSTATE_REVERSE) {
        return false;
    }

    return this->m_progress >= 1.0f;
}

// OFFSET: %0x00537380
void CSimpleAnimGroup::SetInitialOffset(const C2Vector& offset) {
    if (offset.x * offset.x + offset.y * offset.y <= 0.00000023841858f) {
        this->m_initialOffset.x = 0.0f;
        this->m_initialOffset.y = 0.0f;
    } else {
        this->m_initialOffset = offset;
    }
}

// OFFSET: %0x005375E0
void CSimpleAnimGroup::RunOnFinishedScript(bool requested) {
    if (this->HasScript(this->m_onFinished)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushboolean(L, requested);
        this->RunScript(this->m_onFinished, 1, NULL);
    }
}

// OFFSET: %0x00537630
void CSimpleAnimGroup::RunOnUpdateScript(float elapsedSec) {
    if (this->HasScript(this->m_onUpdate)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushnumber(L, elapsedSec);
        this->RunScript(this->m_onUpdate, 1, NULL);
    }
}

// OFFSET: %0x00537C50
void CSimpleAnimGroup::Finish() {
    if (!this->IsDone() && this->m_loopState) {
        this->m_flags |= 2;
    }
}

// OFFSET: %0x00537CA0
void CSimpleAnimGroup::RunOnLoopScript(SimpleAnimLoopState state) {
    if (this->HasScript(this->m_onLoop)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushstring(L, LoopStateToString(state));
        this->RunScript(this->m_onLoop, 1, NULL);
    }
}

// OFFSET: %0x00538170
int32_t CSimpleAnimGroup::GetMaxAnimOrder() {
    if (!this->m_animLists.Count()) {
        return -1;
    }

    return this->m_animLists[this->m_animLists.Count() - 1]->Head()->m_anim->m_order;
}

// OFFSET: %0x005381A0
TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* CSimpleAnimGroup::FindAnimList(int32_t order) {
    if (order == -1) {
        return NULL;
    }

    int32_t low = 0;
    int32_t high = this->m_animLists.Count() - 1;

    while (low <= high) {
        uint32_t middle = static_cast<uint32_t>(high + low) >> 1;
        TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* list = this->m_animLists[middle];

        if (list->Head()) {
            int32_t listOrder = list->Head()->m_anim->m_order;

            if (listOrder < order) {
                low = middle + 1;
            } else if (listOrder > order) {
                high = middle - 1;
            } else {
                return list;
            }
        }
    }

    return NULL;
}

// OFFSET: %0x00538EC0
bool CSimpleAnimGroup::HasAnimOfType(int32_t type) {
    for (CSimpleAnim* anim = this->m_animations.Head(); anim; anim = this->m_animations.Next(anim)) {
        if (anim->IsA(type)) {
            return true;
        }
    }

    return false;
}

// OFFSET: %0x005393E0
TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* CSimpleAnimGroup::RegisterAnimList(int32_t order) {
    if (order == -1) {
        return NULL;
    }

    uint32_t count = this->m_animLists.Count();

    if (!count) {
        TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >** slot = this->m_animLists.New();

        if (!slot) {
            return NULL;
        }

        void* mem = SMemAlloc(sizeof(TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >), __FILE__, __LINE__, 0);
        *slot = mem ? new (mem) TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >() : NULL;

        return *slot;
    }

    uint32_t index = count;

    for (uint32_t i = 0; i < count; i++) {
        TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* list = this->m_animLists[i];
        SIMPLEANIMNODE* head = list->Head();

        if (!head) {
            return list;
        }

        int32_t listOrder = head->m_anim->m_order;

        if (order == listOrder) {
            return list;
        }

        if (order < listOrder) {
            index = i;
            break;
        }
    }

    if (!this->m_animLists.New()) {
        return NULL;
    }

    for (uint32_t i = this->m_animLists.Count() - 1; i > index; i--) {
        this->m_animLists[i] = this->m_animLists[i - 1];
    }

    void* mem = SMemAlloc(sizeof(TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >), __FILE__, __LINE__, 0);
    this->m_animLists[index] = mem ? new (mem) TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >() : NULL;

    return this->m_animLists[index];
}

// OFFSET: %0x005399D0
CSimpleAnimGroup::CSimpleAnimGroup(CScriptRegion* owner) {
    this->m_region = NULL;
    this->unk_005C = 0;
    this->m_loopType = ANIM_LOOPTYPE_NONE;
    this->m_duration = 0.0f;
    this->m_initialOffset.x = 0.0f;
    this->m_initialOffset.y = 0.0f;
    this->m_loopState = ANIM_LOOPSTATE_NONE;
    this->m_flags = 0;
    this->m_currentOrder = -1;
    this->m_elapsed = 0.0f;
    this->m_progress = 0.0f;

    if (owner) {
        this->m_region = owner;
        owner->RegisterAnimGroup(this);
    }

    this->m_animations.Clear();
    this->m_animLists.Clear();
}

// OFFSET: %0x00A71500
CScriptObject* CSimpleAnimGroup::GetScriptObjectParent() {
    return this->m_region;
}

// OFFSET: %0x00539AB0
bool CSimpleAnimGroup::IsA(int32_t type) {
    return type == CSimpleAnimGroup::s_objectType || type == CScriptObject::s_objectType;
}

// OFFSET: %0x00539AE0
bool CSimpleAnimGroup::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleAnimGroup::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// OFFSET: %0x00539B20
const char* CSimpleAnimGroup::GetObjectTypeName() {
    return CSimpleAnimGroup::s_objectTypeName;
}

// OFFSET: %0x00539B30
int32_t CSimpleAnimGroup::GetScriptMetaTable() {
    return CSimpleAnimGroup::s_metatable;
}

// OFFSET: %0x00539B40
void CSimpleAnimGroup::LoadXML(linkObject* node, CStatus* status) {
    AnimationGroup* group = static_cast<AnimationGroup*>(node);

    this->m_flags |= 1;

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

    if (group->m_groupFlags & 4) {
        lua_State* L = FrameScript_GetContext();
        CScriptObject* parent = this->GetScriptObjectParent();

        if (parent) {
            if (!parent->m_luaRegistered) {
                parent->RegisterScriptObject(NULL);
            }

            lua_rawgeti(L, LUA_REGISTRYINDEX, parent->m_luaObjectRef);
            lua_pushstring(L, group->m_parentKey.m_string);

            if (!this->m_luaRegistered) {
                this->RegisterScriptObject(NULL);
            }

            lua_rawgeti(L, LUA_REGISTRYINDEX, this->m_luaObjectRef);
            lua_settable(L, -3);
            lua_settop(L, -2);
        }
    }

    if (group->m_groupFlags & 8) {
        const char* looping = group->m_looping.m_string;
        SimpleAnimLoopType loopType;

        if (StringToLoopType(looping, loopType)) {
            this->m_loopType = loopType;
        } else {
            const char* name = this->GetName();
            status->Add(STATUS_WARNING, "%s %s: Invalid looping value: %s", this->GetObjectTypeName(), name ? name : "<unnamed>", looping);
        }
    }

    float offsetX = 0.0f;
    float offsetY = 0.0f;

    if (group->m_groupFlags & 1) {
        offsetX = NDCToDDCWidth(group->m_initialOffsetX / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    if (group->m_groupFlags & 2) {
        offsetY = NDCToDDCWidth(group->m_initialOffsetY / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    C2Vector offset;
    offset.x = offsetX;
    offset.y = offsetY;
    this->SetInitialOffset(offset);

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        switch (child->m_element) {
            case XML_SCRIPTS:
                this->LoadXML_Scripts(child, status);
                break;

            case XML_ANIMATION: {
                void* mem = SMemAlloc(sizeof(CSimpleAnim), __FILE__, __LINE__, 0);
                this->LoadXML_Anim(child, status, mem ? new (mem) CSimpleAnim(this) : NULL);
                break;
            }

            case XML_TRANSLATION: {
                void* mem = SMemAlloc(sizeof(CSimpleTranslationAnim), __FILE__, __LINE__, 0);
                this->LoadXML_Anim(child, status, mem ? new (mem) CSimpleTranslationAnim(this) : NULL);
                break;
            }

            case XML_ROTATION: {
                void* mem = SMemAlloc(sizeof(CSimpleRotationAnim), __FILE__, __LINE__, 0);
                this->LoadXML_Anim(child, status, mem ? new (mem) CSimpleRotationAnim(this) : NULL);
                break;
            }

            case XML_SCALE: {
                void* mem = SMemAlloc(sizeof(CSimpleScaleAnim), __FILE__, __LINE__, 0);
                this->LoadXML_Anim(child, status, mem ? new (mem) CSimpleScaleAnim(this) : NULL);
                break;
            }

            case XML_PATH: {
                void* mem = SMemAlloc(sizeof(CSimplePathAnim), __FILE__, __LINE__, 0);
                this->LoadXML_Anim(child, status, mem ? new (mem) CSimplePathAnim(this) : NULL);
                break;
            }

            case XML_ALPHA: {
                void* mem = SMemAlloc(sizeof(CSimpleAlphaAnim), __FILE__, __LINE__, 0);
                this->LoadXML_Anim(child, status, mem ? new (mem) CSimpleAlphaAnim(this) : NULL);
                break;
            }
        }
    }
}

// OFFSET: %0x0053A0C0
bool CSimpleAnimGroup::AddAnimListNode(TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* list, CSimpleAnim* anim) {
    if (!list) {
        return false;
    }

    SIMPLEANIMNODE* node = list->NewNode(2, 0, 0);

    if (!node) {
        return false;
    }

    node->m_anim = anim;
    return true;
}

// OFFSET: [%0x0053A250, %0x0053A9F0]
CSimpleAnimGroup::~CSimpleAnimGroup() {
    for (uint32_t i = 0; i < this->m_animLists.Count(); i++) {
        this->m_animLists[i]->Clear();
        delete this->m_animLists[i];
    }

    this->m_animLists.Clear();
    this->m_animations.Clear();
}

// OFFSET: %0x0053A3E0
void CSimpleAnimGroup::Pause() {
    if (!this->m_loopState || (this->m_flags & 0x4)) {
        return;
    }

    this->m_flags |= 0x4;

    TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* list = this->m_animLists[this->m_currentOrder];

    for (SIMPLEANIMNODE* node = list->Head(); node; node = list->Next(node)) {
        node->m_anim->Pause();
    }

    if (this->HasScript(this->m_onPause)) {
        this->RunScript(this->m_onPause, 0, NULL);
    }
}

// OFFSET: %0x0053A4A0
void CSimpleAnimGroup::Stop(bool requested) {
    if (!this->m_loopState || (this->m_flags & 0x20)) {
        return;
    }

    this->m_flags |= 0x20;

    TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* list = this->m_animLists[this->m_currentOrder];

    for (SIMPLEANIMNODE* node = list->Head(); node; node = list->Next(node)) {
        node->m_anim->Stop(requested);
    }

    if (this->HasScript(this->m_onStop)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushboolean(L, requested);
        this->RunScript(this->m_onStop, 1, NULL);
    }

    this->m_region->NotifyAnimEnd(this);

    this->m_loopState = ANIM_LOOPSTATE_NONE;
    this->m_flags = 0;
    this->m_currentOrder = -1;
    this->m_elapsed = 0.0f;
    this->m_progress = 0.0f;
}

// OFFSET: %0x0053A5E0
void CSimpleAnimGroup::ComputeDuration() {
    this->m_duration = 0.0f;

    for (uint32_t i = 0; i < this->m_animLists.Count(); i++) {
        float duration = 0.0f;
        TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* list = this->m_animLists[i];

        for (SIMPLEANIMNODE* node = list->Head(); node; node = list->Next(node)) {
            CSimpleAnim* anim = node->m_anim;
            float total = anim->m_duration + anim->m_endDelay + anim->m_startDelay;

            if (total > duration) {
                duration = total;
            }
        }

        for (SIMPLEANIMNODE* node = list->Head(); node; node = list->Next(node)) {
            CSimpleAnim* anim = node->m_anim;
            anim->m_orderPadding = duration - (anim->m_duration + anim->m_endDelay + anim->m_startDelay);
        }

        this->m_duration += duration;
    }
}

// OFFSET: %0x0053A6B0
void CSimpleAnimGroup::NotifyAnimAdded(CSimpleAnim* anim) {
    if (!anim) {
        return;
    }

    if (this->AddAnimListNode(this->RegisterAnimList(anim->m_order), anim)) {
        this->ComputeDuration();
    }
}

// OFFSET: %0x0053A6F0
void CSimpleAnimGroup::NotifyOrderChanged(CSimpleAnim* anim, int32_t oldOrder) {
    if (!anim) {
        return;
    }

    if (this->RemoveAnimListNode(this->FindAnimList(oldOrder), anim)) {
        if (this->AddAnimListNode(this->RegisterAnimList(anim->m_order), anim)) {
            this->ComputeDuration();
        }
    }
}

// OFFSET: %0x0053A740
bool CSimpleAnimGroup::AdvanceLoopState(float elapsedSec, float remaining) {
    int32_t count = this->m_animLists.Count();
    bool reverse = this->m_loopState == ANIM_LOOPSTATE_REVERSE;
    int32_t step;
    int32_t first;
    int32_t end;

    if (reverse) {
        step = -1;
        first = count - 1;
        end = -1;
    } else {
        step = 1;
        first = 0;
        end = count;
    }

    this->m_currentOrder += step;

    if (this->m_currentOrder == end) {
        if (!this->m_loopType || (this->m_flags & 0x2)) {
            return true;
        }

        if (this->m_loopType == ANIM_LOOPTYPE_BOUNCE) {
            this->m_loopState = !reverse + 1;
            this->m_currentOrder -= step;
        } else {
            this->m_currentOrder = first;
        }

        this->m_elapsed = 0.0f;
        this->m_progress = 0.0f;

        for (int32_t i = 0; i < count; i++) {
            TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* list = this->m_animLists[i];

            for (SIMPLEANIMNODE* node = list->Head(); node; node = list->Next(node)) {
                CSimpleAnim* anim = node->m_anim;
                anim->m_loopState = this->m_loopState > 0 && this->m_loopState <= 2 ? this->m_loopState : 0;
                anim->m_playState = 0;
                anim->m_elapsed = 0.0f;
                anim->m_progressWithDelay = 0.0f;
                anim->m_progress = 0.0f;
            }
        }

        this->RunOnLoopScript(static_cast<SimpleAnimLoopState>(this->m_loopState));

        if (!this->m_loopState || (this->m_flags & 0x4)) {
            return true;
        }
    }

    TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* list = this->m_animLists[this->m_currentOrder];

    for (SIMPLEANIMNODE* node = list->Head(); node; node = list->Next(node)) {
        node->m_anim->Play();
    }

    return false;
}

// OFFSET: %0x0053AA20
void CSimpleAnimGroup::SortAnimations() {
    if (!this->m_loopState || (this->m_flags & 0x4)) {
        this->m_loopState = ANIM_LOOPSTATE_NONE;
        this->m_flags = 0;
        this->m_currentOrder = -1;
        this->m_elapsed = 0.0f;
        this->m_progress = 0.0f;
    } else {
        this->Stop(false);
    }

    if (!this->m_animations.Head()) {
        for (uint32_t i = 0; i < this->m_animLists.Count(); i++) {
            this->m_animLists[i]->Clear();
            delete this->m_animLists[i];
        }

        this->m_animLists.Clear();
        return;
    }

    for (uint32_t i = 0; i < this->m_animLists.Count(); i++) {
        this->m_animLists[i]->Clear();
    }

    int32_t nextOrder = 0;

    for (CSimpleAnim* anim = this->m_animations.Head(); anim; anim = this->m_animations.Next(anim)) {
        int32_t order = anim->m_order;

        if (order == -1) {
            order = nextOrder++;
            anim->m_order = order < 0 ? 0 : order;
        } else {
            nextOrder = order + 1;
        }

        if (nextOrder > 99) {
            nextOrder = 0;
        }

        this->AddAnimListNode(this->RegisterAnimList(order), anim);
    }

    int32_t count = this->m_animLists.Count();
    int32_t last = count - 1;

    while (last >= 0 && !this->m_animLists[last]->Head()) {
        last--;
    }

    if (last < 0) {
        return;
    }

    uint32_t newCount = last + 1;

    if (static_cast<int32_t>(newCount) < count) {
        for (int32_t i = newCount; i < count; i++) {
            delete this->m_animLists[i];
        }

        this->m_animLists.SetCount(newCount);

        if (newCount) {
            this->m_animLists.ReallocData(newCount);
        } else {
            this->m_animLists.Clear();
        }
    }
}

// OFFSET: %0x0053AD00
bool CSimpleAnimGroup::DoGroupUpdateWork(float elapsedSec) {
    float remaining = elapsedSec < 0.0f ? 0.0f : (elapsedSec >= 60.0f ? 60.0f : elapsedSec);

    while (true) {
        TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* current = this->m_animLists[this->m_currentOrder];

        for (SIMPLEANIMNODE* node = current->Head(); node; node = current->Next(node)) {
            CScriptRegion* region = node->m_anim->GetRegionParent();

            if (region) {
                region->PreOnAnimUpdate();
            }
        }

        for (int32_t i = 0; i < this->m_currentOrder; i++) {
            TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* list = this->m_animLists[i];

            for (SIMPLEANIMNODE* node = list->Head(); node; node = list->Next(node)) {
                node->m_anim->Apply(1.0f);
            }
        }

        float maxConsumed = 0.0f;
        bool finished = true;
        TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* list = this->m_animLists[this->m_currentOrder];

        for (SIMPLEANIMNODE* node = list->Head(); node; node = list->Next(node)) {
            float consumed;
            finished = node->m_anim->OnUpdate(remaining, consumed) && finished;

            if (consumed > maxConsumed) {
                maxConsumed = consumed;
            }
        }

        if (!(this->m_flags & 0x4) && this->m_loopState) {
            remaining -= maxConsumed;
            this->m_elapsed += maxConsumed;
            this->m_progress = this->m_elapsed < this->m_duration ? this->m_elapsed / this->m_duration : 1.0f;

            if (finished && this->AdvanceLoopState(elapsedSec, remaining)) {
                return true;
            }
        }

        if (remaining <= 0.0001f || this->IsDone() || !this->m_loopState || (this->m_flags & 0x4)) {
            return false;
        }
    }
}

// OFFSET: %0x0053AEF0
bool CSimpleAnimGroup::RemoveAnimListNode(TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* list, CSimpleAnim* anim) {
    if (!list) {
        return false;
    }

    for (SIMPLEANIMNODE* node = list->Head(); node; node = list->Next(node)) {
        if (node->m_anim == anim) {
            list->DeleteNode(node);

            if (!list->Head()) {
                this->SortAnimations();
            }

            return true;
        }
    }

    return false;
}

// OFFSET: %0x0053B770
void CSimpleAnimGroup::PostLoadXML(linkObject* node, CStatus* status) {
    this->m_flags ^= 1;

    for (CSimpleAnim* anim = this->m_animations.Head(); anim; anim = this->m_animations.Next(anim)) {
        anim->PostLoadXML(node, status);
    }

    this->SortAnimations();
    this->ComputeDuration();

    if (this->HasScript(this->m_onLoad)) {
        this->RunScript(this->m_onLoad, 0, NULL);
    }
}

// OFFSET: %0x0053B7F0
bool CSimpleAnimGroup::OnUpdate(float elapsedSec) {
    if (this->IsDone() || !this->m_loopState) {
        return true;
    }

    bool done = this->DoGroupUpdateWork(elapsedSec);
    this->RunOnUpdateScript(elapsedSec);

    if (done) {
        this->m_region->NotifyAnimEnd(this);

        bool requested = (this->m_flags & 0x2) != 0;
        this->m_loopState = ANIM_LOOPSTATE_NONE;
        this->m_flags = 0;
        this->m_currentOrder = -1;
        this->m_elapsed = 0.0f;
        this->m_progress = 0.0f;

        this->RunOnFinishedScript(requested);
    }

    return this->IsDone();
}

// OFFSET: %0x0053B8C0
void CSimpleAnimGroup::NotifyAnimRemoved(CSimpleAnim* anim) {
    if (!anim) {
        return;
    }

    if (this->RemoveAnimListNode(this->FindAnimList(anim->m_order), anim)) {
        this->ComputeDuration();
    }
}

// OFFSET: %0x0053B950
bool CSimpleAnimGroup::Play(bool reverse) {
    if (!this->m_animations.Head()) {
        return false;
    }

    if (this->m_loopState && !(this->m_flags & 0x4)) {
        return true;
    }

    if (this->m_flags & 0x4) {
        this->m_flags &= ~0x4;
    } else {
        this->m_flags = 0;
        this->m_loopState = ANIM_LOOPSTATE_NONE;
        this->m_currentOrder = reverse ? 0x7FFF : -1;
        this->m_elapsed = 0.0f;
        this->m_progress = 0.0f;
    }

    if (this->HasScript(this->m_onPlay)) {
        this->RunScript(this->m_onPlay, 0, NULL);
    }

    int32_t count = this->m_animLists.Count();

    if (count > 0) {
        if (this->m_currentOrder < 0 || this->m_currentOrder >= count) {
            if (this->m_currentOrder < 0) {
                this->m_loopState = ANIM_LOOPSTATE_FORWARD;
                this->m_currentOrder = 0;
            } else {
                this->m_loopState = ANIM_LOOPSTATE_REVERSE;
                this->m_currentOrder = count - 1;
            }

            for (CSimpleAnim* anim = this->m_animations.Head(); anim; anim = this->m_animations.Next(anim)) {
                anim->m_loopState = this->m_loopState > 0 && this->m_loopState <= 2 ? this->m_loopState : 0;
                anim->m_playState = 0;
                anim->m_elapsed = 0.0f;
                anim->m_progressWithDelay = 0.0f;
                anim->m_progress = 0.0f;
            }
        }

        TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* list = this->m_animLists[this->m_currentOrder];

        for (SIMPLEANIMNODE* node = list->Head(); node; node = list->Next(node)) {
            node->m_anim->Play();
        }

        this->m_region->NotifyAnimBegin(this);
    }

    this->OnUpdate(0.0f);

    return true;
}
