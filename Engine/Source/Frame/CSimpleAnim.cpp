#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleAnimGroup.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/FrameXML/FrameXML.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: 0x00CA4700/0x140B427B0
int32_t CSimpleAnim::s_metatable = LUA_NOREF;

// STATIC: 0x00DD3CCC/0x140CB9A6C
int32_t CSimpleAnim::s_objectType;

// STATIC: 0x00CA3820/0x140B40A90
const char* CSimpleAnim::s_objectTypeName = "Animation";

// PROVEN: e7006d69 IDENTICAL/IDENTICAL
// STATIC: 0x00CA3840/0x140B40AD0
float CSimpleAnim::s_frameInterval = 1.0f / 30.0f;

// INLINED
CSimpleAnimSmoother::CSimpleAnimSmoother() {
    this->m_in = 0.0f;
    this->m_out = 0.0f;
}

// PROVEN: e7006d69 IDENTICAL/IDENTICAL
// OFFSET: 0x005275F0/0x140176960
void CSimpleAnimSCurveSmoother::SetWeights(float in, float out) {
    this->m_in = in < 0.0f ? 0.0f : (in < 1.0f ? in : 1.0f);
    this->m_out = out < 0.0f ? 0.0f : (out < 1.0f ? out : 1.0f);
}

// OFFSET: 0x00527650/0x1401769A0
float CSimpleAnimSCurveSmoother::GetSmoothProgress(float progress) {
    if (fabsf(this->m_in) >= 0.001f && fabsf(this->m_out) >= 0.001f) {
        return 0.5f - cosf(progress * 3.1415927f) * 0.5f;
    }

    if (fabsf(this->m_in) >= 0.001f) {
        return 1.0f - cosf(progress * 1.5707964f);
    }

    if (fabsf(this->m_out) >= 0.001f) {
        return cosf((progress + 1.0f) * 1.5707964f) * -1.0f;
    }

    return progress;
}

// PROVEN: f6a6534e IDENTICAL/IDENTICAL
// OFFSET: 0x00526D30/0x1401760F0
FrameScript_Object::ScriptIx* CSimpleAnim::GetScriptByName(const char* name, FrameScript_Object::ScriptData& data) {
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
        return &this->m_onFinished;
    }

    if (!SStrCmp(name, "onupdate", STORM_MAX_STR)) {
        data.m_wrapper = "return function(self,elapsed) %s end";
        return &this->m_onUpdate;
    }

    return NULL;
}

// PROVEN: 018eb404 IDENTICAL/IDENTICAL
// OFFSET: 0x00526EE0/0x1401761F0
void CSimpleAnim::GetScriptTime(lua_State* L, double* time, uint32_t* calls) {
    this->FrameScript_Object::GetScriptTime(L, this->m_onLoad, time, calls);
    this->FrameScript_Object::GetScriptTime(L, this->m_onPlay, time, calls);
    this->FrameScript_Object::GetScriptTime(L, this->m_onPause, time, calls);
    this->FrameScript_Object::GetScriptTime(L, this->m_onStop, time, calls);
    this->FrameScript_Object::GetScriptTime(L, this->m_onFinished, time, calls);
    this->FrameScript_Object::GetScriptTime(L, this->m_onUpdate, time, calls);
}

// PROVEN: 3736e17f IDENTICAL/IDENTICAL
// OFFSET: 0x00526F60/0x1401762B0
void CSimpleAnim::SetSmoothProgress(float progress) {
    this->m_smoothProgress = progress;
}

// PROVEN: ef1299d1 IDENTICAL/DIFFERENT
// OFFSET: 0x00526F80/0x1401762C0
void CSimpleAnim::SetSmoothing(float in, float out) {
    if ((in > 0.0f && in <= 1.0f) || (out > 0.0f && out <= 1.0f)) {
        if (!this->m_smoother) {
            void* mem = SMemAlloc(sizeof(CSimpleAnimSCurveSmoother), __FILE__, __LINE__, 0);
            this->m_smoother = mem ? new (mem) CSimpleAnimSCurveSmoother() : NULL;
        }

        if (this->m_smoother) {
            this->m_smoother->SetWeights(in, out);
        }
    }
}

// PROVEN: a63a4375 IDENTICAL/IDENTICAL
// OFFSET: 0x00527020/0x140176370
bool CSimpleAnim::GetSmoothing(float& in, float& out) {
    CSimpleAnimSmoother* smoother = this->m_smoother;

    if (smoother) {
        in = smoother->m_in;
        out = smoother->m_out;
        return true;
    }

    in = 0.0f;
    out = 0.0f;
    return false;
}

