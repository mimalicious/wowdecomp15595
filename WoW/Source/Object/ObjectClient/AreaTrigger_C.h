#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_AREATRIGGER_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_AREATRIGGER_C_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/Object/AreaTrigger.h"
#include "WoW/Source/Object/Passenger.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"

class CClientObjCreate;
class SpellRec;
class SpellVisualEffectNameRec;
class SpellVisualKitRec;

class CGAreaTriggerPassenger : public CGAreaTrigger {
    public:
    /* 0010 */ CPassenger m_passenger;

    virtual C3Vector GetPosition(uint32_t time) const = 0;

    CGAreaTriggerPassenger(const WOWGUID& transportGuid, const C3Vector& position, const WOWGUID& ownerGuid);
};

class CGAreaTrigger_C : public CGObject_C, public CGAreaTriggerPassenger {
    public:
    /* 0138 */ TSList<void> m_areaTriggerMirrorLists [6];
    /* 0180 */ uint32_t m_createTime;
    /* 0184 */ uint32_t m_hasStandLoop : 1;

    ~CGAreaTrigger_C();
    void PostInit(uint32_t time, const CClientObjCreate& create, bool reenable) override;
    void Disable() override;
    void Reenable(const CClientObjCreate& create) override;
    void PostReenable(const CClientObjCreate& create) override;
    void HandleOutOfRange(OUT_OF_RANGE_TYPE type) override;
    C3Vector GetPosition() const override;
    C3Vector GetRawPosition() const override;
    float GetFacing() const override;
    float GetRawFacing() const override;
    uint64_t GetTransportGUID() const override;
    void SetFrameOfReference(const C44Matrix* matrix) override;
    int32_t GetModelFileName(const char** name) const override;
    void ModelLoaded(CM2Model* model) override;
    void ShouldRender(uint32_t flags, int32_t& render, int32_t& a3) override;
    C44Matrix GetMatrix() const override;
    C3Vector GetPosition(uint32_t time) const override;
    virtual C3Vector GetRawPosition(uint32_t time) const;

    CGAreaTrigger_C(uint32_t time, const CClientObjCreate& create);
    const SpellVisualEffectNameRec* GetVisualEffectNameRec(const SpellRec* spell, const SpellVisualKitRec*& kit) const;
    void HandleAnimEvent(uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time);
    void PlayStandAnim();
    void SetStorage(uint32_t* storage, uint32_t* saved);
    void UpdateDisplay(uint32_t time);
};

#endif
