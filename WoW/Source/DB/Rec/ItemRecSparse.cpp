#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include <cstring>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ImportPriceArmorRec.h"
#include "WoW/Source/DB/Rec/ImportPriceQualityRec.h"
#include "WoW/Source/DB/Rec/ImportPriceShieldRec.h"
#include "WoW/Source/DB/Rec/ImportPriceWeaponRec.h"
#include "WoW/Source/DB/Rec/ItemClassRec.h"
#include "WoW/Source/DB/Rec/ItemDisenchantLootRec.h"
#include "WoW/Source/DB/Rec/ItemPriceBaseRec.h"
#include "WoW/Source/DB/Rec/ItemSubClassRec.h"

// STATIC: 0x00C1C6E8
static const int32_t s_fieldOffsets[] = { 0, 4, 8, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 136, 176, 216, 256, 260, 264, 268, 272, 292, 312, 332, 352, 372, 392, 396, 400, 404, 408, 412, 416, 420, 424, 428, 432, 436, 440, 444, 448, 452, 456, 460, 464, 468, 472, 484, 496, 500, 504, 508, 512, 516, 520, 524, 528 };

// STATIC: 0x00C1C7F8
static const int32_t s_fieldSizes[] = { 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 40, 40, 40, 40, 4, 4, 4, 4, 20, 20, 20, 20, 20, 20, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 12, 12, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1C908
static const int32_t s_fieldTypes[] = { 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0 };

// STATIC: 0x00C1C6B4
const DB2Meta ItemRecSparse::s_meta = { "DBFilesClient\\Item-sparse.db2", "Item-sparse.adb", 68, 532, ItemRecSparse::FixupRecord, 0, true, true, s_fieldOffsets, s_fieldSizes, s_fieldTypes, 0x919BE54E, 0x50238EC2, 5 };

// STATIC: 0x00D9A9D0
static TSGrowableArray<TSGrowableArray<TSGrowableArray<int32_t> > > s_disenchantLootBySubClass;

// STATIC: 0x00D9A9E0
static TSGrowableArray<TSGrowableArray<TSGrowableArray<int32_t> > > s_disenchantLootByClass;

// STATIC: 0x00D9A9F0
static TSGrowableArray<TSGrowableArray<int32_t> > s_itemSubClassIds;

// PROVEN: 96001d76 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemRecSparse::FixupRecord(void* record) {
}

// OFFSET: 0x004B1990
static bool FindItemCurrencyCost(const uint8_t* record, WowClientDB2_Base* db, void* param) {
    struct Search {
        bool m_found;
        int32_t m_itemID;
    };

    Search* search = static_cast<Search*>(param);

    if (!record || reinterpret_cast<const ItemCurrencyCostRec*>(record)->m_itemID != search->m_itemID) {
        return true;
    }

    search->m_found = true;
    return false;
}

// OFFSET: 0x004B19B0/0x1400E8090
const char* ItemRecSparse::GetName(int32_t index) const {
    switch (index) {
        case 0:
            if (this->m_name && *this->m_name) {
                return this->m_name;
            }

            break;

        case 1:
            if (this->m_name && *this->m_name2) {
                return this->m_name2;
            }

            break;

        case 2:
            if (this->m_name && *this->m_name3) {
                return this->m_name3;
            }

            break;

        case 3:
            if (this->m_name && *this->m_name4) {
                return this->m_name4;
            }

            break;

        default:
            break;
    }

    return this->m_name;
}

// OFFSET: 0x00A2E7F0/0x1407D78E0
ItemRec* ItemRecSparse::GetNonSparse() const {
    WOWGUID none = WOWGUID();
    return g_itemDB.GetRecord(this->m_ID, none, NULL, NULL, false);
}

