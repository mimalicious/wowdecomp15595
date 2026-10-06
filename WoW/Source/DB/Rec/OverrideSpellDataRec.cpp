#include "WoW/Source/DB/Rec/OverrideSpellDataRec.h"

// STATIC: %0x00B45B24
const DBCMeta OverrideSpellDataRec::s_meta = { "DBFilesClient\\OverrideSpellData.dbc", 4, 52, OverrideSpellDataRec::FixupStrings, 0 };

// OFFSET: %0x0097C310
void OverrideSpellDataRec::FixupStrings(void* record, const char* strings) {
    OverrideSpellDataRec* rec = static_cast<OverrideSpellDataRec*>(record);
    rec->unk_0030 = &strings[reinterpret_cast<uintptr_t>(rec->unk_0030)];
}

#if defined(WOWDECOMP_ARCH_64)
void OverrideSpellDataRec::ConvertRow(OverrideSpellDataRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_spells, &row[0x0004], sizeof(rec->m_spells));
    memcpy(&rec->unk_002C, &row[0x002C], sizeof(rec->unk_002C));
    rec->unk_0030 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0030])];
}
#endif
