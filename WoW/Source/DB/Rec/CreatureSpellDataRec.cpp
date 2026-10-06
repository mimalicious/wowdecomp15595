#include "WoW/Source/DB/Rec/CreatureSpellDataRec.h"

// STATIC: %0x00B46E9C
const DBCMeta CreatureSpellDataRec::s_meta = { "DBFilesClient\\CreatureSpellData.dbc", 3, 36, CreatureSpellDataRec::FixupStrings, 0 };

// PROVEN: 86e4a389 IDENTICAL/NONE
// OFFSET: 0x00401000
void CreatureSpellDataRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void CreatureSpellDataRec::ConvertRow(CreatureSpellDataRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_spells, &row[0x0004], sizeof(rec->m_spells));
    memcpy(rec->m_availability, &row[0x0014], sizeof(rec->m_availability));
}
#endif
