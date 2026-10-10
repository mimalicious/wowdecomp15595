#include "Corpse_C.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureModelDataRec.h"
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/SpellVisualEffectNameRec.h"
#include "WoW/Source/Glue/ComponentData.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/CClientObjCreate.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/CGTabardCreationFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/UIUtil/PortraitButton.h"
#include "WoW/Source/WorldClient/World.h"

struct CorpseGuildRequest {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint32_t m_flags;
};

// OFFSET: 0x00597930
void CGCorpse_C::SetStorage(uint32_t* storage, uint32_t* saved) {
    this->CGObject_C::SetStorage(storage, saved);
    this->m_corpse = reinterpret_cast<CGCorpseData*>(storage + OBJECT_END);
    this->m_corpseMirror = saved + CGObject::TotalFieldsSaved();
}

// OFFSET: 0x00597970
void CGCorpse_C::PostReenable(const CClientObjCreate& create) {
    this->m_passenger.PostInit(create.m_stationaryPosition);
    CGObject_C::PostReenable(create);
}

// OFFSET: 0x005979A0/0x140205830
int32_t CGCorpse_C::GetModelFileName(const char** name) const {
    static char s_modelName[260];

    if (this->m_corpse->m_corpseFieldFlags & 0x1) {
        ChrRacesRec* race = g_chrRacesDB.GetRecord(this->m_corpse->m_corpseFieldBytes1[1]);

        if (!race) {
            return 0;
        }

        SStrPrintf(s_modelName, sizeof(s_modelName), "World\\Generic\\PassiveDoodads\\DeathSkeletons\\%s%sDeathSkeleton.mdx", static_cast<const char*>(race->m_clientFileString), s_sexNames[this->m_corpse->m_corpseFieldBytes1[2]]);
        *name = s_modelName;
        return 1;
    }

    uint32_t displayId = this->m_corpse->m_corpseFieldDisplayId;
    CreatureDisplayInfoRec* display = g_creatureDisplayInfoDB.GetRecord(displayId);

    if (!display) {
        SysMsgPrintf(STATUS_WARNING, 16, "INVALIDPLAYERDISPLAYID|%d|%d|%d", displayId, this->m_corpse->m_corpseFieldBytes1[1], this->m_corpse->m_corpseFieldBytes1[2]);
        return 0;
    }

    CreatureModelDataRec* model = g_creatureModelDataDB.GetRecord(display->m_modelID);

    if (!model) {
        SysMsgPrintf(STATUS_WARNING, 16, "INVALIDPLAYERMODELRECORD|%d|%d|%d", display->m_modelID, this->m_corpse->m_corpseFieldBytes1[1], this->m_corpse->m_corpseFieldBytes1[2]);
        return 0;
    }

    *name = model->m_modelName;
    return *name != NULL;
}

// OFFSET: 0x00597AD0
const char* CGCorpse_C::GetPortraitTexture() const {
    uint32_t displayId = this->m_corpse->m_corpseFieldDisplayId;

    if (!displayId) {
        return NULL;
    }

    CreatureDisplayInfoRec* rec = g_creatureDisplayInfoDB.GetRecord(displayId);

    return rec ? static_cast<const char*>(rec->m_portraitTextureName) : NULL;
}

// OFFSET: 0x00597B10
void CGCorpse_C::ShouldRender(uint32_t flags, int32_t& render, int32_t& a3) {
    CGObject_C::ShouldRender(flags, render, a3);

    if (render || a3) {
        if (this->m_component) {
            this->m_component->Prep();
        }
    } else if (((this->m_corpseFlags & 0x2) && (this->m_corpse->m_corpseFieldFlags & 0x1)) || (this->m_component && !this->m_component->RenderPrep(0))) {
        a3 = 1;
    }
}

// OFFSET: 0x00597B80
int32_t CGCorpse_C::ShouldFadeout() {
    return !this->m_component || (TextureIsReadyNow(this->m_component->m_baseTexture) && this->m_component->RenderPrep(0));
}

// OFFSET: 0x00597BC0
int32_t CGCorpse_C::CanBeLooted() const {
    return this->m_corpse->m_corpseFieldDynamicFlags & 0x1;
}

// OFFSET: 0x00597BD0
void CGCorpse_C::ShowLootEffect() {
    if (this->m_lootEffect) {
        return;
    }

    const SpellVisualEffectNameRec* effect = GetSpecialSpellVisualEffectNameRec(4);
    this->m_lootEffect = this->m_worldModel->m_scene->CreateModel(effect->m_fileName, 0, 3);

    if (this->m_lootEffect) {
        this->m_lootEffect->AttachToParent(this->m_worldModel, 19, NULL, 0);
    }
}

