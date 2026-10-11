#include "Item_C.h"
#include "WoW/Source/DB/Rec/ItemVisualsRec.h"
#include "WoW/Source/Ui/ItemSocketInfo.h"
#include "WoW/Source/Ui/LootFrame.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Services/Texture.h"
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"
#include "WoW/Source/Game/ViolenceLevels.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemRandomPropertiesRec.h"
#include "WoW/Source/DB/Rec/ItemRandomSuffixRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/ItemReforgeRec.h"
#include "WoW/Source/DB/Rec/ItemLimitCategoryRec.h"
#include "WoW/Source/DB/Rec/MaterialRec.h"
#include "WoW/Source/DB/Rec/RandPropPointsRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/DB/Rec/SpellCooldownsRec.h"
#include "WoW/Source/DB/Rec/SpellCategoriesRec.h"
#include "WoW/Source/DB/Rec/SpellEquippedItemsRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/OverrideSpellDataRec.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/DB/Rec/SpellShapeshiftFormRec.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/DB/Rec/DurabilityCostsRec.h"
#include "WoW/Source/DB/Rec/DurabilityQualityRec.h"
#include "WoW/Source/DB/Rec/LockRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/ContainerFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PaperDollInfoFrame.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/Reforge.h"
#include "WoW/Source/Ui/ScalingStats.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/TradeFrame.h"
#include "WoW/Source/Ui/TradeSkillFrame.h"
#include "WoW/Source/Ui/Transmogrify.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstring>
#include <ctime>
#include "WoW/Source/Ui/ItemTextFrame.h"
#include "WoW/Source/DB/Rec/ItemGroupSoundsRec.h"
#include "WoW/Source/Object/Item.h"

// STATIC: 0x00B9E5A0
const uint32_t s_inventoryTypeSlots[29] = { 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 3072, 12288, 98304, 65536, 131072, 16384, 98304, 7864320, 262144, 16, 98304, 98304, 65536, 0, 131072, 131072, 0, 131072 };

// STATIC: 0x00DD5E40
WOWGUID CGItem_C::s_bindOnUseTarget;

// STATIC: 0x00DD5E50
WOWGUID CGItem_C::s_bindOnUseItem;

// STATIC: 0x00DD5E58
static TSHashTable<INVENTORYART, HASHKEY_NONE> s_inventoryArt;

// STATIC: 0x00DD5E84
static TSHashTable<ITEMBYNAME, HASHKEY_UTF8I> s_itemByName;

// OFFSET: 0x005A5070
static int32_t ReforgeStatIsInvalid(int32_t stat, int32_t amount, const ItemReforgeRec* reforge, int32_t& reason) {
    if (stat == reforge->m_targetStat && amount > 0) {
        reason = 3;
        return 1;
    }

    if (stat == reforge->m_sourceStat && amount > 0 && static_cast<int32_t>(static_cast<float>(static_cast<int32_t>(static_cast<float>(amount) * reforge->m_sourceMultiplier)) * reforge->m_targetMultiplier) <= 0) {
        reason = 2;
        return 1;
    }

    return 0;
}

// OFFSET: 0x005A50D0
void INVENTORYART::SetTexture(const char* texture) {
    if (this->m_texture) {
        SMemFree(this->m_texture, __FILE__, __LINE__, 0);
        this->m_texture = NULL;
    }

    if (texture && *texture) {
        this->m_texture = SStrDupA(texture, __FILE__, __LINE__);
    }
}

// OFFSET: 0x005A5120
void CGItem_C::UpdateEnchantmentTime(int32_t slot, int32_t seconds) {
    if (seconds > 0) {
        this->m_enchantmentExpiration[slot] = 1000 * seconds + FrameTime::GetCurTimeMs();
    } else {
        this->m_enchantmentExpiration[slot] = 0;
    }
}

// OFFSET: 0x005A5160
uint32_t CGItem_C::GetEnchantmentTimeLeft(int32_t slot) const {
    if (this->m_enchantmentExpiration[slot]) {
        uint32_t now = FrameTime::GetCurTimeMs();
        uint32_t expiration = this->m_enchantmentExpiration[slot];

        if (static_cast<int32_t>(now - expiration) < 0) {
            return expiration - now;
        }
    }

    return 0;
}

// OFFSET: 0x005A51A0
void CGItem_C::SetItemPurchaseData(ItemPurchaseData* data) {
    if (this->m_itemPurchaseData) {
        delete this->m_itemPurchaseData;
    }

    this->m_itemPurchaseData = data;

    if (data) {
        this->m_itemFlags &= ~0x4;
    }
}

// OFFSET: 0x005A51E0/0x140216AD0
int32_t CGItem_C::GetRandomPropertyOverrideAmountEx(int32_t slot, int32_t randomPropertiesId, int32_t points) {
    float value = 0.0f;

    if (randomPropertiesId < 0) {
        ItemRandomSuffixRec* suffix = g_itemRandomSuffixDB.GetRecord(-randomPropertiesId);

        if (suffix) {
            uint32_t index = slot - 10;

            if (static_cast<int32_t>(index) >= 0 && index < 5) {
                return static_cast<int32_t>(static_cast<float>(suffix->m_allocationPct[index]) * 0.000099999997f * static_cast<float>(points));
            }
        }
    }

    return static_cast<int32_t>(value);
}

// OFFSET: 0x005A5240/0x140216050
int32_t CalculateItemBonusFromRandomEnchant(int32_t stat, int32_t randomPropertiesId, int32_t propertySeed) {
    if (randomPropertiesId > 0) {
        ItemRandomPropertiesRec* properties = g_itemRandomPropertiesDB.GetRecord(randomPropertiesId);

        if (!properties) {
            return 0;
        }

        for (uint32_t i = 0; i < 5; i++) {
            SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(properties->m_enchantment[i]);

            if (!enchant) {
                continue;
            }

            for (uint32_t j = 0; j < 3; j++) {
                if (enchant->m_effect[j] == 5 && enchant->m_effectArg[j] == stat) {
                    return (enchant->m_effectPointsMin[j] + enchant->m_effectPointsMax[j]) / 2;
                }
            }
        }

        return 0;
    }

    ItemRandomSuffixRec* suffix = g_itemRandomSuffixDB.GetRecord(-randomPropertiesId);

    if (!suffix) {
        return 0;
    }

    for (uint32_t i = 0; i < 5; i++) {
        SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(suffix->m_enchantment[i]);

        if (!enchant) {
            continue;
        }

        for (uint32_t j = 0; j < 3; j++) {
            if (enchant->m_effect[j] == 5 && enchant->m_effectArg[j] == stat) {
                return CGItem_C::GetRandomPropertyOverrideAmountEx(i + 10, randomPropertiesId, static_cast<uint16_t>(propertySeed));
            }
        }
    }

    return 0;
}

// OFFSET: 0x005A5380/0x1402161F0
void EvalutateReforgeOfRandomProp(int32_t randomPropertiesId, int32_t propertySeed, const ItemReforgeRec* reforge, int32_t& hasSourceStat, int32_t& invalid, int32_t& reason) {
    hasSourceStat = 0;
    invalid = 0;

    if (randomPropertiesId > 0) {
        ItemRandomPropertiesRec* properties = g_itemRandomPropertiesDB.GetRecord(randomPropertiesId);

        if (!properties) {
            return;
        }

        for (uint32_t i = 0; i < 5; i++) {
            SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(properties->m_enchantment[i]);

            if (!enchant) {
                continue;
            }

            for (uint32_t j = 0; j < 3; j++) {
                if (enchant->m_effect[j] != 5) {
                    continue;
                }

                int32_t stat = enchant->m_effectArg[j];
                int32_t amount = (enchant->m_effectPointsMin[j] + enchant->m_effectPointsMax[j]) / 2;

                if (ReforgeStatIsInvalid(stat, amount, reforge, reason)) {
                    invalid = 1;
                }

                if (!hasSourceStat && stat == reforge->m_sourceStat && amount > 0 && static_cast<int32_t>(static_cast<float>(amount) * reforge->m_sourceMultiplier) > 0) {
                    hasSourceStat = 1;
                }
            }
        }
    } else if (randomPropertiesId < 0) {
        ItemRandomSuffixRec* suffix = g_itemRandomSuffixDB.GetRecord(-randomPropertiesId);

        if (!suffix) {
            return;
        }

        for (uint32_t i = 0; i < 5; i++) {
            SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(suffix->m_enchantment[i]);

            if (!enchant) {
                continue;
            }

            for (uint32_t j = 0; j < 3; j++) {
                if (enchant->m_effect[j] != 5) {
                    continue;
                }

                int32_t stat = enchant->m_effectArg[j];
                int32_t amount = CGItem_C::GetRandomPropertyOverrideAmountEx(i + 10, randomPropertiesId, static_cast<uint16_t>(propertySeed));

                if (ReforgeStatIsInvalid(stat, amount, reforge, reason)) {
                    invalid = 1;
                }

                if (!hasSourceStat && stat == reforge->m_sourceStat && amount > 0 && static_cast<int32_t>(static_cast<float>(amount) * reforge->m_sourceMultiplier) > 0) {
                    hasSourceStat = 1;
                }
            }
        }
    }
}

