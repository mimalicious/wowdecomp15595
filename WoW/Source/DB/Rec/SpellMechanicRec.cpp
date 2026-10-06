#include "WoW/Source/DB/Rec/SpellMechanicRec.h"

// STATIC: %0x00B44E9C
const DBCMeta SpellMechanicRec::s_meta = { "DBFilesClient\\SpellMechanic.dbc", 2, 8, SpellMechanicRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void SpellMechanicRec::FixupStrings(void* record, const char* strings) {
    SpellMechanicRec* rec = static_cast<SpellMechanicRec*>(record);
    rec->m_stateName = &strings[reinterpret_cast<uintptr_t>(rec->m_stateName)];
}

#if defined(WOWDECOMP_ARCH_64)
void SpellMechanicRec::ConvertRow(SpellMechanicRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_stateName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
