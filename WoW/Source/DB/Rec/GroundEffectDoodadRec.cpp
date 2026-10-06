#include "WoW/Source/DB/Rec/GroundEffectDoodadRec.h"

// STATIC: %0x00B43C14
const DBCMeta GroundEffectDoodadRec::s_meta = { "DBFilesClient\\GroundEffectDoodad.dbc", 3, 12, GroundEffectDoodadRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void GroundEffectDoodadRec::FixupStrings(void* record, const char* strings) {
    GroundEffectDoodadRec* rec = static_cast<GroundEffectDoodadRec*>(record);
    rec->m_doodadpath = &strings[reinterpret_cast<uintptr_t>(rec->m_doodadpath)];
}

#if defined(WOWDECOMP_ARCH_64)
void GroundEffectDoodadRec::ConvertRow(GroundEffectDoodadRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_doodadpath = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_flags, &row[0x0008], sizeof(rec->m_flags));
}
#endif