// OFFSET: 0x00597C20
static void GuildTabardCallback(const uint64_t& key, const WOWGUID& guid, void* param, bool success) {
    CorpseGuildRequest* request = static_cast<CorpseGuildRequest*>(param);
    CGCorpse_C* corpse = static_cast<CGCorpse_C*>(ClntObjMgrObjectPtr(request->m_guid, TYPEMASK_CORPSE, __FILE__, __LINE__));

    if (corpse) {
        corpse->ApplyGuildColor(request->m_flags);
    }

    delete request;
}

// OFFSET: 0x00597C70
static void ShirtGuildTabardCallback(const uint64_t& key, const WOWGUID& guid, void* param, bool success) {
    CorpseGuildRequest* request = static_cast<CorpseGuildRequest*>(param);
    CGCorpse_C* corpse = static_cast<CGCorpse_C*>(ClntObjMgrObjectPtr(request->m_guid, TYPEMASK_CORPSE, __FILE__, __LINE__));

    if (corpse) {
        corpse->ApplyShirtGuildColor(request->m_flags);
    }

    delete request;
}

// OFFSET: 0x00597CC0
void CGCorpse_C::ApplyGuildColor(uint32_t flags) {
    if (!this->m_component) {
        return;
    }

    int32_t emblemStyle;
    int32_t emblemColor;
    int32_t borderStyle;
    int32_t borderColor;
    int32_t background;
    WOWGUID guild = this->m_obj->m_dataType == 1 ? this->m_obj->m_data : WOWGUID::Make(0);

    if (GuildGetGuildTabard(this->m_corpse->m_corpseFieldOwner, guild, NULL, NULL, emblemStyle, emblemColor, borderStyle, borderColor, background)) {
        this->m_component->ApplyGuildColor(flags, emblemStyle, emblemColor, borderStyle, borderColor, background);
    } else if (flags) {
        CorpseGuildRequest* request = static_cast<CorpseGuildRequest*>(SMemAlloc(sizeof(CorpseGuildRequest), __FILE__, __LINE__, 0));
        request->m_guid = this->m_obj->m_guid;
        guild = this->m_obj->m_dataType == 1 ? this->m_obj->m_data : WOWGUID::Make(0);
        GuildGetGuildTabard(this->m_corpse->m_corpseFieldOwner, guild, &GuildTabardCallback, request, emblemStyle, emblemColor, borderStyle, borderColor, background);
    }
}

// OFFSET: 0x00597DD0
void CGCorpse_C::ApplyShirtGuildColor(uint32_t flags) {
    if (!this->m_component) {
        return;
    }

    int32_t emblemStyle;
    int32_t emblemColor;
    int32_t borderStyle;
    int32_t borderColor;
    int32_t background;
    WOWGUID guild = this->m_obj->m_dataType == 1 ? this->m_obj->m_data : WOWGUID::Make(0);

    if (GuildGetGuildTabard(this->m_corpse->m_corpseFieldOwner, guild, NULL, NULL, emblemStyle, emblemColor, borderStyle, borderColor, background)) {
        this->m_component->ApplyShirtGuildColor(flags, emblemStyle, emblemColor, borderStyle, borderColor, background);
    } else if (flags) {
        CorpseGuildRequest* request = static_cast<CorpseGuildRequest*>(SMemAlloc(sizeof(CorpseGuildRequest), __FILE__, __LINE__, 0));
        request->m_guid = this->m_obj->m_guid;
        guild = this->m_obj->m_dataType == 1 ? this->m_obj->m_data : WOWGUID::Make(0);
        GuildGetGuildTabard(this->m_corpse->m_corpseFieldOwner, guild, &ShirtGuildTabardCallback, request, emblemStyle, emblemColor, borderStyle, borderColor, background);
    }
}

// OFFSET: 0x00597EE0/0x140205F10
uint32_t CGCorpse_C::GetFactionTemplate() const {
    ChrRacesRec* rec = g_chrRacesDB.GetRecord(this->m_corpse->m_corpseFieldBytes1[1]);

    return rec ? rec->m_factionID : 0;
}