// OFFSET: 0x005A5640/0x140216590
uint32_t CGItem_C::GetSpellItemEnchantmentId() const {
    for (uint32_t i = 0; i < 15; i++) {
        if (i == 8 || i == 9) {
            continue;
        }

        uint32_t id = this->m_item->m_itemFieldEnchantment[i].m_id;

        if (!id) {
            continue;
        }

        SpellItemEnchantmentRec* rec = g_spellItemEnchantmentDB.GetRecord(id);

        if (!rec) {
            continue;
        }

        for (uint32_t j = 0; j < 3; j++) {
            if (rec->m_effectArg[j] && rec->m_effect[j] == 7) {
                return this->m_item->m_itemFieldEnchantment[i].m_id;
            }
        }
    }

    return 0;
}

// OFFSET: 0x005A5600/0x140216540
void CGItem_C::SetStorage(uint32_t* storage, uint32_t* saved) {
    this->CGObject_C::SetStorage(storage, saved);
    this->m_item = reinterpret_cast<CGItemData*>(reinterpret_cast<uint8_t*>(storage) + sizeof(CGObjectData));
    this->m_itemMirror = saved + CGObject::TotalFieldsSaved();
}

// OFFSET: 0x005A56E0/0x140216630
int32_t CGItem_C::IsMetal(int32_t materialId) {
    MaterialRec* rec = g_materialDB.GetRecord(materialId);

    if (rec && (rec->m_flags & 0x1)) {
        return 1;
    }

    return 0;
}

// OFFSET: 0x005A5720/0x140216670
void CGItem_C::Lock() {
    if (!(this->m_itemFlags & 1)) {
        this->m_itemFlags |= 1;
        CGItem_C::SignalEvent(this, SCRIPT_EVENT_ITEM_LOCK_CHANGED);
        CGItem_C::SignalEvent(this, SCRIPT_EVENT_ITEM_LOCKED);
    }
}

// OFFSET: 0x005A5760/0x1402166B0
void CGItem_C::Unlock() {
    if (this->m_itemFlags & 1) {
        this->m_itemFlags &= ~1u;
        CGItem_C::SignalEvent(this, SCRIPT_EVENT_ITEM_LOCK_CHANGED);
        CGItem_C::SignalEvent(this, SCRIPT_EVENT_ITEM_UNLOCKED);
    }
}

// OFFSET: 0x005A57A0
void CGItem_C::SetTranslated() {
    this->m_item->m_itemFieldFlags |= 0x2;
}

// OFFSET: 0x005A57B0/0x140216700
void CGItem_C::UpdateExpirationTime(int32_t seconds) {
    if (seconds > 0) {
        this->m_expirationTime = seconds + static_cast<uint32_t>(time(NULL));
    } else {
        this->m_expirationTime = 0;
    }
}

// OFFSET: 0x005A57F0
uint32_t CGItem_C::GetExpirationTimeLeft() const {
    if (!this->m_expirationTime) {
        return 0;
    }

    int32_t left = this->m_expirationTime - static_cast<uint32_t>(time(NULL));

    return left > 0 ? left : 0;
}

// PROVEN: bfc33732 IDENTICAL/NONE
// OFFSET: 0x005A5820/0x140216790
int32_t CGItem_C::GetSelectionHighlightColor(CImVector* color) const {
    CImVector white(0xFF, 0xFF, 0xFF, 0xFF);
    *color = white;

    return 1;
}

// OFFSET: 0x005A5850/0x1402167B0
bool CGItem_C::HasSoulboundEnchant() {
    if (this->GetItemStaticFlags(0) & 0x2000) {
        return false;
    }

    for (uint32_t i = 0; i < 15; i++) {
        if (i == 8 || i == 9) {
            continue;
        }

        uint32_t id = this->m_item->m_itemFieldEnchantment[i].m_id;

        if (!id) {
            continue;
        }

        SpellItemEnchantmentRec* rec = g_spellItemEnchantmentDB.GetRecord(id);

        if (rec && (rec->m_flags & 0x1)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x005A58D0/0x140216850
bool CGItem_C::ReforgeIsValid(const ItemRecSparse* stats, const ItemReforgeRec* reforge, int32_t randomPropertiesId, int32_t propertySeed, int32_t& reason) {
    if (stats->m_itemLevel < 200) {
        reason = 4;
        return false;
    }

    int32_t hasSourceStat = 0;

    for (uint32_t i = 0; i < 10; i++) {
        int32_t stat = stats->m_statModifierBonusStat[i];

        if (stat < 0) {
            continue;
        }

        int32_t amount = stats->m_statModifierBonusAmount[i];

        if (stat == reforge->m_targetStat && amount > 0) {
            reason = 3;
            return false;
        }

        if (stat == reforge->m_sourceStat && amount > 0 && static_cast<int32_t>(static_cast<float>(static_cast<int32_t>(static_cast<float>(amount) * reforge->m_sourceMultiplier)) * reforge->m_targetMultiplier) <= 0) {
            reason = 2;
            return false;
        }

        if (!hasSourceStat && stat == reforge->m_sourceStat && amount > 0 && static_cast<int32_t>(static_cast<float>(amount) * reforge->m_sourceMultiplier) > 0) {
            hasSourceStat = 1;
        }
    }

    int32_t invalid = 0;
    int32_t propertyHasSourceStat = 0;
    EvalutateReforgeOfRandomProp(randomPropertiesId, static_cast<uint16_t>(propertySeed), reforge, propertyHasSourceStat, invalid, reason);

    if (!hasSourceStat && !propertyHasSourceStat) {
        reason = 1;
        return false;
    }

    return !invalid;
}

// OFFSET: 0x005A59F0/0x1402169D0
void CGItem_C::ModStatByReforge(int32_t level, const ItemRecSparse* stats, const ItemReforgeRec* reforge, int32_t stat, int32_t& value, int32_t randomPropertiesId, int32_t propertySeed) {
    if (stat == reforge->m_sourceStat) {
        value -= static_cast<int32_t>(static_cast<float>(value) * reforge->m_sourceMultiplier);
        return;
    }

    if (stat != reforge->m_targetStat) {
        return;
    }

    int32_t amount = 0;
    uint32_t i;

    for (i = 0; i < 10; i++) {
        if (stats->m_statModifierBonusStat[i] >= 0 && stats->m_statModifierBonusStat[i] == reforge->m_sourceStat) {
            amount = CalculateItemBonusFromPercent(level, stats->m_itemLevel, stats->m_statModifierBonusAmount[i], stats->m_statPercentEditor[i], stats->m_statPercentageOfSocket[i], stats->m_quality, stats->m_inventoryType);
            break;
        }
    }

    if (i == 10) {
        amount = CalculateItemBonusFromRandomEnchant(reforge->m_sourceStat, randomPropertiesId, static_cast<uint16_t>(propertySeed));
    }

    value += static_cast<int32_t>(static_cast<float>(static_cast<int32_t>(static_cast<float>(amount) * reforge->m_sourceMultiplier)) * reforge->m_targetMultiplier);
}

// OFFSET: 0x005A5B00
static int32_t OnUpdateSpellCharges(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item->m_item->m_itemFieldOwner == ClntObjMgrGetActivePlayer()) {
        CGActionBar::UpdateItem(item->m_obj->m_entryID);
        CGContainerInfo::UpdateCooldowns();
    }

    return 1;
}

// OFFSET: 0x005A5B60
static void ItemIDChangedCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        int32_t slot = player->m_bag.FindItemSlot(guid);

        if (slot == 15 || slot == 16) {
            CGActionBar::UpdateAttackAction();
        } else if (slot == 17) {
            CGActionBar::UpdateRangedAttackAction();
        }
    }

    CGActionBar::UpdateItem(id);
    CGContainerInfo::UpdateItem(guid);
    CGCharacterInfo::UpdateItem(guid);
}

// OFFSET: 0x005A5BE0
static int32_t OnUpdateDynamicFlags(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item && ((*static_cast<const uint8_t*>(oldValue) ^ item->m_item->m_itemFieldFlags) & 0x10)) {
        CGPlayer_C* owner = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(item->m_item->m_itemFieldOwner, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (owner && owner->m_obj->m_guid == ClntObjMgrGetActivePlayer() && static_cast<uint32_t>(owner->GetBag()->FindItemSlot(item->m_obj->m_guid)) <= 18) {
            CGCharacterInfo::UpdateInventoryAlerts();
        }
    }

    return 1;
}

// OFFSET: 0x005A5C90
static int32_t OnUpdateDurability(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        CGPlayer_C* owner = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(item->m_item->m_itemFieldOwner, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (owner) {
            int32_t oldDurability = *static_cast<const int32_t*>(oldValue);

            if ((item->m_item->m_itemFieldFlags & 0x8) || !item->m_item->m_itemFieldDurability) {
                int32_t invSlot = -1;
                int32_t slot = owner->GetBag()->FindItemSlot(item->m_obj->m_guid);

                if (slot == 15) {
                    invSlot = 15;
                } else if (slot == 16) {
                    invSlot = 16;
                } else if (slot == 17) {
                    invSlot = (item->m_virtualItem.m_inventoryType != 26) + 15;
                }

                if (oldDurability > 0) {
                    owner->RemoveHandItemModel(invSlot, item->m_virtualItem.m_sheatheType, item->m_virtualItem.m_inventoryType == 14);
                    WOWGUID ownerGuid = owner->m_obj->m_guid;
                    CGGameUI::UnitModelUpdate(ownerGuid, 3);
                }
            } else if (!(item->m_item->m_itemFieldFlags & 0x8) && item->m_item->m_itemFieldDurability && !oldDurability) {
                owner->EquipItem(item);
                WOWGUID ownerGuid = owner->m_obj->m_guid;
                CGGameUI::UnitModelUpdate(ownerGuid, 3);
            }
        }
    }

    CGCharacterInfo::UpdateInventoryAlerts();

    return 1;
}

