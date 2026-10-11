#include "VisibleItem_C.h"
#include <cstdlib>
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/Object/ItemStats.h"

// STATIC
// PROVEN: 9c793a34 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
static void VisibleItemCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
}

// OFFSET: 0x005FF300
uint32_t VisibleItem_C::GetClassID() {
    WOWGUID guid = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(abs(this->m_itemId), guid, &VisibleItemCacheCallback, NULL, true);

    return stats ? stats->m_class : 0;
}

// OFFSET: 0x005FF350
uint32_t VisibleItem_C::GetSubtypeID() {
    WOWGUID guid = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(abs(this->m_itemId), guid, &VisibleItemCacheCallback, NULL, true);

    return stats ? stats->m_subClass : 0;
}

// OFFSET: 0x005FF3A0
uint32_t VisibleItem_C::GetInventoryType() {
    WOWGUID guid = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(abs(this->m_itemId), guid, &VisibleItemCacheCallback, NULL, true);

    if (stats) {
        return stats->m_inventoryType;
    }

    ItemRec* rec = g_itemDB.GetRecord(abs(this->m_itemId), WOWGUID(), NULL, NULL, false);
    return rec ? rec->m_inventoryType : 0;
}

// OFFSET: 0x005FF420/0x140288250
uint32_t VisibleItem_C::GetDisplayID() {
    WOWGUID guid = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(abs(this->m_itemId), guid, &VisibleItemCacheCallback, NULL, true);

    if (stats) {
        return stats->m_displayId;
    }

    ItemRec* rec = g_itemDB.GetRecord(abs(this->m_itemId), WOWGUID(), NULL, NULL, false);
    return rec ? rec->m_displayInfoID : 0;
}

// OFFSET: 0x005FF490
uint32_t VisibleItem_C::GetMaterial() {
    WOWGUID guid = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(abs(this->m_itemId), guid, &VisibleItemCacheCallback, NULL, true);

    if (stats) {
        return stats->m_material;
    }

    ItemRec* rec = g_itemDB.GetRecord(abs(this->m_itemId), WOWGUID(), NULL, NULL, false);
    return rec ? rec->m_material : 0;
}

// OFFSET: 0x005FF510
uint32_t VisibleItem_C::GetSheatheType() {
    WOWGUID guid = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(abs(this->m_itemId), guid, &VisibleItemCacheCallback, NULL, true);

    if (stats) {
        return stats->m_sheatheType;
    }

    ItemRec* rec = g_itemDB.GetRecord(abs(this->m_itemId), WOWGUID(), NULL, NULL, false);
    return rec ? rec->m_sheatheType : 0;
}

// OFFSET: 0x005FF590
uint32_t VisibleItem_C::GetSoundOverrideSubclassID() {
    WOWGUID guid = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(abs(this->m_itemId), guid, &VisibleItemCacheCallback, NULL, true);

    if (stats) {
        return stats->m_soundOverrideSubclass;
    }

    ItemRec* rec = g_itemDB.GetRecord(abs(this->m_itemId), WOWGUID(), NULL, NULL, false);
    return rec ? rec->m_soundOverrideSubclassID : 0;
}

// OFFSET: 0x005FF600/0x1402884D0
int32_t VisibleItem_C::GetItemEnchantmentVisualID() {
    ItemDisplayInfoRec displayRec;

    if (!g_itemDisplayInfoDB.GetRecord(this->GetDisplayID(), &displayRec)) {
        return 0;
    }

    if (g_itemVisualsDB.GetRecord(displayRec.m_itemVisual)) {
        return -1;
    }

    SpellItemEnchantmentRec* enchantment = g_spellItemEnchantmentDB.GetRecord(this->m_enchantment[0]);
    int32_t visual;

    if (enchantment && (visual = enchantment->m_itemVisual) != 0) {
        return visual;
    }

    enchantment = g_spellItemEnchantmentDB.GetRecord(this->m_enchantment[1]);

    if (enchantment && (visual = enchantment->m_itemVisual) != 0) {
        return visual;
    }

    return 0;
}
