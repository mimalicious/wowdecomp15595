#include "WoW/Source/Game/ItemTransmogrify.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/ItemRandomPropertiesRec.h"
#include "WoW/Source/DB/Rec/ItemRandomSuffixRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellAuraOptionsRec.h"

// STATIC: 0x00D27BE8
static const uint32_t s_classWeaponSubclassMask[21] = { 0x6F, 0x67, 0xD, 0xD, 0x47B, 0x463, 0x427, 0x1AF, 0x27, 0x5FF, 0x5D5, 0x5FF, 0x5FF, 0x44D, 0x5FF, 0x5DD, 0x9, 0x5FF, 0xD, 0x190, 0x5FF };

// STATIC: 0x00D27C3C
static const uint32_t s_classArmorSubclassMask[12] = { 0x5FF, 0x190, 0x408, 0x44, 0x23, 0x5FF, 0x43, 0x2, 0x400, 0x40, 0x20, 0x462 };

// STATIC: 0x00D27C6C
static const uint32_t s_classStatCategoryMask[2] = { 0x5D2, 0x46F };

// STATIC: 0x00B9E638
static const int32_t s_inventoryTypeGroup[29] = { -2, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, 16, 17, 14, -1, -1, 18, 4, 15, 16, 16, -2, 17, 17, -2, 17 };

// PROVEN: 286b257e IDENTICAL/NONE
// OFFSET: 0x00B12060
bool IsRestrictedItemMod(int32_t stat) {
    switch (stat) {
        case 0:
        case 5:
        case 6:
        case 18:
        case 21:
        case 30:
        case 41:
        case 42:
        case 43:
        case 45:
        case 47:
            return true;
        default:
            return false;
    }
}

// OFFSET: 0x00B120C0
bool IsRestrictedCombatRating(int32_t rating) {
    return rating == 7 || rating == 10 || rating == 19;
}

// OFFSET: 0x00B120E0
bool CanClassUseArmorSubclass(uint8_t classID, uint32_t subclass) {
    return subclass <= 11 && (s_classArmorSubclassMask[subclass] & (1 << (classID - 1)));
}

// PROVEN: 4c58a2c2 IDENTICAL/NONE
// OFFSET: 0x00B12110
bool CanClassUseWeaponSubclass(int32_t classID, int32_t subclass) {
    return (s_classWeaponSubclassMask[subclass] & (1 << (classID - 1))) != 0;
}

// OFFSET: 0x00B12130
bool ClassUsesItemStatCategory(int32_t classID, int32_t category) {
    return (s_classStatCategoryMask[category] & (1 << (classID - 1))) != 0;
}

// PROVEN: 0f88f0a8 IDENTICAL/NONE
// OFFSET: 0x00B12150
bool CanClassDualWield(int32_t classID) {
    return classID == 1 || classID == 4 || classID == 3 || classID == 7;
}

// PROVEN: 78eca8ea IDENTICAL/IDENTICAL
// OFFSET: 0x00B12180/0x1408D7BA0
bool IsRelicItem(const ItemRecInterface* item) {
    if (item->GetClassID() != 4) {
        return false;
    }

    int32_t subclass = item->GetSubclassID();

    return subclass == 7 || subclass == 8 || subclass == 9 || subclass == 10 || subclass == 11;
}

// PROVEN: 82d6dc1f IDENTICAL/IDENTICAL
// OFFSET: 0x00B121D0/0x1408D7C00
bool IsBowGunOrCrossbow(const ItemRecInterface* item) {
    if (item->GetClassID() != 2) {
        return false;
    }

    int32_t subclass = item->GetSubclassID();

    return subclass == 2 || subclass == 3 || subclass == 18;
}

// PROVEN: 6aca4c7d IDENTICAL/IDENTICAL
// OFFSET: 0x00B12210/0x1408D7C50
bool CanItemBeTransmogrified(const ItemRecInterface* item, int32_t* reason) {
    if (!item) {
        *reason = TRANSMOGRIFY_NO_ITEM;
        return false;
    }

    if (item->GetQuality() == 5) {
        *reason = TRANSMOGRIFY_LEGENDARY;
        return false;
    }

    int32_t classID = item->GetClassID();

    if (classID != 4 && classID != 2) {
        *reason = TRANSMOGRIFY_INVALID_TYPE;
        return false;
    }

    int32_t subclass = item->GetSubclassID();

    if (item->GetClassID() == 2 && subclass == 20) {
        *reason = TRANSMOGRIFY_INVALID_TYPE;
        return false;
    }

    *reason = TRANSMOGRIFY_OK;
    return true;
}

