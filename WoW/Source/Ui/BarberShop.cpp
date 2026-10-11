#include "BarberShop.h"
#include "WoW/Source/Ui/CharacterModelBase.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Common/ComponentCore/Types.h"
#include "WoW/Source/DB/Rec/BarberShopStyleRec.h"
#include "WoW/Source/Object/ObjectClient/VisibleItem_C.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/Net/Types.h"

// STATIC: 0x00F33728
uint8_t CGBarberShop::m_barberShopEnabled;

// STATIC: #0x00BD19B9
uint8_t CGBarberShop::m_helmHidden;

// STATIC: 0x00D108A0
int32_t CGBarberShop::m_prevView = 4;

// STATIC: 0x00D108A4
float CGBarberShop::m_minCameraDistance = 1.0f;

// STATIC: 0x00F33729
uint8_t CGBarberShop::m_initialized;

// STATIC: 0x00F3372A
uint8_t CGBarberShop::m_resetPortrait;

// STATIC: 0x00F33718
STYLEREC* CGBarberShop::m_currentHairStyle;

// STATIC: 0x00F3371C
STYLEREC* CGBarberShop::m_currentFacialHair;

// STATIC: 0x00F33720
STYLEREC* CGBarberShop::m_currentSkinColor;

// STATIC: 0x00F3372C
TSHashTable<USEABLESTYLE, HASHKEY_NONE> CGBarberShop::m_useableStyles;

// STATIC: 0x00F33724
CGCharacterModelBase* CGBarberShop::m_alternateFormFrame;

// OFFSET: 0x008FF5C0
float CGBarberShop::GetMinCameraDistance() {
    return CGBarberShop::m_minCameraDistance;
}

// PROVEN: 7e49b057 IDENTICAL/NONE
// OFFSET: 0x008FF5D0/0x140657050
const BarberShopStyleRec* CGBarberShop::GetCurrentStyle(int32_t type) {
    switch (type) {
        case 0:
            return CGBarberShop::m_currentHairStyle->m_rec;
        case 2:
            return CGBarberShop::m_currentFacialHair->m_rec;
        case 3:
            return CGBarberShop::m_currentSkinColor->m_rec;
    }

    return NULL;
}

// OFFSET: 0x008FF610
void CGBarberShop::AppearanceApplied() {
    Script_SendUniqueSignal(SCRIPT_EVENT_BARBER_SHOP_APPEARANCE_APPLIED);
    CGBarberShop::m_resetPortrait = 1;
}

// OFFSET: 0x008FF6B0
int32_t Packet_SMSG_BARBER_SHOP_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t result = 0xFFFFFFFF;
    msg->Get(result);

    switch (result) {
        case 0:
            FrameScript_SignalEvent(SCRIPT_EVENT_BARBER_SHOP_SUCCESS, NULL);
            break;
        case 1:
        case 3:
            CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
            break;
        case 2:
            CGGameUI::DisplayError(ERR_NOT_BARBER_SITTING);
            break;
        default:
            break;
    }

    return 1;
}

// OFFSET: 0x008FF730/0x1406571E0
void CGBarberShop::UpdateAlternateForm() {
    if (!CGBarberShop::m_alternateFormFrame) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->m_characterComponent) {
        return;
    }

    CCharacterComponent* altered = CGBarberShop::m_alternateFormFrame->m_component;

    if (altered) {
        player->m_characterComponent->CopyChoicesToAlteredFormComponent(altered, COMPONENT_CONTEXT_1);
        altered->RenderPrep(1);
        altered->m_data.m_model->WaitUntilDrawableRecursive();
    }
}

// OFFSET: 0x008FF860/0x1406573A0
bool CGBarberShop::IsCurrentStyle(int32_t type) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CCharacterComponent* component = player->m_characterComponent;

    if (!component) {
        return true;
    }

    switch (type) {
        case 0:
            return player->m_player->m_playerBytes[2] == CGBarberShop::m_currentHairStyle->m_rec->m_data;
        case 1:
            return player->m_player->m_playerBytes[3] == component->m_data.m_preferences.m_hairColorId;
        case 2:
            return player->m_player->m_playerBytes2[0] == CGBarberShop::m_currentFacialHair->m_rec->m_data;
        case 3:
            return player->m_player->m_playerBytes[0] == component->m_data.m_preferences.m_skinID;
        default:
            return false;
    }
}

