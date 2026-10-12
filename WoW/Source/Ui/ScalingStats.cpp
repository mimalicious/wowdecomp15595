#include "WoW/Source/Ui/ScalingStats.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/RandPropPointsRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/DB/Rec/ArmorLocationRec.h"
#include "WoW/Source/DB/Rec/ItemArmorQualityRec.h"
#include "WoW/Source/DB/Rec/ItemArmorShieldRec.h"
#include "WoW/Source/DB/Rec/ItemArmorTotalRec.h"
#include "WoW/Source/DB/Rec/ItemDamageAmmoRec.h"
#include "WoW/Source/DB/Rec/ItemDamageOneHandCasterRec.h"
#include "WoW/Source/DB/Rec/ItemDamageOneHandRec.h"
#include "WoW/Source/DB/Rec/ItemDamageRangedRec.h"
#include "WoW/Source/DB/Rec/ItemDamageThrownRec.h"
#include "WoW/Source/DB/Rec/ItemDamageTwoHandCasterRec.h"
#include "WoW/Source/DB/Rec/ItemDamageTwoHandRec.h"
#include "WoW/Source/DB/Rec/ItemDamageWandRec.h"
#include <cmath>

// OFFSET: 0x00999560/0x14071B0B0
int32_t GetRandomItemSlot(int32_t inventoryType) {
    switch (inventoryType) {
        case 0:
        case 18:
        case 19:
        case 24:
        case 27:
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
        case 28:
            return 4;

        default:
            return -1;
    }
}

// OFFSET: 0x009995E0/0x14071B140
int32_t GetDamageClassFromBonusStat(int32_t stat) {
    switch (stat) {
        case 50:
            return 0;

        case 51:
            return 2;

        case 52:
            return 4;

        case 53:
            return 1;

        case 54:
            return 5;

        case 55:
            return 3;

        case 56:
            return 6;

        default:
            return 7;
    }
}

// OFFSET: 0x00999650/0x14071B1B0
int32_t GetRandomPropertyPointsEx(int32_t itemLevel, int32_t quality, int32_t inventoryType) {
    uint32_t slot = GetRandomItemSlot(inventoryType);

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
        case 5:
            return rec->m_epic[slot];

        default:
            return 0;
    }
}

// OFFSET: 0x009996C0/0x14071B240
int32_t GetEffectiveArmor(int32_t level, int32_t itemLevel, int32_t classID, int32_t subclassID, uint32_t quality, int32_t inventoryType) {
    int32_t effectiveLevel = level;

    if (level <= 0) {
        effectiveLevel = itemLevel;
    } else if (level >= itemLevel) {
        effectiveLevel = itemLevel;
    }

    if (classID == 4 && subclassID == 6) {
        if (quality <= 7) {
            ItemArmorShieldRec* shield = g_itemArmorShieldDB.GetRecord(effectiveLevel);
            return static_cast<int32_t>(floor(shield->m_quality[quality] + 0.5f));
        }

        return 0;
    }

    if (inventoryType != 1 && inventoryType != 5 && inventoryType != 3 && inventoryType != 7 && inventoryType != 8 && inventoryType != 9 && inventoryType != 10 && inventoryType != 6 && inventoryType != 16 && inventoryType != 20) {
        return 0;
    }

    ItemArmorQualityRec* qualityRec = g_itemArmorQualityDB.GetRecord(effectiveLevel);

    if (!qualityRec) {
        return 0;
    }

    ItemArmorTotalRec* total = g_itemArmorTotalDB.GetRecord(effectiveLevel);

    if (!total) {
        return 0;
    }

    ArmorLocationRec* location = g_armorLocationDB.GetRecord(inventoryType == 20 ? 5 : inventoryType);

    if (!location) {
        return 0;
    }

    float locationModifier;
    float totalArmor;

    switch (subclassID) {
        case 1:
            locationModifier = location->m_value[0];
            totalArmor = total->m_value[0];
            break;

        case 2:
            locationModifier = location->m_value[1];
            totalArmor = total->m_value[1];
            break;

        case 3:
            locationModifier = location->m_value[2];
            totalArmor = total->m_value[2];
            break;

        case 4:
            locationModifier = location->m_value[3];
            totalArmor = total->m_value[3];
            break;

        default:
            return 0;
    }

    if (locationModifier <= 0.0f) {
        return 0;
    }

    return static_cast<int32_t>(floor(qualityRec->m_qualityMod[quality] * totalArmor * locationModifier + 0.5f));
}

