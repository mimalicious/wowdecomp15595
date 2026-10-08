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

class CGDynamicObject_C : public CGObject_C, public CGDynamicObject {
    public:
    static void Initialize();

    /* 0100 */ CPassenger m_passenger;
    /* 0130 */ TSList<void> m_dynamicObjectMirrorLists [6];
    /* 0178 */ uint32_t unk_0178;
    /* 017C */ float m_scale;
    /* 0180 */ BlizzardObject* m_blizzardVisual;
    /* 0184 */ uint32_t m_soundKit;
    /* 0188 */ uint32_t unk_0188;

    ~CGDynamicObject_C();
    virtual C3Vector GetPosition() const;
    virtual C3Vector GetRawPosition() const;
    virtual float GetFacing() const;
    float GetRawFacing() const override;
    float GetScale() const override;
    virtual C44Matrix GetMatrix() const;
    virtual void ShouldRender(uint32_t flags, int32_t& render, int32_t& a3);
    void SetFrameOfReference(const C44Matrix* matrix) override;
    void Disable() override;
    void PostInit(uint32_t time, const CClientObjCreate& create, bool reenable) override;
    void Reenable(const CClientObjCreate& create) override;
    void PostReenable(const CClientObjCreate& create) override;
    void HandleOutOfRange(OUT_OF_RANGE_TYPE type) override;
    void ModelLoaded(CM2Model* model) override;
    int32_t GetModelFileName(const char** name) const override;

    CGDynamicObject_C(uint32_t time, const CClientObjCreate& create);
    void HandleAnimEvent(uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time);
    void ObjectVisKitProc();
    void SetStorage(uint32_t* storage, uint32_t* saved);
    void UpdateDisplay(uint32_t time);
    SpellVisualEffectNameRec* GetVisualEffectNameRec(const SpellRec* spell) const;
};

#endif
