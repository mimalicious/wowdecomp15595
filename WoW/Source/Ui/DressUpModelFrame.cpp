#include "DressUpModelFrame.h"
#include <cstdlib>
#include <cstring>
#include <new>
#include <typeinfo>
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleModel.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/DB/DBClient/CreatureStats_C.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoExtraRec.h"
#include "WoW/Source/DB/Rec/CreatureDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/CreatureModelDataRec.h"
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Glue/ComponentData.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/VisibleItem_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/CGTabardCreationFrame.h"
#include "WoW/Source/Ui/GameUI.h"

// STATIC: 0x00D176E4
const char* CGDressUpModelFrame::s_objectTypeName = "DressUpModel";

// STATIC: 0x00D17708
int32_t CGDressUpModelFrame::s_metatable = LUA_NOREF;

// STATIC: 0x00F76C88
int32_t CGDressUpModelFrame::s_objectType;

// STATIC: #0x00C0E500
CDataAllocator CGDressUpModelFrame::s_dressUpModelHeap(sizeof(CGDressUpModelFrame), 5);

// STATIC: 0x00F76C8C
static int32_t s_mainHandItemId;

// STATIC: 0x00F76C90
static int32_t s_offHandItemId;

// STATIC: 0x00D176E0
static int32_t s_nextHand = 15;

// INLINED
void CGDressUpModelFrame::operator delete(void* ptr) {
    CGDressUpModelFrame::s_dressUpModelHeap.PutData(ptr, NULL, 0);
}

// OFFSET: 0x008C6510
CSimpleFrame* CGDressUpModelFrame::Create(CSimpleFrame* parent) {
    if (CGDressUpModelFrame::s_dressUpModelHeap.m_bytesPerData < sizeof(CGDressUpModelFrame)) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    void* data = CGDressUpModelFrame::s_dressUpModelHeap.GetData(0, typeid(CGDressUpModelFrame).raw_name(), SLOG_OBJECT);
    return data ? new (data) CGDressUpModelFrame(parent) : NULL;
}

// OFFSET: 0x00A04840
CGDressUpModelFrame::CGDressUpModelFrame(CSimpleFrame* parent) : CGCharacterModelBase(parent) {
    this->m_dressUpComponent = NULL;
}

// PROVEN: 67d442a2 IDENTICAL/NONE
// OFFSET: 0x00A04870
const char* CGDressUpModelFrame::GetObjectTypeName() {
    return CGDressUpModelFrame::s_objectTypeName;
}

// PROVEN: 5a0a4bc2 IDENTICAL/NONE
// OFFSET: 0x00A04880
int32_t CGDressUpModelFrame::GetScriptMetaTable() {
    return CGDressUpModelFrame::s_metatable;
}

// OFFSET: 0x00A04890/0x1407A3ED0
void CGDressUpModelFrame::Undress() {
    if (!this->m_model) {
        return;
    }

    if (this->m_dressUpComponent) {
        for (int32_t i = 0; i < 12; i++) {
            this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(i));
        }
    }

    if (this->m_mainHand.m_classID) {
        if (this->m_dressUpComponent) {
            this->m_dressUpComponent->RemoveHandItem(static_cast<INVENTORY_SLOTS>(15), static_cast<SHEATHE_TYPE>(this->m_mainHand.m_sheatheType), this->m_mainHand.m_inventoryType == 14);
        }

        memset(&this->m_mainHand, 0, sizeof(this->m_mainHand));
    }

    if (this->m_offHand.m_classID) {
        if (this->m_dressUpComponent) {
            this->m_dressUpComponent->RemoveHandItem(static_cast<INVENTORY_SLOTS>(16), static_cast<SHEATHE_TYPE>(this->m_offHand.m_sheatheType), this->m_offHand.m_inventoryType == 14);
        }

        memset(&this->m_offHand, 0, sizeof(this->m_offHand));
    }
}

// OFFSET: 0x00A04950/0x1407A3FA0
static bool HandItemsCompatible(uint32_t mainHandType, uint32_t offHandType, bool dualWield) {
    if (mainHandType != 13 && mainHandType != 21) {
        return mainHandType == 17 && offHandType != 15 && CGItem_C::CanGoInSlot(s_mainHandItemId, 16, 0);
    }

    if (offHandType == 14 || offHandType == 23) {
        return true;
    }

    if (offHandType != 13 && offHandType != 22) {
        return (offHandType == 21 || offHandType == 17) && CGItem_C::CanGoInSlot(s_offHandItemId, 16, 0);
    }

    return dualWield;
}

