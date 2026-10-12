#include "AreaTrigger_C.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellVisualEffectNameRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitRec.h"
#include "WoW/Source/DB/Rec/SpellVisualRec.h"
#include "WoW/Source/ObjectMgrClient/CClientObjCreate.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// INLINED
CGAreaTriggerPassenger::CGAreaTriggerPassenger(const WOWGUID& transportGuid, const C3Vector& position, const WOWGUID& ownerGuid) : m_passenger(transportGuid, position, ownerGuid) {
}

// OFFSET: 0x005FA180
void CGAreaTrigger_C::Disable() {
    CGObject_C::Disable();
    this->m_passenger.m_link.Unlink();
}

// OFFSET: 0x005FA1A0
void CGAreaTrigger_C::PlayStandAnim() {
    if (!this->m_hasStandLoop) {
        this->GetObjectModel()->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    } else {
        this->GetObjectModel()->SetBoneSequence(0xFFFFFFFF, 158, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    }
}

// PROVEN: b5a58ca6 IDENTICAL/IDENTICAL
// OFFSET: 0x005FA1F0/0x14027F170
void CGAreaTrigger_C::UpdateDisplay(uint32_t time) {
    CM2Model* model = this->GetObjectModel();

    if (model) {
        model->SetEmittersEnabled(1);
        model->SetRibbonsEnabled(1);
    }
}

// PROVEN: d7726d37 IDENTICAL/IDENTICAL
// OFFSET: 0x005FA220/0x14027F1B0
void CGAreaTrigger_C::ShouldRender(uint32_t flags, int32_t& render, int32_t& a3) {
    CGObject_C::ShouldRender(flags, render, a3);
}

// PROVEN: a61dfffe IDENTICAL/IDENTICAL
// OFFSET: 0x005FA230/0x14027F1C0
void CGAreaTrigger_C::SetFrameOfReference(const C44Matrix* matrix) {
    CGObject_C::SetFrameOfReference(matrix);
}

// PROVEN: ebcf092b DIFFERENT/IDENTICAL
// OFFSET: 0x005FA240/0x14027F1D0
static void AreaTriggerDecayHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    CGAreaTrigger_C* object = reinterpret_cast<CGAreaTrigger_C*>(static_cast<uintptr_t>(param));

    if (object) {
        ClntObjMgrUnlockObject(object->m_obj->m_guid, __FILE__, __LINE__);
    }
}

// PROVEN: 2271dc6a IDENTICAL/DIFFERENT
// OFFSET: 0x005FA270/0x14027F200
void CGAreaTrigger_C::HandleOutOfRange(OUT_OF_RANGE_TYPE type) {
    if (type != 1) {
        return;
    }

    CM2Model* model = this->GetObjectModel();

    if (model && model->IsLoaded() && model->HasSequence(159)) {
        ClntObjMgrLockObject(this->m_obj->m_guid);
        model->SetSequenceCallback(reinterpret_cast<void*>(&AreaTriggerDecayHandler), reinterpret_cast<uintptr_t>(this));
        model->SetBoneSequence(0xFFFFFFFF, 159, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    }
}

// PROVEN: f154334a IDENTICAL/NONE
// OFFSET: 0x005FA300
void CGAreaTrigger_C::SetStorage(uint32_t* storage, uint32_t* saved) {
    this->CGObject_C::SetStorage(storage, saved);
    this->m_areaTrigger = reinterpret_cast<CGAreaTriggerData*>(storage + OBJECT_END);
    this->m_areaTriggerMirror = saved + CGObject::TotalFieldsSaved();
}

// PROVEN: 03166c26 IDENTICAL/IDENTICAL
// OFFSET: 0x005FA340/0x14027F2C0
C3Vector CGAreaTrigger_C::GetRawPosition(uint32_t time) const {
    float progress = static_cast<float>(static_cast<int32_t>(time - this->m_createTime)) / static_cast<float>(this->m_areaTrigger->m_areatriggerDuration);

    if (progress < 0.0f) {
        progress = 0.0f;
    } else if (!(1.0f > progress)) {
        progress = 1.0f;
    }

    C3Vector start = this->m_passenger.m_position;
    return (*reinterpret_cast<const C3Vector*>(this->m_areaTrigger->m_areatriggerFinalPos) - start) * progress + start;
}

// PROVEN: ee705007 IDENTICAL/NONE
// OFFSET: 0x005FA410
static void AnimFinishedCallback(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    if (interrupted) {
        return;
    }

    CGAreaTrigger_C* object = static_cast<CGAreaTrigger_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_AREATRIGGER, __FILE__, __LINE__));

    if (object) {
        object->PlayStandAnim();
    }
}

