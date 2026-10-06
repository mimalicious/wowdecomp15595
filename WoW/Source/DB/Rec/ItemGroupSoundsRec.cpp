#include "WoW/Source/DB/Rec/ItemGroupSoundsRec.h"

// STATIC: %0x00B4608C
const DBCMeta ItemGroupSoundsRec::s_meta = { "DBFilesClient\\ItemGroupSounds.dbc", 2, 20, ItemGroupSoundsRec::FixupStrings, 0 };

// PROVEN: 761e0107 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemGroupSoundsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void ItemGroupSoundsRec::ConvertRow(ItemGroupSoundsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_sound, &row[0x0004], sizeof(rec->m_sound));
}
#endif
