#include "WoW/Source/DB/Rec/WeaponSwingSounds2Rec.h"

// STATIC: %0x00B445B4
const DBCMeta WeaponSwingSounds2Rec::s_meta = { "DBFilesClient\\WeaponSwingSounds2.dbc", 4, 16, WeaponSwingSounds2Rec::FixupStrings, 0 };

// OFFSET: %0x00401000
void WeaponSwingSounds2Rec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void WeaponSwingSounds2Rec::ConvertRow(WeaponSwingSounds2Rec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_swingType, &row[0x0004], sizeof(rec->m_swingType));
    memcpy(&rec->m_crit, &row[0x0008], sizeof(rec->m_crit));
    memcpy(&rec->m_soundID, &row[0x000C], sizeof(rec->m_soundID));
}
#endif