// OFFSET: 0x008FFB90
bool CGBarberShop::IsSlotHidden(int32_t slot) {
    switch (slot) {
        case INVSLOT_HEAD:
        case INVSLOT_SHOULDER:
            return true;
        case INVSLOT_OFFHAND: {
            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (player) {
                VisibleItem_C* item = const_cast<VisibleItem_C*>(player->GetVisibleItem(INVSLOT_OFFHAND));

                if (item->GetDisplayID() && item->GetInventoryType() == 14) {
                    return true;
                }
            }

            break;
        }
    }

    return false;
}

// OFFSET: 0x008FFC00/0x1406577F0
void CGBarberShop::ResetAppearance() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CCharacterComponent* component = player->m_characterComponent;

    if (!component) {
        return;
    }

    component->SetHairStyle(player->m_player->m_playerBytes[2], NULL);
    component->SetHairColor(player->m_player->m_playerBytes[3], true, NULL);
    component->SetBeardStyle(player->m_player->m_playerBytes2[0], true, NULL);
    component->SetSkinColor(player->m_player->m_playerBytes[0], true, true, NULL);
    component->m_data.m_model->WaitUntilDrawableRecursive();
    CGBarberShop::UpdateAlternateForm();
}

// OFFSET: 0x008FFCA0/0x1406578C0
void CGBarberShop::SetAlternateForm() {
    CGBarberShop::m_alternateFormFrame->SetBarberShopAlternateForm();
    CCharacterComponent* altered = CGBarberShop::m_alternateFormFrame->m_component;

    if (altered) {
        altered->RemoveItemBySlot(static_cast<INVENTORY_SLOTS>(2));
        altered->RemoveItemBySlot(static_cast<INVENTORY_SLOTS>(0));
        altered->AddItemBySlot(static_cast<INVENTORY_SLOTS>(18), 54559, 0);
        CGBarberShop::UpdateAlternateForm();
    }
}

// OFFSET: 0x008FFCF0/0x140657920
int32_t CGBarberShop::GetTotalCost() {
    if (!CGBarberShop::m_barberShopEnabled) {
        return 0;
    }

    int32_t cost = 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player->m_characterComponent) {
        return 0;
    }

    float base = GameFormulaEvaluateTable(0, player->m_unit->m_unitFieldLevel - 1, 0);
    int32_t baseCost = static_cast<int32_t>(base);

    if (!CGBarberShop::IsCurrentStyle(0)) {
        float value = baseCost * CGBarberShop::m_currentHairStyle->m_rec->m_costModifier;
        cost = static_cast<int32_t>(value);
    } else if (!CGBarberShop::IsCurrentStyle(1)) {
        float value = baseCost * 0.5f;
        cost = static_cast<int32_t>(value);
    }

    if (!CGBarberShop::IsCurrentStyle(2)) {
        float value = baseCost * CGBarberShop::m_currentFacialHair->m_rec->m_costModifier;
        cost += static_cast<int32_t>(value);
    }

    if (CGBarberShop::m_currentSkinColor && !CGBarberShop::IsCurrentStyle(3)) {
        float value = baseCost * CGBarberShop::m_currentSkinColor->m_rec->m_costModifier;
        cost += static_cast<int32_t>(value);
    }

    return cost;
}