// OFFSET: 0x00597F20
float CGCorpse_C::GetDisplayScale() const {
    CreatureDisplayInfoRec* displayRec = g_creatureDisplayInfoDB.GetRecord(this->m_corpse->m_corpseFieldDisplayId);

    if (displayRec) {
        CreatureModelDataRec* modelRec = g_creatureModelDataDB.GetRecord(displayRec->m_modelID);

        if (modelRec) {
            return modelRec->m_modelScale * displayRec->m_creatureModelScale;
        }
    }

    return 1.0f;
}

// OFFSET: 0x00597F80
void CGCorpse_C::SetDeathSequence(CM2Model* model, int32_t underwater) {
    uint32_t sequence = underwater ? 132 : 6;
    model->SetBoneSequence(0xFFFFFFFF, sequence, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);

    if (!this->m_component) {
        return;
    }

    uint32_t ids[6];
    uint32_t count = this->m_component->GetHandItemAttachIDs(ids, 6);

    for (uint32_t i = 0; i < count; i++) {
        for (CM2Model* attached = model->m_attachList; attached; attached = attached->m_attachNext) {
            if (attached->m_attachmentId == ids[i]) {
                attached->SetBoneSequence(0xFFFFFFFF, sequence, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
                break;
            }
        }
    }
}

// OFFSET: 0x00598030
void CGCorpse_C::PostInit(uint32_t time, const CClientObjCreate& create, bool reenable) {
    this->m_passenger.PostInit(create.m_stationaryPosition);

    if (!this->GetTransportGUID()) {
        this->m_passenger.m_facing = create.m_stationaryFacing;
    }

    this->CGObject_C::PostInit(time, create, reenable);

    CM2Model* model = this->m_worldModel;
    int32_t useComponent = 1;
    CreatureDisplayInfoRec* displayRec = g_creatureDisplayInfoDB.GetRecord(this->m_corpse->m_corpseFieldDisplayId);
    CreatureModelDataRec* modelRec = NULL;

    if (displayRec) {
        modelRec = g_creatureModelDataDB.GetRecord(displayRec->m_modelID);

        if (modelRec && !(modelRec->m_flags & 0x4)) {
            useComponent = 0;
        }
    }

    if (!(this->m_corpse->m_corpseFieldFlags & 0x1)) {
        if (useComponent) {
            this->m_component = CCharacterComponent::AllocComponent();

            ComponentData data;
            data.m_preferences.m_raceID = this->m_corpse->m_corpseFieldBytes1[1];
            data.m_preferences.m_sexID = this->m_corpse->m_corpseFieldBytes1[2];
            data.m_preferences.m_skinID = this->m_corpse->m_corpseFieldBytes1[3];
            data.m_preferences.m_faceId = this->m_corpse->m_corpseFieldBytes2[0];
            data.m_preferences.m_hairStyleId = this->m_corpse->m_corpseFieldBytes2[1];
            data.m_preferences.m_hairColorId = this->m_corpse->m_corpseFieldBytes2[2];
            data.m_preferences.m_facialHairId = this->m_corpse->m_corpseFieldBytes2[3];
            data.m_model = model;

            uint32_t activePlayer = this->m_corpse->m_corpseFieldOwner == ClntObjMgrGetActivePlayer();
            data.m_flags ^= (data.m_flags ^ (activePlayer << 1)) & 0x2;
            CCharacterComponent::ValidateComponentData(&data, static_cast<COMPONENT_CONTEXT>(1));
            model->m_refCount++;
            this->m_component->Init(&data, NULL);
        } else {
            CCharacterComponent::ReplaceMonsterSkin(model, displayRec, modelRec);
        }
    }

    if (this->m_component) {
        for (uint32_t slot = 0; slot < 19; slot++) {
            ItemDisplayInfoRec rec;

            if (!g_itemDisplayInfoDB.GetRecord(this->m_corpse->m_corpseFieldItem[slot] & 0xFFFFFF, &rec) || slot == 17) {
                continue;
            }

            if (slot == 15 || slot == 16) {
                CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(this->m_corpse->m_corpseFieldItem[slot]), TYPEMASK_ITEM, __FILE__, __LINE__));

                if (item) {
                    bool shield = item->GetInventoryType() == 14;
                    this->m_component->AddHandItem(&rec, static_cast<INVENTORY_SLOTS>(slot), static_cast<SHEATHE_TYPE>(item->GetSheatheType()), false, shield, false, 0);
                }

                continue;
            }

            if (slot == 0) {
                if (this->m_corpse->m_corpseFieldFlags & 0x8) {
                    continue;
                }
            } else if (slot == 14 && (this->m_corpse->m_corpseFieldFlags & 0x10)) {
                continue;
            }

            this->m_component->AddItemBySlot(static_cast<INVENTORY_SLOTS>(slot), rec.m_id, 0);

            if (slot == 18) {
                if (rec.m_flags & 0x19) {
                    this->ApplyGuildColor(rec.m_flags & 0x19);
                }
            } else if (slot == 3 && (rec.m_flags & 0x19)) {
                this->ApplyShirtGuildColor(rec.m_flags & 0x19);
            }
        }
    }

    uintptr_t worldObject = this->m_worldObject;
    C3Vector position = this->GetPosition();
    uint32_t liquidType;
    float height;
    int32_t liquidFlags;

    if (World::QueryObjectLiquid(worldObject, liquidType, height, liquidFlags) && height - position.z > 0.6666667f) {
        this->m_corpseFlags |= 0x2;
        this->SetDeathSequence(model, 1);
    } else {
        this->m_corpseFlags &= ~0x2;
        this->SetDeathSequence(model, 0);
    }

    if ((this->m_corpse->m_corpseFieldDynamicFlags & 0x1) && !this->m_lootEffect) {
        const SpellVisualEffectNameRec* effect = GetSpecialSpellVisualEffectNameRec(4);
        this->m_lootEffect = this->m_worldModel->m_scene->CreateModel(effect->m_fileName, 0, 3);

        if (this->m_lootEffect) {
            this->m_lootEffect->AttachToParent(this->m_worldModel, 19, NULL, 0);
        }
    }

    if (!(this->m_corpse->m_corpseFieldFlags & 0x1) && this->m_corpse->m_corpseFieldOwner == ClntObjMgrGetActivePlayer()) {
        CGGameUI::SetActiveCorpse(this->m_obj->m_guid);
    }
}

