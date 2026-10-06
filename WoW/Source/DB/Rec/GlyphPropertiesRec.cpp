#include "WoW/Source/DB/Rec/GlyphPropertiesRec.h"

// STATIC: %0x00B46944
const DBCMeta GlyphPropertiesRec::s_meta = { "DBFilesClient\\GlyphProperties.dbc", 4, 16, GlyphPropertiesRec::FixupStrings, 0 };

// PROVEN: b9193c7d IDENTICAL
// OFFSET: 0x00401000
void GlyphPropertiesRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void GlyphPropertiesRec::ConvertRow(GlyphPropertiesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_spellID, &row[0x0004], sizeof(rec->m_spellID));
    memcpy(&rec->m_glyphSlotFlags, &row[0x0008], sizeof(rec->m_glyphSlotFlags));
    memcpy(&rec->m_spellIconID, &row[0x000C], sizeof(rec->m_spellIconID));
}
#endif