// OFFSET: 0x00B122B0
bool SpellHasRestrictedStat(const SpellRec* spell) {
    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect || effect->m_effect != 6) {
            continue;
        }

        switch (effect->m_effectAura) {
            case 13:
            case 79:
                if (effect->m_effectMiscValue & 7) {
                    return true;
                }

                break;

            case 29:
            case 80:
            case 220:
                if (static_cast<uint32_t>(effect->m_effectMiscValue - 3) <= 1) {
                    return true;
                }

                break;

            case 35:
            case 85:
            case 110:
            case 132:
                if (!effect->m_effectMiscValue) {
                    return true;
                }

                break;

            case 42:
            case 231: {
                if (!spell->m_spellAuraOptionsID) {
                    break;
                }

                const SpellAuraOptionsRec* options = g_spellAuraOptionsDB.GetRecord(spell->m_spellAuraOptionsID);

                if (options && (options->m_procTypeMask & 0x54000)) {
                    return true;
                }

                break;
            }

            case 50:
            case 55:
            case 57:
            case 65:
            case 71:
            case 115:
            case 118:
            case 135:
            case 136:
            case 175:
            case 181:
            case 266:
                return true;

            case 72:
            case 73:
                if (effect->m_effectMiscValueB & 1) {
                    return true;
                }

                break;

            case 189:
                for (int32_t rating = 0; rating < 26; rating++) {
                    if (((1 << rating) & effect->m_effectMiscValue) && IsRestrictedCombatRating(rating)) {
                        return true;
                    }
                }

                break;
        }
    }

    return false;
}

// PROVEN: c67213c7 IDENTICAL/DIFFERENT
// OFFSET: 0x00B124B0/0x1408D7F70
bool EnchantmentHasRestrictedStat(int32_t enchantmentId) {
    const SpellItemEnchantmentRec* enchantment = g_spellItemEnchantmentDB.GetRecord(enchantmentId);

    if (!enchantment) {
        return false;
    }

    for (uint32_t i = 0; i < 3; i++) {
        bool restricted;

        if (enchantment->m_effect[i] == 5) {
            restricted = IsRestrictedItemMod(enchantment->m_effectArg[i]);
        } else if (enchantment->m_effect[i] == 3) {
            restricted = SpellHasRestrictedStat(g_spellDB.GetRecord(enchantment->m_effectArg[i]));
        } else {
            continue;
        }

        if (restricted) {
            return true;
        }
    }

    return false;
}

// PROVEN: c4b458d4 IDENTICAL/NONE
// OFFSET: 0x00B12540/0x1408D8090
int32_t GetItemStatCategory(const ItemRecInterface* item, int32_t randomPropertiesId) {
    if (item->GetFlags(1) & 8) {
        return 0;
    }

    if (item->GetFlags(1) & 0x10) {
        return 1;
    }

    for (int32_t i = 0; i < item->GetNumStats(); i++) {
        if (item->GetStatValue(i) > 0 && item->GetStatType(i) == 3) {
            return 1;
        }
    }

    for (int32_t i = 0; i < item->GetNumStats(); i++) {
        if (item->GetStatValue(i) > 0 && IsRestrictedItemMod(item->GetStatType(i))) {
            return 0;
        }
    }

    for (int32_t i = 0; i < item->GetNumSpells(); i++) {
        if (item->GetSpellID(i) > 0 && (item->GetSpellTrigger(i) == 1 || !item->GetSpellTrigger(i)) && SpellHasRestrictedStat(g_spellDB.GetRecord(item->GetSpellID(i)))) {
            return 0;
        }
    }

    if (randomPropertiesId > 0) {
        const ItemRandomPropertiesRec* properties = g_itemRandomPropertiesDB.GetRecord(randomPropertiesId);

        for (uint32_t i = 0; i < 5; i++) {
            if (properties->m_enchantment[i] && EnchantmentHasRestrictedStat(properties->m_enchantment[i])) {
                return 0;
            }
        }
    } else if (randomPropertiesId < 0) {
        const ItemRandomSuffixRec* suffix = g_itemRandomSuffixDB.GetRecord(-randomPropertiesId);

        for (uint32_t i = 0; i < 5; i++) {
            if (suffix->m_enchantment[i] && EnchantmentHasRestrictedStat(suffix->m_enchantment[i])) {
                return 0;
            }
        }
    }

    return 1;
}