// OFFSET: 0x008FFF80/0x140657C00
void CGBarberShop::DisableBarberShop() {
    if (!CGBarberShop::m_barberShopEnabled) {
        return;
    }

    CGBarberShop::m_barberShopEnabled = 0;
    FrameScript_SignalEvent(SCRIPT_EVENT_BARBER_SHOP_CLOSE, NULL);
    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (camera) {
        camera->SetView(CGBarberShop::m_prevView, 1, 0);
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    player->ChangeStandState(0);
    CCharacterComponent* component = player->m_characterComponent;

    if (!component || player->m_unit->m_unitFieldDisplayid != player->m_unit->m_unitFieldNativedisplayid) {
        return;
    }

    CGBarberShop::ResetAppearance();

    if (CGBarberShop::m_helmHidden) {
        VisibleItem_C* head = const_cast<VisibleItem_C*>(player->GetVisibleItem(INVSLOT_HEAD));

        if (head && head->GetDisplayID() && player->CanEquipItemsInThisForm()) {
            component->AddItemBySlot(INVSLOT_HEAD, head->GetDisplayID(), 0);
        }

        CGBarberShop::m_helmHidden = 0;
    }

    VisibleItem_C* shoulder = const_cast<VisibleItem_C*>(player->GetVisibleItem(INVSLOT_SHOULDER));

    if (shoulder->GetDisplayID()) {
        component->AddItemBySlot(INVSLOT_SHOULDER, shoulder->GetDisplayID(), 0);
    }

    VisibleItem_C* offhand = const_cast<VisibleItem_C*>(player->GetVisibleItem(INVSLOT_OFFHAND));

    if (offhand->GetDisplayID() && offhand->GetInventoryType() == INVTYPE_SHIELD) {
        player->EquipVisibleItem(offhand, INVSLOT_OFFHAND);
    }

    VisibleItem_C* tabard = const_cast<VisibleItem_C*>(player->GetVisibleItem(INVSLOT_TABARD));

    if (tabard && tabard->GetDisplayID()) {
        player->LoadGuildTextures(static_cast<int32_t>(tabard->GetDisplayID()));
    } else {
        component->RemoveItemBySlot(INVSLOT_TABARD);
    }

    if (CGBarberShop::m_resetPortrait) {
        WOWGUID guid = ClntObjMgrGetActivePlayer();
        CGGameUI::UnitModelUpdate(guid, 3);
        CGBarberShop::m_resetPortrait = 0;
    }
}

// OFFSET: 0x009000F0/0x140657DD0
void CGBarberShop::ApplyAppearanceChange() {
    if (!CGBarberShop::m_barberShopEnabled) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CCharacterComponent* component = player->m_characterComponent;

    if (!component) {
        return;
    }

    if (player->m_player->m_playerBytes[3] == component->m_data.m_preferences.m_hairColorId && player->m_player->m_playerBytes[2] == CGBarberShop::m_currentHairStyle->m_rec->m_data && player->m_player->m_playerBytes2[0] == CGBarberShop::m_currentFacialHair->m_rec->m_data && (!CGBarberShop::m_currentSkinColor || player->m_player->m_playerBytes[0] == CGBarberShop::m_currentSkinColor->m_rec->m_data)) {
        return;
    }

    uint32_t coinage = player->GetCoinage();

    if (static_cast<uint32_t>(CGBarberShop::GetTotalCost()) > coinage) {
        CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ALTER_APPEARANCE));
    msg.Put(static_cast<uint32_t>(CGBarberShop::m_currentHairStyle->m_rec->m_id));
    msg.Put(static_cast<uint32_t>(component->m_data.m_preferences.m_hairColorId));
    msg.Put(static_cast<uint32_t>(CGBarberShop::m_currentFacialHair->m_rec->m_id));

    if (CGBarberShop::m_currentSkinColor) {
        msg.Put(static_cast<uint32_t>(CGBarberShop::m_currentSkinColor->m_rec->m_id));
    } else {
        msg.Put(static_cast<uint32_t>(0));
    }

    msg.Finalize();
    ClientServices::SendPacket(&msg);
}

// OFFSET: 0x00900250/0x140657FE0
STYLEREC* CGBarberShop::GetStyle(int32_t type, int32_t race, int32_t sex, int32_t data) {
    HASHKEY_NONE key;
    USEABLESTYLE* useable = CGBarberShop::m_useableStyles.Ptr(type, key);

    if (!useable) {
        return NULL;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return NULL;
    }

    CCharacterComponent* component = player->m_characterComponent;

    if (!component) {
        return NULL;
    }

    if (type == 0) {
        int32_t count = CCharacterComponent::GetNumHairStylesForColor(component->m_data.m_preferences.m_raceID, component->m_data.m_preferences.m_sexID, component->m_data.m_preferences.m_classId, component->m_data.m_preferences.m_hairColorId, COMPONENT_CONTEXT_1);

        if (count > 0) {
            data %= count;
        }
    } else if (type == 2) {
        int32_t count = CCharacterComponent::GetNumFacialFeaturesForHairColor(component->m_data.m_preferences.m_raceID, component->m_data.m_preferences.m_sexID, component->m_data.m_preferences.m_classId, component->m_data.m_preferences.m_hairColorId, COMPONENT_CONTEXT_1);

        if (count > 0) {
            data %= count;
        }
    } else if (type == 3) {
        int32_t count = CCharacterComponent::GetNumSkins(component->m_data.m_preferences.m_raceID, component->m_data.m_preferences.m_sexID, component->m_data.m_preferences.m_classId, COMPONENT_CONTEXT_1);

        if (count) {
            data %= count;
        }
    }

    for (STYLEREC* style = useable->m_styles.Head(); !useable->m_styles.IsEnd(style); style = useable->m_styles.RawNext(style)) {
        const BarberShopStyleRec* rec = style->m_rec;

        if (rec->m_race == race && (rec->m_sex == sex || rec->m_sex == -1) && rec->m_data == data) {
            return style;
        }
    }

    return NULL;
}

