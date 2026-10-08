#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_EFFECT_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_EFFECT_C_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/SoundInterface/SI3.h"

class C44Matrix;
class CGObject_C;
class CGUnit_C;
class CMapEntity;
class LightningObject;
class MountTransitionObject;
class CM2Lighting;
class CM2Model;
class SpellVisualEffectNameRec;
class SpellVisualKitModelAttachRec;
class SpellVisualKitRec;

class CEffect {
    public:
    void ApplyItemVisual();
    static void ApplyItemVisualByItemId(int32_t id, const WOWGUID& guid, void* param, bool success);
    static void MountTransitionCacheCB(int32_t id, const WOWGUID& guid, void* param, bool success);
    int32_t LoadMountTransitionModel();
    void ClearItemVisualEffect();
    void ClearLightning();
    C3Vector TransformPositionByTransport();
    static float __stdcall ComputeModelScaleFromTarget(CGObject_C* object);
    static void ApplyModelAttachRecToMatrix(const SpellVisualKitModelAttachRec* attach, C44Matrix& matrix, float facing);
    C44Matrix* TransformPosition(C44Matrix* matrix, CGObject_C* object);
    void PostInitialize();
    void UpdateWorldObject(float elapsed, uint32_t flags);
    void AddLootEffect(CGUnit_C* unit, void (*callback)(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7));
    static void CleanUpStaticLists();
    static void Destroy();
    static void DrawFishingString(CM2Model* model, const CM2Lighting* lighting, void* arg);

    /* 0000 */ CM2Model* m_model;
    /* 0004 */ CM2Model* m_lineModel;
    /* 0008 */ WOWGUID m_sourceGuid;
    /* 0010 */ WOWGUID m_targetGuid;
    /* 0018 */ uint32_t m_spellId;
    /* 001C */ const SpellVisualKitRec* m_kitRec;
    /* 0020 */ const SpellVisualEffectNameRec* m_effectNameRec;
    /* 0024 */ int32_t m_attachmentId;
    /* 0028 */ C3Vector m_offset;
    /* 0034 */ uint32_t m_soundKitObject;
    /* 0038 */ uint32_t m_flags;
    /* 003C */ LightningObject* m_lightning [12];
    /* 006C */ uint32_t unk_007C;
    /* 0070 */ uint32_t unk_0080;
    /* 0074 */ uint32_t unk_0084;
    /* 0078 */ float m_facing;
    /* 007C */ int32_t unk_008C;
    /* 0080 */ MountTransitionObject* m_mountTransition;
    /* 0084 */ int32_t m_pendingCacheId;
    /* 0088 */ int32_t m_displayId;
    /* 008C */ int32_t m_itemSlot;
    /* 0090 */ uint32_t unk_00A0;
    /* 0094 */ C3Vector m_position;
    /* 00A0 */ C3Vector m_worldPosition;
    /* 00AC */ uint32_t unk_00BC;
    /* 00B0 */ WOWGUID m_transportGuid;
    /* 00B8 */ uint32_t unk_00C8;
    /* 00BC */ uint32_t m_expireTime;
    /* 00C0 */ uint32_t unk_00D0;
    /* 00C4 */ uint32_t m_attachToModel;
    /* 00C8 */ uint32_t m_attachToMount;
    /* 00CC */ uint32_t unk_00DC;
    /* 00D0 */ float unk_00E0;
    /* 00D4 */ float unk_00E4;
    /* 00D8 */ float unk_00E8;
    /* 00DC */ CImVector m_color;
    /* 00E0 */ uintptr_t m_mapEntity;
    /* 00E4 */ int32_t m_refCount;
    /* 00E8 */ uint32_t m_lightningRefCount;
    /* 00EC */ int32_t m_lightningState;
    /* 00F0 */ const SpellVisualKitModelAttachRec* m_attachRec;
    /* 00F4 */ CEffect** m_prevLink;
    /* 00F8 */ CEffect* m_next;
    /* 00FC */ uint32_t unk_010C;

    CEffect();
    ~CEffect();
    void AddFishingLine(CGObject_C* object, const WOWGUID& target);
    static void CreateModelFromEffect(CM2Model* model, void* arg);
    void AddAnimKitEffect(int32_t spellId, const SpellVisualKitRec* kit, const WOWGUID* guid, uint32_t flags, uint32_t a5);
    void AddSoundEffect(int32_t soundId, int32_t a2, int32_t spellId, const C3Vector* position, CGObject_C* object, uint32_t a6);
    static void __stdcall DestroyMatchingEffects(CGObject_C* object, const SpellVisualEffectNameRec* effectName, int32_t attachType, int32_t spellId, int32_t kitId, const SpellVisualKitModelAttachRec* attach);
    static void EffectEvent(CM2Model* model, uint32_t boneId, uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time, uint64_t param);
    void AddEffect(int32_t attachType, int32_t spellId, const SpellVisualKitRec* kit, const SpellVisualEffectNameRec* effectName, const WOWGUID* target, const WOWGUID* source, uint32_t flags, void (*callback)(CM2Model*, uint32_t, uint32_t, int32_t, uint32_t, uint64_t, int32_t), const C3Vector* position, uint32_t a10, const SpellVisualKitModelAttachRec* attach);
    void InitFromUnit(C3Vector position, uint32_t time, int32_t spellId, const SpellVisualKitRec* kit, const SpellVisualEffectNameRec* effectName, const WOWGUID* guid, uint32_t flags, void (*callback)(CM2Model*, uint32_t, uint32_t, int32_t, uint32_t, uint64_t, int32_t), uint32_t a9, const SpellVisualKitModelAttachRec* attach, WOWGUID transport);
    void AddSpecialEffect(int32_t type, const WOWGUID& target, void (*callback)(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7));
    void AddSpecialByPosition(int32_t type, C3Vector position, const WOWGUID* guid, uint32_t flags, void (*callback)(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7));
    void ConnectToLightningThisFrame();
    void DetachFromParent();
    int32_t UpdateAttachment();
    void InitLightningEndpoint();
    void LinkTo(CEffect** head);
    void AddLightning(int32_t a2, uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID* guid, uint32_t flags);
    void AddMountTransition(uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID* guid);
    void InitAtPosition(int32_t spellId, const SpellVisualKitRec* kit, const SpellVisualEffectNameRec* effectName, uint32_t flags, void (*callback)(CM2Model*, uint32_t, uint32_t, int32_t, uint32_t, uint64_t, int32_t), const C3Vector* position, uint32_t a8, WOWGUID transport);
    void CreateProceduralEffectImpl(int32_t a2, float param, uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID* guid, uint32_t flags);
    void InitProceduralEffect(int32_t a2, int32_t param, uint32_t spellId, const SpellVisualKitRec* kit, const WOWGUID* guid, uint32_t flags);
    void MoveToDestroyList();
    void ReleaseLightningRef();
    void EffectUpdate();
    void MoveToOrphanList();
    void Release();
    void SetEmissionRateIgnoresDistance(int32_t ignore);
    void SetEnabled(int32_t enabled);
    void UnLinkSelf();
};

const SpellVisualEffectNameRec* GetSpecialSpellVisualEffectNameRec(int32_t effect);
int32_t SpecialUnitEffectsInitialize();

#endif
