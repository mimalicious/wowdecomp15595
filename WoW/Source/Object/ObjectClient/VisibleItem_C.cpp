#include "VisibleItem_C.h"
#include <cstdlib>
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"

// STATIC
// PROVEN: 9c793a34 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
static void VisibleItemCacheCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
}

// OFFSET: 0x005FF300/0x140288110
uint32_t VisibleItem_C::GetClassID() {
    ItemRec* rec = g_itemDB.GetRecord(abs(this->m_itemId), WOWGUID(), &VisibleItemCacheCallback, NULL, true);

    return rec ? rec->m_classID : 0;
}

// OFFSET: 0x005FF350/0x140288160
uint32_t VisibleItem_C::GetSubtypeID() {
    ItemRec* rec = g_itemDB.GetRecord(abs(this->m_itemId), WOWGUID(), &VisibleItemCacheCallback, NULL, true);

    return rec ? rec->m_subclassID : 0;
}

// OFFSET: 0x005FF3A0/0x1402881B0
uint32_t VisibleItem_C::GetInventoryType() {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(abs(this->m_itemId), WOWGUID(), &VisibleItemCacheCallback, NULL, true);

    if (stats) {
        return stats->m_inventoryType;
    }

    ItemRec* rec = g_itemDB.GetRecord(abs(this->m_itemId), WOWGUID(), NULL, NULL, false);
    return rec ? rec->m_inventoryType : 0;
}

// OFFSET: 0x005FF420/0x140288250
uint32_t VisibleItem_C::GetDisplayID() {
    ItemRec* rec = g_itemDB.GetRecord(abs(this->m_itemId), WOWGUID(), &VisibleItemCacheCallback, NULL, true);

    if (rec) {
        return rec->m_displayInfoID;
    }

    rec = g_itemDB.GetRecord(abs(this->m_itemId), WOWGUID(), NULL, NULL, false);
    return rec ? rec->m_displayInfoID : 0;
}

// OFFSET: 0x005FF490/0x1402882F0
uint32_t VisibleItem_C::GetMaterial() {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(abs(this->m_itemId), WOWGUID(), &VisibleItemCacheCallback, NULL, true);

    if (stats) {
        return stats->m_material;
    }

    ItemRec* rec = g_itemDB.GetRecord(abs(this->m_itemId), WOWGUID(), NULL, NULL, false);
    return rec ? rec->m_material : 0;
}

// OFFSET: 0x005FF510/0x140288390
uint32_t VisibleItem_C::GetSheatheType() {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(abs(this->m_itemId), WOWGUID(), &VisibleItemCacheCallback, NULL, true);

    if (stats) {
        return stats->m_sheatheType;
    }

    ItemRec* rec = g_itemDB.GetRecord(abs(this->m_itemId), WOWGUID(), NULL, NULL, false);
    return rec ? rec->m_sheatheType : 0;
}

// OFFSET: 0x005FF590/0x140288430
uint32_t VisibleItem_C::GetSoundOverrideSubclassID() {
    ItemRec* rec = g_itemDB.GetRecord(abs(this->m_itemId), WOWGUID(), &VisibleItemCacheCallback, NULL, true);

    if (rec) {
        return rec->m_soundOverrideSubclassID;
    }

    rec = g_itemDB.GetRecord(abs(this->m_itemId), WOWGUID(), NULL, NULL, false);
    return rec ? rec->m_soundOverrideSubclassID : 0;
}

// PROVEN: 32a46960 IDENTICAL/UNPROVEN
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
