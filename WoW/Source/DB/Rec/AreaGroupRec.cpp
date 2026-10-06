#include "WoW/Source/DB/Rec/AreaGroupRec.h"

// STATIC: %0x00B472A8
const DBCMeta AreaGroupRec::s_meta = { "DBFilesClient\\AreaGroup.dbc", 3, 32, AreaGroupRec::FixupStrings, 0 };

// PROVEN: c030281e IDENTICAL
// OFFSET: 0x00401000
void AreaGroupRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void AreaGroupRec::ConvertRow(AreaGroupRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_areaID, &row[0x0004], sizeof(rec->m_areaID));
    memcpy(&rec->m_nextAreaID, &row[0x001C], sizeof(rec->m_nextAreaID));
}
#endif
