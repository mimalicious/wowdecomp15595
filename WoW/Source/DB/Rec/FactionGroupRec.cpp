#include "WoW/Source/DB/Rec/FactionGroupRec.h"

// STATIC: %0x00B43D18
const DBCMeta FactionGroupRec::s_meta = { "DBFilesClient\\FactionGroup.dbc", 4, 16, FactionGroupRec::FixupStrings, 0 };

// OFFSET: %0x0097C460
void FactionGroupRec::FixupStrings(void* record, const char* strings) {
    FactionGroupRec* rec = static_cast<FactionGroupRec*>(record);
    rec->m_internalName = &strings[reinterpret_cast<uintptr_t>(rec->m_internalName)];
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void FactionGroupRec::ConvertRow(FactionGroupRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_maskID, &row[0x0004], sizeof(rec->m_maskID));
    rec->m_internalName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
}
#endif