// OFFSET: 0x00A049C0/0x1407A4020
void CGDressUpModelFrame::UndressSlot(int32_t slot) {
    if (!this->m_dressUpComponent) {
        return;
    }

    switch (slot) {
        case 0:
            this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(0));
            break;
        case 2:
            this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(1));
            break;
        case 3:
            this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(2));
            break;
        case 4:
            this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(3));
            break;
        case 5:
            this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(4));
            break;
        case 6:
            this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(5));
            break;
        case 7:
            this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(6));
            break;
        case 8:
            this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(7));
            break;
        case 9:
            this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(8));
            break;
        case 14:
            this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(10));
            break;
        case 15:
            s_mainHandItemId = 0;
            this->m_dressUpComponent->RemoveHandItem(static_cast<INVENTORY_SLOTS>(15), static_cast<SHEATHE_TYPE>(this->m_mainHand.m_sheatheType), this->m_mainHand.m_inventoryType == 14);
            memset(&this->m_mainHand, 0, sizeof(this->m_mainHand));
            break;
        case 16:
            s_offHandItemId = 0;
            this->m_dressUpComponent->RemoveHandItem(static_cast<INVENTORY_SLOTS>(16), static_cast<SHEATHE_TYPE>(this->m_offHand.m_sheatheType), this->m_offHand.m_inventoryType == 14);
            memset(&this->m_offHand, 0, sizeof(this->m_offHand));
            break;
        case 18:
            this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(9));
            break;
        default:
            break;
    }
}

// OFFSET: [0x00A04B90, 0x00A04C30]
CGDressUpModelFrame::~CGDressUpModelFrame() {
    if (this->m_dressUpComponent) {
        CCharacterComponent::FreeComponent(this->m_dressUpComponent);
        this->m_dressUpComponent = NULL;
    }
}

// OFFSET: 0x00A04BA0
bool CGDressUpModelFrame::IsA(int32_t type) {
    return type == CGDressUpModelFrame::s_objectType || type == CGCharacterModelBase::s_objectType || type == CSimpleModel::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// OFFSET: 0x00A04BF0
bool CGDressUpModelFrame::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CGDressUpModelFrame::s_objectTypeName, STORM_MAX_STR) || CGCharacterModelBase::IsA(typeName);
}

// PROVEN: 0eade77d IDENTICAL/NONE
// OFFSET: 0x00A04C90
CM2Model* CGDressUpModelFrame::GetDuplicateUnitCharacterModel(CGUnit_C* unit) {
    if (!unit) {
        return NULL;
    }

    CreatureDisplayInfoRec* displayRec = g_creatureDisplayInfoDB.GetRecord(unit->GetCurrentClientNativeDisplayID());

    if (!displayRec) {
        return NULL;
    }

    CreatureModelDataRec* modelRec = g_creatureModelDataDB.GetRecord(displayRec->m_modelID);

    if (!modelRec) {
        return NULL;
    }

    const char* modelName = modelRec->m_modelName;

    if (!modelName || !*modelName) {
        return NULL;
    }

    return this->GetScene()->CreateModel(modelName, 0, 3);
}

// PROVEN: c1967315 IDENTICAL/NONE
// OFFSET: 0x00A04E40
void CGDressUpModelFrame::SetUpModel(CGUnit_C* unit) {
    CGCharacterModelBase::SetUpModel(unit);

    if (this->m_model) {
        this->m_model->m_fadeAlpha = 1.0f;
    }
}

// OFFSET: 0x00A04E70
void CGDressUpModelFrame::UpdateModel() {
    CGCharacterModelBase::UpdateModel();

    if (!this->m_dressUpComponent) {
        return;
    }

    uint32_t ready = this->m_dressUpComponent->RenderPrep(0) & 1;
    CM2Model* model = this->m_dressUpComponent->m_data.m_model;

    if (model->m_attachParent) {
        model->m_fFlags = (model->m_fFlags & ~0x40u) | (ready << 6);
        model->m_fFlags = (model->m_fFlags & ~0x10000u) | (ready << 16);
    } else {
        model->m_fFlags = (model->m_fFlags & ~0x8u) | (ready << 3);
        model->m_fFlags = (model->m_fFlags & ~0x8000u) | (ready << 15);
    }
}

