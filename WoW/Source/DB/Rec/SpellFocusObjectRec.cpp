#include "WoW/Source/DB/Rec/SpellFocusObjectRec.h"

// STATIC: %0x00B44FF0
const DBCMeta SpellFocusObjectRec::s_meta = { "DBFilesClient\\SpellFocusObject.dbc", 2, 8, SpellFocusObjectRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void SpellFocusObjectRec::FixupStrings(void* record, const char* strings) {
    SpellFocusObjectRec* rec = static_cast<SpellFocusObjectRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void SpellFocusObjectRec::ConvertRow(SpellFocusObjectRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
