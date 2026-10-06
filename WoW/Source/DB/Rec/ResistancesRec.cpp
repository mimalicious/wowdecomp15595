#include "WoW/Source/DB/Rec/ResistancesRec.h"

// STATIC: %0x00B43664
const DBCMeta ResistancesRec::s_meta = { "DBFilesClient\\Resistances.dbc", 4, 16, ResistancesRec::FixupStrings, 0 };

// OFFSET: %0x0097C4E0
void ResistancesRec::FixupStrings(void* record, const char* strings) {
    ResistancesRec* rec = static_cast<ResistancesRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void ResistancesRec::ConvertRow(ResistancesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_flags, &row[0x0004], sizeof(rec->m_flags));
    memcpy(&rec->m_fizzleSoundID, &row[0x0008], sizeof(rec->m_fizzleSoundID));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
}
#endif