// PROVEN: 07c61422 IDENTICAL/NONE
// OFFSET: 0x005A5DD0/0x140216EC0
int32_t CGItem_C::HasOnUseEnchant() const {
    return this->GetSpellItemEnchantmentId() != 0;
}

// OFFSET: 0x005A5DE0/0x140216EE0
int32_t CGItem_C::HasUseEffect() {
    if (this->GetItemStaticFlags(0) & 0x40) {
        return 1;
    }

    return this->GetSpellItemEnchantmentId() ? 1 : 0;
}

// OFFSET: 0x005A5E10
void CGItem_C::UpdateEnchantments(uint16_t changed) {
    if (this->GetItemStaticFlags(0) & 0x2000) {
        return;
    }

    CGPlayer_C* owner = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(this->m_item->m_itemFieldOwner, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!owner) {
        return;
    }

    if (owner->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
        CGActionBar::UpdateItem(this->m_obj->m_entryID);
        CGTradeInfo::UpdatePlayerItem(this->m_obj->m_guid);
        CGContainerInfo::UpdateItem(this->m_obj->m_guid);
        CGCharacterInfo::UpdateItem(this->m_obj->m_guid);
        CGCharacterInfo::UpdateInventoryAlerts();
        CGItemSocketInfo::UpdateItem(this->m_obj->m_guid);

        if (changed & 0x100) {
            CGReforge::UpdateEnchantment(this->m_obj->m_guid);
        }
    }

    if (!(changed & 0x203)) {
        return;
    }

    uint8_t slot = owner->FindSlotIndex(this->m_obj->m_guid);

    if (slot > 18) {
        return;
    }

    if (changed & 0x3) {
        owner->UpdateObjComponentVisualsSlot(owner->GetVisibleItem(slot), slot);
    } else if (changed & 0x200) {
        CGTransmogrify::OnTransmogrifySuccess(slot);
    }
}

// OFFSET: 0x005A5F50/0x140217050
int32_t CGItem_C::GetPrismaticSocketCount() {
    if (this->GetItemStaticFlags(0) & 0x2000) {
        return 0;
    }

    uint32_t id = this->m_item->m_itemFieldEnchantment[6].m_id;

    if (!id) {
        return 0;
    }

    SpellItemEnchantmentRec* rec = g_spellItemEnchantmentDB.GetRecord(id);

    if (!rec) {
        return 0;
    }

    for (uint32_t i = 0; i < 3; i++) {
        if (rec->m_effect[i] == 8) {
            return rec->m_effectPointsMin[i];
        }
    }

    return 0;
}

// OFFSET: 0x005A5FC0
void CGItem_C::UpdateSockets(uint32_t a2, uint32_t a3, uint32_t a4, uint32_t a5) {
    uint16_t changed = 0;

    if (a2) {
        this->m_item->m_itemFieldEnchantment[2].m_id = a2;
        changed = 0x4;
    }

    if (a3) {
        this->m_item->m_itemFieldEnchantment[3].m_id = a3;
        changed |= 0x8;
    }

    if (a4) {
        this->m_item->m_itemFieldEnchantment[4].m_id = a4;
        changed |= 0x10;
    }

    if (a5) {
        this->m_item->m_itemFieldEnchantment[5].m_id = a5;
        changed |= 0x20;
    }

    this->UpdateEnchantments(changed);
    FrameScript_SignalEvent(SCRIPT_EVENT_SOCKET_INFO_SUCCESS, NULL);
}

// OFFSET: 0x005A5AC0/0x140216B40
bool CGItem_C::IsTemporarilyBoundForTrade() {
    return this->HasSoulboundEnchant();
}

// OFFSET: 0x005A6030/0x140217160
bool CGItem_C::IsBound() {
    if (this->m_item->m_itemFieldFlags & 0x1) {
        return true;
    }

    return this->HasSoulboundEnchant();
}

// OFFSET: 0x005A6050/0x140217190
void CGItem_C::RequestItemPurchaseData() {
    if (this->m_itemFlags & 0x4) {
        return;
    }

    this->m_itemFlags |= 0x4;

    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GET_ITEM_PURCHASE_DATA));
    msg.Put(this->m_obj->m_guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x005A6130/0x140217290
bool CGItem_C::HasEnchantments() {
    if (!(this->GetItemStaticFlags(0) & 0x2000) && this->m_item->m_itemFieldEnchantment[0].m_id) {
        return true;
    }

    for (uint32_t i = 2; i < 4; i++) {
        if (!(this->GetItemStaticFlags(0) & 0x2000) && this->m_item->m_itemFieldEnchantment[i].m_id) {
            return true;
        }
    }

    return !(this->GetItemStaticFlags(0) & 0x2000) && this->m_item->m_itemFieldEnchantment[6].m_id;
}

// OFFSET: 0x005A61B0/0x140217340
bool CGItem_C::IsPermanentlyBoundForTrade() {
    if (!(this->m_item->m_itemFieldFlags & 0x1) && !this->HasSoulboundEnchant()) {
        return false;
    }

    if (this->m_item->m_itemFieldFlags & 0x100) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && this->m_item->m_itemFieldOwner == player->m_obj->m_guid) {
            int32_t createPlayedTime = this->m_item->m_itemFieldCreatePlayedTime;

            return createPlayedTime - player->GetPlayedTime() + 7200 <= 0;
        }
    }

    return true;
}

// OFFSET: 0x005A6240
static int32_t OnUpdateEnchantments(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        const ItemEnchantment* old = static_cast<const ItemEnchantment*>(oldValue);
        uint16_t changed = 0;

        for (uint32_t i = 0; i < 15; i++) {
            uint32_t id = (item->GetItemStaticFlags(0) & 0x2000) ? 0 : item->m_item->m_itemFieldEnchantment[i].m_id;

            if (id != old[i].m_id) {
                changed = static_cast<uint16_t>(1 << i);
            }
        }

        item->UpdateEnchantments(changed);
    }

    return 1;
}

// PROVEN: 195b6a47 IDENTICAL/NONE
// OFFSET: 0x005A62D0/0x1402174C0
bool CGItem_C::IsBoundForTrade() {
    return this->IsPermanentlyBoundForTrade() || this->HasSoulboundEnchant();
}

// OFFSET: 0x005A6300/0x140217550
int32_t GetItemIDByName(const char* name) {
    const char* link = SStrStr(name, "item:");

    if (link) {
        return SStrToInt(link + 5);
    }

    ITEMBYNAME* entry = s_itemByName.Ptr(name);

    if (entry) {
        return entry->m_itemId;
    }

    return 0;
}

// OFFSET: 0x005A6350/0x1402175A0
void CGItem_C::RemoveNameFromCache(const char* name) {
    ITEMBYNAME* entry = s_itemByName.Ptr(name);

    if (entry) {
        s_itemByName.Delete(entry);
    }
}

// OFFSET: 0x005A69B0
static int32_t OnUpdateStackCount(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item->m_item->m_itemFieldOwner == ClntObjMgrGetActivePlayer()) {
        ItemRecSparse* stats = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

        if (stats && stats->m_stackable > 1) {
            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (player) {
                player->m_bag.UnlockItemsOfType(item->m_obj->m_entryID, 8);
            }
        } else {
            CGGameUI::UnlockItem(item->m_obj->m_guid);
        }

        CGTradeSkillInfo::RefreshList(0);
        CGActionBar::UpdateItem(item->m_obj->m_entryID);
        CGTradeInfo::UpdatePlayerItem(item->m_obj->m_guid);
        CGContainerInfo::UpdateItem(item->m_obj->m_guid);
        CGCharacterInfo::UpdateItem(item->m_obj->m_guid);
        CGQuestLog::UpdateQuestItem(item->m_obj->m_entryID);
        CGItemSocketInfo::RemoveItem(item->m_obj->m_guid);
    }

    return 1;
}

// OFFSET: 0x005A6AE0
static int32_t OnUpdateItemID(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item && item->m_item->m_itemFieldOwner == ClntObjMgrGetActivePlayer()) {
        CGActionBar::UpdateItem(*static_cast<const int32_t*>(oldValue));
        WOWGUID itemGuid = item->m_obj->m_guid;

        if (g_itemSparseDB.GetRecord(item->m_obj->m_entryID, itemGuid, ItemIDChangedCacheCallback, NULL, false)) {
            itemGuid = item->m_obj->m_guid;
            ItemIDChangedCacheCallback(item->m_obj->m_entryID, itemGuid, NULL, true);
        }

        Script_SendUniqueSignal(SCRIPT_EVENT_QUEST_LOG_UPDATE);
    }

    return 1;
}

