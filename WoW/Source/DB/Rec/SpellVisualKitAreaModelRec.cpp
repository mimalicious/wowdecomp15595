#include "WoW/Source/DB/Rec/SpellVisualKitAreaModelRec.h"

// STATIC: %0x00B44BA4
const DBCMeta SpellVisualKitAreaModelRec::s_meta = { "DBFilesClient\\SpellVisualKitAreaModel.dbc", 3, 12, SpellVisualKitAreaModelRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void SpellVisualKitAreaModelRec::FixupStrings(void* record, const char* strings) {
    SpellVisualKitAreaModelRec* rec = static_cast<SpellVisualKitAreaModelRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void SpellVisualKitAreaModelRec::ConvertRow(SpellVisualKitAreaModelRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_enumID, &row[0x0008], sizeof(rec->m_enumID));
}
#endif
