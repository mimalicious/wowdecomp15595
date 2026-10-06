#include "WoW/Source/DB/Rec/UnitBloodRec.h"

// STATIC: %0x00B447CC
const DBCMeta UnitBloodRec::s_meta = { "DBFilesClient\\UnitBlood.dbc", 4, 40, UnitBloodRec::FixupStrings, 0 };

// OFFSET: %0x0097C190
void UnitBloodRec::FixupStrings(void* record, const char* strings) {
    UnitBloodRec* rec = static_cast<UnitBloodRec*>(record);
    rec->m_groundBlood[0] = &strings[reinterpret_cast<uintptr_t>(rec->m_groundBlood[0])];
    rec->m_groundBlood[1] = &strings[reinterpret_cast<uintptr_t>(rec->m_groundBlood[1])];
    rec->m_groundBlood[2] = &strings[reinterpret_cast<uintptr_t>(rec->m_groundBlood[2])];
    rec->m_groundBlood[3] = &strings[reinterpret_cast<uintptr_t>(rec->m_groundBlood[3])];
    rec->m_groundBlood[4] = &strings[reinterpret_cast<uintptr_t>(rec->m_groundBlood[4])];
}

#if defined(WOWDECOMP_ARCH_64)
void UnitBloodRec::ConvertRow(UnitBloodRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_combatBloodSpurtFront, &row[0x0004], sizeof(rec->m_combatBloodSpurtFront));
    memcpy(rec->m_combatBloodSpurtBack, &row[0x000C], sizeof(rec->m_combatBloodSpurtBack));
    rec->m_groundBlood[0] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0014])];
    rec->m_groundBlood[1] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0018])];
    rec->m_groundBlood[2] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x001C])];
    rec->m_groundBlood[3] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0020])];
    rec->m_groundBlood[4] = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0024])];
}
#endif
