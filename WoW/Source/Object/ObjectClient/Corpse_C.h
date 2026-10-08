#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_CORPSE_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_CORPSE_C_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Object/Corpse.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/Passenger.h"

class CCharacterComponent;
class CClientObjCreate;
class CM2Model;

class CGCorpse_C : public CGObject_C, public CGCorpse {
    public:
    static void Initialize();
    /* 0100 */ CPassenger m_passenger;
    /* 0130 */ TSList<void> m_corpseMirrorLists [28];
    /* 0280 */ CCharacterComponent* m_component;
    /* 0284 */ C3Vector m_groundNormal;
    /* 0290 */ uint32_t m_corpseFlags;
    /* 0294 */ float m_selectionRadius;
    /* 0298 */ CM2Model* m_lootEffect;

    ~CGCorpse_C();
    int32_t ShouldFadeout() override;
    float GetTrueScale() const override;
    int32_t CanHighlight() const override;
    float GetFacing() const override;
    void ModelLoaded(CM2Model* model) override;
    void Disable() override;
    void PostInit(uint32_t time, const CClientObjCreate& create, bool reenable) override;
    void Reenable(const CClientObjCreate& create) override;
    void PostReenable(const CClientObjCreate& create) override;
    int32_t GetModelFileName(const char** name) const override;
    void RenderTargetSelection() const override;
    int32_t Animate(float elapsed) override;
    void ShouldRender(uint32_t flags, int32_t& render, int32_t& a3) override;
    void OnRightClick() override;
    void OnInteract() override;

    CGCorpse_C(uint32_t time, const CClientObjCreate& create);
    void ApplyGuildColor(int32_t request);
    int32_t CanBeLooted() const;
    void SetStorage(uint32_t* storage, uint32_t* saved);
    float GetDisplayScale() const;
    uint32_t GetFactionTemplate() const;
    const char* GetPortraitTexture() const;
    void ShowLootEffect();
    void OnFlagChanged(uint32_t oldFlags);
    void OnDynamicFlagChanged(uint32_t oldFlags);
    C3Vector GetGroundNormal();
};

int32_t CorpseDynamicFlagUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param);
int32_t CorpseFlagUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param);

#endif
