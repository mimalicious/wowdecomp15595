#include "Object_C.h"
#include "WoW/Source/Ui/CGTabardCreationFrame.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Quaternion.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Object/ObjectClient/ObjectEffect.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/WorldClient/World.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "WoW/Source/Object/ObjectClient/Corpse_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "Engine/Source/Services/SysMessage.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Game/GameClient/PlayerName.h"
#include "WoW/Source/Game/GameClient/CDrawBowString.h"
#include "WoW/Source/Ui/MinimapFrame.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include <cmath>
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "WoW/Source/DB/Rec/SpellVisualKitRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/GuildColorBackgroundRec.h"
#include "WoW/Source/DB/Rec/GuildColorBorderRec.h"
#include "WoW/Source/DB/Rec/GuildColorEmblemRec.h"
#include "Engine/Source/Os/TimeManager.h"
#include "WoW/Source/Magic/MagicClient/SpellVisuals.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Object/MovementShared.h"
#include "WoW/Source/DB/Rec/SpellVisualRec.h"
#include "WoW/Source/DB/Rec/SpellVisualEffectNameRec.h"
#include "WoW/Source/DB/Rec/SpellVisualKitModelAttachRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"

// STATIC: 0x00CA6568
CGObject_C::InteractIcon CGObject_C::s_interactIcons[12] = {
    { NULL, 1.0f, NULL, 0 },
    { NULL, 1.0f, "Interface\\Buttons\\TalkToMe.mdx", 0 },
    { NULL, 1.0f, "Interface\\Buttons\\TalkToMeQuestion_LTBlue.mdx", 0 },
    { NULL, 1.0f, "Interface\\Buttons\\TalkToMeBlue.mdx", 0 },
    { NULL, 1.0f, "Interface\\Buttons\\TalkToMe.mdx", 0 },
    { NULL, 1.0f, "Interface\\Buttons\\TalkToMeQuestion_Grey.mdx", 0 },
    { NULL, 1.0f, "Interface\\Buttons\\TalkToMeGrey.mdx", 0 },
    { NULL, 1.0f, "Interface\\Buttons\\TalkToMeGreen.mdx", 0 },
    { NULL, 1.0f, NULL, 0 },
    { NULL, 1.0f, "Interface\\Buttons\\TalkToMeQuestionMark.mdx", 0 },
    { NULL, 1.0f, "Interface\\Buttons\\TalkToMeQuestion_LTBlue.mdx", 0 },
    { NULL, 1.0f, "Interface\\Buttons\\TalkToMeBlue.mdx", 0 },
};

// STATIC: 0x00BB3034
const uint32_t CGObject_C::s_questStatusIcons[11] = { 0, 6, 1, 2, 3, 5, 10, 11, 4, 9, 9 };

// STATIC: 0x00CA6628
static TSExplicitList<CGObject_C, offsetof(CGObject_C, m_fadeoutLink)> s_fadeoutList;

// STATIC: 0x00DD6320
CTexture* CGObject_C::s_selectionTexture;

// STATIC: 0x00DD6328
TSObjectAllocPool<CGObject_C::FadeoutInfo>* CGObject_C::s_fadeoutHeap;

// PROVEN: aa91eca0 IDENTICAL/NONE
// OFFSET: 0x005DBCC0
static void LoadInteractIcons(int32_t load) {
    for (uint32_t i = 0; i < sizeof(CGObject_C::s_interactIcons) / sizeof(CGObject_C::s_interactIcons[0]); i++) {
        CGObject_C::InteractIcon& icon = CGObject_C::s_interactIcons[i];

        if (icon.m_model) {
            icon.m_model->Release();
            icon.m_model = NULL;
        }

        if (!load) {
            icon.m_model = NULL;
        } else if (icon.m_fileName && *icon.m_fileName) {
            icon.m_model = World::GetScene()->CreateModel(icon.m_fileName, 0, 3);
        }
    }
}

// PROVEN: 1b2222af IDENTICAL/NONE
// OFFSET: 0x005DBD20/0x140257E90
void CGObject_C::ModelLoadedCallback(CM2Model* model, void* arg) {
    CGObject_C* object = static_cast<CGObject_C*>(arg);

    if (object) {
        object->ModelLoaded(model);
    }
}

// PROVEN: ce46b500 IDENTICAL/NONE
// OFFSET: 0x005DBD40
void CGObject_C::SetTypeID(OBJECT_TYPE_ID typeId) {
    this->m_typeId = typeId;

    switch (typeId) {
        case ID_OBJECT:
            this->m_obj->m_type = TYPEMASK_OBJECT;
            break;

        case ID_ITEM:
            this->m_obj->m_type = TYPEMASK_OBJECT | TYPEMASK_ITEM;
            break;

        case ID_CONTAINER:
            this->m_obj->m_type = TYPEMASK_OBJECT | TYPEMASK_ITEM | TYPEMASK_CONTAINER;
            break;

        case ID_UNIT:
            this->m_obj->m_type = TYPEMASK_OBJECT | TYPEMASK_UNIT;
            break;

        case ID_PLAYER:
            this->m_obj->m_type = TYPEMASK_OBJECT | TYPEMASK_UNIT | TYPEMASK_PLAYER;
            break;

        case ID_GAMEOBJECT:
            this->m_obj->m_type = TYPEMASK_OBJECT | TYPEMASK_GAMEOBJECT;
            break;

        case ID_DYNAMICOBJECT:
            this->m_obj->m_type = TYPEMASK_OBJECT | TYPEMASK_DYNAMICOBJECT;
            break;

        case ID_CORPSE:
            this->m_obj->m_type = TYPEMASK_OBJECT | TYPEMASK_CORPSE;
            break;

        case ID_AREATRIGGER:
            this->m_obj->m_type = TYPEMASK_OBJECT | TYPEMASK_AREATRIGGER;
            break;

        default:
            break;
    }
}

// PROVEN: c5445680 IDENTICAL/NONE
// OFFSET: 0x005DBE10
void CGObject_C::ReleaseModel() {
    if (!this->m_worldModel) {
        return;
    }

    this->m_worldModel->SetLoadedCallback(NULL, NULL);
    this->m_worldModel->SetSequenceCallback(NULL, 0);
    this->m_worldModel->SetEventCallback(NULL, 0);
    this->m_worldModel->Release();
    this->m_worldModel = NULL;
}

// PROVEN: ad14dc82 IDENTICAL/NONE
// OFFSET: 0x005DBE60/0x140257F80
void CGObject_C::AuraDecayHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    if (!interrupted) {
        reinterpret_cast<CEffect*>(static_cast<uintptr_t>(param))->MoveToDestroyList();
    }
}