// OFFSET: 0x00598410
void CGCorpse_C::Disable() {
    CGObject_C::Disable();
    this->m_passenger.m_link.Unlink();

    if (!(this->m_corpse->m_corpseFieldFlags & 0x1) && this->m_corpse->m_corpseFieldOwner == ClntObjMgrGetActivePlayer()) {
        CGGameUI::SetActiveCorpse(WOWGUID());
    }

    if (this->m_lootEffect) {
        if (this->m_lootEffect->m_attachParent) {
            this->m_lootEffect->DetachFromParent();
        }

        this->m_lootEffect->Release();
        this->m_lootEffect = NULL;
    }
}

// OFFSET: 0x00598480
void CGCorpse_C::Reenable(const CClientObjCreate& create) {
    if (create.m_hasGOTransport) {
        this->m_passenger.Reenable(create.m_goTransport.m_guid, create.m_goTransport.m_position);
    } else {
        this->m_passenger.Reenable(WOWGUID::Make(0), create.m_stationaryPosition);
    }

    CGObject_C::Reenable(create);
    this->m_corpseFlags &= ~0x1;

    if (this->m_corpse->m_corpseFieldDynamicFlags & 0x1) {
        this->ShowLootEffect();
    }

    if (!(this->m_corpse->m_corpseFieldFlags & 0x1) && this->m_corpse->m_corpseFieldOwner == ClntObjMgrGetActivePlayer()) {
        CGGameUI::SetActiveCorpse(this->m_obj->m_guid);
    }
}

// OFFSET: 0x00598520
void CGCorpse_C::ModelLoaded(CM2Model* model) {
    this->CGObject_C::ModelLoaded(model);
    this->SetDeathSequence(model, this->m_corpseFlags & 0x2);
    this->m_selectionRadius = model->GetBoundingSphere().r * 0.5f;
}

// OFFSET: 0x00598570
void CGCorpse_C::OnRightClick() {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return;
    }

    if ((player->m_mountDisplayId <= 0 || (player->m_clientFlags & 0x10000000)) && (this->m_corpse->m_corpseFieldDynamicFlags & 0x1)) {
        if (player->m_obj->m_guid != CGUnit_C::s_activeMover) {
            return;
        }

        if (player->GetClientStandState()) {
            if (!player->CanAutoStand()) {
                CGGameUI::DisplayError(ERR_LOOT_NOTSTANDING);
                return;
            }

            player->ChangeStandState(0);
        }

        player->SetAutoLoot(CGGameUI::IsAutoLooting());
        player->LootCorpse(this, player->CanAutoInteract());
    } else if ((this->m_corpse->m_corpseFieldFlags & 0x20) && CGSpellBook::GetSkinningSpell(this) && !player->CanAssist(this)) {
        player->HandleSkinning(this, player->CanAutoInteract(), true);
    }
}