// OFFSET: 0x00900370/0x140658160
void CGBarberShop::Reset() {
    if (!CGBarberShop::m_barberShopEnabled) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->m_characterComponent) {
        return;
    }

    CGBarberShop::m_currentHairStyle = CGBarberShop::GetStyle(0, player->m_unit->m_unitFieldBytes0[0], player->m_player->m_playerBytes3[0], player->m_player->m_playerBytes[2]);
    CGBarberShop::m_currentFacialHair = CGBarberShop::GetStyle(2, player->m_unit->m_unitFieldBytes0[0], player->m_player->m_playerBytes3[0], player->m_player->m_playerBytes2[0]);
    CGBarberShop::ResetAppearance();
}

// OFFSET: 0x009005B0/0x140658420
void CGBarberShop::LeaveWorld() {
    CGBarberShop::DisableBarberShop();
}

// OFFSET: 0x009005C0
void CGBarberShop::ShutdownGame() {
    CGBarberShop::m_initialized = 0;
    CGBarberShop::m_useableStyles.InternalClear(0);
    CGBarberShop::m_alternateFormFrame = NULL;
    ClientServices::ClearMessageHandler(SMSG_ENABLE_BARBER_SHOP);
    ClientServices::ClearMessageHandler(SMSG_BARBER_SHOP_RESULT);
}

// OFFSET: 0x00900600
int32_t Packet_SMSG_ENABLE_BARBER_SHOP(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 1;
    }

    CCharacterComponent* component = player->m_characterComponent;

    if (!component) {
        return 1;
    }

    if (!(player->m_player->m_playerFlags & 0x400)) {
        if (player->CanEquipItemsInThisForm()) {
            component->RemoveItemBySlot(INVSLOT_HEAD);
        }

        CGBarberShop::m_helmHidden = 1;
    }

    component->RemoveItemBySlot(INVSLOT_SHOULDER);
    VisibleItem_C* offhand = const_cast<VisibleItem_C*>(player->GetVisibleItem(INVSLOT_OFFHAND));

    if (offhand->GetDisplayID() && offhand->GetInventoryType() == INVTYPE_SHIELD) {
        const CGUnitVirtualItem* ranged = player->GetVirtualItem(2, false);

        if ((!ranged || (ranged->m_inventoryType != 26 && ranged->m_inventoryType != 25)) && player->m_unit->m_unitFieldBytes2[0] == 2) {
            CM2Model* attached = NULL;

            for (CM2Model* child = player->m_worldModel->m_attachList; child; child = child->m_attachNext) {
                if (child->m_attachmentId == 2) {
                    child->m_refCount++;
                    attached = child;
                    attached->DetachFromParent();
                    break;
                }
            }

            CCharacterComponent::RemoveHandItem(attached, &component->m_attachMask, INVSLOT_OFFHAND, static_cast<SHEATHE_TYPE>(offhand->GetSheatheType()), offhand->GetInventoryType() == INVTYPE_SHIELD);

            if (attached) {
                attached->AttachToParent(attached, 2, NULL, 0);
                attached->Release();
            }
        } else {
            CM2Model* model = player->m_worldModel;
            CCharacterComponent::RemoveHandItem(model, &component->m_attachMask, INVSLOT_OFFHAND, static_cast<SHEATHE_TYPE>(offhand->GetSheatheType()), offhand->GetInventoryType() == INVTYPE_SHIELD);
        }
    }

    component->AddItemBySlot(INVSLOT_TABARD, 54559, 0);
    CGBarberShop::m_currentHairStyle = CGBarberShop::GetStyle(0, player->m_unit->m_unitFieldBytes0[0], player->m_player->m_playerBytes3[0], player->m_player->m_playerBytes[2]);
    CGBarberShop::m_currentFacialHair = CGBarberShop::GetStyle(2, player->m_unit->m_unitFieldBytes0[0], player->m_player->m_playerBytes3[0], player->m_player->m_playerBytes2[0]);
    CGBarberShop::m_currentSkinColor = CGBarberShop::GetStyle(3, player->m_unit->m_unitFieldBytes0[0], player->m_player->m_playerBytes3[0], component->m_data.m_preferences.m_skinID);
    CGBarberShop::m_barberShopEnabled = 1;
    FrameScript_SignalEvent(SCRIPT_EVENT_BARBER_SHOP_OPEN, NULL);
    CGBarberShop::m_minCameraDistance = 1.0f;
    M2SequenceInfo info;
    info.m_bounds.b = C3Vector();
    info.m_bounds.t = C3Vector();
    info.m_center = C3Vector();
    info.m_radius = 0.0f;
    CM2Model* model = player->m_worldModel;

    if (model && model->IsLoaded()) {
        model->GetSequenceInfo(player->GetCurrentTorsoAnimId(), 0, info);
        CGBarberShop::m_minCameraDistance = info.m_radius;
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (camera) {
        CGBarberShop::m_prevView = camera->m_viewIndex;
        float distance;

        if (player->m_unit->m_unitFieldBytes0[0] == 7) {
            distance = CGBarberShop::m_minCameraDistance * 3.0f;
        } else {
            distance = CGBarberShop::m_minCameraDistance + CGBarberShop::m_minCameraDistance;
        }

        camera->BarberShopSetView(distance, 3.1415927f, 0.0f);
    }

    return 1;
}

