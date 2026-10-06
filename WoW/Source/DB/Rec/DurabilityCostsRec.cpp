#include "WoW/Source/DB/Rec/DurabilityCostsRec.h"

// STATIC: %0x00B46C78
const DBCMeta DurabilityCostsRec::s_meta = { "DBFilesClient\\DurabilityCosts.dbc", 3, 120, DurabilityCostsRec::FixupStrings, 0 };

// PROVEN: 0500123f IDENTICAL
// OFFSET: 0x00401000
void DurabilityCostsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void DurabilityCostsRec::ConvertRow(DurabilityCostsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_weaponSubClassCost, &row[0x0004], sizeof(rec->m_weaponSubClassCost));
    memcpy(rec->m_armorSubClassCost, &row[0x0058], sizeof(rec->m_armorSubClassCost));
}
#endif