// OFFSET: 0x004B1A40/0x1400E8170
int32_t ItemRecSparse::GetBuyPrice(bool* computed) const {
    if (computed) {
        *computed = false;
    }

    if (this->m_flags[1] & 0x4000) {
        return this->m_buyPrice;
    }

    ImportPriceQualityRec* quality = g_importPriceQualityDB.GetRecord(this->m_quality + 1);

    if (!quality) {
        return 0;
    }

    ItemPriceBaseRec* base = g_itemPriceBaseDB.GetRecord(this->m_itemLevel);

    if (!base) {
        return 0;
    }

    int32_t inventoryType = this->m_inventoryType;
    float typeFactor;

    if (inventoryType == 13 || inventoryType == 17 || inventoryType == 21 || inventoryType == 22 || inventoryType == 15 || inventoryType == 25 || inventoryType == 26) {
        typeFactor = base->m_weapon;
    } else {
        typeFactor = base->m_armor;
    }

    int32_t slot = inventoryType;

    if (inventoryType == 20) {
        slot = 5;
    }

    ItemRec* rec = this->GetNonSparse();

    if (!rec) {
        return 0;
    }

    float factor;

    switch (slot) {
        case 1:
        case 3:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 16: {
            ImportPriceArmorRec* armor = g_importPriceArmorDB.GetRecord(slot);

            if (!armor) {
                return 0;
            }

            switch (rec->m_subclassID) {
                case 0:
                case 1:
                    factor = armor->m_clothFactor;
                    break;

                case 2:
                    factor = armor->m_leatherFactor;
                    break;

                case 3:
                    factor = armor->m_mailFactor;
                    break;

                case 4:
                    factor = armor->m_plateFactor;
                    break;

                default:
                    factor = 1.0f;
                    break;
            }

            break;
        }

        case 14: {
            ImportPriceShieldRec* shield = g_importPriceShieldDB.GetRecordByIndex(1);

            if (!shield) {
                return 0;
            }

            factor = shield->m_data;
            break;
        }

        case 13:
        case 15:
        case 17:
        case 21:
        case 22:
        case 26:
        case 28: {
            int32_t index;

            if (slot == 13) {
                index = 2;
            } else if (slot == 17) {
                index = 3;
            } else if (slot == 21) {
                index = 0;
            } else if (slot == 22) {
                index = 1;
            } else {
                index = 4;
            }

            ImportPriceWeaponRec* weapon = g_importPriceWeaponDB.GetRecordByIndex(index);

            if (!weapon) {
                return 0;
            }

            factor = weapon->m_data;
            break;
        }

        default:
            return this->m_buyPrice;
    }

    if (computed) {
        *computed = true;
    }

    return static_cast<int32_t>(quality->m_data * this->m_priceVariance * this->m_priceRandomValue * factor * typeFactor);
}

// OFFSET: 0x004B1C60/0x1400E8410
int32_t ItemRecSparse::GetSellPrice() const {
    if (this->m_flags[1] & 0x4000) {
        return this->m_sellPrice;
    }

    bool computed = false;
    int32_t price = this->GetBuyPrice(&computed);

    if (!computed) {
        return this->m_sellPrice;
    }

    if (this->m_buyCount > 1) {
        return price / (4 * this->m_buyCount);
    }

    ItemRec* rec = this->GetNonSparse();

    if (rec) {
        ItemClassRec* itemClass = g_itemClassDB.GetRecord(rec->m_classID);

        if (itemClass) {
            return static_cast<int32_t>(static_cast<float>(price) * itemClass->m_priceFactor);
        }
    }

    return 0;
}

// OFFSET: 0x004B1CF0/0x1400E84D0
static int32_t GetDisenchantLootID(const ItemRecSparse* item) {
    if (!item) {
        return 0;
    }

    if ((item->m_flags[0] & 0x8002) || item->m_bonding == 4 || item->m_zoneBound || item->m_instanceBound || item->m_stackable > 1) {
        return 0;
    }

    if (!item->GetSellPrice()) {
        struct Search {
            bool m_found;
            int32_t m_itemID;
        };

        Search search;
        search.m_found = false;
        search.m_itemID = item->m_ID;
        g_itemCurrencyCostDB.IterateOverRecords(FindItemCurrencyCost, &search);

        if (!search.m_found) {
            return 0;
        }
    }

    ItemRec* rec = item->GetNonSparse();

    if (!rec) {
        return 0;
    }

    int32_t classID = rec->m_classID;

    if (classID < 0 || classID >= static_cast<int32_t>(s_itemSubClassIds.m_count)) {
        return 0;
    }

    int32_t subClassID = rec->m_subclassID;

    if (subClassID < 0 || subClassID >= static_cast<int32_t>(s_itemSubClassIds.m_data[classID].m_count)) {
        return 0;
    }

    int32_t subClassRecID = s_itemSubClassIds.m_data[classID].m_data[subClassID];

    if (subClassRecID < 0 || subClassRecID >= static_cast<int32_t>(s_disenchantLootBySubClass.m_count)) {
        return 0;
    }

    int32_t quality = item->m_quality;

    if (quality < 0 || quality >= static_cast<int32_t>(s_disenchantLootBySubClass.m_data[subClassRecID].m_count)) {
        return 0;
    }

    int32_t itemLevel = item->m_itemLevel;

    if (itemLevel < 0 || itemLevel >= static_cast<int32_t>(s_disenchantLootBySubClass.m_data[subClassRecID].m_data[quality].m_count)) {
        return 0;
    }

    int32_t lootID = s_disenchantLootBySubClass.m_data[subClassRecID].m_data[quality].m_data[itemLevel];

    if (!lootID) {
        return s_disenchantLootByClass.m_data[classID].m_data[quality].m_data[itemLevel];
    }

    return lootID;
}