// PROVEN: e3342db4 IDENTICAL/NONE
// OFFSET: 0x005DBE80/0x140257FA0
void CGObject_C::OnSpellEffectClear(CEffect* effect) {
    uint32_t sequence = 159;

    if (effect->m_attachRec && effect->m_attachRec->unk_0034 != -1) {
        sequence = effect->m_attachRec->unk_0034;
    }

    if (effect->m_model && effect->m_model->IsLoaded() && effect->m_model->HasSequence(sequence)) {
        effect->m_flags &= ~0x1000;
        effect->m_model->SetBoneSequence(0xFFFFFFFF, sequence, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
        effect->m_model->SetSequenceCallback(reinterpret_cast<void*>(&CGObject_C::AuraDecayHandler), reinterpret_cast<uintptr_t>(effect));
        return;
    }

    if (effect->m_flags & 0x40) {
        SE3::StopSound(effect->m_soundHandle, 300, __FILE__, __LINE__, "CGObject_C::OnSpellEffectClear");
    }

    effect->MoveToDestroyList();
}

// PROVEN: 18c76c1b IDENTICAL/NONE
// OFFSET: 0x005DBF30/0x140258090
void CGObject_C::ShouldRender(uint32_t flags, int32_t& render, int32_t& a3) {
    if (!(flags & 0x1)) {
        render = 1;
    }
}

// PROVEN: 3e230145 IDENTICAL/NONE
// OFFSET: 0x005DBF50/0x1402580A0
void CGObject_C::SetBoundsDirty() {
    this->m_modelFlags |= 0x200000;
}

// OFFSET: 0x005DBF60/0x140207360
int32_t CGObject_C::Animate(float elapsed) {
    CM2Model* model = this->GetObjectModel();

    if (model) {
        float scale = this->GetTrueScale();
        float facing = this->GetDisplayFacing();
        model->SetWorldTransform(this->GetPosition(), facing, scale);
    }

    return 1;
}

// PROVEN: 9aaca960 IDENTICAL/NONE
// OFFSET: 0x005DBFC0/0x140258130
int32_t CGObject_C::IsObjectModelLoaded() {
    if (this->GetObjectModel() && this->GetObjectModel()->IsLoaded()) {
        return 1;
    }

    return 0;
}

// OFFSET: 0x005DC000/0x140258180
int32_t CGObject_C::IsObjectLocked() {
    return static_cast<uint16_t>(this->m_modelFlags) > 0;
}

// PROVEN: 69ad37f1 IDENTICAL/NONE
// OFFSET: 0x005DC010/0x140258190
void CGObject_C::SetObjectLocked(int32_t locked) {
    uint32_t flags = this->m_modelFlags;
    uint32_t count = static_cast<uint16_t>(flags);

    if (locked && count < 0xFFFF) {
        this->m_modelFlags = flags + 1;
    } else if (count > 0) {
        this->m_modelFlags = flags - 1;
    }
}

// PROVEN: fb3761ba IDENTICAL/NONE
// OFFSET: 0x005DC050/0x1402581C0
void CGObject_C::SetDisablePending(int32_t pending) {
    if (pending) {
        this->m_modelFlags |= 0x100000;
    } else {
        this->m_modelFlags &= ~0x100000;
    }
}

// PROVEN: e7f9e1c3 IDENTICAL/NONE
// OFFSET: 0x005DC080
int32_t CGObject_C::IsReadyToDraw() {
    if (this->GetObjectModel() && this->GetObjectModel()->IsDrawable()) {
        return 1;
    }

    return 0;
}

// PROVEN: 6d10ab64 IDENTICAL/NONE
// OFFSET: 0x005DC0C0/0x140258230
void CGObject_C::ObjectNameVisibilityChanged() {
    if (!this->m_interactIcon) {
        return;
    }

    if (PlayerNameShowing(this->m_nameDesc)) {
        this->m_interactIcon->SetBoneSequence(0xFFFFFFFF, 190, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    } else {
        this->m_interactIcon->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    }
}

// PROVEN: c8f06044 IDENTICAL/NONE
// OFFSET: 0x005DC120/0x1402582B0
int32_t CGObject_C::UpdateObjectNameString(int32_t a1, char* buffer, uint32_t size) {
    if (this->GetObjectName()) {
        SStrPrintf(buffer, size, "%s", this->GetObjectName());
    } else {
        *buffer = '\0';
    }

    return 1;
}

// PROVEN: c3316353 IDENTICAL/NONE
// OFFSET: 0x005DC170/0x140258320
int32_t CGObject_C::ShouldRenderObjectName(uint32_t flags) const {
    if (this->m_interactIcon) {
        return 0;
    }

    return (flags >> 11) & 0x1;
}

// PROVEN: f785cfca IDENTICAL/NONE
// OFFSET: 0x005DC190
int32_t CGObject_C::PlaySpellVisualKit_DelayLightningEffects(const SpellRec* spell, const SpellVisualKitRec* kit, SPELL_VISUAL_KIT_TYPE type, const C3Vector* position, WOWGUID guid, int32_t a6, int32_t a7, int32_t a8, int32_t a9, uint32_t a10, const SpellVisualRec* visual, uint32_t a12, int32_t a13) {
    return 0;
}

// PROVEN: 16cd810e IDENTICAL/NONE
// OFFSET: 0x005DC1A0/0x140258340
void CGObject_C::EffectDecayHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    CEffect* effect = reinterpret_cast<CEffect*>(static_cast<uintptr_t>(param));

    if (!interrupted && effect) {
        effect->MoveToDestroyList();
    }
}

// PROVEN: 663c472d IDENTICAL/NONE
// OFFSET: 0x005DC1C0
void CGObject_C::LoopEffectHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    CEffect* effect = reinterpret_cast<CEffect*>(static_cast<uintptr_t>(param));

    if (!interrupted && effect && effect->m_model) {
        effect->m_model->SetBoneSequence(0xFFFFFFFF, sequenceId, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    }
}

// OFFSET: 0x005DC220
void CGObject_C::FadeoutWorldObjectCallback(C44Matrix& matrix, uint64_t transportGuid) {
    CGObject_C* transport = ClntObjMgrObjectPtr(WOWGUID::Make(transportGuid), TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (transport) {
        matrix = transport->GetMatrix();
    } else {
        matrix = C44Matrix();
    }
}

// OFFSET: 0x005DC2D0
void CGObject_C::FadeoutWorldObject() {
    if (!this->m_worldObject) {
        return;
    }

    uint32_t time = FrameTime::GetCurTimeMs();
    FadeoutInfo* info = this->m_fadeoutInfo;
    float fade = (1.0f - (time - info->m_startTime) * 0.0005f) * info->m_alpha;
    float alpha;

    if (fade >= 0.0f) {
        if (fade <= 1.0f) {
            alpha = (3.0f - fade * 2.0f) * fade * fade;
        } else {
            alpha = 1.0f;
        }
    } else {
        alpha = 0.0f;
    }

    if (info->m_transportGuid) {
        C44Matrix matrix;
        CGObject_C* transport = ClntObjMgrObjectPtr(info->m_transportGuid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

        if (transport) {
            matrix = transport->GetMatrix().AffineInverse();
            World::FadeoutObject(this->m_worldObject, alpha, &matrix, &CGObject_C::FadeoutWorldObjectCallback, this->m_fadeoutInfo->m_transportGuid.m_guid);
            return;
        }
    }

    World::FadeoutObject(this->m_worldObject, alpha, NULL, NULL, 0);
}

// OFFSET: 0x005DC460/0x140258710
CM2Model* CGObject_C::SetModelStart(CM2Model* model) {
    if (model) {
        model->m_refCount++;
    }

    int32_t initialized = this->m_modelFlags & 0x40000;
    CM2Model* previous = this->m_worldModel;
    this->m_worldModel = model;

    if (initialized && model) {
        float scale = this->GetTrueScale();
        float facing = this->GetDisplayFacing();
        model->SetWorldTransform(this->GetPosition(), facing, scale);
    }

    return previous;
}

// PROVEN: 2a571a25 IDENTICAL/NONE
// OFFSET: 0x005DC4D0/0x1402587B0
void CGObject_C::SetModelFinish(CM2Model* model) {
    CEffect* effect = this->m_effectList;

    while (effect) {
        CEffect* next = effect->m_next;
        effect->DetachFromParent();

        if (this->m_worldModel->IsLoaded()) {
            effect->UpdateAttachment();
        }

        effect = next;
    }

    if (model) {
        model->SetLoadedCallback(NULL, NULL);

        if (model->m_attachParent) {
            model->DetachFromParent();
        }

        model->Release();
    }

    if (this->m_worldModel) {
        this->m_worldModel->SetLoadedCallback(&CGObject_C::ModelLoadedCallback, this);
    }

    if (this->m_worldObject) {
        CM2Model* worldModel = this->m_worldModel;

        if (worldModel == this->GetObjectModel()) {
            World::ObjectSetModel(this->m_worldObject, worldModel);
        }
    }
}

// PROVEN: 0c8f62a5 IDENTICAL/NONE
// OFFSET: 0x005DC580
void CGObject_C::SetModel(CM2Model* model) {
    if (model != this->m_worldModel) {
        this->SetModelFinish(this->SetModelStart(model));
    }
}

// OFFSET: 0x005DC5B0/0x1402588D0
void CGObject_C::AddWorldObject() {
    if (!this->m_worldModel) {
        const char* fileName;

        if (this->GetModelFileName(&fileName)) {
            CM2Model* model = World::GetScene()->CreateModel(fileName, 0, 3);

            if (model != this->m_worldModel) {
                this->SetModelFinish(this->SetModelStart(model));
            }

            model->Release();
        }

        if (!this->m_worldModel) {
            return;
        }
    }

    if (ClntObjMgrGetPlayerType()) {
        return;
    }

    if (this->m_worldObject) {
        SysMsgPrintf(STATUS_WARNING, 2, "OBJECTALREADYACTIVE|0x%016llX", this->m_obj->m_guid.m_guid);
        return;
    }

    uint32_t flags = 0;

    switch (this->m_typeId) {
        case ID_UNIT:
        case ID_PLAYER: {
            CGUnit_C* unit = static_cast<CGUnit_C*>(this);

            if (unit->IsLinkAll()) {
                flags = 8;
            }

            if (unit->HasNoShadowBlob()) {
                flags |= 0x2;
            }

            flags |= 0x60;
            break;
        }

        case ID_GAMEOBJECT:
            flags = 11;
            break;

        case ID_DYNAMICOBJECT:
        case ID_AREATRIGGER:
            flags = 10;
            break;

        case ID_CORPSE:
            if (static_cast<CGCorpse_C*>(this)->m_corpse->m_corpseFieldFlags & 0x1) {
                flags = 2;
            }

            break;

        default:
            break;
    }

    this->m_worldObject = World::ObjectCreate(this->GetObjectModel(), &CGWorldFrame::ObjectEnumProc, NULL, this->m_obj->m_guid.m_guid, 0, flags, __FILE__, __LINE__);

    if ((this->m_modelFlags & 0x40000) && !(this->m_modelFlags & 0x20000)) {
        this->UpdateWorldObject(0);
    }
}

// OFFSET: 0x005DC720/0x140258AA0
void CGObject_C::UpdateWorldObject(int32_t a2) {
    if (!this->m_worldObject) {
        return;
    }

    C44Matrix matrix;
    matrix.Translate(this->GetPosition());
    matrix.RotateAroundZ(this->GetFacing());
    matrix.Scale(this->GetTrueScale());

    C3Vector center(0.0f, 0.0f, 0.0f);
    CAaBox box(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f));
    CAaSphere sphere;
    sphere.c = C3Vector(0.0f, 0.0f, 0.0f);
    sphere.r = 0.0f;
    C3Vector position(0.0f, 0.0f, 0.0f);
    CM2Model* model = this->m_worldModel;

    if (model && model->IsLoaded()) {
        model->GetCombinedBounds(box, sphere);
        model = this->m_worldModel;

        if (!(model->m_shared->m_flags & 0x1)) {
            model->WaitForLoad(NULL);
        }

        const CAaBox& bounds = model->m_shared->m_data->m_collisionBounds.m_extent;
        center.x = (bounds.t.x + bounds.b.x) * 0.5f;
        center.y = (bounds.t.y + bounds.b.y) * 0.5f;
        center.z = (bounds.t.z + bounds.b.z) * 0.5f;
        position = center;
    }

    World::ObjectUpdate(this->m_worldObject, matrix, box, sphere, position, a2);
}

// PROVEN: 89ba69ca IDENTICAL/NONE
// OFFSET: 0x005DC940/0x140258D30
void CGObject_C::UpdateEffectAttachments() {
    if (this->m_worldModel && this->m_worldModel->IsLoaded()) {
        CEffect* effect = this->m_effectList;

        while (effect) {
            CEffect* next = effect->m_next;

            if (effect->m_flags & 0x800) {
                effect->UpdateAttachment();
            }

            effect = next;
        }
    }
}

// PROVEN: b907529f IDENTICAL/NONE
// OFFSET: 0x005DC980
void CGObject_C::ClearSpellEffect(uint32_t spellId, int32_t all, uint64_t casterGuid) {
    if (!spellId) {
        return;
    }

    CEffect* effect = this->m_effectList;

    while (effect) {
        CEffect* next = effect->m_next;

        if (effect->m_spellId == spellId && (all || !(effect->m_flags & 0x20000)) && (!casterGuid || !effect->m_targetGuid || casterGuid == effect->m_targetGuid.m_guid) && !(effect->m_flags & 0x20)) {
            this->OnSpellEffectClear(effect);
        }

        effect = next;
    }
}

// PROVEN: 39f03cfa IDENTICAL/NONE
// OFFSET: 0x005DCA00
void CGObject_C::RetargetEffects(CEffect* effect, uint32_t spellId, uint64_t oldTarget, uint64_t newTarget) {
    if (!spellId) {
        return;
    }

    for (; effect; effect = effect->m_next) {
        if (effect->m_spellId == spellId && effect->m_targetGuid.m_guid == oldTarget) {
            effect->m_targetGuid.m_guid = newTarget;
        }
    }
}

// OFFSET: 0x005DCA50/0x140258E70
const SpellVisualRec* CGObject_C::GetAppropriateSpellVisual(const SpellRec* spell, SpellVisualRec& visual, int32_t a3, int32_t a4) const {
    if (this->m_obj->m_type & TYPEMASK_UNIT) {
        return GetSpellVisual(spell, static_cast<const CGUnit_C*>(this));
    }

    return GetSpellVisual(spell, NULL);
}

// PROVEN: 01d65df4 IDENTICAL/NONE
// OFFSET: 0x005DCA90/0x140258EA0
void CGObject_C::ClearEffectKitByID(uint32_t spellId, int32_t kitId, uint32_t param, int32_t matchSpell, int32_t matchParam) {
    if (!kitId) {
        return;
    }

    CEffect* effect = this->m_effectList;

    while (effect) {
        const SpellVisualKitRec* kit = effect->m_kitRec;
        CEffect* next = effect->m_next;

        if (kit && kit->m_id == kitId && (!matchSpell || effect->m_spellId == spellId) && (!matchParam || effect->m_param == param)) {
            this->OnSpellEffectClear(effect);
        }

        effect = next;
    }
}

// PROVEN: 371a7503 IDENTICAL/NONE
// OFFSET: 0x005DCB00
void CGObject_C::SetBlock(uint32_t index, uint32_t value) {
    reinterpret_cast<uint32_t*>(this->m_obj)[index] = value;
}

// OFFSET: 0x005DCB20/0x140258F70
void CGObject_C::HideHighlightType(int32_t type, int32_t force) {
    this->m_modelFlags &= ~(1 << (type + 24));

    uint64_t guid = this->m_obj->m_guid.m_guid;

    if (World::GetTrackedMapObjGUID() == guid) {
        World::ClearTrackedMapObjGUID();
    }

    CM2Model* model;

    if (this->IsHighlightSuppressed()) {
        if (!force) {
            return;
        }

        model = this->GetObjectModel();

        if (!model) {
            return;
        }
    } else {
        model = this->GetObjectModel();

        if (!model || (this->m_modelFlags & 0x7000000)) {
            return;
        }
    }

    model->m_emissiveTintR = 0.0f;
    model->m_emissiveTintG = 0.0f;
    model->unk_01D0 = 1.0f;
    model->m_emissiveTintB = 0.0f;
}

// OFFSET: 0x005DCBF0/0x1402590A0
void CGObject_C::ShowHighlightType(int32_t type, int32_t force) {
    this->m_modelFlags |= 1 << (type + 24);

    if (this->IsHighlightSuppressed()) {
        if (!force) {
            return;
        }

        CM2Model* model = this->GetObjectModel();

        if (model) {
            model->m_emissiveTintR = 0.0f;
            model->m_emissiveTintG = 0.0f;
            model->m_emissiveTintB = 0.0f;
            model->unk_01D0 = 1.0f;
        }

        return;
    }

    World::SetTrackedMapObjGUID(this->m_obj->m_guid.m_guid);
    CM2Model* model = this->GetObjectModel();

    if (!model) {
        return;
    }

    const DayNight::LightInfo* info = DayNight::GetLightInfo();
    model->m_emissiveTintR = info->m_ambientColor.r * (1.0f / 255.0f);
    model->m_emissiveTintG = info->m_ambientColor.g * (1.0f / 255.0f);
    model->m_emissiveTintB = info->m_ambientColor.b * (1.0f / 255.0f);

    if ((this->m_modelFlags & 0x7000000) != 0x4000000) {
        model->unk_01D0 = 1.0f;
    }
}

// OFFSET: 0x005DCD30
static int32_t GuildDataTypeUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    int16_t oldType = *static_cast<const int16_t*>(oldValue);
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object && (oldType == 1 || object->m_obj->m_dataType == 1)) {
        object->HandleGuildIDUpdate();
    }

    return 1;
}

