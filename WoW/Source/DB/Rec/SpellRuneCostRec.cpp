#include "WoW/Source/DB/Rec/SpellRuneCostRec.h"

// STATIC: %0x00B44D28
const DBCMeta SpellRuneCostRec::s_meta = { "DBFilesClient\\SpellRuneCost.dbc", 5, 20, SpellRuneCostRec::FixupStrings, 0 };

// PROVEN: 55483cd9 IDENTICAL/NONE
// OFFSET: 0x00401000
void SpellRuneCostRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellRuneCostRec::ConvertRow(SpellRuneCostRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_blood, &row[0x0004], sizeof(rec->m_blood));
    memcpy(&rec->m_unholy, &row[0x0008], sizeof(rec->m_unholy));
    memcpy(&rec->m_frost, &row[0x000C], sizeof(rec->m_frost));
    memcpy(&rec->m_runicPower, &row[0x0010], sizeof(rec->m_runicPower));
}
#endif
