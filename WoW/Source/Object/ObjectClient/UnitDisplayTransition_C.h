#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_UNITDISPLAYTRANSITION_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_UNITDISPLAYTRANSITION_C_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaSphere.h"

class CAnimKitInst;
class CCharacterComponent;
class CGUnit_C;
class CM2Model;
class CreatureDisplayInfoRec;
class CreatureDisplayInfoExtraRec;
class CreatureModelDataRec;
class CreatureSoundDataRec;

class CUnitDisplayTransition_C {
    public:
    static TSObjectAllocPool<CUnitDisplayTransition_C>* s_pool;

    static void AnimKitStopCallback(CAnimKitInst* inst, uint64_t param);
    static CUnitDisplayTransition_C* Create(CGUnit_C* unit, int32_t race, int32_t displayID, int32_t attachment, uint32_t startAnim, uint32_t endAnim, const char* effectModelName, int32_t effectAttachment, int32_t delay);
    static void InitSystem();
    static void UninitSystem();

    /* 0000 */ uint32_t m_handle;
    /* 0004 */ CGUnit_C* m_unit;
    /* 0008 */ uint32_t m_flags;
    /* 000C */ int32_t m_state;
    /* 0010 */ int32_t m_race;
    /* 0014 */ CreatureDisplayInfoRec* m_displayInfo;
    /* 0018 */ CreatureDisplayInfoExtraRec* m_displayInfoExtra;
    /* 001C */ CreatureModelDataRec* m_modelData;
    /* 0020 */ CreatureSoundDataRec* m_soundData;
    /* 0024 */ CM2Model* m_model;
    /* 0028 */ int32_t m_attachment;
    /* 002C */ uintptr_t m_worldObject;
    /* 0030 */ CCharacterComponent* m_component;
    /* 0034 */ uint32_t m_startAnim;
    /* 0038 */ uint32_t m_endAnim;
    /* 003C */ CAnimKitInst* m_swapAnimKit;
    /* 0040 */ CAnimKitInst* m_animKit;
    /* 0044 */ CM2Model* m_effectModel;
    /* 0048 */ int32_t m_effectAttachment;
    /* 004C */ int32_t m_delay;
    /* 0050 */ uint32_t m_delayEndTime;

    void ApplyGuildColor(uint32_t flags, int32_t emblem, int32_t emblemColor, int32_t border, int32_t borderColor, int32_t background);
    void ApplyShirtGuildColor(uint32_t flags, int32_t emblem, int32_t emblemColor, int32_t border, int32_t borderColor, int32_t background);
    void BeginAnimation();
    void BeginTransition();
    void Cancel();
    void Destroy();
    void Expedite(int32_t swap);
    int32_t GetTargetDisplayID() const;
    void HandleAnimSwapEvent();
    void HandleComponentPrep();
    void HandleComponentRenderPrep();
    bool IsReadyToBegin() const;
    int32_t MaybeCreateCharacterComponent();
    void MirrorTransformToSwapModel();
    void MirrorTransformToSwapWorldObject(const C44Matrix& matrix, const CAaBox& box, const CAaSphere& sphere, const C3Vector& position, int32_t a6, int32_t keyBone);
    void PerformSwap();
    void ReleaseAnimKits();
    bool UpdateState(int32_t begin);
};

#endif
