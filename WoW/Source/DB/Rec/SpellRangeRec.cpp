#include "WoW/Source/DB/Rec/SpellRangeRec.h"

// STATIC: %0x00B44DC4
const DBCMeta SpellRangeRec::s_meta = { "DBFilesClient\\SpellRange.dbc", 6, 32, SpellRangeRec::FixupStrings, 0 };

// OFFSET: %0x0097C1D0
void SpellRangeRec::FixupStrings(void* record, const char* strings) {
    SpellRangeRec* rec = static_cast<SpellRangeRec*>(record);
    rec->m_displayName = &strings[reinterpret_cast<uintptr_t>(rec->m_displayName)];
    rec->m_displayNameShort = &strings[reinterpret_cast<uintptr_t>(rec->m_displayNameShort)];
}

#if defined(WOWDECOMP_ARCH_64)
void SpellRangeRec::ConvertRow(SpellRangeRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_rangeMin, &row[0x0004], sizeof(rec->m_rangeMin));
    memcpy(rec->m_rangeMax, &row[0x000C], sizeof(rec->m_rangeMax));
    memcpy(&rec->m_flags, &row[0x0014], sizeof(rec->m_flags));
    rec->m_displayName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0018])];
    rec->m_displayNameShort = &strings[*reinterpret_cast<const uint32_t*>(&row[0x001C])];
}
#endif