// OFFSET: 0x005DCD80
static int32_t GuildDataUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object && object->m_obj->m_dataType == 1) {
        object->HandleGuildIDUpdate();
    }

    return 1;
}

// OFFSET: 0x005DCDD0/0x140259290
void CGObject_C::ApplyAlpha(uint32_t time) {
    CM2Model* model = this->GetObjectModel();

    if (!model) {
        return;
    }

    int32_t duration = this->m_fadeDuration;

    if (duration) {
        if (static_cast<int32_t>(time - this->m_fadeStartTime - duration) < 0) {
            this->m_alpha = this->m_fadeStartAlpha + static_cast<int32_t>((time - this->m_fadeStartTime) * (this->m_fadeEndAlpha - this->m_fadeStartAlpha)) / duration;
        } else {
            this->m_fadeDuration = 0;
            this->m_alpha = this->m_fadeEndAlpha;
        }
    }

    model->m_fadeAlpha = static_cast<float>(this->m_alphaScale) * static_cast<float>(this->m_alpha) * (1.0f / 65025.0f);
}

// OFFSET: 0x005DCE80/0x140259340
void CGObject_C::PreAnimate(CGWorldFrame* frame) {
    if (CGGameUI::m_lockedTarget == this->m_obj->m_guid && ClntObjMgrGetActivePlayer() != this->m_obj->m_guid && CGObject_C::s_cvObjectSelectionCircle->m_intValue) {
        frame->m_trackedEffectGuidA = this->m_obj->m_guid;
    }

    if (CGPetInfo::GetTarget() == this->m_obj->m_guid && ClntObjMgrGetActivePlayer() != this->m_obj->m_guid) {
        frame->m_trackedEffectGuidB = this->m_obj->m_guid;
    }

    uint32_t time = frame->GetSceneTime();
    this->ApplyAlpha(time);

    if (this->m_scaleChangeTime && time > this->m_scaleChangeTime) {
        int32_t elapsed = time - this->m_scaleChangeTime;

        if (elapsed >= 2000) {
            this->m_scale = this->m_obj->m_scale;
            this->m_scaleChangeTime = 0;
            this->ScaleChangeFinished();
        } else {
            float start = this->m_scaleChangeStart;
            this->m_scale = (this->GetScale() - start) * (-cos(elapsed * 0.0005f * 3.1415927f) * 0.5f + 0.5f) + start;
            this->ScaleChangeUpdate();
        }
    }
}

// PROVEN: 7136ba38 IDENTICAL/NONE
// OFFSET: 0x005DCFC0
void CGObject_C::PostReenable(const CClientObjCreate& create) {
    this->RefreshInteractIcon();
    this->m_modelFlags &= ~0x20000;

    if (!this->m_worldObject && this->GetObjectModel()) {
        this->AddWorldObject();
    }
}

// OFFSET: 0x005DD000/0x140259500
void CGObject_C::DoFade(float alpha, uint32_t time) {
    uint8_t target = CMath::fuint8_n(alpha);

    if (target == this->m_alpha) {
        this->m_fadeEndAlpha = target;
        this->m_fadeDuration = 0;
        this->m_alpha = target;
        return;
    }

    this->m_fadeStartTime = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    this->m_fadeStartAlpha = this->m_alpha;
    this->m_fadeDuration = time;
    this->m_fadeEndAlpha = target;

    if (!time) {
        this->m_alpha = target;
    }
}

// PROVEN: 05a9c7de IDENTICAL/NONE
// OFFSET: 0x005DD0B0/0x1402595A0
static int32_t UpdateAllWorldObjectsCallback(WOWGUID guid, void* param) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        object->UpdateWorldObject(0);
    }

    return 1;
}

// PROVEN: ceef41e5 IDENTICAL/NONE
// OFFSET: 0x005DD0F0/0x1402595E0
int32_t CGObject_C::UpdateAllWorldObjects() {
    ClntObjMgrDestroyLazyCleanupObjects();
    return ClntObjMgrEnumVisibleObjects(&UpdateAllWorldObjectsCallback, NULL);
}

// PROVEN: e51a96b6 IDENTICAL/NONE
// OFFSET: 0x005DD110
static int32_t UpdateAllObjectEffectManagersCallback(WOWGUID guid, void* param) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object && object->m_effectManager) {
        object->m_effectManager->UpdateAllPlayingEffects();
    }

    return 1;
}

// PROVEN: 87f4c961 IDENTICAL/NONE
// OFFSET: 0x005DD150
int32_t CGObject_C::UpdateAllObjectEffectManagers() {
    return ClntObjMgrEnumVisibleObjects(&UpdateAllObjectEffectManagersCallback, NULL);
}

// OFFSET: 0x005DD160
void __stdcall CGObject_C::BuildSelectionRotMatrix(C44Matrix& matrix, const C3Vector& position) {
    matrix = C44Matrix();
    C3Vector camera(0.0f, 0.0f, 0.0f);
    CGWorldFrame::GetCameraPosition(&camera);
    matrix.RotateAroundZ(-CalculateFacingTo(&camera, &position));
}

// OFFSET: 0x005DD200/0x1402596F0
void CGObject_C::HandleScaleUpdate(float scale) {
    this->m_scaleChangeTime = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    this->m_scaleChangeStart = scale;
    this->m_scale = scale;

    WOWGUID guid = this->m_obj->m_guid;

    if (guid == CGWorldFrame::GetActiveCamera()->m_targetGUID) {
        CGGameUI::ResetCamera(this->m_obj->m_guid);
    }
}

// OFFSET: 0x005DD260/0x140259760
void CGObject_C::HandleGuildIDUpdate() {
    this->LoadGuildTextures(this->GetGuildGUID().m_guid);
}

// PROVEN: 656610be IDENTICAL/NONE
// OFFSET: 0x005DD2A0/0x140259790
void CGObject_C::GuildTextureCallback(const uint64_t& guildGUID, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        object->LoadGuildTextures(guildGUID);
    }
}

// PROVEN: 98849f62 IDENTICAL/NONE
// OFFSET: 0x005DD2F0
void CGObject_C::ModelChanged() {
    int32_t loaded = this->GetObjectModel()->IsLoadedRecursive();
    CAaBox box = this->GetObjectModel()->GetBoundingBox();

    if (box.t.x > box.b.x && box.t.y > box.b.y && box.t.z > box.b.z) {
        this->m_height = box.t.z - box.b.z;
    } else {
        this->m_height = 0.0f;
    }

    if (!(this->m_modelFlags & 0x10000)) {
        this->UpdateWorldObject(0);
    }

    if (loaded) {
        this->m_modelFlags &= ~0x200000;
    } else {
        this->m_modelFlags |= 0x200000;
    }
}

// OFFSET: 0x005DD3A0
void CGObject_C::ModelLoaded(CM2Model* model) {
    if (model != this->GetObjectModel()) {
        return;
    }

    this->ModelChanged();

    CEffect* effect = this->m_effectList;

    while (effect) {
        CEffect* next = effect->m_next;
        effect->UpdateAttachment();
        effect = next;
    }

    this->LoadGuildTextures(this->GetGuildGUID().m_guid);
}

// PROVEN: 78491650 IDENTICAL/NONE
// OFFSET: 0x005DD420/0x140259970
void CGObject_C::ConnectToLightningThisFrame(uint32_t a1, int32_t a2) {
    for (CEffect* effect = this->m_effectList; effect; effect = effect->m_next) {
        if (effect->m_spellId == a1 && (effect->m_flags & 0x40000) && effect->unk_00A0 == a2) {
            effect->ConnectToLightningThisFrame();
        }
    }
}

// PROVEN: 166bb603 IDENTICAL/NONE
// OFFSET: 0x005DD470/0x1402599D0
void CGObject_C::SetFrameOfReference(const C44Matrix* matrix) {
    CM2Model* model = this->m_worldModel;

    if (!model || !(model->m_fFlags & 0x1)) {
        return;
    }

    if (matrix) {
        model->m_frameOfReferenceMatrix = *matrix;
        model->ChangeFrameOfReference(&model->m_frameOfReferenceMatrix);
    } else {
        model->ChangeFrameOfReference(NULL);
    }
}

// PROVEN: 7a6cfbd2 IDENTICAL/NONE
// OFFSET: 0x005DD4C0/0x140259A80
void CGObject_C::RemoveInteractIcon() {
    if (!this->m_interactIcon) {
        return;
    }

    if (this->m_interactIcon->m_attachParent) {
        this->m_interactIcon->DetachFromParent();
    }

    this->m_interactIcon->Release();
    this->m_interactIcon = NULL;
}

// PROVEN: 921b9e05 IDENTICAL/NONE
// OFFSET: 0x005DD4F0
void CGObject_C::UpdateInteractIcon(uint32_t status) {
    uint32_t previous = this->m_questStatus;

    if (status == previous && (!CGObject_C::s_questStatusIcons[status] || this->m_interactIcon)) {
        return;
    }

    uint32_t icon = CGObject_C::s_questStatusIcons[status];
    int32_t changed = icon != CGObject_C::s_questStatusIcons[previous] || (icon && !this->m_interactIcon);
    this->m_questStatus = status;

    if (changed) {
        this->UpdateInteractIcon();
    }
}

// PROVEN: 62ef3f0f IDENTICAL/NONE
// OFFSET: 0x005DD550/0x140259B40
void CGObject_C::UpdateInteractIconAttach() {
    if (!this->m_interactIcon || !this->m_worldModel || !this->m_worldModel->IsLoaded()) {
        return;
    }

    if (this->m_interactIcon->m_attachParent) {
        this->m_interactIcon->DetachFromParent();
    }

    if (this->m_worldModel->HasAttachment(18)) {
        this->m_interactIcon->AttachToParent(this->m_worldModel, 18, NULL, 0);
        this->UpdateInteractIconScale();
        this->ObjectNameVisibilityChanged();
    }
}

