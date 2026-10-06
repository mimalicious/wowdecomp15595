#ifndef ENGINE_SOURCE_FRAME_CSIMPLEANIM_H
#define ENGINE_SOURCE_FRAME_CSIMPLEANIM_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/Types.h"
#include "Engine/Source/Storm/H/stpl.h"

class C2Vector;
class CScriptRegion;
class CSimpleAnimGroup;
class CStatus;
class linkObject;

class CSimpleAnimSmoother {
    public:
    /* 0004 */ float m_in;
    /* 0008 */ float m_out;

    virtual void SetWeights(float in, float out) = 0;
    virtual float GetSmoothProgress(float progress) = 0;

    CSimpleAnimSmoother();
};

class CSimpleAnimSCurveSmoother : public CSimpleAnimSmoother {
    public:
    virtual void SetWeights(float in, float out);
    virtual float GetSmoothProgress(float progress);
};

class CSimpleAnim : public CScriptObject {
    public:
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static int32_t GetObjectType();
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static void RegisterScriptMethods(lua_State* L);

    static float s_frameInterval;

    /* 0020 */ TSLink<CSimpleAnim> m_link;
    /* 0028 */ FrameScript_Object::ScriptIx m_onLoad;
    /* 0029 */ FrameScript_Object::ScriptIx m_onPlay;
    /* 002A */ FrameScript_Object::ScriptIx m_onPause;
    /* 002B */ FrameScript_Object::ScriptIx m_onStop;
    /* 002C */ FrameScript_Object::ScriptIx m_onFinished;
    /* 002D */ FrameScript_Object::ScriptIx m_onUpdate;
    /* 002E */ uint8_t unk_002E[2];
    /* 0030 */ float m_orderPadding;
    /* 0034 */ float m_startDelay;
    /* 0038 */ float m_endDelay;
    /* 003C */ float m_duration;
    /* 0040 */ float m_frameElapsed;
    /* 0044 */ float m_elapsed;
    /* 0048 */ float m_progressWithDelay;
    /* 004C */ float m_progress;
    /* 0050 */ float m_smoothProgress;
    /* 0054 */ CSimpleAnimGroup* m_group;
    /* 0058 */ CScriptRegion* m_target;
    /* 005C */ char m_targetName[64];
    /* 009C */ CSimpleAnimSmoother* m_smoother;
    /* 00A0 */ int8_t m_order;
    /* 00A1 */ uint8_t m_flags;
    /* 00A2 */ uint8_t unk_00A2[2];
    /* 00A4 */ uint8_t m_playState;
    /* 00A5 */ int8_t m_loopState;
    /* 00A6 */ uint8_t unk_00A6[2];

    virtual ~CSimpleAnim();
    virtual int32_t GetScriptMetaTable();
    virtual FrameScript_Object::ScriptIx* GetScriptByName(const char* name, FrameScript_Object::ScriptData& data);
    virtual bool IsA(int32_t type);
    virtual CScriptObject* GetScriptObjectParent();
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
    virtual void LoadXML(linkObject* node, CStatus* status);
    virtual void PostLoadXML(linkObject* node, CStatus* status);
    virtual void GetScriptTime(lua_State* L, double* time, uint32_t* calls);
    virtual void Apply(float progress);

    CSimpleAnim(CSimpleAnimGroup* group);
    bool ChangeParent(CSimpleAnimGroup* group);
    CScriptRegion* GetRegionParent();
    bool GetSmoothing(float& in, float& out);
    bool IsDelaying();
    void LoadXML_Scripts(linkObject* node, CStatus* status);
    bool OnUpdate(float elapsedSec, float& consumed);
    void Pause();
    bool Play();
    void RunOnFinishedScript();
    void RunOnStopScript(bool requested);
    void RunOnUpdateScript(float elapsedSec);
    void SetDuration(float duration, bool notify);
    void SetEndDelay(float delay, bool notify);
    void SetOrder(int32_t order, bool notify, bool wrap);
    void SetSmoothing(float in, float out);
    void SetSmoothProgress(float progress);
    void SetStartDelay(float delay, bool notify);
    void Stop(bool requested);
    float UpdateProgress(float elapsedSec);
};

void ReadOrigin(lua_State* L, int32_t index, FRAMEPOINT& point, C2Vector& offset);

#include "Engine/Source/Frame/CSimpleAnim.inl"

#endif