// OFFSET: 0x005FA450/0x14027F430
void CGAreaTrigger_C::Reenable(const CClientObjCreate& create) {
    if (create.m_hasGOTransport) {
        const CMovementStatus::TransportInfo& transport = create.m_goTransport;
        this->m_passenger.Reenable(transport.m_guid, transport.m_position);
    } else {
        this->m_passenger.Reenable(WOWGUID::Make(0), create.m_stationaryPosition);
    }

    CGObject_C::Reenable(create);
}

// PROVEN: 3a373977 IDENTICAL/DIFFERENT
// OFFSET: 0x005FA4A0/0x14027F430
void CGAreaTrigger_C::PostReenable(const CClientObjCreate& create) {
    this->m_passenger.PostInit(create.m_stationaryPosition);
    CGObject_C::PostReenable(create);
}

// PROVEN: e869b79d IDENTICAL/IDENTICAL
// OFFSET: 0x005FA4D0/0x14027F470
void CGAreaTrigger_C::HandleAnimEvent(uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time) {
    SE3SoundKitProperties properties;

    if (eventId == 0x444E5324) {
        SE3::PlaySoundKit(data, position, &properties, NULL, __FILE__, __LINE__, "CGAreaTrigger_C::HandleAnimEvent() $SND", 0, NULL);
    }
}

// PROVEN: f1d87232 DIFFERENT/IDENTICAL
// OFFSET: 0x005FA540/0x14027F510
C44Matrix CGAreaTrigger_C::GetMatrix() const {
    C44Matrix matrix;
    reinterpret_cast<C3Vector&>(matrix.d0) = this->GetRawPosition(FrameTime::GetCurTimeMs());
    matrix.RotateAroundZ(this->GetRawFacing());

    if (this->GetTransportGUID()) {
        CGObject_C* transport = ClntObjMgrObjectPtr(WOWGUID::Make(this->GetTransportGUID()), TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (transport) {
            matrix *= transport->GetMatrix();
        }
    }

    return matrix;
}

// PROVEN: 2c4671a5 IDENTICAL/UNPROVEN
// OFFSET: 0x005FA640/0x14027F620
static void AnimEventCallback(CM2Model* model, uint32_t boneId, uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time, uint64_t param) {
    CGAreaTrigger_C* object = static_cast<CGAreaTrigger_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_AREATRIGGER, __FILE__, __LINE__));

    if (object) {
        object->HandleAnimEvent(eventId, data, position, time);
    }
}

// OFFSET: 0x005FA680/0x14027F680
void CGAreaTrigger_C::PostInit(uint32_t time, const CClientObjCreate& create, bool reenable) {
    this->m_passenger.PostInit(create.m_stationaryPosition);

    if (!this->GetTransportGUID()) {
        this->m_passenger.m_facing = create.m_stationaryFacing;
    }

    CGObject_C::PostInit(time, create, reenable);

    if (this->GetObjectModel()) {
        this->GetObjectModel()->SetEventCallback(reinterpret_cast<void*>(&AnimEventCallback), this->m_obj->m_guid.m_guid);
        this->GetObjectModel()->SetSequenceCallback(reinterpret_cast<void*>(&AnimFinishedCallback), this->m_obj->m_guid.m_guid);
    }

    this->Animate(0.0f);
}

// PROVEN: b9ab70ca IDENTICAL/UNPROVEN
// OFFSET: 0x005FA740/0x14027F770
const SpellVisualEffectNameRec* CGAreaTrigger_C::GetVisualEffectNameRec(const SpellRec* spell, const SpellVisualKitRec*& kit) const {
    kit = NULL;

    if (!spell) {
        SysMsgPrintf(STATUS_WARNING, 2, "NOSPELLIDFOUND|%d", this->m_areaTrigger->m_areatriggerSpellid);
        return NULL;
    }

    int32_t quiet = spell->m_attributesExI & 0x200000;
    const SpellVisualRec* visual = g_spellVisualDB.GetRecord(this->m_areaTrigger->m_areatriggerSpellvisualid);

    if (!visual) {
        if (!quiet) {
            SysMsgPrintf(STATUS_WARNING, 2, "SPELLVISUALIDNOTFOUND|%d|%d", this->m_areaTrigger->m_areatriggerSpellvisualid, this->m_areaTrigger->m_areatriggerSpellid);
        }

        return NULL;
    }

    kit = g_spellVisualKitDB.GetRecord(visual->m_persistentAreaKit);

    if (!kit) {
        if (!quiet) {
            SysMsgPrintf(STATUS_WARNING, 2, "SPELLVISUALKITIDNOTFOUND|%d", visual->m_persistentAreaKit);
        }

        return NULL;
    }

    const SpellVisualEffectNameRec* effect = g_spellVisualEffectNameDB.GetRecord(kit->m_worldEffect);

    if (!effect) {
        effect = g_spellVisualEffectNameDB.GetRecord(kit->m_baseEffect);

        if (!effect && !quiet) {
            SysMsgPrintf(STATUS_WARNING, 2, "SPELLEFFECTIDNOTFOUND|%d", kit->m_worldEffect);
        }
    }

    return effect;
}