// PROVEN: 7b271f0a IDENTICAL/UNPROVEN
// OFFSET: 0x00526CE0/0x140176450
void CSimpleAnim::LoadXML_Scripts(linkObject* node, CStatus* status) {
    lua_State* L = FrameScript_GetContext();

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        if (child->m_flags & 0x20) {
            this->LoadFunction(L, static_cast<Function*>(child), status);
        }
    }
}

// INLINED
void CSimpleAnim::RunOnStopScript(bool requested) {
    if (this->HasScript(this->m_onStop)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushboolean(L, requested);
        this->RunScript(this->m_onStop, 1, NULL);
    }
}

// PROVEN: ba90a49c IDENTICAL/NONE
// OFFSET: 0x005274D0
void CSimpleAnim::RunOnFinishedScript() {
    if (this->HasScript(this->m_onFinished)) {
        this->RunScript(this->m_onFinished, 0, NULL);
    }
}

// PROVEN: 84e83861 IDENTICAL/NONE
// OFFSET: 0x00527500
void CSimpleAnim::RunOnUpdateScript(float elapsedSec) {
    if (this->HasScript(this->m_onUpdate)) {
        lua_State* L = FrameScript_GetContext();
        lua_pushnumber(L, elapsedSec);
        this->RunScript(this->m_onUpdate, 1, NULL);
    }
}

// PROVEN: fbda7b67 IDENTICAL/IDENTICAL
// OFFSET: 0x005276E0/0x140176A50
void CSimpleAnim::PostLoadXML(linkObject* node, CStatus* status) {
    this->m_flags &= ~1;

    if (this->HasScript(this->m_onLoad)) {
        this->RunScript(this->m_onLoad, 0, NULL);
    }
}

// PROVEN: 84fdfb93 IDENTICAL/IDENTICAL
// OFFSET: 0x00527710/0x140176A90
bool CSimpleAnim::Play() {
    if (this->m_targetName[0]) {
        this->m_target = static_cast<CScriptRegion*>(CScriptObject::GetScriptObjectByName(this->m_targetName, CScriptRegion::GetObjectType()));
    }

    if (this->m_playState != 1) {
        this->m_playState = 1;

        if (this->HasScript(this->m_onPlay)) {
            this->RunScript(this->m_onPlay, 0, NULL);
        }
    }

    return true;
}

// PROVEN: 618b9b25 IDENTICAL/UNPROVEN
// OFFSET: 0x00527780/0x140176B10
void CSimpleAnim::Pause() {
    this->m_group->Pause();

    if (this->m_playState == 1) {
        this->m_playState = 2;

        if (this->HasScript(this->m_onPause)) {
            this->RunScript(this->m_onPause, 0, NULL);
        }
    }
}

// PROVEN: 969b1f0d IDENTICAL/IDENTICAL
// OFFSET: 0x005277C0/0x140176B70
void CSimpleAnim::Stop(bool requested) {
    if (!this->m_playState) {
        return;
    }

    this->m_loopState = ANIM_LOOPSTATE_NONE;
    this->m_playState = 0;
    this->m_elapsed = 0.0f;
    this->m_progressWithDelay = 0.0f;
    this->m_progress = 0.0f;
    this->RunOnStopScript(requested);
}

// OFFSET: 0x00527830/0x140176C00
bool CSimpleAnim::IsDelaying() {
    if (this->m_progressWithDelay <= 0.0f || this->m_progressWithDelay >= 1.0f) {
        return false;
    }

    float total = this->m_endDelay + this->m_duration + this->m_startDelay;

    if (fabsf(total) < 0.00000023841858f) {
        return false;
    }

    if (this->m_loopState == ANIM_LOOPSTATE_REVERSE) {
        if (this->m_endDelay > this->m_elapsed) {
            return true;
        }

        if (this->m_endDelay + this->m_duration > this->m_elapsed) {
            return false;
        }
    } else {
        if (this->m_startDelay > this->m_elapsed) {
            return true;
        }

        if (this->m_duration + this->m_startDelay > this->m_elapsed) {
            return false;
        }
    }

    return total > this->m_elapsed;
}

// PROVEN: fd12db21 IDENTICAL/IDENTICAL
// OFFSET: 0x005278D0/0x140176C80
CScriptRegion* CSimpleAnim::GetRegionParent() {
    return this->m_group ? this->m_group->m_region : NULL;
}