// OFFSET: 0x005A6BA0/0x140218140
char* CGItem_C::BuildItemName(char* buffer, uint32_t size, int32_t itemId, int32_t randomPropertiesId) {
    *buffer = '\0';

    ItemRecSparse* stats = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

    if (!stats) {
        return NULL;
    }

    const char* name = stats->GetName(0);

    if (!name) {
        return NULL;
    }

    if (randomPropertiesId > 0) {
        ItemRandomPropertiesRec* rec = g_itemRandomPropertiesDB.GetRecord(randomPropertiesId);

        if (rec && rec->m_nameLang && *rec->m_nameLang) {
            SStrPrintf(buffer, size, FrameScript_GetText("ITEM_SUFFIX_TEMPLATE", -1, GENDER_NOT_APPLICABLE), name, static_cast<const char*>(rec->m_nameLang));

            return buffer;
        }
    }

    if (randomPropertiesId < 0) {
        ItemRandomSuffixRec* rec = g_itemRandomSuffixDB.GetRecord(-randomPropertiesId);
        const char* suffix = rec ? rec->m_name : "";
        SStrPrintf(buffer, size, FrameScript_GetText("ITEM_SUFFIX_TEMPLATE", -1, GENDER_NOT_APPLICABLE), name, suffix);

        return buffer;
    }

    SStrCopy(buffer, name, size);

    return buffer;
}

// OFFSET: 0x005A6CC0/0x1402182E0
char* CGItem_C::GetFullItemName(char* buffer, uint32_t size) const {
    int32_t randomPropertiesId = (this->m_item->m_itemFieldFlags & 0x8) ? 0 : this->m_item->m_itemFieldRandomPropertiesId;

    return CGItem_C::BuildItemName(buffer, size, this->m_obj->m_entryID, randomPropertiesId);
}

// OFFSET: 0x005A6D00/0x140218320
int32_t CGItem_C::GetOnUseSlot() const {
    WOWGUID guid = ClntObjMgrGetActivePlayer();
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, guid, NULL, NULL, false);

    if (!stats) {
        return -1;
    }

    for (int32_t i = 0; i < 5; i++) {
        if (stats->m_spellID[i] && !stats->m_spellTrigger[i]) {
            return i;
        }
    }

    return -1;
}

// OFFSET: 0x005A6D60/0x140218390
int32_t CGItem_C::GetUseSpell(int32_t itemId) {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

    if (!stats) {
        return 0;
    }

    for (uint32_t i = 0; i < 5; i++) {
        if (stats->m_spellID[i] && !stats->m_spellTrigger[i]) {
            return stats->m_spellID[i];
        }
    }

    return 0;
}

// OFFSET: 0x005A6DC0/0x140218400
int32_t CGItem_C::GetEnchantmentSpell(int32_t effect) {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    if (!stats) {
        return 0;
    }

    int32_t spellId = 0;

    for (uint32_t i = 0; i < 5; i++) {
        if (!stats->m_spellID[i] || stats->m_spellTrigger[i]) {
            continue;
        }

        bool match = !effect;

        if (!match) {
            uint32_t count;
            const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(stats->m_spellID[i], count);

            for (uint32_t j = 0; j < count; j++) {
                const SpellEffectRec* rec = effects->m_data[j];

                if (rec && rec->m_effect == effect) {
                    match = true;
                    break;
                }
            }
        }

        if (match) {
            spellId = stats->m_spellID[i];

            if (spellId) {
                return spellId;
            }

            break;
        }
    }

    for (uint32_t slot = 0; slot < 15; slot++) {
        if (slot == 8 || slot == 9) {
            continue;
        }

        uint32_t id = this->m_item->m_itemFieldEnchantment[slot].m_id;

        if (id) {
            SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(id);

            if (enchant) {
                for (uint32_t i = 0; i < 3; i++) {
                    if (enchant->m_effect[i] == 7) {
                        spellId = enchant->m_effectArg[i];
                        break;
                    }
                }
            }
        }

        if (spellId) {
            break;
        }
    }

    return spellId;
}

// OFFSET: 0x005A6F00/0x140218580
int32_t CGItem_C::GetCurrentSpellCharges() {
    if (this->GetSpellItemEnchantmentId()) {
        SpellItemEnchantmentRec* rec = g_spellItemEnchantmentDB.GetRecord(this->GetSpellItemEnchantmentId());

        if (rec) {
            for (uint32_t i = 0; i < 3; i++) {
                if (rec->m_effect[i] == 7 && rec->m_effectArg[i]) {
                    if (!(this->GetItemStaticFlags(0) & 0x2000)) {
                        return this->m_item->m_itemFieldEnchantment[i].m_charges;
                    }

                    return 0;
                }
            }
        }
    }

    uint32_t slot = this->GetOnUseSlot();

    if (slot <= 4) {
        return this->m_item->m_itemFieldSpellCharges[slot];
    }

    return 0;
}

// OFFSET: 0x005A6FA0/0x1402186C0
int32_t CGItem_C::NumBaseCharges(int32_t itemId) {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

    if (!stats) {
        return 0;
    }

    for (uint32_t i = 0; i < 5; i++) {
        if (stats->m_spellID[i] && !stats->m_spellTrigger[i]) {
            return stats->m_spellCharges[i];
        }
    }

    return 0;
}

// OFFSET: 0x005A7000/0x140218730
void CGItem_C::GetCooldowns(int32_t itemId, uint32_t& cooldown, uint32_t& categoryCooldown, uint32_t& category) {
    cooldown = 0;
    categoryCooldown = 0;
    category = 0;

    ItemRecSparse* stats = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

    if (!stats) {
        return;
    }

    for (uint32_t i = 0; i < 5; i++) {
        if (stats->m_spellID[i] && !stats->m_spellTrigger[i]) {
            cooldown = stats->m_spellCooldown[i];
            categoryCooldown = stats->m_spellCategoryCooldown[i];
            category = stats->m_spellCategory[i];
            return;
        }
    }
}

// OFFSET: 0x005A70A0/0x1402187E0
void CGItem_C::Initialize() {
    ItemSparseInitialize();
    ClntObjMgrSetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldStackCount), sizeof(uint32_t), OnUpdateStackCount, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldSpellCharges), sizeof(((CGItemData*)0)->m_itemFieldSpellCharges), OnUpdateSpellCharges, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldEnchantment), sizeof(((CGItemData*)0)->m_itemFieldEnchantment), OnUpdateEnchantments, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_OBJECT, offsetof(CGObjectData, m_entryID), sizeof(uint32_t), OnUpdateItemID, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldFlags), sizeof(uint16_t), OnUpdateDynamicFlags, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldDurability), sizeof(uint32_t), OnUpdateDurability, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
}

// OFFSET: 0x005A7140
void CGItem_C::Shutdown() {
    ClntObjMgrUnsetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldStackCount), OnUpdateStackCount, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldSpellCharges), OnUpdateSpellCharges, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldEnchantment), OnUpdateEnchantments, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_OBJECT, offsetof(CGObjectData, m_entryID), OnUpdateItemID, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldFlags), OnUpdateDynamicFlags, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_ITEM, offsetof(CGItemData, m_itemFieldDurability), OnUpdateDurability, NULL);
    s_inventoryArt.Clear();
    s_itemByName.Clear();
}

// OFFSET: 0x005A71D0/0x140218A50
int32_t CGItem_C::GetNumSockets() {
    int32_t sockets = 0;

    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    if (stats) {
        if (stats->m_socketColor[0]) {
            sockets = 1;
        }

        if (stats->m_socketColor[1]) {
            sockets++;
        }

        if (stats->m_socketColor[2]) {
            sockets++;
        }
    }

    return sockets + this->GetPrismaticSocketCount();
}

// OFFSET: 0x005A7230
int32_t CGItem_C::GetPageTextID(void (*callback)(int32_t, const WOWGUID&, void*, bool)) const {
    WOWGUID guid = this->m_obj->m_guid;
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, guid, callback, NULL, false);

    return stats ? stats->m_pageText : 0;
}

// OFFSET: 0x005A7280
const char* CGItem_C::GetObjectName() const {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    return stats ? stats->GetName(0) : NULL;
}

// OFFSET: 0x005A72D0/0x140218B60
uint32_t CGItem_C::GetClassID() const {
    ItemRec* rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    return rec ? rec->m_classID : 0;
}

// OFFSET: 0x005A7310/0x140218BB0
uint32_t CGItem_C::GetSubtypeID() const {
    ItemRec* rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    return rec ? rec->m_subclassID : 0;
}

// OFFSET: 0x005A7350/0x140218C00
uint32_t CGItem_C::GetInventoryType() const {
    ItemRec* rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    return rec ? rec->m_inventoryType : 0;
}

// OFFSET: 0x005A7390/0x140218C50
uint32_t CGItem_C::GetBagFamily() const {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    return stats ? stats->m_bagFamily : 0;
}

// OFFSET: 0x005A73E0/0x140218CA0
uint32_t CGItem_C::GetDisplayID() const {
    ItemRec* rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    return rec ? rec->m_displayInfoID : 0;
}

// OFFSET: 0x005A7420/0x140218CF0
uint32_t CGItem_C::GetSheatheType() const {
    ItemRec* rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    return rec ? rec->m_sheatheType : 0;
}

