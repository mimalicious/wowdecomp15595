#include "WoW/Source/DB/Rec/PackageRec.h"

// STATIC: %0x00B45AEC
const DBCMeta PackageRec::s_meta = { "DBFilesClient\\Package.dbc", 4, 16, PackageRec::FixupStrings, 0 };

// OFFSET: %0x0097C2F0
void PackageRec::FixupStrings(void* record, const char* strings) {
    PackageRec* rec = static_cast<PackageRec*>(record);
    rec->m_icon = &strings[reinterpret_cast<uintptr_t>(rec->m_icon)];
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void PackageRec::ConvertRow(PackageRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_icon = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_cost, &row[0x0008], sizeof(rec->m_cost));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
}
#endif