// OFFSET: 0x005DD5D0/0x140259BE0
void CGObject_C::UpdateInteractIconScale() {
    if (!this->m_interactIcon || !this->m_interactIcon->m_attachParent || !this->m_worldModel || !this->m_worldModel->IsLoaded()) {
        return;
    }

    C44Matrix attachment = this->m_worldModel->GetAttachmentWorldTransform(this->m_interactIcon->m_attachmentId);
    float lengthSq = attachment.a1 * attachment.a1 + attachment.a2 * attachment.a2 + attachment.a0 * attachment.a0;

    if (!(0.0f < lengthSq)) {
        return;
    }

    float scale = PlayerNameComputeScale(this) / sqrtf(lengthSq);

    if (fequal(scale, 1.0f) || !(0.0f < scale)) {
        return;
    }

    C44Matrix matrix;
    matrix = this->m_interactIcon->m_worldTransform;
    float current = sqrtf(matrix.a1 * matrix.a1 + matrix.a2 * matrix.a2 + matrix.a0 * matrix.a0);

    if (0.01f < current && fabsf(scale - current) > 0.01f) {
        matrix.Scale(scale / current);
        this->m_interactIcon->SetWorldTransform(matrix);
    }
}

// OFFSET: 0x005DD780
int32_t CGObject_C::GetQuestGiverCursor(int32_t highlighted) {
    switch (this->m_questStatus) {
        case 2:
        case 3:
        case 4:
        case 5:
        case 7:
        case 8:
            return highlighted ? 49 : 23;

        case 6:
            return highlighted ? 50 : 24;

        case 9:
        case 10:
            return highlighted ? 51 : 25;

        default:
            return 0;
    }
}

// OFFSET: 0x005DD800
uint32_t CGObject_C::GetQuestInteractIcon() {
    if (this->m_interactIconType == 2) {
        return 7;
    }

    return 0;
}

// OFFSET: 0x005DD820/0x14025A000
uint32_t CGObject_C::GetNPCInteractIcon() {
    if (!(this->m_obj->m_type & TYPEMASK_UNIT) || !static_cast<CGUnit_C*>(this)->IsTracked()) {
        return 0;
    }

    uint32_t icon = 0;

    while (!(static_cast<CGUnit_C*>(this)->m_unit->m_unitNpcFlags & CGObject_C::s_interactIcons[icon].m_npcFlagMask)) {
        if (++icon >= 12) {
            return 0;
        }
    }

    return icon;
}

// PROVEN: 40d04b8b IDENTICAL/NONE
// OFFSET: 0x005DD870/0x14025A050
C3Vector CGObject_C::GetModelPosition() {
    CM2Model* model = this->GetObjectModel();

    if (model && (model->m_fFlags & 0x4000)) {
        return model->GetWorldPosition();
    }

    return this->GetPosition();
}

// PROVEN: bc846953 IDENTICAL/NONE
// OFFSET: 0x005DD8C0
C44Matrix CGObject_C::GetModelMatrix() {
    CM2Model* model = this->GetObjectModel();

    if (model && (model->m_fFlags & 0x4000)) {
        model->Animate();
        return model->m_matrixF4 * model->m_scene->m_viewInv;
    }

    return this->GetMatrix();
}

// OFFSET: 0x005DD930/0x14025A140
void CGObject_C::PlayEffectName(int32_t attachType, uint32_t expireTime, uint32_t spellId, const SpellVisualKitRec* kit, const SpellVisualEffectNameRec* effectName, uint32_t& flags, EFFECT_CALLBACK callback, const C3Vector* position, uint64_t target, uint32_t param, const SpellVisualKitModelAttachRec* attach, int32_t kitType, uint32_t displayId, int32_t inventoryType) {
    CEffect* effect = CEffect::Create();
    effect->m_expireTime = expireTime;

    WOWGUID self = this->m_obj->m_guid;
    WOWGUID source = target ? WOWGUID::Make(target) : this->m_obj->m_guid;

    effect->AddEffect(attachType, spellId, kit, effectName, source, WOWGUID::Make(target), self, flags, callback, position, param, attach, kitType, displayId, inventoryType);

    if (flags & 0x40000) {
        effect->SetEnabled(0);
    }

    if (kit->m_flags & 0x200) {
        effect->m_flags |= 0x2000000;
    }

    if (kit->m_flags & 0x10000) {
        effect->m_flags2 |= 0x10;
    }

    effect->Release();
    flags &= ~0x10;
}

// PROVEN: b86acf8b IDENTICAL/NONE
// OFFSET: 0x005DDA00/0x14025A2A0
void CGObject_C::EffectOnceHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    CEffect* effect = reinterpret_cast<CEffect*>(static_cast<uintptr_t>(param));

    if (interrupted || !effect) {
        return;
    }

    if (effect->m_model && effect->m_model->HasSequence(159)) {
        effect->m_model->SetSequenceCallback(reinterpret_cast<void*>(&CGObject_C::EffectDecayHandler), reinterpret_cast<uintptr_t>(effect));
        effect->m_model->SetBoneSequence(0xFFFFFFFF, 159, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
        return;
    }

    M2BoneSequenceInfo info;
    effect->m_model->GetBoneSequenceInfo(boneId, info);
    effect->m_model->SetBoneSequenceTime(boneId, info.m_endTime - info.m_startTime - 1);
    uint32_t sceneTime = effect->m_model->m_scene->m_time;

    if (!sceneTime) {
        sceneTime = 1;
    }

    effect->m_model->m_lastAnimTime = sceneTime;
    effect->MoveToDestroyList();
}

// PROVEN: 9d79b5a0 IDENTICAL/NONE
// OFFSET: 0x005DDAB0
void CGObject_C::AuraEffectHoldHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    CEffect* effect = reinterpret_cast<CEffect*>(static_cast<uintptr_t>(param));

    if (interrupted || !effect || !effect->m_model) {
        return;
    }

    uint32_t expireTime = effect->m_expireTime;

    if (expireTime && static_cast<int32_t>(CGWorldFrame::s_currentWorldFrame->GetSceneTime() - expireTime) > 0) {
        if (effect->m_model->HasSequence(159)) {
            effect->m_model->SetSequenceCallback(reinterpret_cast<void*>(&CGObject_C::EffectDecayHandler), reinterpret_cast<uintptr_t>(effect));
            effect->m_model->SetBoneSequence(0xFFFFFFFF, 159, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
        } else {
            effect->MoveToDestroyList();
        }
    } else {
        effect->m_model->SetBoneSequence(0xFFFFFFFF, sequenceId, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    }
}

// OFFSET: 0x005DDB70
void CGObject_C::AuraEffectHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    CEffect* effect = reinterpret_cast<CEffect*>(static_cast<uintptr_t>(param));

    if (interrupted || !effect || !effect->m_model) {
        return;
    }

    uint32_t sequence;

    if (effect->m_attachRec && effect->m_attachRec->unk_0030 != -1 && effect->m_model->HasSequence(effect->m_attachRec->unk_0030)) {
        sequence = effect->m_attachRec->unk_0030;

        if (sequence == -1) {
            return;
        }
    } else if (effect->m_kitRec && (effect->m_kitRec->m_flags & 0x20)) {
        sequence = 0;
    } else if (effect->m_model->HasSequence(158)) {
        sequence = 158;
    } else {
        return;
    }

    effect->m_model->SetSequenceCallback(reinterpret_cast<void*>(&CGObject_C::AuraEffectHoldHandler), reinterpret_cast<uintptr_t>(effect));
    effect->m_model->SetBoneSequence(0xFFFFFFFF, sequence, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
}

// OFFSET: [0x005DDC20/0x14025A5B0<WowClientDB<GuildColorBackgroundRec>>, 0x005DDC20/0x14025A5B0<WowClientDB<GuildColorBorderRec>>, 0x005DDC20/0x14025A5B0<WowClientDB<GuildColorEmblemRec>>]
template <class T>
static int32_t GetGuildColor(T db, uint8_t index, CImVector& color) {
    if (!db.GetRecord(index)) {
        return 0;
    }

    color.b = db.GetRecord(index)->m_blue;
    color.r = db.GetRecord(index)->m_red;
    color.g = db.GetRecord(index)->m_green;
    color.a = 0xFF;
    return 1;
}

// OFFSET: 0x005DDCB0
void CGObject_C::PostInit(uint32_t time, const CClientObjCreate& create, bool reenable) {
    this->m_modelFlags |= 0x40000;
    this->DoFade(this->GetBaseAlpha(), this->ShouldFadeIn() ? 1000 : 0);
    this->RefreshInteractIcon();
    CM2Model* model = this->m_worldModel;
    this->m_scaleChangeTime = 0;

    if (model) {
        float scale = this->GetTrueScale();
        float facing = this->GetDisplayFacing();
        model->SetWorldTransform(this->GetPosition(), facing, scale);
    }

    if (this->m_worldObject) {
        this->UpdateWorldObject(0);
    }
}

// PROVEN: e6f1feff IDENTICAL/NONE
// OFFSET: 0x005DDD60/0x14025A760
void CGObject_C::ClearEffectList(int32_t reason) {
    CEffect* effect = this->m_effectList;

    if (!reason) {
        while (effect) {
            CEffect* next = effect->m_next;
            const SpellRec* spell = g_spellDB.GetRecord(effect->m_spellId);

            if (!spell) {
                effect->MoveToDestroyList();
            } else if (!(spell->m_attributesExC & 0x100000)) {
                SpellVisualRec visualRec;
                const SpellVisualRec* visual = this->GetAppropriateSpellVisual(spell, visualRec, 0, 0);

                if (!visual || !effect->m_kitRec || !visual->m_missileTargetingKit || visual->m_missileTargetingKit != effect->m_kitRec->m_id) {
                    effect->MoveToDestroyList();
                }
            }

            effect = next;
        }
    } else {
        for (; effect; effect = this->m_effectList) {
            effect->UnLinkSelf();

            if (effect->m_attachmentId == -1 && effect->m_expireTime) {
                effect->MoveToOrphanList(1);
            } else {
                effect->Release();
            }
        }
    }
}

// OFFSET: 0x005DDE70/0x14025A8F0
int32_t CGObject_C::CanSquelchEffect(uint64_t guid) {
    if (!g_emphasizeMySpellEffectsCVar || !g_emphasizeMySpellEffectsCVar->m_intValue) {
        return 0;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(guid), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 0;
    }

    if (unit == ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    const WOWGUID* owner = &unit->m_unit->m_unitFieldCharmedby;

    if (!owner->m_guid) {
        owner = &unit->m_unit->m_unitFieldCreatedby;
    }

    if (*owner == ClntObjMgrGetActivePlayer()) {
        return 0;
    }

    CGObject_C* charmer = ClntObjMgrObjectPtr(*unit->GetCharmerOrCreatorGuid(), TYPEMASK_UNIT, __FILE__, __LINE__);
    return (unit->m_obj->m_type & TYPEMASK_PLAYER) || (charmer && (charmer->m_obj->m_type & TYPEMASK_PLAYER));
}

// OFFSET: 0x005DDF60/0x14025AA10
void CGObject_C::ClearEffectKit(uint32_t spellId, int32_t kitType, uint32_t param, int32_t matchSpell, int32_t matchParam) {
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return;
    }

    SpellVisualRec visualRec;
    const SpellVisualRec* visual = this->GetAppropriateSpellVisual(spell, visualRec, 0, 0);

    if (!visual) {
        return;
    }

    const SpellVisualKitRec* kit;

    switch (kitType) {
        case 0:
            kit = g_spellVisualKitDB.GetRecord(visual->m_castKit);
            break;

        case 1:
            kit = g_spellVisualKitDB.GetRecord(visual->m_impactKit);
            break;

        case 2:
            kit = g_spellVisualKitDB.GetRecord(visual->m_stateKit);
            break;

        case 4:
            kit = g_spellVisualKitDB.GetRecord(visual->m_precastKit);
            break;

        case 5:
            kit = g_spellVisualKitDB.GetRecord(visual->m_casterImpactKit);
            break;

        case 6:
            kit = g_spellVisualKitDB.GetRecord(visual->m_targetImpactKit);
            break;

        case 7:
            kit = g_spellVisualKitDB.GetRecord(visual->m_missileTargetingKit);
            break;

        case 8:
            kit = g_spellVisualKitDB.GetRecord(visual->m_stateDoneKit);
            break;

        default:
            return;
    }

    if (kit) {
        this->ClearEffectKitByID(spellId, kit->m_id, param, matchSpell, matchParam);
    }
}

// PROVEN: b29b1188 IDENTICAL/NONE
// OFFSET: 0x005DE070
static int32_t ScaleUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        object->HandleScaleUpdate(*static_cast<const float*>(oldValue));
    }

    return 1;
}

