#include "WoW/Source/DB/Rec/LfgDungeonGroupRec.h"

// STATIC: %0x00B45E00
const DBCMeta LfgDungeonGroupRec::s_meta = { "DBFilesClient\\LfgDungeonGroup.dbc", 5, 20, LfgDungeonGroupRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void LfgDungeonGroupRec::FixupStrings(void* record, const char* strings) {
    LfgDungeonGroupRec* rec = static_cast<LfgDungeonGroupRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void LfgDungeonGroupRec::ConvertRow(LfgDungeonGroupRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_orderIndex, &row[0x0008], sizeof(rec->m_orderIndex));
    memcpy(&rec->m_parentGroupID, &row[0x000C], sizeof(rec->m_parentGroupID));
    memcpy(&rec->m_typeID, &row[0x0010], sizeof(rec->m_typeID));
}
#endif