// OFFSET: 0x005A7460/0x140218D40
int32_t CGItem_C::CanGoInSlot(int32_t itemId, uint32_t slot, int32_t bypass) {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

    if (!stats) {
        return 0;
    }

    ItemRec* rec = g_itemDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

    if (!rec) {
        return 0;
    }

    if (slot == 17) {
        if (bypass != (rec->m_inventoryType == 28)) {
            return 0;
        }

        return (s_inventoryTypeSlots[rec->m_inventoryType] & (1 << slot)) != 0;
    }

    if ((slot != 15 && slot != 16) || rec->m_classID != 2) {
        if (slot >= 23) {
            return 1;
        }

        return (s_inventoryTypeSlots[rec->m_inventoryType] & (1 << slot)) != 0;
    }

    if (slot == 16) {
        if (rec->m_inventoryType != 17 && rec->m_inventoryType != 21) {
            return (s_inventoryTypeSlots[rec->m_inventoryType] & (1 << slot)) != 0;
        }
    } else if (rec->m_inventoryType != 22) {
        return (s_inventoryTypeSlots[rec->m_inventoryType] & (1 << slot)) != 0;
    }

    if ((stats->m_flags[0] & 0x40000000) || rec->m_subclassID == 13) {
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    int32_t count = player->m_equipBypassSkills.m_count;

    for (int32_t i = 0; i < count; i++) {
        SpellRec* spell = g_spellDB.GetRecord(player->GetEquipBypassSkill(i));

        if (!spell || !spell->m_spellEquippedItemsID) {
            continue;
        }

        SpellEquippedItemsRec* equipped = g_spellEquippedItemsDB.GetRecord(spell->m_spellEquippedItemsID);

        if (!equipped || rec->m_classID != equipped->m_equippedItemClass || !((1 << rec->m_subclassID) & equipped->m_equippedItemSubclass)) {
            continue;
        }

        bool found = false;

        for (int32_t invType = 0; invType < 29; invType++) {
            if (((1 << invType) & equipped->m_equippedItemInvTypes) && ((1 << slot) & s_inventoryTypeSlots[invType])) {
                found = true;
            }
        }

        if (found) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x005A7650/0x140218FF0
int32_t CGItem_C::CanGoInSlot(uint32_t slot, int32_t bypass) {
    return CGItem_C::CanGoInSlot(this->m_obj->m_entryID, slot, bypass);
}

// OFFSET: 0x005A7670/0x140219000
ItemRecSparse* CGItem_C::GetStats() const {
    return g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
}

// OFFSET: 0x005A76A0
uint32_t CGItem_C::GetItemStaticFlags(int32_t index) {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    return stats ? stats->m_flags[index] : 0;
}

// OFFSET: 0x005A76F0/0x1402190A0
bool CGItem_C::IsBoundToAccount() {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    return stats && (stats->m_flags[0] & 0x8000000) == 0x8000000;
}

// OFFSET: 0x005A7750
bool CGItem_C::IsBoundToBattlenetAccount() {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    if (!stats) {
        return false;
    }

    return (stats->m_flags[0] & 0x8000000) && (stats->m_flags[1] & 0x20000);
}

// OFFSET: 0x005A77B0/0x140219150
bool CGItem_C::ReforgeIsValid(const ItemReforgeRec* reforge, int32_t& reason) {
    reason = 0;

    if (!g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false)) {
        return false;
    }

    int32_t randomPropertiesId = this->m_item->m_itemFieldRandomPropertiesId;
    uint32_t propertySeed = this->m_item->m_itemFieldPropertySeed;
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    return CGItem_C::ReforgeIsValid(stats, reforge, randomPropertiesId, static_cast<uint16_t>(propertySeed), reason);
}

// OFFSET: 0x005A7840
int32_t CGItem_C::GetRepairCost() {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    if (!stats) {
        return 0;
    }

    CGItemData* item = this->m_item;

    if ((item->m_itemFieldFlags & 8) || !item->m_itemFieldMaxdurability) {
        return 0;
    }

    uint32_t durability = (item->m_itemFieldFlags & 8) ? 0 : item->m_itemFieldDurability;

    if ((item->m_itemFieldFlags & 8) || durability >= item->m_itemFieldMaxdurability) {
        return 0;
    }

    DurabilityCostsRec* costs = g_durabilityCostsDB.GetRecord(stats->m_itemLevel);

    if (!costs) {
        return 0;
    }

    uint32_t itemClass = this->GetClassID();

    if (itemClass != 2 && itemClass != 4) {
        return 0;
    }

    uint32_t subClass = this->GetSubtypeID();
    int32_t cost;

    if (itemClass == 2) {
        if (subClass > 21) {
            return 0;
        }

        cost = costs->m_weaponSubClassCost[subClass];
    } else {
        if (itemClass != 4 || subClass > 8) {
            return 0;
        }

        cost = costs->m_armorSubClassCost[subClass];
    }

    DurabilityQualityRec* quality = g_durabilityQualityDB.GetRecordByIndex(2 * stats->m_quality + 1);

    if (!quality) {
        return 0;
    }

    int32_t lost = this->GetMaxDurability() - this->GetDurability();
    int32_t result = CMath::fint_n(static_cast<float>(lost) * quality->m_data * static_cast<float>(cost));

    return result ? result : 1;
}

// OFFSET: 0x005A79C0/0x140219440
int32_t CGItem_C::GetTransmogrifyCost() {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    if (!stats) {
        return 0;
    }

    if (stats->GetSellPrice() <= 10000) {
        return 10000;
    }

    return stats->GetSellPrice();
}

// OFFSET: 0x005A7A20
bool CGItem_C::IsZoneBound() const {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    return stats && stats->m_zoneBound;
}

// OFFSET: 0x005A7A70
bool CGItem_C::IsInstanceBound() const {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    return stats && stats->m_instanceBound;
}

// OFFSET: 0x005A7AC0
int32_t CGItem_C::IsLocked(const LockRec* lock, int32_t itemLevel, int32_t* spellId, int32_t* skillValue, int32_t* requiredSkill, int32_t* lockIndex, CGItem_C** key, int32_t* entry) {
    int32_t locked = 0;

    for (int32_t i = 0; i < 8; i++) {
        int32_t type = lock->m_type[i];

        if (!type) {
            continue;
        }

        if (type == 2) {
            locked = 1;

            for (uint32_t j = 0; j < s_openLockSpells.m_count; j++) {
                const SpellRec* spell = g_spellDB.GetRecord(s_openLockSpells.m_data[j]);

                if (!spell) {
                    continue;
                }

                uint32_t count;
                const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

                for (uint32_t effect = 0; effect < count; effect++) {
                    const SpellEffectRec* rec = effects->m_data[effect];

                    if (!rec || rec->m_effect != 33 || rec->m_effectMiscValue != lock->m_index[i]) {
                        continue;
                    }

                    int32_t required = lock->m_skill[i] ? lock->m_skill[i] : 5 * itemLevel;
                    int32_t minPoints;
                    int32_t maxPoints;
                    Spell_C_GetMinMaxPoints(spell, effect, &minPoints, &maxPoints, 0, 0, 0, 0);

                    if (spellId) {
                        *spellId = s_openLockSpells.m_data[j];
                    }

                    if (skillValue) {
                        *skillValue = minPoints;
                    }

                    if (requiredSkill) {
                        *requiredSkill = required;
                    }

                    if (lockIndex) {
                        *lockIndex = lock->m_index[i];
                    }

                    if (minPoints >= required) {
                        if (entry) {
                            *entry = i;
                        }

                        return 0;
                    }
                }
            }
        } else if (type == 3) {
            locked = 1;
            const SpellRec* spell = g_spellDB.GetRecord(lock->m_index[i]);

            if (!spell) {
                continue;
            }

            uint32_t count;
            const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

            for (uint32_t effect = 0; effect < count; effect++) {
                const SpellEffectRec* rec = effects->m_data[effect];

                if (rec && rec->m_effect == 33) {
                    if (spellId) {
                        *spellId = spell->m_id;
                    }

                    if (entry) {
                        *entry = i;
                    }

                    return 0;
                }
            }
        } else if (type == 1) {
            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (player) {
                locked = 1;
                CGItem_C* item = player->m_bag.FindItemOfType(lock->m_index[i], 0);

                if (item) {
                    if (spellId) {
                        *spellId = item->GetEnchantmentSpell(0);
                    }

                    if (key) {
                        *key = item;
                    }

                    if (entry) {
                        *entry = i;
                    }

                    return 0;
                }
            }
        }
    }

    return locked;
}

// OFFSET: 0x005A7D30/0x1402198B0
int32_t CGItem_C::GetRandomItemSlot() const {
    ItemRec* rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
    uint32_t inventoryType = rec ? rec->m_inventoryType : 0;

    switch (inventoryType) {
        case 0:
        case 18:
        case 19:
        case 24:
        case 27:
        case 28:
            return -1;

        case 1:
        case 4:
        case 5:
        case 7:
        case 17:
        case 20:
            return 0;

        case 2:
        case 9:
        case 11:
        case 14:
        case 16:
        case 23:
            return 2;

        case 3:
        case 6:
        case 8:
        case 10:
        case 12:
            return 1;

        case 13:
        case 21:
        case 22:
            return 3;

        case 15:
        case 25:
        case 26:
            return 4;

        default:
            return -1;
    }
}

// OFFSET: 0x005A7E00/0x140219990
int32_t CGItem_C::GetRandomPropertyPoints() const {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    if (!stats) {
        return 0;
    }

    int32_t itemLevel = stats->m_itemLevel;
    int32_t quality = stats->m_quality;
    uint32_t slot = this->GetRandomItemSlot();

    if (slot > 4) {
        return 0;
    }

    RandPropPointsRec* rec = g_randPropPointsDB.GetRecord(itemLevel);

    if (!rec) {
        return 0;
    }

    switch (quality) {
        case 2:
            return rec->m_good[slot];

        case 3:
            return rec->m_superior[slot];

        case 4:
            return rec->m_epic[slot];

        default:
            return 0;
    }
}

// OFFSET: 0x005A7EB0/0x140219A70
int32_t CGItem_C::GetRandomPropertyOverrideAmount(int32_t slot) {
    int32_t randomPropertiesId = this->m_item->m_itemFieldRandomPropertiesId;
    int32_t points = this->GetRandomPropertyPoints();

    return CGItem_C::GetRandomPropertyOverrideAmountEx(slot, randomPropertiesId, points);
}

// OFFSET: 0x005A8820
CGItem_C::CGItem_C(uint32_t time, const CClientObjCreate& create) : CGObject_C(time, create) {
    this->m_itemFlags = 0;
    this->m_expirationTime = 0;
    this->m_groupSounds = NULL;

    ClntObjMgrHideObject(this->m_obj->m_guid);

    ItemRec* rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
    this->m_virtualItem.m_classID = rec ? rec->m_classID : 0;

    rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
    this->m_virtualItem.m_subclassID = rec ? rec->m_subclassID : 0;

    rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
    this->m_virtualItem.m_material = rec ? rec->m_material : 0;

    rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
    this->m_virtualItem.m_inventoryType = rec ? rec->m_inventoryType : 0;

    rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
    this->m_virtualItem.m_sheatheType = rec ? rec->m_sheatheType : 0;

    rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
    this->m_virtualItem.m_soundOverrideSubclassID = rec ? rec->m_soundOverrideSubclassID : -1;

    memset(this->m_enchantmentExpiration, 0, sizeof(this->m_enchantmentExpiration));
    this->m_itemPurchaseData = NULL;
}

// PROVEN: ad527087 IDENTICAL/NONE
// OFFSET: 0x005A89E0/0x14021AA10
int32_t CGItem_C::CanBeUsed() {
    return this->GetEnchantmentSpell(0) != 0;
}

// OFFSET: 0x005A89F0/0x14021AA30
int32_t CGItem_C::GetUsableSpellCharges() const {
    uint32_t id = this->GetSpellItemEnchantmentId();

    if (id) {
        SpellItemEnchantmentRec* rec = g_spellItemEnchantmentDB.GetRecord(id);

        if (rec) {
            for (uint32_t i = 0; i < 3; i++) {
                if (rec->m_effectArg[i] && rec->m_effect[i] == 7) {
                    return rec->m_charges;
                }
            }
        }
    }

    return CGItem_C::NumBaseCharges(this->m_obj->m_entryID);
}

// OFFSET: 0x005A8A50/0x14021AB20
void CGItem_C::GetSpellCooldownInfo(uint32_t& cooldown, uint32_t& categoryCooldown, uint32_t& category) {
    cooldown = 0;
    categoryCooldown = 0;
    category = 0;

    if (this->GetSpellItemEnchantmentId()) {
        SpellRec* spell = g_spellDB.GetRecord(this->GetEnchantmentSpell(0));

        if (spell) {
            if (spell->m_spellCooldownsID) {
                SpellCooldownsRec* cooldowns = g_spellCooldownsDB.GetRecord(spell->m_spellCooldownsID);

                if (cooldowns) {
                    cooldown = cooldowns->m_recoveryTime;
                    categoryCooldown = cooldowns->m_categoryRecoveryTime;
                }
            }

            if (spell->m_spellCategoriesID) {
                SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID);

                if (categories) {
                    category = categories->m_category;
                }
            }

            return;
        }
    }

    CGItem_C::GetCooldowns(this->m_obj->m_entryID, cooldown, categoryCooldown, category);
}

// OFFSET: 0x005A8B40/0x14021AC30
int32_t CGItem_C::GetItemEnchantmentVisualID() const {
    ItemRec* rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
    int32_t displayId = rec ? rec->m_displayInfoID : 0;
    ItemDisplayInfoRec displayRec;

    if (!g_itemDisplayInfoDB.GetRecord(displayId, &displayRec)) {
        return 0;
    }

    if (g_itemVisualsDB.GetRecord(displayRec.m_itemVisual)) {
        return -1;
    }

    for (uint32_t i = 0; i < 15; i++) {
        if (i == 8 || i == 9) {
            continue;
        }

        SpellItemEnchantmentRec* enchantment = g_spellItemEnchantmentDB.GetRecord(this->m_item->m_itemFieldEnchantment[i].m_id);

        if (enchantment && enchantment->m_itemVisual) {
            return enchantment->m_itemVisual;
        }
    }

    return 0;
}

// OFFSET: 0x005A8C40/0x14021AD90
bool CGItem_C::CanBeUsedInArenas() {
    if ((this->m_obj->m_type & TYPEMASK_CONTAINER) || (this->m_itemFlags & 1)) {
        return false;
    }

    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    if (!stats) {
        return false;
    }

    if (stats->m_flags[0] & 0x200000) {
        return true;
    }

    if (stats->m_bonding == 4 || (stats->m_flags[0] & 0x4000000)) {
        return false;
    }

    uint32_t cooldown;
    uint32_t categoryCooldown;
    uint32_t category;
    this->GetSpellCooldownInfo(cooldown, categoryCooldown, category);
    Spell_C_ModifyCategoryRecovery(reinterpret_cast<int32_t&>(categoryCooldown), category, 1);

    return this->GetUsableSpellCharges() >= 0 && static_cast<int32_t>(cooldown) <= 900000 && static_cast<int32_t>(categoryCooldown) <= 900000;
}

// OFFSET: 0x005A8D00/0x14021AE60
bool CGItem_C::CanBeUsedInRatedBattleground() {
    if ((this->m_obj->m_type & TYPEMASK_CONTAINER) || (this->m_itemFlags & 1)) {
        return false;
    }

    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    if (!stats) {
        return false;
    }

    if (stats->m_flags[1] & 0x8000) {
        return true;
    }

    if (stats->m_bonding == 4 || (stats->m_flags[1] & 0x10000)) {
        return false;
    }

    uint32_t cooldown;
    uint32_t categoryCooldown;
    uint32_t category;
    this->GetSpellCooldownInfo(cooldown, categoryCooldown, category);
    Spell_C_ModifyCategoryRecovery(reinterpret_cast<int32_t&>(categoryCooldown), category, 1);

    return this->GetUsableSpellCharges() >= 0 && static_cast<int32_t>(cooldown) <= 900000 && static_cast<int32_t>(categoryCooldown) <= 900000;
}

// OFFSET: 0x005A8DC0
bool CGItem_C::CanBeStoredInVoidStorage(int32_t& error) {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    if (!stats) {
        error = ERR_INTERNAL_BAG_ERROR;
        return false;
    }

    if (this->m_item->m_itemFieldFlags & 0x8) {
        error = ERR_VOID_STORAGE_WRAPPED;
        return false;
    }

    if (stats->m_stackable != 1) {
        error = ERR_VOID_STORAGE_STACKABLE;
        return false;
    }

    if (!(this->m_item->m_itemFieldFlags & 0x1) && !this->HasSoulboundEnchant()) {
        error = ERR_VOID_STORAGE_UNBOUND;
        return false;
    }

    if (this->m_item->m_itemFieldFlags & 0x200) {
        error = ERR_VOID_STORAGE_MAIL;
        return false;
    }

    if (this->GetBag() && this->GetBag()->GetNumItems()) {
        error = ERR_VOID_STORAGE_BAG;
        return false;
    }

    if (this->GetDurability() != this->GetMaxDurability()) {
        error = ERR_VOID_STORAGE_REPAIR;
        return false;
    }

    if (this->GetCurrentSpellCharges() != this->GetUsableSpellCharges()) {
        error = ERR_VOID_STORAGE_CHARGES;
        return false;
    }

    if (stats->m_bonding == 4) {
        error = ERR_VOID_STORAGE_QUEST;
        return false;
    }

    if (stats->m_itemLimitCategory && !(g_itemLimitCategoryDB.GetRecord(stats->m_itemLimitCategory)->m_flags & 1)) {
        error = ERR_VOID_STORAGE_UNIQUE;
        return false;
    }

    if (stats->m_maxCount >= 1) {
        error = ERR_VOID_STORAGE_UNIQUE;
        return false;
    }

    if (this->GetItemStaticFlags(0) & 0x4) {
        error = ERR_VOID_STORAGE_LOOT;
        return false;
    }

    if (this->GetItemStaticFlags(0) & 0x2) {
        error = ERR_VOID_STORAGE_CONJURED;
        return false;
    }

    if (stats->m_holidayID) {
        error = ERR_VOID_STORAGE_HOLIDAY;
        return false;
    }

    if (static_cast<int32_t>(this->m_item->m_itemFieldDuration) > 0) {
        error = ERR_VOID_STORAGE_DURATION;
        return false;
    }

    error = ERR_MAX_VALUE;
    return true;
}

// OFFSET: 0x005A9140/0x14021B1F0
int32_t CGItem_C::UseItem(WOWGUID* target, int32_t a2) {
    if ((this->m_obj->m_type & TYPEMASK_CONTAINER) || (this->m_itemFlags & 1)) {
        return 0;
    }

    ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    if (!stats) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    int32_t spellId = this->GetEnchantmentSpell(0);
    const SpellRec* spell = g_spellDB.GetRecord(spellId);
    int16_t overrideId = *reinterpret_cast<const int16_t*>(player->m_player->m_playerFieldBytes2);

    if (overrideId) {
        OverrideSpellDataRec* overrideRec = g_overrideSpellDataDB.GetRecord(overrideId);

        if (overrideRec && !(overrideRec->m_flags & 9)) {
            CGGameUI::DisplayError(ERR_CLIENT_LOCKED_OUT);
            return 0;
        }
    }

    if (player->HasVehicleTransport() && !spell) {
        WOWGUID transport;
        transport.m_guid = player->GetTransportGUID();
        CGUnit_C* vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(transport, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (vehicle) {
            VehicleSeatRec* seat = vehicle->GetVehicleSeatRec(player->m_movementData.m_transportSeat);

            if (seat && !(seat->m_flags & 0x4000)) {
                CGGameUI::DisplayError(ERR_CLIENT_LOCKED_OUT);
                return 0;
            }
        }
    }

    if (Spell_C_IsTargeting() && Spell_C_CanTargetItems()) {
        CGGameUI::CastItemSpell(this->m_obj->m_guid, 0);
        return 0;
    }

    if (stats->m_flags[0] & 0x200) {
        if (this->m_item->m_itemFieldFlags & 0x8) {
            player->OpenWrappedItem(this);
        } else {
            CGPlayer_C::StartGiftWrap(this);
        }

        return 0;
    }

    if (stats->m_startQuest) {
        CGPlayer_C::QueryQuest(this->m_obj->m_guid, stats->m_startQuest);
        return 0;
    }

    bool arena = false;

    if (CGGameUI::IsInArenaOrRatedBattleground(&arena)) {
        if (arena) {
            if (!this->CanBeUsedInArenas()) {
                CGGameUI::DisplayError(ERR_CANT_USE_ITEM_IN_ARENA);
                return 0;
            }
        } else if (!this->CanBeUsedInRatedBattleground()) {
            CGGameUI::DisplayError(ERR_CANT_USE_ITEM_IN_RATED_BATTLEGROUND);
            return 0;
        }
    }

    int32_t error = ERR_MAX_VALUE;

    if (!player->CanUseItem(stats, error)) {
        if (error == ERR_CANT_EQUIP_LEVEL_I) {
            CGGameUI::DisplayError(ERR_CANT_EQUIP_LEVEL_I, stats->m_requiredLevel);
        } else {
            CGGameUI::DisplayError(static_cast<UIErrors>(error));
        }

        return 0;
    }

    uint32_t enchantId = this->GetSpellItemEnchantmentId();

    if (enchantId) {
        SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(enchantId);

        if (enchant && enchant->m_requiredSkillID && static_cast<int32_t>(player->GetSkillValue(player->GetSkillIndex(enchant->m_requiredSkillID))) < enchant->m_requiredSkillRank) {
            CGGameUI::DisplayError(static_cast<UIErrors>(GetDisplayErrorByInventoryError(2)));
            return 0;
        }
    }

    if (stats->m_pageText || (this->m_item->m_itemFieldFlags & 0x200)) {
        CGItemText::SetItem(this->m_obj->m_guid, 0);
        return 0;
    }

    if (!CGGameUI::m_hasControl) {
        int32_t allowed = 0;

        if (!spell) {
            CGGameUI::DisplayError(ERR_CLIENT_LOCKED_OUT);
            return 0;
        }

        if ((player->m_unit->m_unitFieldFlags & 0x800000) && ((spell->m_attributesExE & 0x20000) || Spell_C_CancelsFear(player, spell, 0))) {
            allowed = 1;
        }

        if ((player->m_unit->m_unitFieldFlags & 0x400000) && ((spell->m_attributesExE & 0x40000) || Spell_C_CancelsConfuse(player, spell, 0))) {
            allowed = 1;
        }

        if (player->IsCharmed() && ((spell->m_attributesExH & 0x20) || Spell_C_CancelsCharm(player, spell, 0))) {
            allowed = 1;
        }

        if ((!(player->m_unit->m_unitFieldFlags & 0x100000) || !(spell->m_attributes & 0x1000000)) && !allowed) {
            CGGameUI::DisplayError(ERR_CLIENT_LOCKED_OUT);
            return 0;
        }
    }

    if (stats->m_flags[0] & 0x4) {
        player->OpenLootItem(this);
        return 0;
    }

    if (stats->m_flags[0] & 0x2000) {
        CGPlayer_C::RequestPetitionSignatures(this->m_obj->m_guid);
        return 0;
    }

    if (!this->GetEnchantmentSpell(0)) {
        return 0;
    }

    if (this->GetInventoryType()) {
        if (this->m_item->m_itemFieldContained != ClntObjMgrGetActivePlayer()) {
            CGGameUI::DisplayError(ERR_MUST_EQUIP_ITEM);
            return 0;
        }

        if (this->GetMaxDurability() && !this->GetDurability()) {
            CGGameUI::DisplayError(ERR_MUST_REPAIR_DURABILITY);
            return 0;
        }

        int32_t slot = player->GetBag()->FindItemSlot(this->m_obj->m_guid);

        if (slot == 15) {
            if (player->IsDisarmed(0)) {
                CGGameUI::DisplayError(ERR_CANT_USE_DISARMED);
                return 0;
            }
        } else if (slot == 16) {
            if (player->IsDisarmed(1)) {
                CGGameUI::DisplayError(ERR_CANT_USE_DISARMED);
                return 0;
            }
        } else if (slot >= 23) {
            CGGameUI::DisplayError(ERR_MUST_EQUIP_ITEM);
            return 0;
        }
    }

    if (spell) {
        if (Spell_C_IsActiveSpell(spell, player)) {
            if (!Spell_C_WasItemRecentlyUsed(this->m_obj->m_entryID)) {
                Spell_C_CancelAura(spell, player, -1);
            }

            return 0;
        }

        if (this->GetUsableSpellCharges() > 0 && !this->GetCurrentSpellCharges()) {
            CGGameUI::DisplayError(ERR_SPELL_FAILED_S, FrameScript_GetText("SPELL_FAILED_NO_CHARGES_REMAIN", -1, GENDER_NOT_APPLICABLE));
            return 0;
        }

        if (SpellHasAuraEffect(spell, 78) && !player->CanCurrentFormMount() && player->CanAutoUnshift()) {
            player->CancelShapeshiftForMount();
        }
    }

    if (player->IsShapeShifted()) {
        SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(player->GetShapeshiftForm());

        if (form && !(form->m_flags & 0x80) && (!(form->m_flags & 0x40) || !this->GetInventoryType()) && !(stats->m_flags[0] & 0x800000)) {
            if (!player->CanAutoUnshift()) {
                CGGameUI::DisplayError(ERR_NO_ITEMS_WHILE_SHAPESHIFTED);
                return 0;
            }

            player->CancelShapeshift();
        }
    }

    if (stats->m_bonding == 3 && !this->IsBound() && !a2) {
        CGItem_C::s_bindOnUseItem = this->m_obj->m_guid;
        CGItem_C::s_bindOnUseTarget = *target;
        FrameScript_SignalEvent(SCRIPT_EVENT_USE_BIND_CONFIRM, NULL);
        return 0;
    }

    if (!(stats->m_flags[1] & 0x40000) || a2) {
        return Spell_C_CastSpell(spellId, this, *target, false);
    }

    CGItem_C::s_bindOnUseItem = this->m_obj->m_guid;
    CGItem_C::s_bindOnUseTarget = *target;
    FrameScript_SignalEvent(SCRIPT_EVENT_CONFIRM_BEFORE_USE, NULL);
    return 0;
}

// OFFSET: 0x005A9880/0x14021B970
void CGItem_C::ConfirmBindOnUse() {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGItem_C::s_bindOnUseItem, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        item->UseItem(&CGItem_C::s_bindOnUseTarget, 1);
    }

    CGItem_C::s_bindOnUseItem = WOWGUID();
}

// OFFSET: 0x005A98D0/0x14021B9C0
void CGItem_C::ConfirmOnUse() {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGItem_C::s_bindOnUseItem, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        item->UseItem(&CGItem_C::s_bindOnUseTarget, 1);
    }

    CGItem_C::s_bindOnUseItem = WOWGUID();
}

