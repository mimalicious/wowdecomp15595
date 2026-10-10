#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_DYNAMICOBJECT_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_DYNAMICOBJECT_C_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/Object/DynamicObject.h"
#include "WoW/Source/Object/Passenger.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/SoundInterface/SI3.h"

class BlizzardObject;
class CClientObjCreate;
class SpellRec;
class SpellVisualEffectNameRec;
class SpellVisualKitRec;

class CGDynamicObject_C : public CGObject_C, public CGDynamicObject {
    public:
    static void BlizzardDeletedCallback(BlizzardObject* blizzard, void* param);
    static void Initialize();

    /* 0100 */ CPassenger m_passenger;
    /* 0130 */ TSList<void> m_dynamicObjectMirrorLists [6];
    /* 0178 */ uint32_t m_hasStandLoop : 1;
    /* 0178 */ uint32_t m_inTransit : 1;
    /* 017C */ float m_scale;
    /* 0180 */ BlizzardObject* m_blizzardVisual [2];
    /* 0188 */ uint32_t m_sound;

    ~CGDynamicObject_C();
    void PostInit(uint32_t time, const CClientObjCreate& create, bool reenable) override;
    void Disable() override;
    void Reenable(const CClientObjCreate& create) override;
    void PostReenable(const CClientObjCreate& create) override;
    void HandleOutOfRange(OUT_OF_RANGE_TYPE type) override;
    C3Vector GetPosition() const override;
    C3Vector GetRawPosition() const override;
    float GetFacing() const override;
    float GetRawFacing() const override;
    float GetScale() const override;
    uint64_t GetTransportGUID() const override;
    void SetFrameOfReference(const C44Matrix* matrix) override;
    int32_t GetModelFileName(const char** name) const override;
    void ModelLoaded(CM2Model* model) override;
    void ShouldRender(uint32_t flags, int32_t& render, int32_t& a3) override;
    C44Matrix GetMatrix() const override;

    CGDynamicObject_C(uint32_t time, const CClientObjCreate& create);
    void ClearSound();
    void ClearVisuals();
    const SpellVisualEffectNameRec* GetVisualEffectNameRec(const SpellRec* spell, const SpellVisualKitRec*& kit) const;
    void HandleAnimEvent(uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time);
    void ObjectVisKitProc();
    void SetStorage(uint32_t* storage, uint32_t* saved);
    void UpdateDisplay(uint32_t time);
};

#endif
