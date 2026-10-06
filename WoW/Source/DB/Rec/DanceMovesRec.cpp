#include "WoW/Source/DB/Rec/DanceMovesRec.h"

// STATIC: %0x00B46DC4
const DBCMeta DanceMovesRec::s_meta = { "DBFilesClient\\DanceMoves.dbc", 8, 32, DanceMovesRec::FixupStrings, 0 };

// OFFSET: %0x0097C220
void DanceMovesRec::FixupStrings(void* record, const char* strings) {
    DanceMovesRec* rec = static_cast<DanceMovesRec*>(record);
    rec->m_internalName = &strings[reinterpret_cast<uintptr_t>(rec->m_internalName)];
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void DanceMovesRec::ConvertRow(DanceMovesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_type, &row[0x0004], sizeof(rec->m_type));
    memcpy(&rec->m_param, &row[0x0008], sizeof(rec->m_param));
    memcpy(&rec->m_fallback, &row[0x000C], sizeof(rec->m_fallback));
    memcpy(&rec->m_racemask, &row[0x0010], sizeof(rec->m_racemask));
    rec->m_internalName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0014])];
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0018])];
    memcpy(&rec->m_lockID, &row[0x001C], sizeof(rec->m_lockID));
}
#endif