// OFFSET: 0x00598680
void CGCorpse_C::OnFlagChanged(uint32_t oldFlags) {
    if (this->m_lootEffect) {
        if (this->m_lootEffect->m_attachParent) {
            this->m_lootEffect->DetachFromParent();
        }

        this->m_lootEffect->Release();
        this->m_lootEffect = NULL;
    }

    this->PrepareToFade(static_cast<float>(this->m_alpha) * 0.0039215689f, this->GetTransportGUID());
    this->FadeoutWorldObject();
    this->m_worldObject = 0;

    if (this->m_component) {
        CCharacterComponent::FreeComponent(this->m_component);
        this->m_component = NULL;
    }

    this->m_corpseFlags &= ~0x1;
    const char* fileName;

    if (!this->GetModelFileName(&fileName)) {
        return;
    }

    CM2Model* model = World::GetScene()->CreateModel(fileName, 0, 3);
    this->SetModel(model);

    if (this->m_corpse->m_corpseFieldDynamicFlags & 0x1) {
        this->ShowLootEffect();
    }

    if (model) {
        model->Release();
    }

    this->AddWorldObject();
}

// OFFSET: 0x00598770
void CGCorpse_C::OnDynamicFlagChanged(uint32_t oldFlags) {
    if (!((static_cast<uint8_t>(oldFlags) ^ this->m_corpse->m_corpseFieldDynamicFlags) & 0x1)) {
        return;
    }

    if (this->m_corpse->m_corpseFieldDynamicFlags & 0x1) {
        this->ShowLootEffect();
        return;
    }

    if (this->m_lootEffect) {
        if (this->m_lootEffect->m_attachParent) {
            this->m_lootEffect->DetachFromParent();
        }

        this->m_lootEffect->Release();
        this->m_lootEffect = NULL;
    }
}

// PROVEN: d336bac1 IDENTICAL/NONE
// OFFSET: 0x005987D0/0x140206980
int32_t CorpseFlagUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGCorpse_C* corpse = static_cast<CGCorpse_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_CORPSE, __FILE__, __LINE__));

    if (corpse) {
        corpse->OnFlagChanged(*static_cast<const uint32_t*>(oldValue));
    }

    return 1;
}

// PROVEN: a2431141 IDENTICAL/NONE
// OFFSET: 0x00598810/0x1402069C0
int32_t CorpseDynamicFlagUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGCorpse_C* corpse = static_cast<CGCorpse_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_CORPSE, __FILE__, __LINE__));

    if (corpse) {
        corpse->OnDynamicFlagChanged(*static_cast<const uint32_t*>(oldValue));
    }

    return 1;
}

