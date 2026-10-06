#include "WoW/Source/DB/Rec/MaterialRec.h"

// STATIC: %0x00B45C24
const DBCMeta MaterialRec::s_meta = { "DBFilesClient\\Material.dbc", 5, 20, MaterialRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void MaterialRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void MaterialRec::ConvertRow(MaterialRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_flags, &row[0x0004], sizeof(rec->m_flags));
    memcpy(&rec->m_foleySoundID, &row[0x0008], sizeof(rec->m_foleySoundID));
    memcpy(&rec->m_sheatheSoundID, &row[0x000C], sizeof(rec->m_sheatheSoundID));
    memcpy(&rec->m_unsheatheSoundID, &row[0x0010], sizeof(rec->m_unsheatheSoundID));
}
#endif
