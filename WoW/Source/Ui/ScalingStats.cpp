#include "WoW/Source/Ui/ScalingStats.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/RandPropPointsRec.h"

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

// OFFSET: 0x00999AF0/0x14071B830
int32_t CalculateItemBonusFromPercent(int32_t level, int32_t itemLevel, int32_t amount, int32_t percentEditor, int32_t socketPercentage, int32_t quality, int32_t inventoryType) {
    if (level <= 0 || level >= itemLevel) {
        return amount;
    }

    int32_t points = GetRandomPropertyPointsEx(level, quality, inventoryType);

    if (!points) {
        return amount;
    }

    return static_cast<int32_t>(static_cast<float>(percentEditor * points) * 0.000099999997f - static_cast<float>(points) / static_cast<float>(GetRandomPropertyPointsEx(itemLevel, quality, inventoryType)) * static_cast<float>(socketPercentage) + 0.5f);
}