// PROVEN: 6a184609 IDENTICAL/NONE
// OFFSET: 0x005DE0B0
void CGObject_C::Shutdown() {
    if (CGObject_C::s_selectionTexture) {
        TextureRelease(CGObject_C::s_selectionTexture);
    }

    CGObject_C::s_selectionTexture = NULL;
    LoadInteractIcons(0);
    ClntObjMgrUnsetTypeMirrorHandler(ID_OBJECT, OBJECT_FIELD_SCALE_X * sizeof(uint32_t), &ScaleUpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_OBJECT, OBJECT_FIELD_TYPE * sizeof(uint32_t) + sizeof(uint16_t), &GuildDataTypeUpdateHandler, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_OBJECT, OBJECT_FIELD_DATA * sizeof(uint32_t), &GuildDataUpdateHandler, NULL);
}

// OFFSET: 0x005DE130
int32_t CGObject_C::SetCircleRenderStates() {
    CGxTex* texture = TextureGetGxTex(CGObject_C::s_selectionTexture);

    if (!texture) {
        return 0;
    }

    GxRsSet(GxRs_BlendingMode, GxBlend_Add);
    GxRsSet(GxRs_AlphaRef, static_cast<int32_t>(CGxDevice::s_alphaRef[g_theGxDevicePtr->m_appRenderStates[GxRs_BlendingMode].m_value.m_data.i[0]]));
    GxRsSet(GxRs_Culling, 1);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, texture);
    GxRsSet(GxRs_ColorOp0, 0);
    GxRsSet(GxRs_AlphaOp0, 0);
    return 1;
}

// OFFSET: 0x005DE310/0x14025AE20
bool CGObject_C::GetGuildLogoInfo(int32_t background, int32_t borderColor, int32_t emblemColor, int32_t emblemStyle, CImVector& backgroundColor, CImVector& border, CImVector& emblem, char* fileName, int32_t size) {
    return GetGuildColor(g_guildColorBackgroundDB, background, backgroundColor) && GetGuildColor(g_guildColorBorderDB, borderColor, border) && GetGuildColor(g_guildColorEmblemDB, emblemColor, emblem) && CCharacterComponent::GetProceduralEmblemFileName(emblemStyle, fileName, size);
}

// OFFSET: 0x005DE460/0x14025AFB0
void CGObject_C::GetDefaultGuildTextureSettings(CImVector& backgroundColor, CImVector& border, CImVector& emblem, char* fileName, int32_t size, int32_t factionIndex) {
    if (factionIndex == 0) {
        GetGuildColor(g_guildColorBackgroundDB, 5, backgroundColor);
        GetGuildColor(g_guildColorBorderDB, 15, border);
        GetGuildColor(g_guildColorEmblemDB, 15, emblem);
        SStrCopy(fileName, "Textures\\GuildEmblems\\emblem_Default_Horde", size);
    } else if (factionIndex == 1) {
        GetGuildColor(g_guildColorBackgroundDB, 30, backgroundColor);
        GetGuildColor(g_guildColorBorderDB, 16, border);
        GetGuildColor(g_guildColorEmblemDB, 16, emblem);
        SStrCopy(fileName, "Textures\\GuildEmblems\\emblem_Default_Alliance", size);
    } else {
        backgroundColor.a = 0xFF;
        backgroundColor.r = 0x80;
        backgroundColor.g = 0x80;
        backgroundColor.b = 0x80;
        border.a = 0;
        border.r = 0;
        border.g = 0;
        border.b = 0;
        emblem.a = 0;
        emblem.r = 0;
        emblem.g = 0;
        emblem.b = 0;
        CCharacterComponent::GetProceduralEmblemFileName(0, fileName, size);
    }

    SStrPack(fileName, ".blp", size);
}

// OFFSET: 0x005DE790
CGObject_C* CGObject_C::GetFocusObject() {
    CGObject_C* object = ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__);

    if (!object) {
        object = ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__);

        if (!object) {
            return NULL;
        }
    }

    if (!(object->m_obj->m_type & TYPEMASK_PLAYER) || !(static_cast<CGPlayer_C*>(object)->m_combatModeFlags & 1)) {
        return object;
    }

    uint64_t farSight = static_cast<CGPlayer_C*>(object)->GetFarSightGuid();

    if (!farSight) {
        return object;
    }

    CGObject_C* target = ClntObjMgrObjectPtr(WOWGUID::Make(farSight), TYPEMASK_OBJECT, __FILE__, __LINE__);
    return target ? target : object;
}

// OFFSET: 0x005DE820/0x14025B410
void CGObject_C::SetInteractIconVisible(int32_t visible) {
    CM2Model* icon = this->m_interactIcon;

    if (!icon || !icon->m_attachParent) {
        return;
    }

    CM2Model* parent = icon->m_attachParent;
    uint32_t flag = visible & 0x1;

    if (parent) {
        icon->m_fFlags = (icon->m_fFlags & ~0x40) | (flag << 6);
    } else {
        icon->m_fFlags = (icon->m_fFlags & ~0x8) | (flag << 3);
    }

    if (parent) {
        icon->m_fFlags = (icon->m_fFlags & ~0x10000) | (flag << 16);
    } else {
        icon->m_fFlags = (icon->m_fFlags & ~0x8000) | (flag << 15);
    }
}

// PROVEN: 6a59f33f IDENTICAL/NONE
// OFFSET: 0x005DE890/0x14025B480
C3Vector CGObject_C::GetNamePosition() const {
    if (this->m_worldModel->HasAttachment(18)) {
        return this->m_worldModel->GetAttachmentPosition(18);
    }

    C3Vector position = this->GetPosition();
    position.z += this->m_height * this->m_scale * 1.25f;
    return position;
}