// OFFSET: 0x00B12770
bool IsAllClassesMask(int32_t mask) {
    for (int32_t i = 0; i < g_chrClassesDB.m_numRecords; i++) {
        const ChrClassesRec* rec = i < 0 || i >= g_chrClassesDB.m_numRecords ? NULL : g_chrClassesDB.GetRecordByIndex(i);

        if (rec && !((1 << (static_cast<uint8_t>(rec->m_id) - 1)) & mask)) {
            return false;
        }
    }

    return true;
}

// OFFSET: 0x00B127C0/0x1408D8330
bool IsItemUsefulForClass(int32_t classID, const ItemRecInterface* item, int32_t randomPropertiesId, bool checkLevel, bool ignoreStats) {
    if (item->GetFlags(1) & 0x100) {
        return false;
    }

    if (!IsAllClassesMask(item->GetAllowableClass())) {
        return ((1 << (classID - 1)) & item->GetAllowableClass()) != 0;
    }

    if ((checkLevel && item->GetItemLevel() < 70) || (item->GetClassID() != 4 && item->GetClassID() != 2) || item->GetInventoryType() == 4 || (item->GetFlags(1) & 0x20)) {
        return true;
    }

    int32_t category;

    if (!ignoreStats && (category = GetItemStatCategory(item, randomPropertiesId), !IsRelicItem(item)) && !ClassUsesItemStatCategory(classID, category)) {
        return false;
    }

    if (item->GetClassID() == 4) {
        if (item->GetInventoryType() == 16) {
            return true;
        }

        return CanClassUseArmorSubclass(classID, item->GetSubclassID());
    }

    bool usable = CanClassUseWeaponSubclass(classID, item->GetSubclassID());

    if (usable && item->GetInventoryType() == 22) {
        return CanClassDualWield(classID);
    }

    return usable;
}

// OFFSET: 0x00B12910/0x1408D8530
bool ItemHasStats(const ItemRecInterface* item, int32_t randomPropertiesId) {
    for (int32_t i = 0; i < item->GetNumStats(); i++) {
        if (item->GetStatValue(i) > 0) {
            return true;
        }
    }

    if (randomPropertiesId > 0) {
        const ItemRandomPropertiesRec* properties = g_itemRandomPropertiesDB.GetRecord(randomPropertiesId);

        if (properties) {
            for (uint32_t i = 0; i < 5; i++) {
                const SpellItemEnchantmentRec* enchantment = g_spellItemEnchantmentDB.GetRecord(properties->m_enchantment[i]);

                if (!enchantment) {
                    continue;
                }

                for (uint32_t j = 0; j < 3; j++) {
                    int32_t effect = enchantment->m_effect[j];

                    if (effect == 5 || effect == 2 || effect == 4 || effect == 3) {
                        return true;
                    }
                }
            }
        }
    } else {
        const ItemRandomSuffixRec* suffix = g_itemRandomSuffixDB.GetRecord(-randomPropertiesId);

        if (suffix) {
            for (uint32_t i = 0; i < 5; i++) {
                const SpellItemEnchantmentRec* enchantment = g_spellItemEnchantmentDB.GetRecord(suffix->m_enchantment[i]);

                if (!enchantment) {
                    continue;
                }

                for (uint32_t j = 0; j < 3; j++) {
                    int32_t effect = enchantment->m_effect[j];

                    if (effect == 5 || effect == 2 || effect == 4 || effect == 3) {
                        return true;
                    }
                }
            }
        }
    }

    if (item->GetScalingStatDistribution() > 0) {
        return true;
    }

    for (int32_t i = 0; i < item->GetNumSpells(); i++) {
        if (item->GetSpellID(i) <= 0) {
            continue;
        }

        if (item->GetSpellTrigger(i) == 2) {
            return true;
        }

        if (item->GetSpellTrigger(i) != 1) {
            continue;
        }

        const SpellRec* spell = g_spellDB.GetRecord(item->GetSpellID(i));
        uint32_t count;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

        for (uint32_t j = 0; j < count; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (effect && effect->m_effect == 6 && (effect->m_effectAura == 42 || effect->m_effectAura == 231)) {
                return true;
            }
        }
    }

    return false;
}

