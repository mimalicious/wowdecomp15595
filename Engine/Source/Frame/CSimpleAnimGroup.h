#ifndef ENGINE_SOURCE_FRAME_CSIMPLEANIMGROUP_H
#define ENGINE_SOURCE_FRAME_CSIMPLEANIMGROUP_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Storm/H/stpl.h"

class CScriptRegion;
class CStatus;
class linkObject;

struct SIMPLEANIMNODE : public TSLinkedNode<SIMPLEANIMNODE> {
    /* 0008 */ CSimpleAnim* m_anim;
};

class CSimpleAnimGroup : public CScriptObject {
    public:
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static int32_t GetObjectType();
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static void RegisterScriptMethods(lua_State* L);

    /* 0020 */ TSLink<CSimpleAnimGroup> m_regionLink;
    /* 0028 */ TSLink<CSimpleAnimGroup> m_playingLink;
    /* 0030 */ CScriptRegion* m_region;
    /* 0034 */ TSExplicitList<CSimpleAnim, offsetof(CSimpleAnim, m_link)> m_animations;
    /* 0040 */ TSGrowableArray<TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >*> m_animLists;
    /* 0050 */ FrameScript_Object::ScriptIx m_onLoad;
    /* 0051 */ FrameScript_Object::ScriptIx m_onPlay;
    /* 0052 */ FrameScript_Object::ScriptIx m_onPause;
    /* 0053 */ FrameScript_Object::ScriptIx m_onStop;
    /* 0054 */ FrameScript_Object::ScriptIx m_onFinished;
    /* 0055 */ FrameScript_Object::ScriptIx m_onUpdate;
    /* 0056 */ FrameScript_Object::ScriptIx m_onLoop;
    /* 0057 */ uint8_t unk_0057;
    /* 0058 */ int8_t m_loopType;
    /* 0059 */ int8_t m_loopState;
    /* 005A */ uint8_t unk_005A[2];
    /* 005C */ uint8_t m_flags;
    /* 005D */ uint8_t unk_005D[3];
    /* 0060 */ int32_t unk_005C : 1;
    /* 0064 */ int32_t m_currentOrder;
    /* 0068 */ float m_elapsed;
    /* 006C */ float m_duration;
    /* 0070 */ float m_progress;
    /* 0074 */ C2Vector m_initialOffset;

    virtual ~CSimpleAnimGroup();
    virtual int32_t GetScriptMetaTable();
    virtual FrameScript_Object::ScriptIx* GetScriptByName(const char* name, FrameScript_Object::ScriptData& data);
    virtual bool IsA(int32_t type);
    virtual CScriptObject* GetScriptObjectParent();
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();

    CSimpleAnimGroup(CScriptRegion* owner);
    bool AddAnimListNode(TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* list, CSimpleAnim* anim);
    bool AdvanceLoopState(float elapsedSec, float remaining);
    void ComputeDuration();
    bool DoGroupUpdateWork(float elapsedSec);
    TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* FindAnimList(int32_t order);
    void Finish();
    int32_t GetMaxAnimOrder();
    bool HasAnimOfType(int32_t type);
    bool IsDone();
    void LoadXML(linkObject* node, CStatus* status);
    void LoadXML_Anim(linkObject* node, CStatus* status, CSimpleAnim* anim);
    void LoadXML_Scripts(linkObject* node, CStatus* status);
    void NotifyAnimAdded(CSimpleAnim* anim);
    void NotifyAnimRemoved(CSimpleAnim* anim);
    void NotifyOrderChanged(CSimpleAnim* anim, int32_t oldOrder);
    bool OnUpdate(float elapsedSec);
    void Pause();
    bool Play(bool reverse);
    void PostLoadXML(linkObject* node, CStatus* status);
    void PreLoadXML(linkObject* node, CStatus* status);
    TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* RegisterAnimList(int32_t order);
    bool RemoveAnimListNode(TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE> >* list, CSimpleAnim* anim);
    void RunOnFinishedScript(bool requested);
    void RunOnLoopScript(SimpleAnimLoopState state);
    void RunOnUpdateScript(float elapsedSec);
    void SetInitialOffset(const C2Vector& offset);
    void SortAnimations();
    void Stop(bool requested);
};

#include "Engine/Source/Frame/CSimpleAnimGroup.inl"

#endif
