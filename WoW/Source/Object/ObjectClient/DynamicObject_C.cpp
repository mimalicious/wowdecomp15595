#include "DynamicObject_C.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellEffectCameraShakesRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellVisualEffectNameRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitRec.h"
#include "WoW/Source/DB/Rec/SpellVisualRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Magic/MagicClient/SpellVisuals.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/CClientObjCreate.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/StaticSingleton.h"
#include "WoW/Source/UIUtil/CameraManager.h"

// INLINED
void CGDynamicObject_C::ClearSound() {
    SE3::StopSound(this->m_sound, 3000, __FILE__, __LINE__, "CGDynamicObject_C::ClearSound");
}

// PROVEN: ffbb7f5f IDENTICAL/IDENTICAL
// OFFSET: 0x00596900/0x140204370
void CGDynamicObject_C::SetFrameOfReference(const C44Matrix* matrix) {
    CGObject_C::SetFrameOfReference(matrix);

    for (uint32_t i = 0; i < 2; i++) {
        if (this->m_blizzardVisual[i]) {
            SpellVisualsBlizzardSetFrameOfReference(this->m_blizzardVisual[i], matrix);
        }
    }
}

// PROVEN: ff7bbce8 IDENTICAL/DIFFERENT
// OFFSET: 0x00596940/0x1402043C0
void CGDynamicObject_C::BlizzardDeletedCallback(BlizzardObject* blizzard, void* param) {
    CGDynamicObject_C* object = static_cast<CGDynamicObject_C*>(param);

    if (blizzard == object->m_blizzardVisual[0]) {
        object->m_blizzardVisual[0] = NULL;
    }

    if (blizzard == object->m_blizzardVisual[1]) {
        object->m_blizzardVisual[1] = NULL;
    }
}

// PROVEN: d3da6d16 IDENTICAL/DIFFERENT
// OFFSET: 0x00596970/0x1402043F0
void CGDynamicObject_C::ClearVisuals() {
    for (uint32_t i = 0; i < 2; i++) {
        if (this->m_blizzardVisual[i]) {
            SpellVisualsBlizzardStop(this->m_blizzardVisual[i]);
        }
    }

    this->ClearSound();
}

// PROVEN: e847c2ba IDENTICAL/DIFFERENT
// OFFSET: 0x005969C0/0x140205A20
void CGDynamicObject_C::ShouldRender(uint32_t flags, int32_t& render, int32_t& a3) {
    CGObject_C::ShouldRender(flags, render, a3);

    if (this->m_inTransit) {
        a3 = 1;
    }
}

// PROVEN: 74ad724e DIFFERENT/IDENTICAL
// OFFSET: 0x00596A40/0x140204500
static void DynamicObjectDecayHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    CGDynamicObject_C* object = reinterpret_cast<CGDynamicObject_C*>(static_cast<uintptr_t>(param));

    if (object) {
        ClntObjMgrUnlockObject(object->m_obj->m_guid, __FILE__, __LINE__);
    }
}

// PROVEN: be54dd87 IDENTICAL/DIFFERENT
// OFFSET: 0x00596A70/0x140204530
void CGDynamicObject_C::HandleOutOfRange(OUT_OF_RANGE_TYPE type) {
    if (type != 1) {
        return;
    }

    CM2Model* model = this->GetObjectModel();

    if (model && model->IsLoaded() && model->HasSequence(159)) {
        ClntObjMgrLockObject(this->m_obj->m_guid);
        model->SetSequenceCallback(reinterpret_cast<void*>(&DynamicObjectDecayHandler), reinterpret_cast<uintptr_t>(this));
        model->SetBoneSequence(0xFFFFFFFF, 159, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    }
}

// OFFSET: 0x00596B00
static void AnimFinishedCallback(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    if (interrupted) {
        return;
    }

    CGDynamicObject_C* object = static_cast<CGDynamicObject_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_DYNAMICOBJECT, __FILE__, __LINE__));

    if (!object) {
        return;
    }

    if (object->m_hasStandLoop) {
        object->GetObjectModel()->SetBoneSequence(0xFFFFFFFF, 158, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    } else {
        object->GetObjectModel()->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    }
}