// OFFSET: 0x005DE910/0x14025B530
void CGObject_C::PlaySpellVisualKit(const PlaySpellVisuaKitData& data) {
    uint32_t flags = data.m_flags;
    uint32_t spellId = 0;
    uint32_t expireTime = 0;
    int32_t hasEffect = 0;

    if (data.m_spellRec) {
        spellId = data.m_spellRec->m_id;
    }

    if (!data.m_impactVisualKit) {
        return;
    }

    if (data.m_timeOffset) {
        expireTime = data.m_timeOffset + CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    }

    const C3Vector* position = NULL;
    C3Vector transformed(0.0f, 0.0f, 0.0f);

    if (data.m_position) {
        if (data.m_transportGUID.m_guid) {
            position = &transformed;
            C44Matrix transport;
            MovementGetTransportMtxX(data.m_transportGUID, &transport, WOWGUID(), __FILE__, __LINE__);
            transformed = *data.m_position * transport;
        } else {
            position = data.m_position;
        }
    }

    const SpellVisualKitRec* kit = data.m_impactVisualKit;
    EFFECT_CALLBACK callback = NULL;

    if (kit->m_shakeID) {
        flags |= 0x10;
    }

    if (SE3::IsLoopingOrPeriodic_ByDef(kit->m_soundID)) {
        flags |= 0x1;
    }

    const SpellRec* spell = data.m_spellRec;

    if (spell && spell->m_spellMissileID > 0) {
        flags |= 0x400000;
    }

    const SpellVisualRec* visual = NULL;

    if (spell) {
        visual = GetSpellVisual(spell, static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(data.m_someGUID, TYPEMASK_UNIT, __FILE__, __LINE__)));
    }

    if (this->PlaySpellVisualKit_DelayLightningEffects(spell, kit, static_cast<SPELL_VISUAL_KIT_TYPE>(data.m_kitType), data.m_position, data.m_transportGUID, data.m_animParam, data.unk_0024, data.unk_0028, data.unk_002C, flags, visual, data.m_displayId, data.m_inventoryType)) {
        return;
    }

    if (visual && (visual->m_flags & 0x10)) {
        flags |= 0x200000;
    }

    if (kit->m_flags & 0xC) {
        flags |= 0x1000000;
    }

    uint32_t animParam = data.m_animParam;

    switch (data.m_kitType) {
        case 0:
            flags |= 0x80;
            callback = &CGObject_C::EffectOnceHandler;
            flags = (flags & ~0x21) | 0x20;
            break;

        case 1:
        case 5:
        case 6:
            flags = (flags & ~0x20000021) | 0x20000020;
            callback = &CGObject_C::EffectOnceHandler;

            if (CGObject_C::CanSquelchEffect(data.m_someGUID.m_guid)) {
                flags |= 0x40000000;
            }

            break;

        case 2:
            callback = &CGObject_C::AuraEffectHandler;
            flags |= 0x21000;

            if (spell) {
                uint32_t count;
                const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

                for (uint32_t i = 0; i < count; i++) {
                    if (effects->m_data[i] && effects->m_data[i]->m_effectAura == 78) {
                        flags |= 0x800000;
                    }
                }
            }

            break;

        case 3:
            if (spell) {
                callback = &CGObject_C::EffectOnceHandler;
                flags = (flags & ~0x21) | 0x20;
            }

            animParam = 0;
            flags |= 0x22000;
            break;

        case 4:
            callback = (kit->m_flags & 0x8000) ? &CGObject_C::AuraEffectHandler : &CGObject_C::LoopEffectHandler;
            flags |= 0x40;
            break;

        case 7:
            if (spell) {
                callback = &CGObject_C::AuraEffectHandler;
            }

            flags |= 0x22000;
            break;

        case 8:
            callback = &CGObject_C::EffectOnceHandler;
            flags = (flags & ~0x21) | 0x20;
            break;

        default:
            break;
    }

    const SpellVisualEffectNameRec* effectName = g_spellVisualEffectNameDB.GetRecord(kit->m_headEffect);

    if (effectName) {
        this->PlayEffectName(20, expireTime, spellId, kit, effectName, flags, callback, position, data.m_someGUID.m_guid, data.unk_0038, NULL, data.m_kitType, data.m_displayId, data.m_inventoryType);
        hasEffect = 1;
    }

    effectName = g_spellVisualEffectNameDB.GetRecord(data.m_impactVisualKit->m_worldEffect);

    if (effectName) {
        CEffect* effect = CEffect::Create();
        effect->m_expireTime = expireTime;

        if (data.m_spellRec && !data.m_position) {
            uint32_t count;
            const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(data.m_spellRec->m_id, count);

            for (uint32_t i = 0; i < count; i++) {
                if (effects->m_data[i] && effects->m_data[i]->m_effectAura == 26) {
                    flags |= 0x4000;
                    break;
                }
            }
        }

        WOWGUID transport = data.m_transportGUID;
        C3Vector objectPosition(0.0f, 0.0f, 0.0f);

        if (!data.m_position) {
            flags |= 0x200;
            objectPosition = this->GetRawPosition();
            transport = WOWGUID::Make(this->GetTransportGUID());

            if ((this->m_obj->m_type & TYPEMASK_UNIT) && (static_cast<CGUnit_C*>(this)->m_movement->m_flags & 0x10000000)) {
                objectPosition.z -= static_cast<CGUnit_C*>(this)->m_unit->m_unitFieldHoverheight;
            }
        }

        if (!data.m_position && (flags & 0x4000) && (this->m_obj->m_type & TYPEMASK_UNIT) && static_cast<CGUnit_C*>(this)->IsVehiclePassengerSeated()) {
            WOWGUID self = this->m_obj->m_guid;
            effect->AddEffect(19, spellId, data.m_impactVisualKit, effectName, data.m_someGUID, data.m_someGUID, self, flags, callback, position, data.unk_0038, NULL, data.m_kitType, data.m_displayId, data.m_inventoryType);

            if (data.m_impactVisualKit->m_flags & 0x200) {
                effect->m_flags |= 0x2000000;
            }

            if (data.m_impactVisualKit->m_flags & 0x10000) {
                effect->m_flags2 |= 0x10;
            }
        } else {
            WOWGUID self = this->m_obj->m_guid;
            const C3Vector* effectPosition = data.m_position ? data.m_position : &objectPosition;
            effect->AddEffectByPosition(*effectPosition, data.m_orientation, CGWorldFrame::s_currentWorldFrame->GetSceneTime(), spellId, data.m_impactVisualKit, effectName, self, data.m_someGUID, flags, callback, data.unk_0038, NULL, transport, data.m_kitType, data.m_displayId, data.m_inventoryType);
        }

        effect->Release();
        flags &= ~0x11;
        hasEffect = 1;
    }

    if (flags & 0x2000) {
        effectName = g_spellVisualEffectNameDB.GetRecord(data.m_impactVisualKit->m_baseEffect);

        if (!effectName || !data.m_position) {
            goto attachments;
        }

        CEffect* effect = CEffect::Create();
        effect->m_expireTime = expireTime;
        WOWGUID self = this->m_obj->m_guid;
        effect->AddEffectByPosition(*data.m_position, data.m_orientation, CGWorldFrame::s_currentWorldFrame->GetSceneTime(), spellId, data.m_impactVisualKit, effectName, self, data.m_someGUID, flags, callback, data.unk_0038, NULL, data.m_transportGUID, data.m_kitType, data.m_displayId, data.m_inventoryType);
        effect->Release();
        flags &= ~0x11;
    } else {
        effectName = g_spellVisualEffectNameDB.GetRecord(data.m_impactVisualKit->m_baseEffect);

        if (effectName) {
            this->PlayEffectName(19, expireTime, spellId, data.m_impactVisualKit, effectName, flags, callback, position, data.m_someGUID.m_guid, data.unk_0038, NULL, data.m_kitType, data.m_displayId, data.m_inventoryType);
            hasEffect = 1;
        }

        effectName = g_spellVisualEffectNameDB.GetRecord(data.m_impactVisualKit->m_leftHandEffect);

        if (effectName) {
            this->PlayEffectName(21, expireTime, spellId, data.m_impactVisualKit, effectName, flags, callback, position, data.m_someGUID.m_guid, data.unk_0038, NULL, data.m_kitType, data.m_displayId, data.m_inventoryType);
            hasEffect = 1;
        }

        effectName = g_spellVisualEffectNameDB.GetRecord(data.m_impactVisualKit->m_rightHandEffect);

        if (effectName) {
            this->PlayEffectName(22, expireTime, spellId, data.m_impactVisualKit, effectName, flags, callback, position, data.m_someGUID.m_guid, data.unk_0038, NULL, data.m_kitType, data.m_displayId, data.m_inventoryType);
            hasEffect = 1;
        }

        effectName = g_spellVisualEffectNameDB.GetRecord(data.m_impactVisualKit->m_breathEffect);

        if (effectName) {
            this->PlayEffectName(17, expireTime, spellId, data.m_impactVisualKit, effectName, flags, callback, position, data.m_someGUID.m_guid, data.unk_0038, NULL, data.m_kitType, data.m_displayId, data.m_inventoryType);
            hasEffect = 1;
        }

        effectName = g_spellVisualEffectNameDB.GetRecord(data.m_impactVisualKit->m_chestEffect);

        if (effectName) {
            this->PlayEffectName(34, expireTime, spellId, data.m_impactVisualKit, effectName, flags, callback, position, data.m_someGUID.m_guid, data.unk_0038, NULL, data.m_kitType, data.m_displayId, data.m_inventoryType);
            hasEffect = 1;
        }

        this->PlaySpellVisualKit_HandleWeapons(data.m_spellRec, data.m_impactVisualKit, position, data.m_someGUID, data.unk_0038, flags, callback, &hasEffect, data.m_displayId, data.m_inventoryType);

        const SpellVisualKitRec* special = data.m_impactVisualKit;
        effectName = g_spellVisualEffectNameDB.GetRecord(special->m_specialEffect[0]);

        if (effectName) {
            this->PlayEffectName(23, expireTime, spellId, special, effectName, flags, callback, position, data.m_someGUID.m_guid, data.unk_0038, NULL, data.m_kitType, data.m_displayId, data.m_inventoryType);
            hasEffect = 1;
        }

        effectName = g_spellVisualEffectNameDB.GetRecord(data.m_impactVisualKit->m_specialEffect[1]);

        if (effectName) {
            this->PlayEffectName(24, expireTime, spellId, data.m_impactVisualKit, effectName, flags, callback, position, data.m_someGUID.m_guid, data.unk_0038, NULL, data.m_kitType, data.m_displayId, data.m_inventoryType);
            hasEffect = 1;
        }

        effectName = g_spellVisualEffectNameDB.GetRecord(data.m_impactVisualKit->m_specialEffect[2]);

        if (!effectName) {
            goto attachments;
        }

        this->PlayEffectName(25, expireTime, spellId, data.m_impactVisualKit, effectName, flags, callback, data.m_position, data.m_someGUID.m_guid, data.unk_0038, NULL, data.m_kitType, data.m_displayId, data.m_inventoryType);
    }

    hasEffect = 1;

attachments:
    int32_t attachCount = SpellVisualsGetModelAttachRecCount(data.m_impactVisualKit->m_id);

    for (int32_t i = 0; i < attachCount; i++) {
        const SpellVisualKitModelAttachRec* attach = SpellVisualsGetModelAttachRec(data.m_impactVisualKit->m_id, i);
        const SpellVisualEffectNameRec* attachEffect = attach ? g_spellVisualEffectNameDB.GetRecord(attach->m_spellVisualEffectNameID) : NULL;

        if (!attachEffect) {
            continue;
        }

        if (flags & 0x2000) {
            if (!data.m_position) {
                continue;
            }

            CEffect* effect = CEffect::Create();
            effect->m_expireTime = expireTime;
            WOWGUID self = this->m_obj->m_guid;
            effect->AddEffectByPosition(*data.m_position, data.m_orientation, CGWorldFrame::s_currentWorldFrame->GetSceneTime(), spellId, data.m_impactVisualKit, attachEffect, self, data.m_someGUID, flags, callback, data.unk_0038, attach, data.m_transportGUID, data.m_kitType, data.m_displayId, data.m_inventoryType);
            effect->Release();
            flags &= ~0x11;
        } else if (attach->m_attachmentID == -1) {
            CEffect* effect = CEffect::Create();
            effect->m_expireTime = expireTime;

            if (data.m_spellRec && !data.m_position) {
                uint32_t count;
                const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(data.m_spellRec->m_id, count);

                for (uint32_t j = 0; j < count; j++) {
                    if (effects->m_data[j] && effects->m_data[j]->m_effectAura == 26) {
                        flags |= 0x4000;
                        break;
                    }
                }
            }

            WOWGUID transport = data.m_transportGUID;
            C3Vector objectPosition(0.0f, 0.0f, 0.0f);

            if (!data.m_position) {
                flags |= 0x200;
                objectPosition = this->GetRawPosition();
                transport = WOWGUID::Make(this->GetTransportGUID());

                if ((this->m_obj->m_type & TYPEMASK_UNIT) && (static_cast<CGUnit_C*>(this)->m_movement->m_flags & 0x10000000)) {
                    objectPosition.z -= static_cast<CGUnit_C*>(this)->m_unit->m_unitFieldHoverheight;
                }
            }

            WOWGUID self = this->m_obj->m_guid;
            const C3Vector* effectPosition = data.m_position ? data.m_position : &objectPosition;
            effect->AddEffectByPosition(*effectPosition, data.m_orientation, CGWorldFrame::s_currentWorldFrame->GetSceneTime(), spellId, data.m_impactVisualKit, attachEffect, self, data.m_someGUID, flags, callback, data.unk_0038, attach, transport, data.m_kitType, data.m_displayId, data.m_inventoryType);
            effect->Release();
            flags &= ~0x11;
        } else {
            this->PlayEffectName(attach->m_attachmentID, expireTime, spellId, data.m_impactVisualKit, attachEffect, flags, callback, position, data.m_someGUID.m_guid, data.unk_0038, attach, -1, data.m_displayId, data.m_inventoryType);
        }

        hasEffect = 1;
    }

    PlaySpellVisualKitAnimData anim;
    anim.m_spellRec = data.m_spellRec;
    anim.m_kit = data.m_impactVisualKit;
    anim.m_kitType = data.m_kitType;
    anim.m_position = position;
    anim.m_animParam = animParam;
    anim.unk_0014 = data.unk_0024;
    anim.m_guid = data.m_someGUID;
    anim.unk_0020 = data.unk_0038;
    anim.m_visual = visual;
    anim.m_flags = flags;
    anim.m_hasEffect = hasEffect;
    anim.m_stopCallback = data.m_stopCallback;
    anim.m_stopParam = data.m_stopParam;
    anim.unk_0040 = data.unk_0048;
    this->PlaySpellVisualKit_PlayAnims(&anim);

    if (data.m_impactVisualKit->m_soundID && (this->m_obj->m_type & TYPEMASK_UNIT)) {
        CEffect* effect = CEffect::Create();
        effect->m_flags = flags;
        effect->m_kitRec = data.m_impactVisualKit;
        effect->AddSoundEffect(expireTime, spellId, this, data.m_someGUID, position, data.unk_0038);
        effect->Release();
    }
}