// OFFSET: 0x004B1E00/0x1400E8600
int32_t ItemRecSparse::GetDisenchantSkillRequired() const {
    int32_t lootID = GetDisenchantLootID(this);

    if (lootID <= 0) {
        return -1;
    }

    ItemDisenchantLootRec* loot = g_itemDisenchantLootDB.GetRecord(lootID);

    if (loot) {
        return loot->m_skillRequired;
    }

    return 0;
}

// OFFSET: 0x004B2410/0x1400E8E60
void ItemRecSparse::Initialize() {
    s_disenchantLootBySubClass.SetCount(g_itemSubClassDB.m_maxID + 1);

    for (uint32_t i = 0; i < s_disenchantLootBySubClass.m_count; i++) {
        s_disenchantLootBySubClass.m_data[i].SetCount(8);

        for (uint32_t j = 0; j < s_disenchantLootBySubClass.m_data[i].m_count; j++) {
            TSGrowableArray<int32_t>& levels = s_disenchantLootBySubClass.m_data[i].m_data[j];
            levels.SetCount(1001);
            memset(levels.m_data, 0, sizeof(int32_t) * levels.m_count);
        }
    }

    int32_t maxClass = 0;
    int32_t maxSubClass = 0;

    for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
        ItemSubClassRec* rec = g_itemSubClassDB.GetRecordByIndex(i);

        if (maxClass <= rec->m_classID) {
            maxClass = rec->m_classID;
        }

        if (maxSubClass <= rec->m_subClassID) {
            maxSubClass = rec->m_subClassID;
        }
    }

    s_disenchantLootByClass.SetCount(maxClass + 1);
    s_itemSubClassIds.SetCount(maxClass + 1);

    for (uint32_t i = 0; i < s_disenchantLootByClass.m_count; i++) {
        s_disenchantLootByClass.m_data[i].SetCount(8);

        for (uint32_t j = 0; j < s_disenchantLootByClass.m_data[i].m_count; j++) {
            TSGrowableArray<int32_t>& levels = s_disenchantLootByClass.m_data[i].m_data[j];
            levels.SetCount(1001);
            memset(levels.m_data, 0, sizeof(int32_t) * levels.m_count);
        }
    }

    for (uint32_t i = 0; i < s_itemSubClassIds.m_count; i++) {
        TSGrowableArray<int32_t>& ids = s_itemSubClassIds.m_data[i];
        ids.SetCount(maxSubClass + 1);

        for (int32_t j = 0; j < maxSubClass + 1; j++) {
            ids.m_data[j] = -1;
        }
    }

    for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
        ItemSubClassRec* rec = g_itemSubClassDB.GetRecordByIndex(i);

        if (rec->m_classID >= 0 && rec->m_subClassID >= 0) {
            s_itemSubClassIds.m_data[rec->m_classID].m_data[rec->m_subClassID] = rec->m_id;
        }
    }

    for (int32_t i = 0; i < g_itemDisenchantLootDB.m_numRecords; i++) {
        ItemDisenchantLootRec* rec = g_itemDisenchantLootDB.GetRecordByIndex(i);

        if (rec->m_subclass >= 0) {
            int32_t index = s_itemSubClassIds.m_data[rec->m_class].m_data[rec->m_subclass];

            if (index >= 0) {
                for (int32_t level = rec->m_minLevel; level <= rec->m_maxLevel; level++) {
                    s_disenchantLootBySubClass.m_data[index].m_data[rec->m_quality].m_data[level] = rec->m_id;
                }
            }
        } else if (static_cast<uint32_t>(rec->m_class) < s_disenchantLootByClass.m_count) {
            for (int32_t level = rec->m_minLevel; level <= rec->m_maxLevel; level++) {
                s_disenchantLootByClass.m_data[rec->m_class].m_data[rec->m_quality].m_data[level] = rec->m_id;
            }
        }
    }
}

// OFFSET: 0x004B2850
void ItemSparseInitialize() {
    ItemRecSparse::Initialize();
}
