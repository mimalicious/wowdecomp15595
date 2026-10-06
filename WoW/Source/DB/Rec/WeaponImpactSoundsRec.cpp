#include "WoW/Source/DB/Rec/WeaponImpactSoundsRec.h"

// STATIC: %0x00B445F0
const DBCMeta WeaponImpactSoundsRec::s_meta = { "DBFilesClient\\WeaponImpactSounds.dbc", 5, 92, WeaponImpactSoundsRec::FixupStrings, 0 };

// PROVEN: 67adb663 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void WeaponImpactSoundsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void WeaponImpactSoundsRec::ConvertRow(WeaponImpactSoundsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_weaponSubClassID, &row[0x0004], sizeof(rec->m_weaponSubClassID));
    memcpy(&rec->m_parrySoundType, &row[0x0008], sizeof(rec->m_parrySoundType));
    memcpy(rec->m_impactSoundID, &row[0x000C], sizeof(rec->m_impactSoundID));
    memcpy(rec->m_critImpactSoundID, &row[0x0034], sizeof(rec->m_critImpactSoundID));
}
#endif
