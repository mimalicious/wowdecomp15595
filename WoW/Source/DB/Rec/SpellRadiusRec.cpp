#include "WoW/Source/DB/Rec/SpellRadiusRec.h"

// STATIC: %0x00B44DF8
const DBCMeta SpellRadiusRec::s_meta = { "DBFilesClient\\SpellRadius.dbc", 4, 16, SpellRadiusRec::FixupStrings, 0 };

// PROVEN: ff33f4ef IDENTICAL
// OFFSET: 0x00401000
void SpellRadiusRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellRadiusRec::ConvertRow(SpellRadiusRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_radius, &row[0x0004], sizeof(rec->m_radius));
    memcpy(&rec->m_radiusPerLevel, &row[0x0008], sizeof(rec->m_radiusPerLevel));
    memcpy(&rec->m_radiusMax, &row[0x000C], sizeof(rec->m_radiusMax));
}
#endif
