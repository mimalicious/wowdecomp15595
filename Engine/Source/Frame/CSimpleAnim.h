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

class CSimpleAnimSCurveSmoother {
    public:
    /* 0004 */ float m_in;
    /* 0008 */ float m_out;

    virtual void SetWeights(float in, float out);
    virtual float GetSmoothProgress(float progress);

    CSimpleAnimSCurveSmoother();
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

    /* 0020 */ TSLink<CSimpleAnim> m_link;
    /* 0028 */ CSimpleAnimGroup* m_group;
    /* 002C */ CScriptRegion* m_target;
    /* 0030 */ char m_targetName[64];
    /* 0070 */ uint8_t unk_0070;
    /* 0071 */ uint8_t unk_0071[3];
    /* 0074 */ int8_t m_order;
    /* 0075 */ uint8_t m_flags;
    /* 0076 */ uint8_t unk_0076[2];
    /* 0078 */ uint8_t m_playState;
    /* 0079 */ int8_t m_loopState;
    /* 007A */ uint8_t unk_007A[5];
    /* 007F */ FrameScript_Object::ScriptIx m_onLoad;
    /* 0080 */ FrameScript_Object::ScriptIx m_onPlay;
    /* 0081 */ FrameScript_Object::ScriptIx m_onPause;
    /* 0082 */ FrameScript_Object::ScriptIx m_onStop;
    /* 0083 */ FrameScript_Object::ScriptIx m_onFinished;
    /* 0084 */ FrameScript_Object::ScriptIx m_onUpdate;
    /* 0085 */ uint8_t unk_0085[3];
    /* 0088 */ float m_orderPadding;
    /* 008C */ float m_startDelay;
    /* 0090 */ float m_endDelay;
    /* 0094 */ float m_duration;
    /* 0098 */ float m_maxFramerate;
    /* 009C */ float m_frameInterval;
    /* 00A0 */ float m_frameElapsed;
    /* 00A4 */ float m_elapsed;
    /* 00A8 */ float m_progressWithDelay;
    /* 00AC */ float m_progress;
    /* 00B0 */ float m_smoothProgress;
    /* 00B4 */ CSimpleAnimSCurveSmoother* m_smoother;

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
    void SetMaxFramerate(float framerate);
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
