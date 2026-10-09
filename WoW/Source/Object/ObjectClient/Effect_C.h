#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_EFFECT_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_EFFECT_C_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Object/ObjectClient/VisibleItem_C.h"
#include "WoW/Source/Object/ObjectClient/AnimKitManager.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"

class BlizzardObject;
class CAnimKitInst;
class CGObject_C;
class CGUnit_C;
class LightningObject;
class MountTransitionObject;
class CM2Lighting;
class CM2Model;
class SpellVisualEffectNameRec;
class SpellVisualKitModelAttachRec;
class SpellVisualKitRec;

struct CEffectVirtualItem {
    /* 0000 */ uint32_t m_hand;
    /* 0004 */ VisibleItem_C m_item;
};

struct CEffectCurvePoint {
    /* 0000 */ float m_time;
    /* 0004 */ float m_value;
    /* 0008 */ float m_inSlope;
    /* 000C */ float m_outSlope;
    /* 0010 */ float m_coefficients[4];
    /* 0020 */ int32_t m_dirty;
    /* 0024 */ CEffectCurvePoint* m_next;
};

class CEffectCurve {
    public:
    /* 0000 */ CEffectCurvePoint* m_points;

    static CEffectCurve* New();

    void AddPoint(float time, float value, float inSlope, float outSlope);
    void Clear();
    float Evaluate(float time);
};

class CEffect {
    public:
    typedef void (*SEQUENCE_CALLBACK)(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7);

    union EffectData {
        float m_float[2];
        uint32_t m_uint[2];
        CEffectVirtualItem* m_virtualItem;
        CEffectCurve* m_curve;
    };

    static void ApplyModelAttachRecToMatrix(const SpellVisualKitModelAttachRec* attach, C44Matrix& matrix, float facing);
    static void BlizzardStoppedCallback(BlizzardObject* blizzard, void* param);
    static void CleanUpStaticLists();
    static float __stdcall ComputeModelScaleFromTarget(CGObject_C* object);
    static CEffect* Create();
    static CM2Model* CreateModelFromEffect(int32_t effectId, float* scale, float* alpha, WOWGUID guid, uint32_t displayId, uint32_t inventoryType);
    static void Destroy();
    static void __stdcall DestroyMatchingEffects(CGObject_C* object, const SpellVisualEffectNameRec* effectName, int32_t attachType, int32_t spellId, int32_t kitId, const SpellVisualKitModelAttachRec* attach);
    static void DrawFishingString(CM2Model* model, const CM2Lighting* lighting, void* arg);
    static void EffectEvent(CM2Model* model, uint32_t boneId, uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time, uint64_t param);
    static void EffectModelLoadedCallback(CM2Model* model, void* arg);
    static void ItemCacheCB(int32_t id, const WOWGUID& guid, void* param, bool success);
    static void MountTransitionCacheCB(int32_t id, const WOWGUID& guid, void* param, bool success);
    static void MountTransitionSequenceCB(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7);

    /* 0000 */ CM2Model* m_model;
    /* 0004 */ CM2Model* m_lineModel;
    /* 0008 */ WOWGUID m_sourceGuid;
    /* 0010 */ WOWGUID m_targetGuid;
    /* 0018 */ WOWGUID m_attachGuid;
    /* 0020 */ uint32_t m_spellId;
    /* 0024 */ const SpellVisualKitRec* m_kitRec;
    /* 0028 */ const SpellVisualEffectNameRec* m_effectNameRec;
    /* 002C */ float m_scale;
    /* 0030 */ int32_t m_attachmentId;
    /* 0034 */ C3Vector m_offset;
    /* 0040 */ uint32_t m_soundHandle;
    /* 0048 */ WOWGUID m_soundGuid;
    /* 0050 */ uint32_t m_flags;
    /* 0054 */ uint32_t m_flags2;
    /* 0058 */ LightningObject* m_lightning[12];
    /* 0088 */ uint32_t unk_0088;
    /* 008C */ uint32_t unk_008C;
    /* 0090 */ float m_facing;
    /* 0094 */ C3Vector m_orientation;
    /* 00A0 */ int32_t unk_00A0;
    /* 00A4 */ MountTransitionObject* m_mountTransition;
    /* 00A8 */ int32_t m_pendingCacheId;
    /* 00AC */ int32_t m_displayId;
    /* 00B0 */ int32_t m_inventoryType;
    /* 00B4 */ uint32_t m_param;
    /* 00B8 */ EffectData m_data;
    /* 00C0 */ const SpellVisualKitModelAttachRec* m_attachRec;
    /* 00C4 */ C3Vector m_position;
    /* 00D0 */ C3Vector m_worldPosition;
    /* 00E0 */ WOWGUID m_transportGuid;
    /* 00E8 */ uint32_t m_startTime;
    /* 00EC */ uint32_t m_expireTime;
    /* 00F0 */ uint32_t unk_00F0;
    /* 00F4 */ float unk_00F4;
    /* 00F8 */ uint32_t unk_00F8;
    /* 00FC */ uintptr_t m_mapEntity;
    /* 0100 */ int32_t m_refCount;
    /* 0104 */ int32_t m_lightningRefCount;
    /* 0108 */ float m_fade;
    /* 010C */ int32_t m_lightningState;
    /* 0110 */ CAnimKitInst* m_animKitInst;
    /* 0114 */ BlizzardObject* m_blizzard;
    /* 0118 */ CEffect** m_prevLink;
    /* 011C */ CEffect* m_next;