// OFFSET: 0x00527C60
float CSimpleAnim::UpdateProgress(float elapsedSec) {
    float prevElapsed = this->m_elapsed;
    this->m_elapsed += elapsedSec;

    float total = this->m_orderPadding + this->m_startDelay + this->m_endDelay + this->m_duration;
    float elapsed;

    if (total > this->m_elapsed) {
        elapsed = this->m_elapsed;
        this->m_progressWithDelay = elapsed / total;

        if (this->m_progressWithDelay > 1.0f) {
            this->m_progressWithDelay = 1.0f;
        }

        if (fabsf(this->m_duration) < 0.00000023841858f) {
            this->m_progress = 1.0f;
        } else {
            this->m_frameElapsed += elapsedSec;

            if ((this->m_group->m_flags & 0x10) || this->m_frameElapsed >= CSimpleAnim::s_frameInterval) {
                float delay = this->m_loopState == ANIM_LOOPSTATE_REVERSE ? this->m_orderPadding + this->m_endDelay : this->m_startDelay;
                this->m_progress = (elapsed - delay) / this->m_duration;

                if (this->m_progress < 0.0f) {
                    this->m_progress = 0.0f;
                } else if (this->m_progress >= 1.0f) {
                    this->m_progress = 1.0f;
                }

                this->m_frameElapsed = 0.0f;
            }
        }
    } else {
        elapsed = total;
        this->m_progressWithDelay = 1.0f;
        this->m_progress = 1.0f;
    }

    float progress = this->m_loopState == ANIM_LOOPSTATE_REVERSE ? 1.0f - this->m_progress : this->m_progress;

    if (this->m_smoother) {
        progress = this->m_smoother->GetSmoothProgress(progress);
    }

    this->m_smoothProgress = progress;

    return elapsed - prevElapsed;
}

// OFFSET: [0x00528270/0x140177860, 0x00529260]
CSimpleAnim::~CSimpleAnim() {
    delete this->m_smoother;
}

// PROVEN: defe0283 IDENTICAL/IDENTICAL
// OFFSET: 0x005282F0/0x140177900
bool CSimpleAnim::IsA(int32_t type) {
    return type == CSimpleAnim::s_objectType || type == CScriptObject::s_objectType;
}

// PROVEN: bf32b965 IDENTICAL/IDENTICAL
// OFFSET: 0x00528320/0x140177920
bool CSimpleAnim::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleAnim::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CScriptObject::s_objectTypeName, STORM_MAX_STR);
}

// PROVEN: e7d1a958 IDENTICAL/IDENTICAL
// OFFSET: 0x00528360/0x140177960
const char* CSimpleAnim::GetObjectTypeName() {
    return CSimpleAnim::s_objectTypeName;
}

// PROVEN: 8f49d2c3 IDENTICAL/IDENTICAL
// OFFSET: 0x00528370/0x140177970
int32_t CSimpleAnim::GetScriptMetaTable() {
    return CSimpleAnim::s_metatable;
}

// OFFSET: 0x00528380
bool CSimpleAnim::OnUpdate(float elapsedSec, float& consumed) {
    consumed = 0.0f;

    if (this->m_progressWithDelay >= 1.0f) {
        this->Apply(this->m_smoothProgress);
        return true;
    }

    if (!this->m_playState) {
        return true;
    }

    if (this->m_playState == 2) {
        this->Apply(this->m_smoothProgress);
        return false;
    }

    consumed = this->UpdateProgress(elapsedSec);
    this->RunOnUpdateScript(elapsedSec);
    this->Apply(this->m_smoothProgress);

    if (this->m_progressWithDelay < 1.0f) {
        return false;
    }

    this->RunOnFinishedScript();
    return true;
}

// OFFSET: 0x0052A510/0x14017A590
CSimpleAnim::CSimpleAnim(CSimpleAnimGroup* group) {
    this->m_startDelay = 0.0f;
    this->m_endDelay = 0.0f;
    this->m_duration = 0.0f;
    this->m_frameElapsed = 0.0f;
    this->m_elapsed = 0.0f;
    this->m_progressWithDelay = 0.0f;
    this->m_progress = 0.0f;
    this->m_group = NULL;
    this->m_target = NULL;
    this->m_smoother = NULL;
    this->m_order = -1;
    this->m_flags = 0;
    this->m_playState = 0;
    this->m_loopState = 0;
    this->m_targetName[0] = 0;

    if (group) {
        this->m_group = group;
        group->m_animations.LinkToTail(this);

        if (group->AddAnimListNode(group->RegisterAnimList(this->m_order), this)) {
            group->ComputeDuration();
        }
    }
}

// PROVEN: 1c4670ab IDENTICAL/IDENTICAL
// OFFSET: 0x00529290/0x140178BB0
void CSimpleAnim::SetStartDelay(float delay, bool notify) {
    this->m_startDelay = delay;

    if (delay < 0.0f) {
        this->m_startDelay = 0.0f;
    }

    if (notify && this->m_group) {
        this->m_group->ComputeDuration();
    }
}

