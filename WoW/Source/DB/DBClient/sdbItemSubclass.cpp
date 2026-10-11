#include <cstdint>
#include <cstring>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

// STATIC: #0x00C5DEA8
static TSFixedArray<const ItemSubClassRec*> s_itemSubclasses[17];

// OFFSET: 0x004B28D0/0x1400E93D0
const ItemSubClassRec* SDBItemSubclassGetSubClassRec(uint32_t classID, uint32_t subClassID) {
    if (classID >= sizeof(s_itemSubclasses) / sizeof(s_itemSubclasses[0]) || subClassID >= s_itemSubclasses[classID].Count()) {
        return NULL;
    }

    return s_itemSubclasses[classID][subClassID];
}

// OFFSET: 0x004B2900
void SDBItemSubclassInitialize() {
    int32_t counts[sizeof(s_itemSubclasses) / sizeof(s_itemSubclasses[0])];
    memset(counts, 0, sizeof(counts));

    for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
        const ItemSubClassRec* rec = g_itemSubClassDB.GetRecordByIndex(i);

        if (rec->m_classID >= 0 && counts[rec->m_classID] <= rec->m_subClassID) {
            counts[rec->m_classID] = rec->m_subClassID + 1;
        }
    }

    for (uint32_t i = 0; i < sizeof(s_itemSubclasses) / sizeof(s_itemSubclasses[0]); i++) {
        s_itemSubclasses[i].SetCount(counts[i]);
    }

    for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
        const ItemSubClassRec* rec = g_itemSubClassDB.GetRecordByIndex(i);

        if (rec->m_classID >= 0) {
            s_itemSubclasses[rec->m_classID][rec->m_subClassID] = rec;
        }
    }
}