// OFFSET: 0x009008C0/0x1406588E0
void CGBarberShop::SetNextBarberShopStyle(int32_t type, bool backward) {
    if (!CGBarberShop::m_barberShopEnabled) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CCharacterComponent* component = player->m_characterComponent;

    if (!component) {
        return;
    }

    switch (type) {
        case 0:
            component->SetNextHairStyle(COMPONENT_CONTEXT_1, 2 * (backward == 0) - 1);
            CGBarberShop::m_currentHairStyle = CGBarberShop::GetStyle(0, player->m_unit->m_unitFieldBytes0[0], player->m_unit->m_unitFieldBytes0[2], component->m_data.m_preferences.m_hairStyleId);
            break;
        case 1:
            component->SetNextHairColor(COMPONENT_CONTEXT_1, 2 * (backward == 0) - 1);
            break;
        case 2:
            component->SetNextFacialFeature(COMPONENT_CONTEXT_1, 2 * (backward == 0) - 1);
            CGBarberShop::m_currentFacialHair = CGBarberShop::GetStyle(2, player->m_unit->m_unitFieldBytes0[0], player->m_unit->m_unitFieldBytes0[2], component->m_data.m_preferences.m_facialHairId);
            break;
        case 3:
            component->SetNextSkin(COMPONENT_CONTEXT_1, 2 * (backward == 0) - 1);
            CGBarberShop::m_currentSkinColor = CGBarberShop::GetStyle(3, player->m_unit->m_unitFieldBytes0[0], player->m_unit->m_unitFieldBytes0[2], component->m_data.m_preferences.m_skinID);
            break;
        default:
            break;
    }

    component->m_data.m_model->WaitUntilDrawableRecursive();
    component->RenderPrep(1);
    CGBarberShop::UpdateAlternateForm();
}

// OFFSET: 0x00900BE0/0x140658BC0
void CGBarberShop::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_ENABLE_BARBER_SHOP, Packet_SMSG_ENABLE_BARBER_SHOP, NULL);
    ClientServices::SetMessageHandler(SMSG_BARBER_SHOP_RESULT, Packet_SMSG_BARBER_SHOP_RESULT, NULL);
    CGBarberShop::m_initialized = 0;
}

// OFFSET: 0x00901310/0x1406595D0
void CGBarberShop::EnterWorld() {
    if (CGBarberShop::m_initialized) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t race = player->m_unit->m_unitFieldBytes0[0];
    int32_t sex = player->m_player->m_playerBytes3[0];

    for (int32_t i = 0; i < g_barberShopStyleDB.m_numRecords; i++) {
        BarberShopStyleRec* rec = g_barberShopStyleDB.GetRecordByIndex(i);

        if (!rec || rec->m_race != race || (rec->m_sex != sex && rec->m_sex != -1)) {
            continue;
        }

        HASHKEY_NONE key;
        USEABLESTYLE* useable = CGBarberShop::m_useableStyles.Ptr(rec->m_type, key);

        if (!useable) {
            useable = CGBarberShop::m_useableStyles.New(rec->m_type, key, 0, 0);
        }

        STYLEREC* style = useable->m_styles.NewNode(2, 0, 0);
        style->m_rec = rec;
    }

    CGBarberShop::m_currentHairStyle = CGBarberShop::GetStyle(0, player->m_unit->m_unitFieldBytes0[0], player->m_player->m_playerBytes3[0], player->m_player->m_playerBytes[2]);
    CGBarberShop::m_currentFacialHair = CGBarberShop::GetStyle(2, player->m_unit->m_unitFieldBytes0[0], player->m_player->m_playerBytes3[0], player->m_player->m_playerBytes2[0]);
    CGBarberShop::m_currentSkinColor = CGBarberShop::GetStyle(3, player->m_unit->m_unitFieldBytes0[0], player->m_player->m_playerBytes3[0], player->m_player->m_playerBytes[0]);
}

// OFFSET: #0x0052F190
USEABLESTYLE::~USEABLESTYLE() {
    this->m_styles.Clear();
}
