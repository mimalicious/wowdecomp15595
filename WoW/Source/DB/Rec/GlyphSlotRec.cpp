#include "WoW/Source/DB/Rec/GlyphSlotRec.h"

// STATIC: %0x00B4690C
const DBCMeta GlyphSlotRec::s_meta = { "DBFilesClient\\GlyphSlot.dbc", 3, 12, GlyphSlotRec::FixupStrings, 0 };

// PROVEN: c2aed5e8 IDENTICAL
// OFFSET: 0x00401000
void GlyphSlotRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void GlyphSlotRec::ConvertRow(GlyphSlotRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_type, &row[0x0004], sizeof(rec->m_type));
    memcpy(&rec->m_tooltip, &row[0x0008], sizeof(rec->m_tooltip));
}
#endif
