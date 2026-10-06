#include "WoW/Source/DB/Rec/SummonPropertiesRec.h"

// STATIC: %0x00B44ABC
const DBCMeta SummonPropertiesRec::s_meta = { "DBFilesClient\\SummonProperties.dbc", 6, 24, SummonPropertiesRec::FixupStrings, 0 };

// PROVEN: 8c994a57 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SummonPropertiesRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SummonPropertiesRec::ConvertRow(SummonPropertiesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_control, &row[0x0004], sizeof(rec->m_control));
    memcpy(&rec->m_faction, &row[0x0008], sizeof(rec->m_faction));
    memcpy(&rec->m_title, &row[0x000C], sizeof(rec->m_title));
    memcpy(&rec->m_slot, &row[0x0010], sizeof(rec->m_slot));
    memcpy(&rec->m_flags, &row[0x0014], sizeof(rec->m_flags));
}
#endif
