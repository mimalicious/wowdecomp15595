#include "WoW/Source/DB/Rec/ItemVisualsRec.h"

// STATIC: %0x00B43A54
const DBCMeta ItemVisualsRec::s_meta = { "DBFilesClient\\ItemVisuals.dbc", 2, 24, ItemVisualsRec::FixupStrings, 0 };

// PROVEN: d392dd33 IDENTICAL
// OFFSET: 0x00401000
void ItemVisualsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void ItemVisualsRec::ConvertRow(ItemVisualsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_slot, &row[0x0004], sizeof(rec->m_slot));
}
#endif