// PROVEN: fa202c87 IDENTICAL/DIFFERENT
// OFFSET: 0x005FA880/0x14027F900
int32_t CGAreaTrigger_C::GetModelFileName(const char** name) const {
    const SpellRec* spell = g_spellDB.GetRecord(this->m_areaTrigger->m_areatriggerSpellid);

    if (!spell) {
        SysMsgPrintf(STATUS_WARNING, 2, "NOSPELLIDFOUND|%d", this->m_areaTrigger->m_areatriggerSpellid);
        return 0;
    }

    const SpellVisualKitRec* kit = NULL;
    const SpellVisualEffectNameRec* effect = this->GetVisualEffectNameRec(spell, kit);

    if (!effect) {
        return 0;
    }

    *name = effect->m_fileName;
    return *name != NULL;
}

// PROVEN: acd8e290 IDENTICAL/DIFFERENT
// OFFSET: 0x005FA900/0x14027F9D0
void CGAreaTrigger_C::ModelLoaded(CM2Model* model) {
    CGObject_C::ModelLoaded(model);
    CM2Model* objectModel = this->GetObjectModel();
    this->m_hasStandLoop = objectModel->HasSequence(158);
    const SpellRec* spell = g_spellDB.GetRecord(this->m_areaTrigger->m_areatriggerSpellid);
    const SpellVisualKitRec* kit = NULL;
    const SpellVisualEffectNameRec* effect = this->GetVisualEffectNameRec(spell, kit);

    if (spell->m_id && (spell->m_attributesExE & 0x40000000)) {
        model->m_fFlags |= 0x4;
    }

    if (effect) {
        this->DoFade(1.0f, 0);
    }

    if (objectModel->HasSequence(127)) {
        objectModel->SetBoneSequence(0xFFFFFFFF, 127, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    } else if (objectModel->HasSequence(0)) {
        objectModel->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    } else {
        objectModel->SetBoneSequence(0xFFFFFFFF, 158, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    }
}

// OFFSET: 0x005FAA20/0x14027FB30
CGAreaTrigger_C::CGAreaTrigger_C(uint32_t time, const CClientObjCreate& create) : CGObject_C(time, create), CGAreaTriggerPassenger(create.m_hasGOTransport ? create.m_goTransport.m_guid : WOWGUID::Make(0), create.m_hasGOTransport ? create.m_goTransport.m_position : create.m_stationaryPosition, this->m_obj->m_guid) {
    this->m_hasStandLoop = 0;
    this->m_createTime = time;
    this->m_passenger.m_facing = create.m_hasGOTransport ? create.m_goTransport.m_facing : create.m_stationaryFacing;
}

// PROVEN: 9c1ca5b8 IDENTICAL/IDENTICAL
// OFFSET: 0x005FABA0/0x14027FCC0
C3Vector CGAreaTrigger_C::GetPosition() const {
    return this->GetPosition(FrameTime::GetCurTimeMs());
}

// PROVEN: 62cb77c4 IDENTICAL/IDENTICAL
// OFFSET: 0x005FABD0/0x14027FD10
C3Vector CGAreaTrigger_C::GetRawPosition() const {
    return this->m_passenger.m_position;
}

// PROVEN: 9fb89a34 DIFFERENT/IDENTICAL
// OFFSET: 0x005FAC00/0x14027FD50
float CGAreaTrigger_C::GetFacing() const {
    return this->m_passenger.GetFacing(this->m_passenger.m_facing);
}

// PROVEN: 048218ab IDENTICAL/IDENTICAL
// OFFSET: 0x005FAC20/0x14027FD50
float CGAreaTrigger_C::GetRawFacing() const {
    return this->m_passenger.m_facing;
}

// PROVEN: b24a7a98 IDENTICAL/IDENTICAL
// OFFSET: 0x005FAC30/0x14027FD60
uint64_t CGAreaTrigger_C::GetTransportGUID() const {
    return this->m_passenger.m_transportGuid.m_guid;
}

// PROVEN: 6d70812b IDENTICAL/IDENTICAL
// OFFSET: 0x005FAC40/0x14027FD70
C3Vector CGAreaTrigger_C::GetPosition(uint32_t time) const {
    return this->m_passenger.GetPosition(this->GetRawPosition(time));
}

// OFFSET: 0x005FAC80/0x14027FDC0
CGAreaTrigger_C::~CGAreaTrigger_C() {
}
