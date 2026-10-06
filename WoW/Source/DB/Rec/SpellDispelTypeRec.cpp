#include "WoW/Source/DB/Rec/SpellDispelTypeRec.h"

// STATIC: %0x00B45144
const DBCMeta SpellDispelTypeRec::s_meta = { "DBFilesClient\\SpellDispelType.dbc", 5, 20, SpellDispelTypeRec::FixupStrings, 0 };

// OFFSET: %0x0097C2D0
void SpellDispelTypeRec::FixupStrings(void* record, const char* strings) {
    SpellDispelTypeRec* rec = static_cast<SpellDispelTypeRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_internalName = &strings[reinterpret_cast<uintptr_t>(rec->m_internalName)];
}

#if defined(WOWDECOMP_ARCH_64)
void SpellDispelTypeRec::ConvertRow(SpellDispelTypeRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_mask, &row[0x0008], sizeof(rec->m_mask));
    memcpy(&rec->m_immunityPossible, &row[0x000C], sizeof(rec->m_immunityPossible));
    rec->m_internalName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0010])];
}
#endif