    CEffect();
    ~CEffect();
    void AddAnimEffect(uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target, uint32_t flags, uint32_t param);
    void AddAnimKitEffect(uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target, uint32_t flags, uint32_t param, AnimKitStopCallback callback, uint64_t callbackParam);
    void AddBlizzard(uint32_t expireTime, const C3Vector* position, uint32_t spellId, uint32_t index, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target, uint32_t flags);
    void AddCamouflage(uint32_t expireTime, float alpha, float fadeTime, uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target, uint32_t flags);
    void AddCurveEffect(uint32_t duration, CEffectCurve* curve, uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target, uint32_t flags);
    void AddEffect(int32_t attachType, uint32_t spellId, const SpellVisualKitRec* kit, const SpellVisualEffectNameRec* effectName, const WOWGUID& source, const WOWGUID& target, const WOWGUID& attach, uint32_t flags, SEQUENCE_CALLBACK callback, const C3Vector* offset, uint32_t param, const SpellVisualKitModelAttachRec* attachRec, int32_t kitType, uint32_t displayId, uint32_t inventoryType);
    void AddEffectByPosition(C3Vector position, const C3Vector* orientation, uint32_t startTime, uint32_t spellId, const SpellVisualKitRec* kit, const SpellVisualEffectNameRec* effectName, const WOWGUID& source, const WOWGUID& target, uint32_t flags, SEQUENCE_CALLBACK callback, uint32_t param, const SpellVisualKitModelAttachRec* attach, WOWGUID transport, int32_t kitType, uint32_t displayId, uint32_t inventoryType);
    void AddEffectWithModel(CM2Model* model, uint32_t flags, uint32_t attachmentId, const WOWGUID& guid, SEQUENCE_CALLBACK callback);
    void AddFishingLine(CGObject_C* object, const WOWGUID& target);
    void AddFreeze(uint32_t expireTime, float duration, uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target, uint32_t flags);
    void AddItemVisual(uint32_t expireTime, int32_t itemId, uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target, uint32_t flags);
    void AddLightning(uint32_t expireTime, uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target, uint32_t flags);
    void AddLootEffect(CGUnit_C* unit, SEQUENCE_CALLBACK callback);
    void AddMountTransition(uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target);
    void AddOrientation(uint32_t expireTime, float yaw, float pitch, uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID& source, const WOWGUID& target, uint32_t flags);
    void AddOrphanedEffect(uint32_t spellId, const SpellVisualKitRec* kit, const SpellVisualEffectNameRec* effectName, uint32_t flags, SEQUENCE_CALLBACK callback, const C3Vector& position, uint32_t param, WOWGUID transport);
    void AddSoundEffect(uint32_t expireTime, uint32_t spellId, CGObject_C* object, const WOWGUID& target, const C3Vector* position, uint32_t param);
    void AddSpecialByPosition(uint32_t type, C3Vector position, const WOWGUID& guid, uint32_t flags, SEQUENCE_CALLBACK callback);
    void AddSpecialEffect(uint32_t type, const WOWGUID& guid, SEQUENCE_CALLBACK callback);
    void ApplyItemVisual();
    void ClearItemVisualEffect();
    void ClearLightning();
    void ConnectToLightningThisFrame();
    int32_t CreateModelFromEffect(SEQUENCE_CALLBACK callback, uint32_t displayId, uint32_t inventoryType);
    void DetachFromParent();
    void EffectUpdate();
    int32_t GetCamouflage(float& alpha, float& fadeTime) const;
    float GetClampedImpactEffectScale(float scale, const CGObject_C* object) const;
    float GetCurveAlpha(float elapsed);
    int32_t GetOrientationAngles(float& yaw, float& pitch) const;
    void InitLightningEndpoint();
    void LinkTo(CEffect** head);
    void LoadGuildTextures();
    int32_t LoadMountTransitionModel();
    void MoveToDestroyList();
    void MoveToOrphanList(int32_t stopAnimKit);
    void PlayAnimKit(CGUnit_C* unit, AnimKitStopCallback callback, uint64_t param);
    void PostInitialize();
    void Release();
    void ReleaseLightningRef();
    void SetEmissionRateIgnoresDistance(int32_t ignore);
    void SetEnabled(int32_t enabled);
    void StopAnimKit(int32_t keepCallback, CAnimKitInst** stopKitInst);
    C44Matrix TransformPosition(CGObject_C* object);
    C3Vector TransformPositionByTransport();
    void UnLinkSelf();
    void UnrootFromCaster();
    int32_t UpdateAttachment();
    void UpdateWorldObject(float elapsed, uint32_t flags);
};

const SpellVisualEffectNameRec* GetSpecialSpellVisualEffectNameRec(int32_t id);

#endif
