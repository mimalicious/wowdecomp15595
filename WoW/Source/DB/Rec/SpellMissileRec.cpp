#include "WoW/Source/DB/Rec/SpellMissileRec.h"

// STATIC: %0x00B44E68
const DBCMeta SpellMissileRec::s_meta = { "DBFilesClient\\SpellMissile.dbc", 15, 60, SpellMissileRec::FixupStrings, 0 };

// PROVEN: fe6a3b17 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellMissileRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellMissileRec::ConvertRow(SpellMissileRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_flags, &row[0x0004], sizeof(rec->m_flags));
    memcpy(&rec->m_defaultPitchMin, &row[0x0008], sizeof(rec->m_defaultPitchMin));
    memcpy(&rec->m_defaultPitchMax, &row[0x000C], sizeof(rec->m_defaultPitchMax));
    memcpy(&rec->m_defaultSpeedMin, &row[0x0010], sizeof(rec->m_defaultSpeedMin));
    memcpy(&rec->m_defaultSpeedMax, &row[0x0014], sizeof(rec->m_defaultSpeedMax));
    memcpy(&rec->m_randomizeFacingMin, &row[0x0018], sizeof(rec->m_randomizeFacingMin));
    memcpy(&rec->m_randomizeFacingMax, &row[0x001C], sizeof(rec->m_randomizeFacingMax));
    memcpy(&rec->m_randomizePitchMin, &row[0x0020], sizeof(rec->m_randomizePitchMin));
    memcpy(&rec->m_randomizePitchMax, &row[0x0024], sizeof(rec->m_randomizePitchMax));
    memcpy(&rec->m_randomizeSpeedMin, &row[0x0028], sizeof(rec->m_randomizeSpeedMin));
    memcpy(&rec->m_randomizeSpeedMax, &row[0x002C], sizeof(rec->m_randomizeSpeedMax));
    memcpy(&rec->m_gravity, &row[0x0030], sizeof(rec->m_gravity));
    memcpy(&rec->m_maxDuration, &row[0x0034], sizeof(rec->m_maxDuration));
    memcpy(&rec->m_collisionRadius, &row[0x0038], sizeof(rec->m_collisionRadius));
}
#endif
