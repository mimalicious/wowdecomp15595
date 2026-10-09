#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_OBJECT_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_OBJECT_C_H

#include <cstddef>
#include <cstdint>
#include <windows.h>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/Object/Object.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/Ui/DBCACHEHASH_NAME.h"

class CObjectEffectManager;
class CTexture;
template <class T>
class TSObjectAllocPool;
struct CVar;
class C3Vector;
class C44Matrix;
class C4Quaternion;
class CClientObjCreate;
class CEffect;
class CGBag_C;
class CGObject_C;
class CGWorldFrame;
class CHashKeyGUID;
class CImVector;
class CM2Model;
class CMapEntity;
class PlaySpellVisuaKitData;
class CPassenger;
class PLAYERNAMEDESC;
class SpellRec;
class SpellVisualKitRec;
class SpellVisualEffectNameRec;
class SpellVisualKitModelAttachRec;
class SpellVisualRec;
enum OUT_OF_RANGE_TYPE : int32_t {
};
enum SPELL_VISUAL_KIT_TYPE : int32_t {
};

class CHashKeyGUID {
    public:
    /* 0000 */ WOWGUID m_guid;

    CHashKeyGUID();
    CHashKeyGUID(uint64_t guid);
    bool operator==(const CHashKeyGUID& key);
};

class PlaySpellVisualKitAnimData {
    public:
    /* 0000 */ const SpellRec* m_spellRec;
    /* 0004 */ const SpellVisualKitRec* m_kit;
    /* 0008 */ uint32_t m_kitType;
    /* 000C */ const C3Vector* m_position;
    /* 0010 */ uint32_t m_animParam;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ WOWGUID m_guid;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ const SpellVisualRec* m_visual;
    /* 0028 */ uint32_t m_flags;
    /* 002C */ int32_t m_hasEffect;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ uint32_t unk_0034;
    /* 0038 */ uint32_t unk_0038;
    /* 003C */ uint32_t unk_003C;
    /* 0040 */ uint32_t unk_0040;
};

class CGObject_C : public CGObject, public TSHashObject<CGObject_C, CHashKeyGUID> {
    public:
    typedef void (*EFFECT_CALLBACK)(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7);

    struct InteractIcon {
        /* 0000 */ CM2Model* m_model;
        /* 0004 */ float m_scale;
        /* 0008 */ const char* m_fileName;
        /* 000C */ uint32_t m_npcFlagMask;
    };

    struct FadeoutInfo {
        /* 0000 */ uint32_t m_handle;
        /* 0004 */ uint32_t m_startTime;
        /* 0008 */ float m_alpha;
        /* 0010 */ WOWGUID m_transportGuid;
        /* 0018 */ C44Matrix m_matrix;
    };

    static InteractIcon s_interactIcons[12];
    static const uint32_t s_questStatusIcons[11];
    static CTexture* s_selectionTexture;
    static CVar* s_cvObjectSelectionCircle;
    static TSObjectAllocPool<FadeoutInfo>* s_fadeoutHeap;