// OFFSET: 0x005A9920
static void AddInventoryArtHash(int32_t displayId, const char* texture) {
    if (s_inventoryArt.Ptr(displayId, HASHKEY_NONE())) {
        return;
    }

    INVENTORYART* art = s_inventoryArt.New(displayId, HASHKEY_NONE(), 0, 0);
    art->SetTexture(texture);
}

// OFFSET: 0x005A99B0/0x14021BC30
const char* CGItem_C::GetInventoryArt(int32_t displayId) {
    INVENTORYART* art = s_inventoryArt.Ptr(displayId, HASHKEY_NONE());

    if (art && art->m_texture) {
        return art->m_texture;
    }

    ItemDisplayInfoRec display;

    if (g_itemDisplayInfoDB.GetRecord(displayId, &display)) {
        const char* icon = ViolenceGetLevel() >= 2 || !*display.m_inventoryIcon[1] ? display.m_inventoryIcon[0] : display.m_inventoryIcon[1];

        if (icon) {
            if (*icon) {
                char buffer[260];

                if (TextureDiscoverFileType(icon) == TEXFILE_TGA) {
                    TexturePickAlternateFilename(icon, TEXFILE_TGA, buffer, sizeof(buffer));
                    icon = buffer;
                }

                AddInventoryArtHash(displayId, icon);
                art = s_inventoryArt.Ptr(displayId, HASHKEY_NONE());
                icon = art ? art->m_texture : NULL;
            }

            if (icon && *icon) {
                return icon;
            }
        }
    }

    SysMsgPrintf(STATUS_ERROR, 2, "NOINVENTORYICON|%d", displayId);
    return "INV_Misc_QuestionMark";
}