// OFFSET: 0x00596B40/0x140204630
void CGDynamicObject_C::ObjectVisKitProc() {
    if (this->m_inTransit) {
        return;
    }

    const SpellRec* spell = g_spellDB.GetRecord(this->m_dynamicObject->m_spellID);

    if (!spell) {
        return;
    }

    const SpellVisualRec* visual = g_spellVisualDB.GetRecord(this->m_dynamicObject->m_typeAndVisualID & 0xFFFFFFF);

    if (!CanPlaySpellVisual(this->m_dynamicObject->m_caster.m_guid, 0, visual)) {
        return;
    }

    const SpellVisualKitRec* kit = g_spellVisualKitDB.GetRecord(visual->m_persistentAreaKit);

    if (!CanPlaySpellVisualKit(this->m_dynamicObject->m_caster.m_guid, 0, kit, NULL)) {
        return;
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (kit->m_charProc[i] != 9) {
            continue;
        }

        uint32_t slot = 0;

        while (this->m_blizzardVisual[slot]) {
            if (++slot >= 2) {
                break;
            }
        }

        if (slot >= 2) {
            break;
        }

        int32_t areaModelID = static_cast<int32_t>(kit->m_charParamZero[i]);
        C3Vector position = this->GetRawPosition();
        BlizzardObject* blizzard = SpellVisualsBlizzardCreate(&position, this->m_dynamicObject->m_radius, 0.0f, this->m_dynamicObject->m_spellID, areaModelID);
        this->m_blizzardVisual[slot] = blizzard;
        SpellVisualsBlizzardSetDeletedCallback(blizzard, CGDynamicObject_C::BlizzardDeletedCallback, this);
    }

    if (kit->m_soundID) {
        this->ClearSound();
        C3Vector position = this->GetPosition();
        SE3SoundKitProperties properties;
        properties.ResetToDefaults();
        properties.m_fadeInTime = 2000;
        properties.m_usage = 1;
        SE3::PlaySoundKit(kit->m_soundID, &position, &properties, &this->m_sound, __FILE__, __LINE__, "CGDynamicObject_C::ObjectVisKitProc()", 0, NULL);
    }
}

// OFFSET: 0x00596D60/0x140206550
void CGDynamicObject_C::Disable() {
    CGObject_C::Disable();

    if (!this->m_fadeoutInfo) {
        for (uint32_t i = 0; i < 2; i++) {
            if (this->m_blizzardVisual[i]) {
                this->AddNonModelToFadeoutList(static_cast<float>(this->m_alpha) * 0.0039215689f, this->GetTransportGUID());
                break;
            }
        }
    }

    this->m_passenger.m_link.Unlink();
    this->ClearVisuals();
}

// OFFSET: 0x00596DE0/0x1402065D0
void CGDynamicObject_C::Reenable(const CClientObjCreate& create) {
    if (create.m_hasGOTransport) {
        this->m_passenger.Reenable(create.m_goTransport.m_guid, create.m_goTransport.m_position);
    } else {
        this->m_passenger.Reenable(WOWGUID::Make(0), create.m_stationaryPosition);
    }

    CGObject_C::Reenable(create);
    this->m_inTransit = Spell_C_IsSpellInTransit(this->m_dynamicObject->m_caster, this->m_dynamicObject->m_spellID, this->m_dynamicObject->m_castTime);
}

// PROVEN: 02734afe IDENTICAL/DIFFERENT
// OFFSET: 0x00596E60/0x1402057F0
void CGDynamicObject_C::PostReenable(const CClientObjCreate& create) {
    this->ObjectVisKitProc();
    this->m_passenger.PostInit(create.m_stationaryPosition);
    CGObject_C::PostReenable(create);
}

