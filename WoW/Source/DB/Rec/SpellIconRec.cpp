#include "WoW/Source/DB/Rec/SpellIconRec.h"

// STATIC: %0x00B44FB8
const DBCMeta SpellIconRec::s_meta = { "DBFilesClient\\SpellIcon.dbc", 2, 8, SpellIconRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void SpellIconRec::FixupStrings(void* record, const char* strings) {
    SpellIconRec* rec = static_cast<SpellIconRec*>(record);
    rec->m_textureFilename = &strings[reinterpret_cast<uintptr_t>(rec->m_textureFilename)];
}

#if defined(WOWDECOMP_ARCH_64)
void SpellIconRec::ConvertRow(SpellIconRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_textureFilename = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
