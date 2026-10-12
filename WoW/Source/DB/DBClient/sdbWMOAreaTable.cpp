#include <cstdint>
#include <cstdlib>
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/LiquidTypeRec.h"
#include "WoW/Source/DB/Rec/WMOAreaTableRec.h"
#include "WoW/Source/WorldClient/World.h"

// OFFSET: 0x00B02420/0x1408C43C0
const LiquidTypeRec* GetLiquidTypeRecForArea(uint32_t areaId, uint32_t liquidType) {
    if (!liquidType) {
        return NULL;
    }

    if (areaId && liquidType < 21) {
        uint32_t index = (liquidType - 1) & 3;
        AreaTableRec* area = g_areaTableDB.GetRecord(areaId);

        if (area) {
            if (!area->m_liquidTypeID[index] && area->m_parentAreaID) {
                area = g_areaTableDB.GetRecord(area->m_parentAreaID);
            }

            if (area && area->m_liquidTypeID[index]) {
                return g_liquidTypeDB.GetRecord(area->m_liquidTypeID[index]);
            }
        }
    }

    return g_liquidTypeDB.GetRecord(liquidType);
}

// PROVEN: 5ac149c8 IDENTICAL/IDENTICAL
// OFFSET: 0x00B024B0/0x1408C44B0
static int WMOAreaTableCompare(const void* a, const void* b) {
    const WMOAreaTableRec* left = static_cast<const WMOAreaTableRec*>(a);
    const WMOAreaTableRec* right = static_cast<const WMOAreaTableRec*>(b);
    int result = left->m_wmoid - right->m_wmoid;

    if (!result) {
        result = left->m_nameSetID - right->m_nameSetID;

        if (!result) {
            return left->m_wmogroupID - right->m_wmogroupID;
        }
    }

    return result;
}

// OFFSET: 0x00B024E0/0x1408C44D0
const WMOAreaTableRec* SDBWMOAreaTableLookup(uint32_t wmoId, uint32_t nameSet, uint32_t groupId) {
    WMOAreaTableRec key;
    key.m_wmoid = wmoId;
    key.m_nameSetID = nameSet;
    key.m_wmogroupID = groupId;
    return static_cast<const WMOAreaTableRec*>(bsearch(&key, g_wmoAreaTableDB.m_numRecords <= 0 ? NULL : g_wmoAreaTableDB.m_records, g_wmoAreaTableDB.m_numRecords, sizeof(WMOAreaTableRec), WMOAreaTableCompare));
}