// OFFSET: 0x00596E90/0x140204AC0
const SpellVisualEffectNameRec* CGDynamicObject_C::GetVisualEffectNameRec(const SpellRec* spell, const SpellVisualKitRec*& kit) const {
    kit = NULL;

    if (!spell) {
        SysMsgPrintf(STATUS_WARNING, 2, "NOSPELLIDFOUND|%d", this->m_dynamicObject->m_spellID);
        return NULL;
    }

    int32_t quiet = spell->m_attributesExI & 0x200000;
    const SpellVisualRec* visual = g_spellVisualDB.GetRecord(this->m_dynamicObject->m_typeAndVisualID & 0xFFFFFFF);

    if (!visual) {
        if (!quiet) {
            SysMsgPrintf(STATUS_WARNING, 2, "SPELLVISUALIDNOTFOUND|%d|%d", this->m_dynamicObject->m_typeAndVisualID);
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

// PROVEN: 3eb000d2 IDENTICAL/DIFFERENT
// OFFSET: 0x00596FE0/0x140204C60
int32_t CGDynamicObject_C::GetModelFileName(const char** name) const {
    const SpellRec* spell = g_spellDB.GetRecord(this->m_dynamicObject->m_spellID);

    if (!spell) {
        SysMsgPrintf(STATUS_WARNING, 2, "NOSPELLIDFOUND|%d", this->m_dynamicObject->m_spellID);
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

// OFFSET: 0x00597060/0x140204D30
void CGDynamicObject_C::HandleAnimEvent(uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time) {
    SpellEffectCameraShakesRec* shakes = g_spellEffectCameraShakesDB.GetRecord(data);
    SE3SoundKitProperties properties;
    properties.ResetToDefaults();

    if (eventId == 0x444E5324) {
        SE3::PlaySoundKit(data, position, &properties, NULL, __FILE__, __LINE__, "CGDynamicObject_C::HandleAnimEvent() $SND", 0, NULL);
        return;
    }

    if (eventId == 0x4B485324 && shakes) {
        for (uint32_t i = 0; i < 3; i++) {
            StaticSingleton<CCameraManager>::Instance()->AddShakeById(shakes->m_cameraShake[i], *position);
        }
    }
}

// OFFSET: 0x00597160/0x140204E90
void CGDynamicObject_C::UpdateDisplay(uint32_t time) {
    if (!this->m_inTransit || Spell_C_IsSpellInTransit(this->m_dynamicObject->m_caster, this->m_dynamicObject->m_spellID, this->m_dynamicObject->m_castTime)) {
        return;
    }

    this->m_inTransit = 0;

    if (this->GetObjectModel()) {
        this->GetObjectModel()->SetEmittersEnabled(1);
        this->GetObjectModel()->SetRibbonsEnabled(1);
        this->GetObjectModel()->SetBoneSequence(0xFFFFFFFF, 127, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
        this->ObjectVisKitProc();
    }
}

// OFFSET: 0x005971F0/0x140204F60
C44Matrix CGDynamicObject_C::GetMatrix() const {
    C44Matrix matrix;
    C3Vector position = this->GetRawPosition();
    matrix.d0 = position.x;
    matrix.d1 = position.y;
    matrix.d2 = position.z;
    matrix.RotateAroundZ(this->GetRawFacing());

    if (this->GetTransportGUID()) {
        CGObject_C* transport = ClntObjMgrObjectPtr(WOWGUID::Make(this->GetTransportGUID()), TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (transport) {
            matrix *= transport->GetMatrix();
        }
    }

    return matrix;
}

// OFFSET: 0x005972E0/0x140205050
void CGDynamicObject_C::ModelLoaded(CM2Model* model) {
    CGObject_C::ModelLoaded(model);
    this->m_hasStandLoop = this->GetObjectModel()->HasSequence(158);
    const SpellRec* spell = g_spellDB.GetRecord(this->m_dynamicObject->m_spellID);
    const SpellVisualKitRec* kit = NULL;
    const SpellVisualEffectNameRec* effect = this->GetVisualEffectNameRec(spell, kit);

    if (spell->m_id && (spell->m_attributesExE & 0x40000000)) {
        model->m_fFlags |= 0x4;
    }

    uint32_t type = this->m_dynamicObject->m_typeAndVisualID & 0xF0000000;

    if (type != 0x20000000 && type != 0x10000000 && type != 0x30000000) {
        CAaSphere sphere = this->GetObjectModel()->GetBoundingSphere();

        if (sphere.r > 0.001f) {
            this->m_scale = this->m_dynamicObject->m_radius / sphere.r;
        } else if (effect && effect->m_areaEffectSize > 0.0f) {
            this->m_scale = this->m_dynamicObject->m_radius / effect->m_areaEffectSize;
        }
    }

    if (effect && effect->m_scale > 0.0f) {
        this->m_scale = this->m_scale * effect->m_scale;
    }

    this->CGObject_C::m_scale = this->m_scale;

    if (effect) {
        this->DoFade(1.0f, 0);
    }

    if (this->GetObjectModel()->HasSequence(127)) {
        this->GetObjectModel()->SetBoneSequence(0xFFFFFFFF, 127, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    }
}

// PROVEN: 2a66e016 IDENTICAL/NONE
// OFFSET: 0x00597490
static void AnimEventCallback(CM2Model* model, uint32_t boneId, uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time, uint64_t param) {
    CGDynamicObject_C* object = static_cast<CGDynamicObject_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_DYNAMICOBJECT, __FILE__, __LINE__));

    if (object) {
        object->HandleAnimEvent(eventId, data, position, time);
    }
}

// OFFSET: 0x005974D0/0x140206110
void CGDynamicObject_C::PostInit(uint32_t time, const CClientObjCreate& create, bool reenable) {
    this->m_passenger.PostInit(create.m_stationaryPosition);

    if (!this->GetTransportGUID()) {
        this->m_passenger.m_facing = create.m_stationaryFacing;
    }

    CGObject_C::PostInit(time, create, reenable);
    this->m_inTransit = Spell_C_IsSpellInTransit(this->m_dynamicObject->m_caster, this->m_dynamicObject->m_spellID, this->m_dynamicObject->m_castTime);

    if (this->GetObjectModel()) {
        this->GetObjectModel()->SetEventCallback(reinterpret_cast<void*>(&AnimEventCallback), this->m_obj->m_guid.m_guid);
        this->GetObjectModel()->SetSequenceCallback(reinterpret_cast<void*>(&AnimFinishedCallback), this->m_obj->m_guid.m_guid);

        if (this->m_inTransit) {
            this->GetObjectModel()->SetEmittersEnabled(0);
            this->GetObjectModel()->SetRibbonsEnabled(0);
        }
    }

    this->ObjectVisKitProc();

    if ((this->m_dynamicObject->m_typeAndVisualID & 0xF0000000) == 0x20000000 && this->m_dynamicObject->m_caster == ClntObjMgrGetActivePlayer()) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && this->m_obj->m_guid.m_guid == player->GetFarSightGuid()) {
            player->SetFarSightFocus(this);
        }
    }

    this->Animate(0.0f);
}

// OFFSET: 0x00597660/0x140205480
CGDynamicObject_C::CGDynamicObject_C(uint32_t time, const CClientObjCreate& create) : CGObject_C(time, create), m_passenger(create.m_hasGOTransport ? create.m_goTransport.m_guid : WOWGUID::Make(0), create.m_hasGOTransport ? create.m_goTransport.m_position : create.m_stationaryPosition, this->m_obj->m_guid), m_scale(1.0f), m_sound(0) {
    this->m_hasStandLoop = 0;
    this->m_inTransit = 0;
    this->m_blizzardVisual[0] = NULL;
    this->m_blizzardVisual[1] = NULL;
    this->m_passenger.m_facing = create.m_hasGOTransport ? create.m_goTransport.m_facing : create.m_stationaryFacing;
}

// OFFSET: 0x005977E0/0x140206DB0
C3Vector CGDynamicObject_C::GetPosition() const {
    return this->m_passenger.GetPosition(this->m_passenger.m_position);
}

// OFFSET: 0x00597800/0x140205600
float CGDynamicObject_C::GetFacing() const {
    return this->m_passenger.GetFacing(this->m_passenger.m_facing);
}

// PROVEN: 67147bd5 IDENTICAL/IDENTICAL
// OFFSET: 0x00597820/0x140205620
float CGDynamicObject_C::GetRawFacing() const {
    return this->m_passenger.m_facing;
}

// PROVEN: 5ebc7960 IDENTICAL/IDENTICAL
// OFFSET: 0x00597830/0x140205630
float CGDynamicObject_C::GetScale() const {
    return this->m_scale;
}

// OFFSET: 0x00597840/0x140206E50
CGDynamicObject_C::~CGDynamicObject_C() {
    this->ClearVisuals();

    for (uint32_t i = 0; i < 2; i++) {
        if (this->m_blizzardVisual[i]) {
            SpellVisualsBlizzardSetDeletedCallback(this->m_blizzardVisual[i], NULL, NULL);
        }
    }
}

// PROVEN: f63808a4 IDENTICAL/DIFFERENT
// OFFSET: 0x00597930/0x140209980
void CGDynamicObject_C::SetStorage(uint32_t* storage, uint32_t* saved) {
    this->CGObject_C::SetStorage(storage, saved);
    this->m_dynamicObject = reinterpret_cast<CGDynamicObjectData*>(storage + OBJECT_END);
    this->m_dynamicObjectMirror = saved + CGObject::TotalFieldsSaved();
}

// PROVEN: dfbf9e0d IDENTICAL/IDENTICAL
// OFFSET: 0x00598B40/0x140206DE0
C3Vector CGDynamicObject_C::GetRawPosition() const {
    return this->m_passenger.m_position;
}

// PROVEN: 6f187274 IDENTICAL/IDENTICAL
// OFFSET: 0x00598B70/0x1402142E0
uint64_t CGDynamicObject_C::GetTransportGUID() const {
    return this->m_passenger.m_transportGuid.m_guid;
}

// PROVEN: 549672d7 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGDynamicObject_C::Initialize() {
}