    static void Initialize();
    static void Shutdown();
    static void PostShutdown();
    static void CreateFadeoutHeap();
    static void UpdateFadeoutList();
    static void FadeoutWorldObjectCallback(C44Matrix& matrix, uint64_t transportGuid);
    static void GuildTextureCallback(const uint64_t& guildGUID, const WOWGUID& guid, void* param, bool success);
    static bool GetGuildLogoInfo(int32_t background, int32_t borderColor, int32_t emblemColor, int32_t emblemStyle, CImVector& backgroundColor, CImVector& border, CImVector& emblem, char* fileName, int32_t size);
    static bool GetGuildLogoInfo(uint64_t guildGUID, CImVector& backgroundColor, CImVector& border, CImVector& emblem, char* fileName, int32_t size);
    static void GetDefaultGuildTextureSettings(CImVector& backgroundColor, CImVector& border, CImVector& emblem, char* fileName, int32_t size, int32_t factionIndex);
    static void ReplaceGuildTextures(CM2Model* model, const CImVector& backgroundColor, const CImVector& border, const CImVector& emblem, const char* fileName);
    static void ReplaceGuildTextures(CM2Model* model, uint64_t guildGUID, int32_t factionIndex);
    static void ModelLoadedCallback(CM2Model* model, void* arg);
    static int32_t UpdateAllObjectEffectManagers();
    static int32_t UpdateAllWorldObjects();
    static CGObject_C* GetFocusObject();
    static int32_t CanSquelchEffect(uint64_t guid);
    static void RetargetEffects(CEffect* effect, uint32_t spellId, uint64_t oldTarget, uint64_t newTarget);
    static void __stdcall BuildSelectionRotMatrix(C44Matrix& matrix, const C3Vector& position);
    static int32_t SetCircleRenderStates();
    static void LoopEffectHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7);
    static void EffectOnceHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7);
    static void EffectDecayHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7);
    static void AuraDecayHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7);
    static void AuraEffectHoldHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7);
    static void AuraEffectHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7);

    /* 0038 */ TSLink<void> m_link;
    /* 0040 */ uint32_t m_disableTime;
    /* 0044 */ TSList<void> m_objMirrorLists [8];
    /* 00A4 */ CM2Model* m_interactIcon;
    /* 00A8 */ uint32_t m_questStatus;
    /* 00AC */ uint32_t m_interactIconType;
    /* 00B0 */ float m_scale;
    /* 00B4 */ float m_scaleModifier;
    /* 00B8 */ uint32_t m_scaleChangeTime;
    /* 00BC */ float m_scaleChangeStart;
    /* 00C0 */ CEffect* m_effectList;
    /* 00C4 */ float m_height;
    /* 00C8 */ PLAYERNAMEDESC* m_nameDesc;
    /* 00CC */ CM2Model* m_worldModel;
    /* 00D0 */ uintptr_t m_worldObject;
    /* 00D4 */ uint32_t m_modelFlags;
    /* 00D8 */ uint32_t m_fadeStartTime;
    /* 00DC */ uint32_t m_fadeDuration;
    /* 00E0 */ uint8_t m_alpha;
    /* 00E1 */ uint8_t m_fadeStartAlpha;
    /* 00E2 */ uint8_t m_fadeEndAlpha;
    /* 00E3 */ uint8_t m_alphaScale;
    /* 00E4 */ CObjectEffectManager* m_effectManager;
    /* 00E8 */ TSLink<CGObject_C> m_fadeoutLink;
    /* 00F0 */ FadeoutInfo* m_fadeoutInfo;

    virtual void PostInit(uint32_t time, const CClientObjCreate& create, bool reenable);
    virtual void Disable();
    virtual void Reenable(const CClientObjCreate& create);
    virtual void PostReenable(const CClientObjCreate& create);
    virtual void HandleOutOfRange(OUT_OF_RANGE_TYPE type);
    virtual void UpdateWorldObject(int32_t a2);
    virtual int32_t ShouldFadeout();
    virtual void UpdateDisplayInfo(int32_t a2);
    virtual C3Vector GetNamePosition() const;
    virtual CGBag_C* GetBag();
    virtual const CGBag_C* GetBag() const;
    virtual C3Vector GetPosition() const;
    virtual C3Vector GetRawPosition() const;
    virtual float GetFacing() const;
    virtual float GetRawFacing() const;
    virtual float GetScale() const;
    virtual uint64_t GetTransportGUID() const;
    virtual C4Quaternion GetRotation() const;
    virtual void SetFrameOfReference(const C44Matrix* matrix);
    virtual int32_t IsQuestGiver() const;
    virtual void RefreshInteractIcon();
    virtual void UpdateInteractIcon();
    virtual void UpdateInteractIconAttach();
    virtual void UpdateInteractIconScale();
    virtual int32_t GetModelFileName(const char** name) const;
    virtual void ScaleChangeUpdate();
    virtual void ScaleChangeFinished();
    virtual void RenderTargetSelection() const;
    virtual void RenderPetTargetSelection() const;
    virtual void Render();
    virtual int32_t GetSelectionHighlightColor(CImVector* color) const;
    virtual float GetTrueScale() const;
    virtual void HandleGuildIDUpdate();
    virtual bool LoadGuildTextures(uint64_t guildGUID);
    virtual void ModelLoaded(CM2Model* model);
    virtual void ApplyAlpha(uint32_t time);
    virtual void PreAnimate(CGWorldFrame* frame);
    virtual int32_t Animate(float elapsed);
    virtual void ShouldRender(uint32_t flags, int32_t& render, int32_t& a3);
    virtual float GetRenderFacing() const;
    virtual void OnSpecialMountAnim();
    virtual int32_t IsSolidSelectable() const;
    virtual int32_t CanHighlight() const;
    virtual int32_t CanBeTargetted() const;
    virtual int32_t FloatingTooltip() const;
    virtual void OnRightClick();
    virtual void OnInteract();
    virtual int32_t IsHighlightSuppressed() const;
    virtual void OnSpellEffectClear(CEffect* effect);
    virtual const SpellVisualRec* GetAppropriateSpellVisual(const SpellRec* spell, SpellVisualRec& visual, int32_t a3, int32_t a4) const;
    virtual float AddSquelchedEffect();
    virtual void ConnectToLightningThisFrame(uint32_t a1, int32_t a2);
    virtual C44Matrix GetMatrix() const;
    virtual void ObjectNameVisibilityChanged();
    virtual int32_t UpdateObjectNameString(int32_t a1, char* buffer, uint32_t size);
    virtual int32_t ShouldRenderObjectName(uint32_t flags) const;
    virtual CM2Model* GetObjectModel() const;
    virtual const char* GetObjectName() const;
    virtual int32_t GetPageTextID(void (*callback)(int32_t, const WOWGUID&, void*, bool)) const;
    virtual void CleanUpVehicleBoneAnimsBeforeObjectModelChange();
    virtual int32_t ShouldFadeIn() const;
    virtual float GetBaseAlpha() const;
    virtual bool IsTransport() const;
    virtual int32_t IsPointInside(const C3Vector& point) const;
    virtual int32_t AddPassenger(CPassenger* passenger, int32_t a2);
    virtual float GetSpeed() const;
    virtual void PlaySpellVisualKit_PlayAnims(const PlaySpellVisualKitAnimData* data);
    virtual void PlaySpellVisualKit_HandleWeapons(const SpellRec* spell, const SpellVisualKitRec* kit, const C3Vector* position, WOWGUID guid, uint32_t a5, uint32_t flags, void (*callback)(CM2Model*, uint32_t, uint32_t, int32_t, uint32_t, uint64_t, int32_t), int32_t* hasEffect, uint32_t a9, int32_t a10);
    virtual int32_t PlaySpellVisualKit_DelayLightningEffects(const SpellRec* spell, const SpellVisualKitRec* kit, SPELL_VISUAL_KIT_TYPE type, const C3Vector* position, WOWGUID guid, int32_t a6, int32_t a7, int32_t a8, int32_t a9, uint32_t a10, const SpellVisualRec* visual, uint32_t a12, int32_t a13);

    CGObject_C();
    CGObject_C(uint32_t time, const CClientObjCreate& create);
    ~CGObject_C();
    void SetTypeID(OBJECT_TYPE_ID typeId);
    void ReleaseModel();
    void SetBoundsDirty();
    int32_t IsObjectModelLoaded();
    int32_t IsObjectLocked();
    void SetObjectLocked(int32_t locked);
    void SetDisablePending(int32_t pending);
    int32_t IsReadyToDraw();
    void FadeoutWorldObject();
    CM2Model* SetModelStart(CM2Model* model);
    void SetModelFinish(CM2Model* model);
    void SetModel(CM2Model* model);
    void AddWorldObject();
    void UpdateEffectAttachments();
    void ClearSpellEffect(uint32_t spellId, int32_t all, uint64_t casterGuid);
    void ClearEffectKitByID(uint32_t spellId, int32_t kitId, uint32_t param, int32_t matchSpell, int32_t matchParam);
    void SetBlock(uint32_t index, uint32_t value);
    void HideHighlightType(int32_t type, int32_t force);
    void ShowHighlightType(int32_t type, int32_t force);
    void DoFade(float alpha, uint32_t time);
    void HandleScaleUpdate(float scale);
    void ModelChanged();
    void RemoveInteractIcon();
    void UpdateInteractIcon(uint32_t status);
    int32_t GetQuestGiverCursor(int32_t highlighted);
    uint32_t GetQuestInteractIcon();
    uint32_t GetNPCInteractIcon();
    C3Vector GetModelPosition();
    C44Matrix GetModelMatrix();
    void PlayEffectName(int32_t attachType, uint32_t expireTime, uint32_t spellId, const SpellVisualKitRec* kit, const SpellVisualEffectNameRec* effectName, uint32_t& flags, EFFECT_CALLBACK callback, const C3Vector* position, uint64_t target, uint32_t param, const SpellVisualKitModelAttachRec* attach, int32_t kitType, uint32_t displayId, int32_t inventoryType);
    void ClearEffectList(int32_t reason);
    void ClearEffectKit(uint32_t spellId, int32_t kitType, uint32_t param, int32_t matchSpell, int32_t matchParam);
    void SetInteractIconVisible(int32_t visible);
    void PlaySpellVisualKit(const PlaySpellVisuaKitData& data);
    void PrepareToFadeInt(float alpha, uint64_t transportGuid, const C44Matrix& matrix);
    void AddNonModelToFadeoutList(float alpha, uint64_t transportGuid);
    int32_t PrepareToFade(float alpha, uint64_t transportGuid);
    void RemoveFromFadeoutList();
    void Fadeout();
    void SetStorage(uint32_t* storage, uint32_t* saved);
    WOWGUID GetGuildGUID() const;
};

struct DBCACHEHASH_NAME : public TSHashObject<DBCACHEHASH_NAME, CHashKeyGUID> {
    /* 0020 */ uint8_t m_record [336];
    /* 0170 */ DBCACHETRAILER_GUID m_trailer;
};

namespace World {
    int32_t QueryObjectVisible(uintptr_t entity);
    void TickObject(uintptr_t entity);
}

#include "WoW/Source/Object/ObjectClient/Object_C.inl"

#endif