// OFFSET: 0x005A9AC0/0x14021BDA0
const char* CGItem_C::GetInventoryArt() const {
    ItemRec* rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    if (rec) {
        return CGItem_C::GetInventoryArt(rec->m_displayInfoID);
    }

    return CGItem_C::GetInventoryArt(0);
}

// OFFSET: 0x005A9B20
void CGItem_C::PostInitWithStats() {
    this->m_itemFlags |= 2;
    ItemRec* rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
    this->m_virtualItem.m_classID = rec ? rec->m_classID : 0;
    rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
    this->m_virtualItem.m_subclassID = rec ? rec->m_subclassID : 0;
    rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
    this->m_virtualItem.m_material = rec ? rec->m_material : 0;
    rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
    this->m_virtualItem.m_inventoryType = rec ? rec->m_inventoryType : 0;
    rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
    this->m_virtualItem.m_sheatheType = rec ? rec->m_sheatheType : 0;
    rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
    this->m_virtualItem.m_soundOverrideSubclassID = rec ? rec->m_soundOverrideSubclassID : -1;
    rec = g_itemDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
    ItemDisplayInfoRec display;

    if (g_itemDisplayInfoDB.GetRecord(rec ? rec->m_displayInfoID : 0, &display)) {
        this->m_groupSounds = g_itemGroupSoundsDB.GetRecord(display.m_groupSoundIndex);
    } else {
        this->m_groupSounds = NULL;
    }

    char name[1024];
    CGItem_C::BuildItemName(name, sizeof(name), this->m_obj->m_entryID, (this->m_item->m_itemFieldFlags & 0x8) ? 0 : this->m_item->m_itemFieldRandomPropertiesId);

    if (!s_itemByName.Ptr(name)) {
        ITEMBYNAME* entry = s_itemByName.New(name, 0, 0);
        entry->m_itemId = this->m_obj->m_entryID;
    }

    if (!CGGameUI::s_inWorld) {
        return;
    }

    if (this->m_item->m_itemFieldOwner == ClntObjMgrGetActivePlayer()) {
        ItemRecSparse* stats = g_itemSparseDB.GetRecord(this->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (stats->m_stackable <= 1) {
            CGGameUI::UnlockItem(this->m_obj->m_guid);
        } else if (player) {
            player->m_bag.UnlockItemsOfType(this->m_obj->m_entryID, 0);
        }

        if (player) {
            player->GetPendingEnchantTime(this);
            int32_t slot = player->m_bag.FindItemSlot(this->m_obj->m_guid);

            if (slot == 15 || slot == 16) {
                CGActionBar::UpdateAttackAction();
            } else if (slot == 17) {
                CGActionBar::UpdateRangedAttackAction();
            }
        }

        CGTradeSkillInfo::RefreshList(0);
        CGActionBar::UpdateItem(this->m_obj->m_entryID);
        CGQuestLog::UpdateQuestItem(this->m_obj->m_entryID);
        CGContainerInfo::UpdateItem(this->m_obj->m_guid);

        if ((stats->m_flags[0] & 0x1000) && !this->m_itemPurchaseData && !(this->m_itemFlags & 4)) {
            this->RequestItemPurchaseData();
        }

        if (CGActionBar::GetMultiCastSlotMaskForTotemCategory(stats->m_totemCategoryID)) {
            CGActionBar::UpdateMultiCastBar();
        }
    }

    Script_SendUnitSignal(this->m_item->m_itemFieldOwner, SCRIPT_EVENT_UNIT_INVENTORY_CHANGED);
}

// OFFSET: 0x005A9F70
static void LoadItemCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item || !success) {
        return;
    }

    item->PostInitWithStats();
    CGPlayer_C* owner = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(item->m_item->m_itemFieldOwner, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (owner) {
        owner->m_pendingItemCacheCount--;
    }
}

