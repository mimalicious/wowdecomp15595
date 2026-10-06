#include "WoW/Source/DB/Rec/GameTablesRec.h"

// STATIC: %0x00B469AC
const DBCMeta GameTablesRec::s_meta = { "DBFilesClient\\GameTables.dbc", 3, 16, GameTablesRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void GameTablesRec::FixupStrings(void* record, const char* strings) {
    GameTablesRec* rec = static_cast<GameTablesRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void GameTablesRec::ConvertRow(GameTablesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_numRows, &row[0x0008], sizeof(rec->m_numRows));
    memcpy(&rec->m_numColumns, &row[0x000C], sizeof(rec->m_numColumns));
}
#endif