// OFFSET: 0x005DF8A0
bool CGObject_C::GetGuildLogoInfo(uint64_t guildGUID, CImVector& backgroundColor, CImVector& border, CImVector& emblem, char* fileName, int32_t size) {
    backgroundColor.b = 0x80;
    backgroundColor.g = 0x80;
    backgroundColor.r = 0x80;
    backgroundColor.a = 0xFF;
    border.b = 0;
    border.g = 0;
    border.r = 0;
    border.a = 0;
    emblem.b = 0;
    emblem.g = 0;
    emblem.r = 0;
    emblem.a = 0;

    if (!guildGUID) {
        return false;
    }

    int32_t emblemStyle = -1;
    int32_t emblemColor = -1;
    int32_t borderStyle = -1;
    int32_t borderColor = -1;
    int32_t background = -1;

    GuildGetGuildTabard(WOWGUID(), WOWGUID::Make(guildGUID), NULL, NULL, emblemStyle, emblemColor, borderStyle, borderColor, background);

    if (emblemStyle == -1 || *reinterpret_cast<const int32_t*>(&emblem) == -1 || background == -1) {
        return false;
    }

    if (!GetGuildColor(g_guildColorBackgroundDB, background, backgroundColor)) {
        return false;
    }

    if (!GetGuildColor(g_guildColorBorderDB, borderColor, border)) {
        return false;
    }

    if (!GetGuildColor(g_guildColorEmblemDB, emblemColor, emblem)) {
        return false;
    }

    return CCharacterComponent::GetProceduralEmblemFileName(emblemStyle, fileName, size) != 0;
}

// OFFSET: 0x005DF9E0/0x14025CED0
void CGObject_C::UpdateInteractIcon() {
    this->RemoveInteractIcon();

    uint32_t icon;
    uint32_t status = this->m_questStatus;

    if (!status) {
        if (this->m_interactIconType == 2) {
            icon = 7;
        } else {
            icon = this->GetNPCInteractIcon();

            if (!icon) {
                return;
            }
        }
    } else {
        if (status - 2 <= 2) {
            if (CGMinimapFrame::IsTrackingTrivialQuests()) {
                icon = CGObject_C::s_questStatusIcons[status];
            } else if (this->m_interactIconType) {
                icon = this->GetQuestInteractIcon();
            } else {
                icon = 0;
            }
        } else {
            icon = CGObject_C::s_questStatusIcons[status];
        }

        if (!icon) {
            icon = this->GetNPCInteractIcon();

            if (!icon) {
                return;
            }
        }
    }

    CM2Model* model = World::GetScene()->DuplicateModel(CGObject_C::s_interactIcons[icon].m_model, 0);
    this->m_interactIcon = model;

    if (model) {
        model->m_fadeAlpha = CGObject_C::s_interactIcons[icon].m_scale;
    }

    this->UpdateInteractIconAttach();
    this->SetInteractIconVisible(CGGameUI::m_visible);
}

// OFFSET: 0x005DFAD0
void CGObject_C::PrepareToFadeInt(float alpha, uint64_t transportGuid, const C44Matrix& matrix) {
    uint32_t time = FrameTime::GetCurTimeMs();

    if (!this->m_fadeoutInfo) {
        this->m_fadeoutInfo = CGObject_C::s_fadeoutHeap->New("", 0);
    }

    this->m_fadeoutInfo->m_startTime = time;
    this->m_fadeoutInfo->m_alpha = alpha;

    CGObject_C* transport;

    if (transportGuid && (transport = ClntObjMgrObjectPtr(WOWGUID::Make(transportGuid), TYPEMASK_GAMEOBJECT, __FILE__, __LINE__)) != NULL) {
        this->m_fadeoutInfo->m_transportGuid = WOWGUID::Make(transportGuid);
        C44Matrix transportMatrix = transport->GetMatrix();
        this->m_fadeoutInfo->m_matrix = matrix * transportMatrix.AffineInverse();
    } else {
        this->m_fadeoutInfo->m_transportGuid = WOWGUID();
    }
}

// OFFSET: 0x005DFBB0
void CGObject_C::AddNonModelToFadeoutList(float alpha, uint64_t transportGuid) {
    C44Matrix matrix;
    this->PrepareToFadeInt(alpha, transportGuid, matrix);
    s_fadeoutList.LinkToTail(this);
}

// PROVEN: 56d140fe IDENTICAL/NONE
// OFFSET: 0x005DFC40
CGObject_C::CGObject_C(uint32_t time, const CClientObjCreate& create) : m_interactIcon(NULL), m_questStatus(0), m_interactIconType(0), m_scale(1.0f), m_scaleModifier(1.0f), m_scaleChangeTime(0), m_scaleChangeStart(1.0f), m_effectList(NULL), m_height(1.0f), m_nameDesc(NULL), m_worldModel(NULL), m_worldObject(0), m_modelFlags(0), m_fadeStartTime(0), m_fadeDuration(0), m_alpha(0), m_fadeStartAlpha(0), m_fadeEndAlpha(0), m_alphaScale(0xFF), m_effectManager(NULL), m_fadeoutInfo(NULL) {
    ClntObjMgrLinkInNewObject(this);
    this->m_scale = this->m_obj->m_scale;
}

// OFFSET: 0x005DFDD0
int32_t CGObject_C::PrepareToFade(float alpha, uint64_t transportGuid) {
    CM2Model* model = this->GetObjectModel();

    if (model && model->IsDrawable() && alpha >= 0.01f) {
        this->PrepareToFadeInt(alpha, transportGuid, model->m_worldTransform);
        return 1;
    }

    if (this->m_worldObject) {
        World::ObjectDestroy(this->m_worldObject);
        this->m_worldObject = NULL;
    }

    return 0;
}

// OFFSET: 0x005DFE50
void CGObject_C::RemoveFromFadeoutList() {
    if (this->m_fadeoutInfo) {
        CGObject_C::s_fadeoutHeap->Delete(this->m_fadeoutInfo, "", 0);
        this->m_fadeoutInfo = NULL;
    }

    this->m_fadeoutLink.Unlink();
}

// OFFSET: 0x005DFEE0
CGObject_C::~CGObject_C() {
    if (this->GetGuildGUID().m_guid) {
        WOWGUID requester;

        if (!g_guildInfoCache.GetRecord2(this->GetGuildGUID().m_guid, requester, NULL, NULL, false)) {
            g_guildInfoCache.CancelCallback(this->GetGuildGUID().m_guid, &CGObject_C::GuildTextureCallback, NULL);
        }
    }

    if (this->m_worldObject) {
        this->RemoveFromFadeoutList();
        World::ObjectDestroy(this->m_worldObject);
        this->m_worldObject = NULL;
    }

    this->ReleaseModel();
    this->ClearEffectList(1);
    this->RemoveInteractIcon();

    if (this->m_effectManager) {
        delete this->m_effectManager;
        this->m_effectManager = NULL;
    }
}

// OFFSET: 0x005E00C0/0x14025D9D0
void CGObject_C::Fadeout() {
    if (!this->m_worldObject) {
        return;
    }

    if (!this->ShouldFadeout()) {
        World::ObjectDestroy(this->m_worldObject);
        this->m_worldObject = NULL;
        return;
    }

    if (this->GetObjectModel() && !this->GetObjectModel()->m_attachParent) {
        this->Animate(0.0f);
    }

    uint64_t transport = this->GetTransportGUID();

    if (this->PrepareToFade(this->m_alpha * 0.0039215689f, transport)) {
        s_fadeoutList.LinkToTail(this);
    }
}