// OFFSET: 0x00598850/0x140206A00
void CGCorpse_C::Initialize() {
    ClntObjMgrSetTypeMirrorHandler(ID_CORPSE, offsetof(CGCorpseData, m_corpseFieldFlags), sizeof(uint32_t), CorpseFlagUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_CORPSE, offsetof(CGCorpseData, m_corpseFieldDynamicFlags), sizeof(uint32_t), CorpseDynamicFlagUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
}

// OFFSET: 0x00598880
void CGCorpse_C::OnInteract() {
    this->OnRightClick();
}

// OFFSET: 0x00598890
void CGCorpse_C::RenderTargetSelection() const {
    g_theGxDevicePtr->RsPush();

    if (CGObject_C::SetCircleRenderStates()) {
        C3Vector position = const_cast<CGCorpse_C*>(this)->GetModelPosition();
        float size = this->GetTrueScale() * this->m_selectionRadius;
        float height = 2.0f;

        if (size > 10.0f) {
            size = 10.0f;
        } else if (size <= 2.0f) {
            height = size;

            if (size <= 1.5f) {
                height = 1.5f;
            }
        }

        CAaBox box;
        box.b.x = position.x - size;
        box.b.y = position.y - size;
        box.b.z = position.z - height;
        box.t.x = position.x + size;
        box.t.y = position.y + size;
        box.t.z = position.z + height;
        C44Matrix matrix;
        C3Vector center = this->GetPosition();
        CGObject_C::BuildSelectionRotMatrix(matrix, center);
        World::ProjectTex2d(box, s_deadSelectionColor, &matrix, 0.5f, 0x200122, 9, GxPolygonOffset_Default);
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x00598A30
CGCorpse_C::CGCorpse_C(uint32_t time, const CClientObjCreate& create) : CGObject_C(time, create), m_passenger(create.m_hasGOTransport ? create.m_goTransport.m_guid : WOWGUID::Make(0), create.m_hasGOTransport ? create.m_goTransport.m_position : create.m_stationaryPosition, this->m_obj->m_guid), m_component(NULL), m_groundNormal(0.0f, 0.0f, 1.0f), m_corpseFlags(0), m_selectionRadius(0.0f), m_lootEffect(NULL) {
    this->m_passenger.m_facing = create.m_hasGOTransport ? create.m_goTransport.m_facing : create.m_stationaryFacing;
}

// OFFSET: 0x00598B80
float CGCorpse_C::GetTrueScale() const {
    return this->GetDisplayScale() * this->m_scaleModifier * this->m_scale;
}

// OFFSET: 0x00598BA0
int32_t CGCorpse_C::CanHighlight() const {
    return !(this->m_corpse->m_corpseFieldFlags & 0x1) || (this->m_corpse->m_corpseFieldDynamicFlags & 0x1);
}

// OFFSET: 0x00598BC0/0x140205640
CGCorpse_C::~CGCorpse_C() {
    if (this->m_component) {
        CCharacterComponent::FreeComponent(this->m_component);
    }

    if (this->m_lootEffect) {
        if (this->m_lootEffect->m_attachParent) {
            this->m_lootEffect->DetachFromParent();
        }

        this->m_lootEffect->Release();
        this->m_lootEffect = NULL;
    }
}

// OFFSET: 0x00598CC0
C3Vector CGCorpse_C::GetGroundNormal() {
    if (!(this->m_corpseFlags & 0x1) && this->GetObjectModel() && this->GetObjectModel()->IsLoaded()) {
        static World::FacetData s_facets;
        CAaBox box = this->GetObjectModel()->GetBoundingBox();
        float scale = this->GetScale();
        float height = (box.t.z - box.b.z) * scale;
        box *= scale;
        box += this->GetPosition();
        box.b.z -= height * 0.5f;

        if (World::GetFacets(box, s_facets, 0x100111) && s_facets.facets.Count()) {
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
            uint32_t count = 0;

            for (uint32_t i = 0; i < s_facets.facets.Count(); i++) {
                const C3Vector& normal = s_facets.facets[i].plane.n;

                if (normal.z > 0.0f) {
                    z += normal.z;
                    x += normal.x;
                    y += normal.y;
                    count++;
                }
            }

            if (count) {
                float inverse = 1.0f / static_cast<float>(count);
                this->m_groundNormal = C3Vector(inverse * x, inverse * y, inverse * z);
            }
        }

        this->m_corpseFlags |= 0x1;
    }

    return this->m_groundNormal;
}

// OFFSET: 0x00598F40
int32_t CGCorpse_C::Animate(float elapsed) {
    for (CEffect* effect = this->m_effectList; effect; ) {
        CEffect* next = effect->m_next;
        effect->EffectUpdate();
        effect = next;
    }

    CM2Model* model = this->GetObjectModel();

    if (model) {
        C3Vector up = this->GetGroundNormal();
        model->SetWorldTransform(this->GetPosition(), this->GetDisplayFacing(), this->GetTrueScale(), up, 0xFFFFFFFF);
    }

    return 1;
}

// OFFSET: 0x005977E0/0x140206DB0
C3Vector CGCorpse_C::GetPosition() const {
    return this->m_passenger.GetPosition(this->m_passenger.m_position);
}

// OFFSET: 0x00597800/0x140205600
float CGCorpse_C::GetFacing() const {
    return this->m_passenger.GetFacing(this->m_passenger.m_facing);
}

// OFFSET: 0x00597820/0x140205620
float CGCorpse_C::GetRawFacing() const {
    return this->m_passenger.m_facing;
}

// OFFSET: 0x00598B40/0x140206DE0
C3Vector CGCorpse_C::GetRawPosition() const {
    return this->m_passenger.m_position;
}

// PROVEN: c85850d8 IDENTICAL/NONE
// OFFSET: 0x00598B70/0x1402142E0
uint64_t CGCorpse_C::GetTransportGUID() const {
    return this->m_passenger.m_transportGuid.m_guid;
}
