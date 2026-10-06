#include "WoW/Source/DB/Rec/AreaTriggerRec.h"

// STATIC: %0x00B471D0
const DBCMeta AreaTriggerRec::s_meta = { "DBFilesClient\\AreaTrigger.dbc", 11, 52, AreaTriggerRec::FixupStrings, 0 };

// PROVEN: 9a7beaa9 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void AreaTriggerRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void AreaTriggerRec::ConvertRow(AreaTriggerRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_continentID, &row[0x0004], sizeof(rec->m_continentID));
    memcpy(rec->m_pos, &row[0x0008], sizeof(rec->m_pos));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
    memcpy(&rec->unk_0018, &row[0x0018], sizeof(rec->unk_0018));
    memcpy(&rec->unk_001C, &row[0x001C], sizeof(rec->unk_001C));
    memcpy(&rec->m_radius, &row[0x0020], sizeof(rec->m_radius));
    memcpy(&rec->m_boxLength, &row[0x0024], sizeof(rec->m_boxLength));
    memcpy(&rec->m_boxWidth, &row[0x0028], sizeof(rec->m_boxWidth));
    memcpy(&rec->m_boxHeight, &row[0x002C], sizeof(rec->m_boxHeight));
    memcpy(&rec->m_boxYaw, &row[0x0030], sizeof(rec->m_boxYaw));
}
#endif