// OFFSET: 0x00A04EF0/0x1407A4650
void CGDressUpModelFrame::TryOn(int32_t itemId, const int32_t* enchants, int32_t hand) {
    int32_t visual = 0;
    ItemVisualsRec* visuals = NULL;
    WOWGUID requester = WOWGUID::Make(0);
    ItemRec* item = g_itemDB.GetRecord(itemId, requester, NULL, NULL, false);

    if (item) {
        ItemDisplayInfoRec display;

        if (g_itemDisplayInfoDB.GetRecord(item->m_displayInfoID, &display)) {
            visuals = g_itemVisualsDB.GetRecord(display.m_itemVisual);
        }
    }

    if (enchants && !visuals) {
        for (int32_t i = 0; i < 15; i++) {
            SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(enchants[i]);

            if (enchant && enchant->m_itemVisual) {
                visual = enchant->m_itemVisual;
                break;
            }
        }
    }

    this->TryOnWithVisual(itemId, visual, hand);
}

// PROVEN: 8c1ea975 IDENTICAL/NONE
// OFFSET: 0x00A04FF0/0x1407A4790
static void DressUpItemCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        static_cast<CGDressUpModelFrame*>(param)->TryOn(id, NULL, -1);
    }
}

// OFFSET: 0x00A05010/0x1407A47C0
void CGDressUpModelFrame::TryOnWithVisual(int32_t itemId, int32_t visual, int32_t hand) {
    WOWGUID requester = WOWGUID::Make(0);
    ItemRecSparse* sparse = g_itemSparseDB.GetRecord(itemId, requester, DressUpItemCallback, this, false);

    if (!sparse) {
        return;
    }

    ItemRec* item = sparse->GetNonSparse();

    if (!item || !item->m_displayInfoID || !this->m_model) {
        return;
    }

    if (visual < 0) {
        visual = 0;
    }

    if (!sparse->m_inventoryType) {
        for (uint32_t i = 0; i < 5; i++) {
            SpellRec* spell;
            const SpellEffectRec* effect;

            if (!sparse->m_spellID[i] || !(spell = g_spellDB.GetRecord(sparse->m_spellID[i])) || !(effect = SpellGetEffect(spell->m_id, 0)) || effect->m_effect != 36) {
                continue;
            }

            int32_t learnedId = effect->m_effectTriggerSpell;

            if (!learnedId) {
                for (uint32_t j = 0; j < 5; j++) {
                    if (sparse->m_spellTrigger[j] == 6) {
                        learnedId = sparse->m_spellID[j];
                        break;
                    }
                }
            }

            const SpellEffectRec* learned = SpellGetEffect(learnedId, 0);

            if (learned && learned->m_effect == 24) {
                this->TryOn(learned->m_effectItemType, NULL, -1);
                return;
            }
        }
    }

    switch (sparse->m_inventoryType) {
        case 1:
            if (this->m_dressUpComponent) {
                this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(0));
                this->m_dressUpComponent->AddItem(static_cast<ITEM_SLOT>(0), item->m_displayInfoID, visual);
            }

            return;
        case 2:
        case 11:
        case 12:
        case 28:
            return;
        case 3:
            if (this->m_dressUpComponent) {
                this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(1));
                this->m_dressUpComponent->AddItem(static_cast<ITEM_SLOT>(1), item->m_displayInfoID, visual);
            }

            return;
        case 4:
            if (this->m_dressUpComponent) {
                this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(2));
                this->m_dressUpComponent->AddItem(static_cast<ITEM_SLOT>(2), item->m_displayInfoID, visual);
                ItemDisplayInfoRec display;

                if (g_itemDisplayInfoDB.GetRecord(item->m_displayInfoID, &display) && (display.m_flags & 0x19)) {
                    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(this->m_unitGuid, TYPEMASK_PLAYER, __FILE__, __LINE__));
                    int32_t emblemStyle;
                    int32_t emblemColor;
                    int32_t borderStyle;
                    int32_t borderColor;
                    int32_t background;

                    if (player && GuildGetGuildTabard(player->m_obj->m_guid, player->GetGuildGUID(), NULL, NULL, emblemStyle, emblemColor, borderStyle, borderColor, background)) {
                        this->m_dressUpComponent->ApplyShirtGuildColor(display.m_flags, emblemStyle, emblemColor, borderStyle, borderColor, background);
                    }
                }
            }

            return;
        case 5:
        case 20:
            if (this->m_dressUpComponent) {
                this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(3));
                this->m_dressUpComponent->AddItem(static_cast<ITEM_SLOT>(3), item->m_displayInfoID, visual);
            }

            return;
        case 6:
            if (this->m_dressUpComponent) {
                this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(4));
                this->m_dressUpComponent->AddItem(static_cast<ITEM_SLOT>(4), item->m_displayInfoID, visual);
            }

            return;
        case 7:
            if (this->m_dressUpComponent) {
                this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(5));
                this->m_dressUpComponent->AddItem(static_cast<ITEM_SLOT>(5), item->m_displayInfoID, visual);
            }

            return;
        case 8:
            if (this->m_dressUpComponent) {
                this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(6));
                this->m_dressUpComponent->AddItem(static_cast<ITEM_SLOT>(6), item->m_displayInfoID, visual);
            }

            return;
        case 9:
            if (this->m_dressUpComponent) {
                this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(7));
                this->m_dressUpComponent->AddItem(static_cast<ITEM_SLOT>(7), item->m_displayInfoID, visual);
            }

            return;
        case 10:
            if (this->m_dressUpComponent) {
                this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(8));
                this->m_dressUpComponent->AddItem(static_cast<ITEM_SLOT>(8), item->m_displayInfoID, visual);
            }

            return;
        case 13:
        case 14:
        case 15:
        case 17:
        case 21:
        case 22:
        case 23:
        case 25:
        case 26: {
            CGPlayer_C* player = NULL;

            if (this->m_unitGuid == ClntObjMgrGetActivePlayer()) {
                player = ClntObjMgrGetActivePlayerObj();
            }

            int32_t slot = hand;

            if (hand == -1) {
                switch (sparse->m_inventoryType) {
                    case 13:
                        if (this->m_mainHand.m_classID && (this->m_mainHand.m_inventoryType == 13 || this->m_mainHand.m_inventoryType == 21 || this->m_mainHand.m_inventoryType == 17) && player && CGPlayer_C::CanDualWield()) {
                            slot = s_nextHand;
                            s_nextHand = (s_nextHand == 15) + 15;
                        } else {
                            slot = 15;
                        }

                        break;
                    case 14:
                    case 15:
                    case 22:
                    case 23:
                        slot = 16;
                        s_nextHand = 15;
                        break;
                    case 17:
                    case 21:
                        if (CGItem_C::CanGoInSlot(itemId, 16, 0)) {
                            slot = s_nextHand;
                            s_nextHand = (s_nextHand == 15) + 15;
                        } else {
                            slot = 15;
                            s_nextHand = 15;
                        }

                        break;
                    default:
                        slot = 15;
                        s_nextHand = 15;
                        break;
                }
            }

            CGUnitVirtualItem* handItem;

            if (slot == 15) {
                s_mainHandItemId = itemId;

                if (this->m_offHand.m_classID && !HandItemsCompatible(sparse->m_inventoryType, this->m_offHand.m_inventoryType, player && CGPlayer_C::CanDualWield())) {
                    if (this->m_dressUpComponent) {
                        this->m_dressUpComponent->RemoveHandItem(static_cast<INVENTORY_SLOTS>(16), static_cast<SHEATHE_TYPE>(this->m_offHand.m_sheatheType), this->m_offHand.m_inventoryType == 14);
                    }

                    s_offHandItemId = 0;
                }

                handItem = &this->m_mainHand;
            } else {
                s_offHandItemId = itemId;

                if (this->m_mainHand.m_classID && !HandItemsCompatible(this->m_mainHand.m_inventoryType, sparse->m_inventoryType, player && CGPlayer_C::CanDualWield())) {
                    if (this->m_dressUpComponent) {
                        this->m_dressUpComponent->RemoveHandItem(static_cast<INVENTORY_SLOTS>(15), static_cast<SHEATHE_TYPE>(this->m_mainHand.m_sheatheType), this->m_mainHand.m_inventoryType == 14);
                    }

                    s_mainHandItemId = 0;
                }

                handItem = &this->m_offHand;
            }

            if (this->m_dressUpComponent && handItem->m_classID) {
                this->m_dressUpComponent->RemoveHandItem(static_cast<INVENTORY_SLOTS>(slot), static_cast<SHEATHE_TYPE>(handItem->m_sheatheType), handItem->m_inventoryType == 14);
            }

            handItem->m_classID = item->m_classID;
            handItem->m_subclassID = item->m_subclassID;
            handItem->m_inventoryType = sparse->m_inventoryType;
            handItem->m_sheatheType = sparse->m_sheatheType;

            if (this->m_dressUpComponent) {
                ItemDisplayInfoRec display;

                if (g_itemDisplayInfoDB.GetRecord(item->m_displayInfoID, &display)) {
                    this->m_dressUpComponent->AddHandItem(&display, static_cast<INVENTORY_SLOTS>(slot), static_cast<SHEATHE_TYPE>(sparse->m_sheatheType), false, sparse->m_inventoryType == 14, sparse->m_inventoryType == 26, visual);
                }
            }

            return;
        }
        case 16:
            if (this->m_dressUpComponent) {
                this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(10));
                this->m_dressUpComponent->AddItem(static_cast<ITEM_SLOT>(10), item->m_displayInfoID, visual);
                CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(this->m_unitGuid, TYPEMASK_PLAYER, __FILE__, __LINE__));

                if (player) {
                    CGObject_C::ReplaceGuildTextures(this->m_dressUpComponent->m_data.m_model, player->GetGuildGUID().m_guid, player->GetPVPFactionIndex());
                }
            }

            return;
        case 19:
            if (this->m_dressUpComponent) {
                this->m_dressUpComponent->RemoveItem(static_cast<ITEM_SLOT>(9));
                this->m_dressUpComponent->AddItem(static_cast<ITEM_SLOT>(9), item->m_displayInfoID, visual);
                ItemDisplayInfoRec display;

                if (g_itemDisplayInfoDB.GetRecord(item->m_displayInfoID, &display) && (display.m_flags & 0x19)) {
                    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(this->m_unitGuid, TYPEMASK_PLAYER, __FILE__, __LINE__));
                    int32_t emblemStyle;
                    int32_t emblemColor;
                    int32_t borderStyle;
                    int32_t borderColor;
                    int32_t background;

                    if (player && GuildGetGuildTabard(player->m_obj->m_guid, player->GetGuildGUID(), NULL, NULL, emblemStyle, emblemColor, borderStyle, borderColor, background)) {
                        this->m_dressUpComponent->ApplyGuildColor(display.m_flags, emblemStyle, emblemColor, borderStyle, borderColor, background);
                    }
                }
            }

            return;
        default:
            CGGameUI::DisplayError(ERR_NOT_EQUIPPABLE);
            return;
    }
}