// OFFSET: 0x005E0170/0x14025DA80
void CGObject_C::Disable() {
    if (this->GetGuildGUID().m_guid) {
        WOWGUID requester;

        if (!g_guildInfoCache.GetRecord2(this->GetGuildGUID().m_guid, requester, NULL, NULL, false)) {
            g_guildInfoCache.CancelCallback(this->GetGuildGUID().m_guid, &CGObject_C::GuildTextureCallback, NULL);
        }
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (camera && camera->m_relativeTo == this->m_obj->m_guid) {
        camera->MakeRelativeTo(WOWGUID());
    }

    this->Fadeout();
    this->RemoveInteractIcon();
    this->m_modelFlags = (this->m_modelFlags & 0xF8FEFFFF) | 0x10000;
    this->m_fadeDuration = 0;
    this->m_fadeEndAlpha = 0;
    this->m_alpha = 0;
    this->m_disableTime = FrameTime::GetCurTimeMs();
}

// OFFSET: 0x005E0280
void CGObject_C::Reenable(const CClientObjCreate& create) {
    this->m_modelFlags = (this->m_modelFlags & 0xFFFCFFFF) | 0x20000;
    this->m_scale = this->GetScale();
    this->m_scaleChangeTime = 0;
    this->RemoveFromFadeoutList();

    if (this->GetObjectModel()) {
        this->m_alpha = static_cast<int32_t>(this->GetObjectModel()->m_fadeAlpha * 255.0f);
    }

    this->DoFade(this->GetBaseAlpha(), this->ShouldFadeIn() ? 1000 : 0);
}

// OFFSET: 0x005E0330
void CGObject_C::UpdateFadeoutList() {
    CGObject_C* object = s_fadeoutList.Head();

    while (object) {
        FadeoutInfo* info = object->m_fadeoutInfo;
        CGObject_C* next = s_fadeoutList.Next(object);
        uint32_t time = FrameTime::GetCurTimeMs();
        CM2Model* model = object->GetObjectModel();
        int32_t elapsed = time - info->m_startTime;

        if (elapsed > 2000 || (model && !model->CanUpdateNonReadyTextures())) {
            object->RemoveFromFadeoutList();

            if (object->m_worldObject) {
                World::ObjectDestroy(object->m_worldObject);
                object->m_worldObject = NULL;
            }

            object = next;
            continue;
        }

        if (info->m_transportGuid.m_guid) {
            CGObject_C* transport = ClntObjMgrObjectPtr(info->m_transportGuid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

            if (transport) {
                C44Matrix transportMatrix = transport->GetMatrix();

                if (model) {
                    C44Matrix world = info->m_matrix * transportMatrix;
                    model->m_fFlags |= 0x4000;
                    model->SetWorldTransform(world);
                }

                object->SetFrameOfReference(&transportMatrix);
            } else {
                info->m_transportGuid = WOWGUID();
            }
        }

        if (model) {
            float fade = (1.0f - elapsed * 0.0005f) * info->m_alpha;

            if (fade < 0.0f) {
                model->m_fadeAlpha = 0.0f;
            } else {
                float alpha = 1.0f;

                if (fade <= 1.0f) {
                    alpha = (3.0f - fade * 2.0f) * fade * fade;
                }

                model->m_fadeAlpha = alpha;
            }
        }

        object = next;
    }
}

// OFFSET: 0x005E04E0
void CGObject_C::PostShutdown() {
    if (CGObject_C::s_fadeoutHeap) {
        delete CGObject_C::s_fadeoutHeap;
        CGObject_C::s_fadeoutHeap = NULL;
    }
}

// OFFSET: 0x005E0500/0x14025DF40
void CGObject_C::ReplaceGuildTextures(CM2Model* model, const CImVector& backgroundColor, const CImVector& border, const CImVector& emblem, const char* fileName) {
    model->m_fFlags |= 0x8000000;
    model->m_guildColors[0] = backgroundColor;
    model->m_guildColors[1] = emblem;
    model->m_guildColors[2] = border;

    CStatus status;
    CTexture* texture = TextureCreate(fileName, CGxTexFlags(), &status, 0, NULL, 2);

    if (texture) {
        model->ReplaceTexture(18, texture);
        TextureRelease(texture);
    }
}

// OFFSET: 0x005E05A0
void CGObject_C::CreateFadeoutHeap() {
    if (CGObject_C::s_fadeoutHeap) {
        delete CGObject_C::s_fadeoutHeap;
    }

    void* memory = SMemAlloc(sizeof(TSObjectAllocPool<FadeoutInfo>), __FILE__, __LINE__, 0);
    CGObject_C::s_fadeoutHeap = memory ? new (memory) TSObjectAllocPool<FadeoutInfo>("ObjectFadeout", 32) : NULL;
}

// OFFSET: 0x005E0620/0x14025E110
void CGObject_C::Initialize() {
    CGObject_C::s_cvObjectSelectionCircle = CVar::Register("ObjectSelectionCircle", NULL, 0, "1", NULL, 0, false, NULL, false);

    if (CGObject_C::s_selectionTexture) {
        TextureRelease(CGObject_C::s_selectionTexture);
    }

    CStatus status;
    CGObject_C::s_selectionTexture = TextureCreate("Textures\\UnitSelectTexture.blp", CGxTexFlags(), &status, 1, NULL, 2);
    LoadInteractIcons(1);
    ClntObjMgrSetTypeMirrorHandler(ID_OBJECT, OBJECT_FIELD_SCALE_X * sizeof(uint32_t), sizeof(float), &ScaleUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_OBJECT, OBJECT_FIELD_TYPE * sizeof(uint32_t) + sizeof(uint16_t), sizeof(uint16_t), &GuildDataTypeUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_OBJECT, OBJECT_FIELD_DATA * sizeof(uint32_t), sizeof(uint32_t), &GuildDataUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    CGObject_C::CreateFadeoutHeap();
}

// OFFSET: 0x005E0710
void CGObject_C::ReplaceGuildTextures(CM2Model* model, uint64_t guildGUID, int32_t factionIndex) {
    if (!model || !model->IsLoaded() || !model->HasTexture(18, NULL)) {
        return;
    }

    CImVector backgroundColor(0);
    CImVector border(0);
    CImVector emblem(0);
    char defaultName[64];
    char fileName[64];

    if (CGObject_C::GetGuildLogoInfo(guildGUID, backgroundColor, border, emblem, fileName, sizeof(fileName))) {
        SStrPack(fileName, ".blp", sizeof(fileName));
    } else {
        CGObject_C::GetDefaultGuildTextureSettings(backgroundColor, border, emblem, defaultName, sizeof(defaultName), factionIndex);
        SStrCopy(fileName, defaultName, sizeof(fileName));
    }

    CGObject_C::ReplaceGuildTextures(model, backgroundColor, border, emblem, fileName);
}

// OFFSET: 0x005E0830/0x14025E500
bool CGObject_C::LoadGuildTextures(uint64_t guildGUID) {
    if (guildGUID) {
        WOWGUID requester = this->m_obj->m_guid;

        if (!g_guildInfoCache.GetRecord2(guildGUID, requester, &CGObject_C::GuildTextureCallback, NULL, false)) {
            return false;
        }
    }

    int32_t factionIndex = -1;

    if (this->m_obj->m_type & TYPEMASK_PLAYER) {
        factionIndex = static_cast<CGPlayer_C*>(this)->GetPVPFactionIndex();
    }

    CGObject_C::ReplaceGuildTextures(this->m_worldModel, guildGUID, factionIndex);
    return true;
}

// OFFSET: %0x005E8F80
void CGObject_C::SetStorage(uint32_t* storage, uint32_t* saved) {
    this->m_obj = reinterpret_cast<CGObjectData*>(storage);
    this->m_objMirror = saved;
}

// OFFSET: 0x0048F630
CGObject_C::CGObject_C() {
}

// PROVEN: 7adf9979 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CGObject_C::HandleOutOfRange(OUT_OF_RANGE_TYPE type) {
}

// PROVEN: de779f0b IDENTICAL/DIFFERENT
// OFFSET: 0x009FCE20/0x14090EBB0
int32_t CGObject_C::ShouldFadeout() {
    return 1;
}

// PROVEN: b5a67297 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CGObject_C::UpdateDisplayInfo(int32_t a2) {
}

// PROVEN: 3e538a10 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
const CGBag_C* CGObject_C::GetBag() const {
    return NULL;
}

// PROVEN: 420f1f3f IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
CGBag_C* CGObject_C::GetBag() {
    return NULL;
}

// PROVEN: 318a75ac IDENTICAL/NONE
// OFFSET: 0x0048F710/0x1400B3EC0
C3Vector CGObject_C::GetPosition() const {
    return C3Vector(0.0f, 0.0f, 0.0f);
}

// PROVEN: c6ae701e IDENTICAL/NONE
// OFFSET: 0x0048F730/0x1400B3ED0
C3Vector CGObject_C::GetRawPosition() const {
    return this->GetPosition();
}

// PROVEN: 79ed4052 IDENTICAL/IDENTICAL
// OFFSET: 0x0076CA20/0x140586E10
float CGObject_C::GetFacing() const {
    return 0.0f;
}

// PROVEN: fd340554 IDENTICAL/NONE
// OFFSET: 0x0048F800/0x1400B3F90
float CGObject_C::GetRawFacing() const {
    return this->GetFacing();
}

// PROVEN: 4971577f IDENTICAL/NONE
// OFFSET: 0x0048F750/0x1400B3EF0
float CGObject_C::GetScale() const {
    return this->m_obj->m_scale;
}

// PROVEN: cb76c4c3 IDENTICAL/NONE
// OFFSET: 0x0048F760
uint64_t CGObject_C::GetTransportGUID() const {
    return 0;
}

// OFFSET: 0x0048F770/0x1400B3F00
C4Quaternion CGObject_C::GetRotation() const {
    C4Quaternion rotation;
    rotation.x = 0.0f;
    rotation.y = 0.0f;
    rotation.z = 0.0f;
    rotation.w = 1.0f;
    rotation.FromAngleAxis(this->GetFacing(), C3Vector(0.0f, 0.0f, 1.0f));
    return rotation;
}

// PROVEN: ce1c9987 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
int32_t CGObject_C::IsQuestGiver() const {
    return 0;
}

// PROVEN: 5fa3d028 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGObject_C::RefreshInteractIcon() {
}

// PROVEN: a0248792 IDENTICAL/DIFFERENT
// OFFSET: 0x00A9AA30/0x1402123D0
int32_t CGObject_C::GetModelFileName(const char** name) const {
    return 0;
}

// PROVEN: a13063c0 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGObject_C::ScaleChangeUpdate() {
}

// PROVEN: 93da7744 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGObject_C::ScaleChangeFinished() {
}

// PROVEN: 67768697 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGObject_C::RenderTargetSelection() const {
}

// PROVEN: adf0f9a2 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGObject_C::RenderPetTargetSelection() const {
}

// PROVEN: 2932844c IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGObject_C::Render() {
}

// PROVEN: 75bd5403 IDENTICAL/NONE
// OFFSET: 0x0048F7D0/0x1400B3F60
int32_t CGObject_C::GetSelectionHighlightColor(CImVector* color) const {
    *reinterpret_cast<uint32_t*>(color) = 0xFFFFFFFF;
    return 1;
}

// PROVEN: 5baa8cd5 IDENTICAL/NONE
// OFFSET: 0x0048F7F0/0x140206E00
float CGObject_C::GetTrueScale() const {
    return this->m_scaleModifier * this->m_scale;
}

// PROVEN: 07784588 IDENTICAL/NONE
// OFFSET: 0x0048F800/0x1400B3F90
float CGObject_C::GetDisplayFacing() const {
    return this->GetFacing();
}

// PROVEN: 5c1e2d9f IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGObject_C::OnSpecialMountAnim() {
}

// PROVEN: bb2ea966 IDENTICAL/DIFFERENT
// OFFSET: 0x009FCE20/0x14090EBB0
int32_t CGObject_C::IsSolidSelectable() const {
    return 1;
}

// PROVEN: 4bc73cc2 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
int32_t CGObject_C::CanHighlight() const {
    return 0;
}

// PROVEN: 9a25a0a5 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
int32_t CGObject_C::CanBeTargetted() const {
    return 0;
}

// PROVEN: 2a12800f IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
int32_t CGObject_C::FloatingTooltip() const {
    return 0;
}

// PROVEN: 6d451c6e IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGObject_C::OnRightClick() {
}

// PROVEN: 569ddc38 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
int32_t CGObject_C::IsHighlightSuppressed() const {
    return 0;
}

// PROVEN: 3c5e4b0f IDENTICAL/NONE
// OFFSET: 0x0048F810
C44Matrix CGObject_C::GetMatrix() const {
    return C44Matrix();
}

// PROVEN: 30ff07df IDENTICAL/NONE
// OFFSET: 0x0048F880/0x1400B3FF0
CM2Model* CGObject_C::GetObjectModel() const {
    return this->m_worldModel;
}

// PROVEN: df8f4393 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
const char* CGObject_C::GetObjectName() const {
    return NULL;
}

// PROVEN: 1b63d1ec IDENTICAL/DIFFERENT
// OFFSET: 0x00A9AA30/0x1402123D0
int32_t CGObject_C::GetPageTextID(void (*callback)(int32_t, const WOWGUID&, void*, bool)) const {
    return 0;
}

// PROVEN: 98c12193 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGObject_C::CleanUpVehicleBoneAnimsBeforeObjectModelChange() {
}

// PROVEN: 4c24fcbb IDENTICAL/DIFFERENT
// OFFSET: 0x009FCE20/0x14090EBB0
int32_t CGObject_C::ShouldFadeIn() const {
    return 1;
}

// PROVEN: fae4e40c IDENTICAL/IDENTICAL
// OFFSET: 0x00599E10/0x1402082B0
float CGObject_C::GetBaseAlpha() const {
    return 1.0f;
}

// PROVEN: d99ff856 IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
bool CGObject_C::IsTransport() const {
    return false;
}

// PROVEN: 8fef785f IDENTICAL/DIFFERENT
// OFFSET: 0x00A9AA30/0x1402123D0
int32_t CGObject_C::IsPointInside(const C3Vector& point) const {
    return 0;
}

// PROVEN: 9f089fd9 IDENTICAL/NONE
// OFFSET: 0x00873190/0x14025E5A0
int32_t CGObject_C::AddPassenger(CPassenger* passenger, int32_t a2) {
    return 1;
}

// PROVEN: 9716fafd IDENTICAL/NONE
// OFFSET: 0x00599E20/0x14025E5A0
float CGObject_C::GetSpeed() const {
    return -1.0f;
}

// PROVEN: dd945a15 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGObject_C::OnInteract() {
}

// PROVEN: 8945cde1 IDENTICAL/IDENTICAL
// OFFSET: 0x0076CA20/0x140586E10
float CGObject_C::AddSquelchedEffect() {
    return 0.0f;
}

// PROVEN: 2c68a582 IDENTICAL/NONE
// OFFSET: 0x00847950
void CGObject_C::PlaySpellVisualKit_PlayAnims(const PlaySpellVisualKitAnimData* data) {
}

// PROVEN: 1e09d327 IDENTICAL/NONE
// OFFSET: 0x006279A0
void CGObject_C::PlaySpellVisualKit_HandleWeapons(const SpellRec* spell, const SpellVisualKitRec* kit, const C3Vector* position, WOWGUID guid, uint32_t a5, uint32_t flags, void (*callback)(CM2Model*, uint32_t, uint32_t, int32_t, uint32_t, uint64_t, int32_t), int32_t* hasEffect, uint32_t a9, int32_t a10) {
}