// OFFSET: 0x005A9FE0
void CGItem_C::RequestItemCacheInfo() {
    WOWGUID guid = this->m_obj->m_guid;

    if (g_itemSparseDB.GetRecord(this->m_obj->m_entryID, guid, LoadItemCacheCallback, this, false)) {
        this->PostInitWithStats();
        return;
    }

    CGPlayer_C* owner = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(this->m_item->m_itemFieldOwner, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (owner) {
        owner->m_pendingItemCacheCount++;
    }
}

// OFFSET: 0x005AA060
void CGItem_C::PostInit(uint32_t time, const CClientObjCreate& create, bool reenable) {
    this->CGObject_C::PostInit(time, create, reenable);
    this->RequestItemCacheInfo();
}

// OFFSET: 0x005AA090
void CGItem_C::PostReenable(const CClientObjCreate& create) {
    this->CGObject_C::PostReenable(create);
    this->RequestItemCacheInfo();
}

// OFFSET: 0x005AA0B0
void CGItem_C::Disable() {
    CGObject_C::Disable();

    if (CGLootInfo::m_lootObj == this->m_obj->m_guid) {
        CGGameUI::CloseLoot(true, false, false);
    }

    WOWGUID guid = this->m_obj->m_guid;
    Spell_C_CancelSpellByCaster(guid);

    if (this->m_obj->m_guid == CGGameUI::GetCursorItem()) {
        CGGameUI::ClearCursor(1, 1);
    }

    if (this->m_obj->m_guid == CGItemText::m_itemGUID) {
        CGItemText::SetItem(WOWGUID(), 0);
    }

    if (!(this->m_itemFlags & 0x2)) {
        g_itemSparseDB.CancelCallback(this->m_obj->m_entryID, LoadItemCacheCallback, this);
        CGPlayer_C* owner = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(this->m_item->m_itemFieldOwner, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (owner) {
            owner->m_pendingItemCacheCount--;
        }
    }

    if (this->m_item->m_itemFieldOwner == ClntObjMgrGetActivePlayer()) {
        CGTradeSkillInfo::RefreshList(0);
        CGActionBar::UpdateItem(this->m_obj->m_entryID);
        CGTradeInfo::RemovePlayerItem(this->m_obj->m_guid);
        CGQuestLog::UpdateQuestItem(this->m_obj->m_entryID);
        CGCharacterInfo::UpdateItem(this->m_obj->m_guid);
        CGItemSocketInfo::RemoveItem(this->m_obj->m_guid);
    }
}

// OFFSET: 0x005AA210/0x1400B2B70
CGItem_C::~CGItem_C() {
    if (!(this->m_itemFlags & 0x2)) {
        g_itemSparseDB.CancelCallback(this->m_obj->m_entryID, LoadItemCacheCallback, this);
    }

    if (this->m_itemPurchaseData) {
        delete this->m_itemPurchaseData;
        this->m_itemPurchaseData = NULL;
    }
}

// PROVEN: 9aeca405 IDENTICAL/NONE
// OFFSET: 0x0044BC80
static int32_t FindItemCallback(const CGItem_C* item, void* param) {
    return item == param;
}

// OFFSET: 0x00450920/0x14005ED00
void CGItem_C::SignalEvent(CGItem_C* item, uint32_t event) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CGBag_C* bag = &player->m_bag;

    if (!bag) {
        return;
    }

    WOWGUID container;
    uint32_t slot;

    if (!bag->FindItem(FindItemCallback, item, container, slot, 136)) {
        return;
    }

    if (container == player->m_bag.m_guid) {
        if (slot <= 22) {
            FrameScript_SignalEvent(event, "%d", slot + 1);
        } else if (slot - 23 <= 15) {
            FrameScript_SignalEvent(event, "%d%d", 0, slot - 22);
        } else if ((slot >= 39 && slot <= 66) || slot - 67 <= 6) {
            FrameScript_SignalEvent(event, "%d%d", -1, slot - 38);
        }

        return;
    }

    for (uint32_t i = 0; i < 4; i++) {
        WOWGUID guid = i + 19 < static_cast<uint32_t>(bag->m_numSlots) ? bag->m_slots[i + 19] : WOWGUID();

        if (guid == container) {
            FrameScript_SignalEvent(event, "%d%d", i + 1, slot + 1);
            return;
        }
    }

    for (uint32_t i = 0; i < 7; i++) {
        WOWGUID guid = i + 67 < static_cast<uint32_t>(bag->m_numSlots) ? bag->m_slots[i + 67] : WOWGUID();

        if (guid == container) {
            FrameScript_SignalEvent(event, "%d%d", i + 5, slot + 1);
            return;
        }
    }
}

// OFFSET: 0x00468F40/0x14007D110
int32_t CGItem::GetPetitionId() {
    if (this->GetItemStaticFlags(0) & 0x2000) {
        return this->m_item->m_itemFieldEnchantment[0].m_id;
    }

    return 0;
}

// OFFSET: 0x00468F60/0x14007D140
int32_t CGItem::GetPetitionSignatures() {
    if (this->GetItemStaticFlags(0) & 0x2000) {
        return this->m_item->m_itemFieldEnchantment[0].m_duration;
    }

    return 0;
}

// OFFSET: 0x00468F80
uint32_t CGItem::GetDurability() const {
    if (this->m_item->m_itemFieldFlags & 8) {
        return 0;
    }

    return this->m_item->m_itemFieldDurability;
}

// OFFSET: 0x00468FA0
uint32_t CGItem::GetMaxDurability() const {
    if (this->m_item->m_itemFieldFlags & 8) {
        return 0;
    }

    return this->m_item->m_itemFieldMaxdurability;
}