// PROVEN: d65997a8 IDENTICAL/IDENTICAL
// OFFSET: 0x005292C0/0x140178BF0
void CSimpleAnim::SetEndDelay(float delay, bool notify) {
    this->m_endDelay = delay;

    if (delay < 0.0f) {
        this->m_endDelay = 0.0f;
    }

    if (notify && this->m_group) {
        this->m_group->ComputeDuration();
    }
}

// PROVEN: 1b8fea6b IDENTICAL/IDENTICAL
// OFFSET: 0x005292F0/0x140178C30
void CSimpleAnim::SetDuration(float duration, bool notify) {
    this->m_duration = duration;

    if (notify && this->m_group) {
        this->m_group->ComputeDuration();
    }
}

// OFFSET: 0x0052A5D0/0x14017A670
void CSimpleAnim::SetOrder(int32_t order, bool notify, bool wrap) {
    int32_t oldOrder = this->m_order;

    if (wrap) {
        this->m_order = order < 0 || order > 99 ? 0 : order;
    } else {
        this->m_order = order < 0 ? 0 : (order > 99 ? 99 : order);
    }

    if (notify && this->m_group && oldOrder != order) {
        this->m_group->NotifyOrderChanged(this, oldOrder);
    }
}

// OFFSET: 0x0052AF10/0x14017B3E0
void CSimpleAnim::LoadXML(linkObject* node, CStatus* status) {
    this->m_flags |= 1;

    if (!(node->m_flags & 0x2000)) {
        return;
    }

    Animation* animation = static_cast<Animation*>(node);

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

    if (animation->m_animFlags & 1) {
        lua_State* L = FrameScript_GetContext();
        CScriptObject* parent = this->GetScriptObjectParent();

        if (parent) {
            if (!parent->m_luaRegistered) {
                parent->RegisterScriptObject(NULL);
            }

            lua_rawgeti(L, LUA_REGISTRYINDEX, parent->m_luaObjectRef);
            lua_pushstring(L, animation->m_parentKey.m_string);

            if (!this->m_luaRegistered) {
                this->RegisterScriptObject(NULL);
            }

            lua_rawgeti(L, LUA_REGISTRYINDEX, this->m_luaObjectRef);
            lua_settable(L, -3);
            lua_settop(L, -2);
        }
    }

    if (animation->m_animFlags2 & 2) {
        char target[1024];
        this->CreateName(animation->m_target.m_string, target, sizeof(target));

        if (strlen(target) + 1 > sizeof(this->m_targetName)) {
            status->Add(STATUS_WARNING, "target key too long: %s", target);
        }

        SStrCopy(this->m_targetName, target, sizeof(this->m_targetName));
    }

    if (animation->m_animFlags & 2) {
        float delay = animation->m_startDelay;
        this->m_startDelay = delay;

        if (delay < 0.0f) {
            this->m_startDelay = 0.0f;
        }
    }

    if (animation->m_animFlags & 4) {
        float delay = animation->m_endDelay;
        this->m_endDelay = delay;

        if (delay < 0.0f) {
            this->m_endDelay = 0.0f;
        }
    }

    if (animation->m_animFlags & 8) {
        this->m_duration = animation->m_duration;
    }

    if (animation->m_hasSmoothing) {
        float in;
        float out;

        if (StringToSmoothWeights(animation->m_smoothing.m_string, in, out)) {
            this->SetSmoothing(in, out);
        } else {
            const char* name = this->GetName();
            status->Add(STATUS_WARNING, "%s: Invalid smoothing value: %s", name ? name : "<unnamed>", animation->m_smoothing.m_string);
        }
    }

    if (animation->m_animFlags2 & 1) {
        int32_t order = animation->m_order - 1;

        if (order < 0 || order > 99) {
            const char* name = this->GetName();
            status->Add(STATUS_WARNING, "%s: Invalid order value: %d. Order must be between %d and %d.", name ? name : "<unnamed>", animation->m_order, 1, 100);
        }

        this->m_order = order < 0 ? 0 : (order > 99 ? 99 : order);
    }

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        if (child->m_element == XML_SCRIPTS) {
            this->LoadXML_Scripts(child, status);
        }
    }
}

// PROVEN: 4620d55f IDENTICAL/IDENTICAL
// OFFSET: 0x0052B7D0/0x14017BFB0
bool CSimpleAnim::ChangeParent(CSimpleAnimGroup* group) {
    if (!group || this->m_group == group) {
        return false;
    }

    this->m_group->NotifyAnimRemoved(this);
    this->m_group = group;
    group->NotifyAnimAdded(this);

    return true;
}

// PROVEN: f0d68712 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CSimpleAnim::Apply(float progress) {
}

// PROVEN: b05b8fdd IDENTICAL/IDENTICAL
// OFFSET: 0x005282E0/0x14090C360
CScriptObject* CSimpleAnim::GetScriptObjectParent() {
    return this->m_group;
}