// OFFSET: 0x00A05850
void CGDressUpModelFrame::TryOnItem(CGItem_C* item, int32_t hand) {
    if (item) {
        this->TryOnWithVisual(item->m_obj->m_entryID, item->GetItemEnchantmentVisualID(), hand);
    }
}

// OFFSET: 0x00A05A70/0x1407A5580
void CGDressUpModelFrame::SetModel(CM2Model* model) {
    CSimpleModel::SetModel(model);

    if (this->m_dressUpComponent) {
        CCharacterComponent::FreeComponent(this->m_dressUpComponent);
        this->m_dressUpComponent = NULL;
    }

    if (!model) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(this->m_unitGuid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        this->m_dressUpComponent = CCharacterComponent::AllocComponent();
        model->m_refCount++;
        ComponentData data;
        int32_t race = this->m_customRace;

        if (race == -1) {
            race = player->GetCurrentClientRace();
        }

        data.m_preferences.m_raceID = race;
        int32_t sex = this->m_customSex;

        if (sex == -1) {
            sex = player->m_player->m_playerBytes3[0];
        }

        data.m_preferences.m_sexID = sex;
        data.m_preferences.m_classId = player->m_unit->m_unitFieldBytes0[1];
        data.m_preferences.m_skinID = player->m_player->m_playerBytes[0];
        data.m_preferences.m_faceId = player->m_player->m_playerBytes[1];
        data.m_preferences.m_hairStyleId = player->m_player->m_playerBytes[2];
        data.m_preferences.m_hairColorId = player->m_player->m_playerBytes[3];
        data.m_preferences.m_facialHairId = player->m_player->m_playerBytes2[0];
        data.m_model = model;
        data.m_flags = (data.m_flags & ~2u) | ((player->m_obj->m_guid == ClntObjMgrGetActivePlayer()) << 1);
        CCharacterComponent::ValidateComponentData(&data, static_cast<COMPONENT_CONTEXT>(2));
        this->m_dressUpComponent->Init(&data, NULL);
        s_mainHandItemId = 0;
        s_offHandItemId = 0;
        s_nextHand = 15;

        if (player->GetBag()) {
            for (uint32_t slot = 0; slot <= 18; slot++) {
                bool skip;

                if (slot == 0) {
                    skip = (player->m_player->m_playerFlags & 0x400) != 0;
                } else if (slot == 14) {
                    skip = (player->m_player->m_playerFlags & 0x800) != 0;
                } else if (slot == 15 || slot == 16) {
                    const VisibleItem_C* visible = player->GetVisibleItem(slot);

                    if (visible) {
                        this->TryOnWithVisual(abs(visible->m_itemId), const_cast<VisibleItem_C*>(visible)->GetItemEnchantmentVisualID(), slot);
                    }

                    continue;
                } else {
                    skip = slot == 17;
                }

                if (skip) {
                    continue;
                }

                const VisibleItem_C* visible = player->GetVisibleItem(slot);

                if (visible) {
                    this->TryOnWithVisual(abs(visible->m_itemId), const_cast<VisibleItem_C*>(visible)->GetItemEnchantmentVisualID(), -1);
                }
            }
        }

        const CGUnitVirtualItem* mainHand = player->GetVirtualItem(0, true);

        if (mainHand) {
            this->m_mainHand = *mainHand;
        } else {
            memset(&this->m_mainHand, 0, sizeof(this->m_mainHand));
        }

        const CGUnitVirtualItem* offHand = player->GetVirtualItem(1, true);

        if (offHand) {
            this->m_offHand = *offHand;
        } else {
            memset(&this->m_offHand, 0, sizeof(this->m_offHand));
        }

        this->m_isNativeDisplay = player->m_unit->m_unitFieldNativedisplayid == player->m_unit->m_unitFieldDisplayid;
        return;
    }

    if (!this->m_creatureStats) {
        return;
    }

    CreatureDisplayInfoRec* displayRec = g_creatureDisplayInfoDB.GetRecord(this->m_creatureStats->m_displayId[0]);

    if (!displayRec) {
        return;
    }

    CreatureDisplayInfoExtraRec* extraRec = g_creatureDisplayInfoExtraDB.GetRecord(displayRec->m_extendedDisplayInfoID);

    if (!extraRec) {
        return;
    }

    const char* bakeName = extraRec->m_bakeName;

    if (!bakeName || !*bakeName) {
        return;
    }

    this->m_dressUpComponent = CCharacterComponent::AllocComponent();
    ComponentData data;
    data.m_preferences.m_raceID = extraRec->m_displayRaceID;
    data.m_preferences.m_sexID = extraRec->m_displaySexID;
    data.m_flags |= 1;
    data.m_preferences.m_skinID = extraRec->m_skinID;
    data.m_preferences.m_faceId = extraRec->m_faceID;
    data.m_preferences.m_classId = 1;
    data.m_preferences.m_hairStyleId = extraRec->m_hairStyleID;
    data.m_preferences.m_hairColorId = extraRec->m_hairColorID;
    data.m_preferences.m_facialHairId = extraRec->m_facialHairID;
    data.m_model = model;
    SStrPrintf(data.m_npcSkinTexture, 260, "%s%s", "Textures\\BakedNpcTextures\\", bakeName);
    model->m_refCount++;
    this->m_dressUpComponent->Init(&data, NULL);

    for (uint32_t i = 0; i < 11; i++) {
        if (extraRec->m_npcitemDisplay[i]) {
            this->m_dressUpComponent->AddItem(static_cast<ITEM_SLOT>(i), extraRec->m_npcitemDisplay[i], 0);
        }
    }
}