// OFFSET: 0x00999890/0x14071B480
float GetEffectiveWeaponDamage(int32_t level, int32_t itemLevel, int32_t classID, int32_t subclassID, uint32_t quality, float damageModifier, int32_t delay, float statScalingFactor, int32_t inventoryType, int32_t flags, bool minimum) {
    int32_t effectiveLevel = level;
    float dps = 0.0f;

    if (level <= 0) {
        effectiveLevel = itemLevel;
    } else if (level >= itemLevel) {
        effectiveLevel = itemLevel;
    }

    if (classID != 2 || quality > 6) {
        return 0.0f;
    }

    switch (inventoryType) {
        case 13:
        case 21:
        case 22:
            if (flags & 0x200) {
                dps = g_itemDamageOneHandCasterDB.GetRecord(effectiveLevel)->m_quality[quality];
            } else {
                dps = g_itemDamageOneHandDB.GetRecord(effectiveLevel)->m_quality[quality];
            }

            break;

        case 17:
            if (flags & 0x200) {
                dps = g_itemDamageTwoHandCasterDB.GetRecord(effectiveLevel)->m_quality[quality];
            } else {
                dps = g_itemDamageTwoHandDB.GetRecord(effectiveLevel)->m_quality[quality];
            }

            break;

        case 24:
            dps = g_itemDamageAmmoDB.GetRecord(effectiveLevel)->m_quality[quality];
            break;

        case 15:
        case 25:
        case 26:
            switch (subclassID) {
                case 2:
                case 3:
                case 18:
                    dps = g_itemDamageRangedDB.GetRecord(effectiveLevel)->m_quality[quality];
                    break;

                case 16:
                    dps = g_itemDamageThrownDB.GetRecord(effectiveLevel)->m_quality[quality];
                    break;

                case 19:
                    dps = g_itemDamageWandDB.GetRecord(effectiveLevel)->m_quality[quality];
                    break;

                default:
                    break;
            }

            break;

        default:
            break;
    }

    float speed = static_cast<float>(delay);
    float average = static_cast<float>(delay) * dps * 0.001f;
    float maxDamage = static_cast<float>(floor((statScalingFactor * 0.5f + 1.0f) * average + 0.5f));
    float minResult;
    float maxResult;

    if (!delay || damageModifier == 0.0f) {
        maxResult = maxDamage;
        minResult = (1.0f - statScalingFactor * 0.5f) * average;
    } else {
        float rate = 1000.0f / speed;
        float minDps = rate * ((1.0f - statScalingFactor * 0.5f) * average) + damageModifier;

        if (minDps < 1.0f) {
            minDps = 1.0f;
        }

        minResult = 1.0f / rate * minDps;
        maxResult = 1.0f / rate * (rate * maxDamage + damageModifier);
    }

    if (minimum) {
        return minResult <= 1.0f ? 1.0f : minResult;
    }

    return maxResult <= 1.0f ? 1.0f : maxResult;
}

// OFFSET: 0x00999AF0/0x14071B830
int32_t CalculateItemBonusFromPercent(int32_t level, int32_t itemLevel, int32_t amount, int32_t percentEditor, int32_t socketPercentage, int32_t quality, int32_t inventoryType) {
    if (level <= 0) {
        return amount;
    }

    if (level >= itemLevel) {
        return amount;
    }

    int32_t points = GetRandomPropertyPointsEx(level, quality, inventoryType);

    if (points) {
        return static_cast<int32_t>(static_cast<float>(percentEditor * points) * 0.000099999997f - static_cast<float>(points) / static_cast<float>(GetRandomPropertyPointsEx(itemLevel, quality, inventoryType)) * static_cast<float>(socketPercentage) + 0.5f);
    }

    return amount;
}

// OFFSET: 0x00999B70/0x14071B8D0
int32_t GetScaledEnchantmentPoints(int32_t itemLevel, int32_t maxItemLevel, const SpellItemEnchantmentRec* enchant, int32_t points) {
    if (itemLevel <= 0) {
        return points;
    }

    if (itemLevel >= maxItemLevel) {
        return points;
    }

    int32_t scaling = enchant->m_scalingItemLevel;

    if (scaling == 1) {
        return points;
    }

    if (!scaling) {
        return static_cast<int32_t>(static_cast<float>(itemLevel) / static_cast<float>(maxItemLevel) * static_cast<float>(points));
    }

    RandPropPointsRec* base = g_randPropPointsDB.GetRecord(scaling);
    int32_t basePoints = base ? base->m_superior[0] : 0;
    float ratio = static_cast<float>(points) * 10000.0f / static_cast<float>(basePoints);
    RandPropPointsRec* current = g_randPropPointsDB.GetRecord(itemLevel);
    int32_t currentPoints = current ? current->m_superior[0] : 0;
    int32_t result = static_cast<int32_t>(static_cast<float>(static_cast<int32_t>(ratio) * currentPoints) * 0.000099999997f + 0.5f);

    if (result >= points) {
        return points;
    }

    return result;
}