// PROVEN: 6b3802e6 IDENTICAL/IDENTICAL
// OFFSET: 0x00B12B80/0x1408D8810
bool CanTransmogrifyTarget(const ItemRecInterface* item, uint32_t bound, int32_t randomPropertiesId, int32_t* reason) {
    if (!CanItemBeTransmogrified(item, reason)) {
        return false;
    }

    if (item->GetFlags(1) & 0x200000) {
        *reason = TRANSMOGRIFY_INVALID_TARGET;
        return false;
    }

    if (!(bound & 1)) {
        *reason = TRANSMOGRIFY_NOT_SOULBOUND;
        return false;
    }

    if (!ItemHasStats(item, randomPropertiesId)) {
        *reason = TRANSMOGRIFY_INVALID_TARGET;
        return false;
    }

    *reason = TRANSMOGRIFY_OK;
    return true;
}

// PROVEN: 625f2908 IDENTICAL/IDENTICAL
// OFFSET: 0x00B12BF0/0x1408D88A0
bool CanTransmogrifySource(const ItemRecInterface* item, int32_t randomPropertiesId, int32_t* reason) {
    if (!CanItemBeTransmogrified(item, reason)) {
        return false;
    }

    if (item->GetFlags(1) & 0x400000) {
        *reason = TRANSMOGRIFY_INVALID_SOURCE;
        return false;
    }

    if (ItemHasStats(item, randomPropertiesId) || (item->GetFlags(1) & 0x800000)) {
        *reason = TRANSMOGRIFY_OK;
        return true;
    }

    *reason = TRANSMOGRIFY_NO_STATS;
    return false;
}

// OFFSET: 0x00B12C70/0x1408D8950
bool CanTransmogrifyItemWithItem(const ItemRecInterface* source, const ItemRecInterface* target, uint32_t bound, int32_t sourceRandomPropertiesId, int32_t targetRandomPropertiesId, int32_t* reason) {
    if (!CanTransmogrifyTarget(target, bound, targetRandomPropertiesId, reason) || !CanTransmogrifySource(source, sourceRandomPropertiesId, reason)) {
        return false;
    }

    if (source->GetID() == target->GetID()) {
        *reason = TRANSMOGRIFY_SAME_ITEM;
        return false;
    }

    if (source->GetClassID() != target->GetClassID()) {
        *reason = TRANSMOGRIFY_MISMATCH;
        return false;
    }

    int32_t sourceType = source->GetInventoryType();
    int32_t targetType = target->GetInventoryType();

    if (sourceType == 18 || sourceType == 19 || sourceType == 28 || sourceType == 4 || sourceType == 11 || sourceType == 12 || sourceType == 24 || sourceType == 27) {
        *reason = TRANSMOGRIFY_INVALID_TYPE;
        return false;
    }

    if (source->GetSubclassID() != target->GetSubclassID() && (source->GetClassID() != 2 || !IsBowGunOrCrossbow(source) || !IsBowGunOrCrossbow(target))) {
        *reason = TRANSMOGRIFY_MISMATCH;
        return false;
    }

    int32_t group = s_inventoryTypeGroup[sourceType];

    if (sourceType == targetType || (group == -1 && sourceType == 13 && (targetType == 22 || targetType == 21)) || group == s_inventoryTypeGroup[targetType]) {
        *reason = TRANSMOGRIFY_OK;
        return true;
    }

    *reason = TRANSMOGRIFY_MISMATCH;
    return false;
}
