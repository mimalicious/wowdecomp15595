#include "AnimReplacementSet.h"
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

// STATIC: 0x00DD58E4/0x140CBBFF0
TSGrowableArray<CAnimReplacementSetDef*> CAnimReplacementSetDef::s_defs;

// OFFSET: 0x0058B960/0x1401F4FC0
CAnimReplacementSetDef* CAnimReplacementSetDef::GetDef(uint32_t id) {
    if (id >= CAnimReplacementSetDef::s_defs.Count()) {
        return NULL;
    }

    return CAnimReplacementSetDef::s_defs[id];
}

// OFFSET: 0x0058B980/0x1401F4FE0
uint32_t CAnimReplacementSetDef::FilterAnim(uint32_t anim) const {
    if (this->m_rangeCount) {
        if (anim - this->m_base < this->m_rangeCount) {
            return this->m_range[anim - this->m_base];
        }
    } else {
        for (uint32_t i = 0; i < this->m_count; i++) {
            if (this->m_list[i]->m_srcAnimID == anim) {
                return this->m_list[i]->m_dstAnimID;
            }
        }
    }

    return anim;
}

// OFFSET: 0x0058BA90/0x1401F5140
void CAnimReplacementSetDef::InitSystem() {
    uint32_t count = g_animReplacementSetDB.m_maxID + 1;
    CAnimReplacementSetDef::s_defs.SetCount(count);
    memset(CAnimReplacementSetDef::s_defs.m_data, 0, sizeof(CAnimReplacementSetDef*) * count);

    for (int32_t i = 0; i < g_animReplacementSetDB.m_numRecords; i++) {
        const AnimReplacementSetRec* rec = g_animReplacementSetDB.GetRecordByIndex(i);
        CAnimReplacementSetDef* def = static_cast<CAnimReplacementSetDef*>(SMemAlloc(sizeof(CAnimReplacementSetDef), __FILE__, __LINE__, 0));
        CAnimReplacementSetDef::s_defs.m_data[rec->m_id] = def;
        def->m_rec = rec;
        def->m_count = 0;
    }

    int32_t numReplacements = g_animReplacementDB.m_numRecords;

    for (int32_t i = 0; i < numReplacements; i++) {
        const AnimReplacementRec* rec = g_animReplacementDB.GetRecordByIndex(i);
        CAnimReplacementSetDef* def = CAnimReplacementSetDef::s_defs.m_data[rec->m_parentAnimReplacementSetID];

        if (!def) {
            continue;
        }

        if (def->m_count) {
            def->m_count++;

            if (def->m_minAnim > static_cast<uint16_t>(rec->m_srcAnimID)) {
                def->m_minAnim = rec->m_srcAnimID;
            }

            if (def->m_maxAnim < static_cast<uint16_t>(rec->m_srcAnimID)) {
                def->m_maxAnim = rec->m_srcAnimID;
            }
        } else {
            def->m_count = 1;
            def->m_minAnim = rec->m_srcAnimID;
            def->m_maxAnim = rec->m_srcAnimID;
        }
    }

    for (uint32_t i = 0; i < CAnimReplacementSetDef::s_defs.Count(); i++) {
        CAnimReplacementSetDef* def = CAnimReplacementSetDef::s_defs.m_data[i];

        if (!def) {
            continue;
        }

        if (def->m_count <= 16) {
            def->m_rangeCount = 0;
            def->m_list = static_cast<const AnimReplacementRec**>(SMemAlloc(sizeof(const AnimReplacementRec*) * def->m_count, __FILE__, __LINE__, 0));
            def->m_count = 0;
        } else {
            uint16_t minAnim = def->m_minAnim;
            def->m_rangeCount = def->m_maxAnim - minAnim + 1;
            def->m_base = minAnim;
            def->m_range = static_cast<uint16_t*>(SMemAlloc(sizeof(uint16_t) * def->m_rangeCount, __FILE__, __LINE__, 0));

            for (uint32_t j = 0; j < def->m_rangeCount; j++) {
                def->m_range[j] = 0xFFFF;
            }
        }
    }

    for (int32_t i = 0; i < numReplacements; i++) {
        const AnimReplacementRec* rec = g_animReplacementDB.GetRecordByIndex(i);
        CAnimReplacementSetDef* def = CAnimReplacementSetDef::s_defs.m_data[rec->m_parentAnimReplacementSetID];

        if (!def) {
            continue;
        }

        uint16_t base = def->m_base;

        if (def->m_rangeCount) {
            def->m_range[rec->m_srcAnimID - base] = rec->m_dstAnimID;
        } else {
            def->m_list[base] = rec;
            def->m_count++;
        }
    }
}

// OFFSET: 0x0058BD10/0x1401F5480
void CAnimReplacementSetDef::UninitSystem() {
    for (uint32_t i = 0; i < CAnimReplacementSetDef::s_defs.Count(); i++) {
        CAnimReplacementSetDef* def = CAnimReplacementSetDef::s_defs.m_data[i];

        if (!def) {
            continue;
        }

        if (def->m_rangeCount) {
            SMemFree(def->m_range, __FILE__, __LINE__, 0);
        } else {
            SMemFree(def->m_list, __FILE__, __LINE__, 0);
        }

        SMemFree(CAnimReplacementSetDef::s_defs.m_data[i], __FILE__, __LINE__, 0);
    }

    CAnimReplacementSetDef::s_defs.m_count = 0;
}
