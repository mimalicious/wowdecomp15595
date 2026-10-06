#include "WoW/Source/DB/Rec/SpellMissileMotionRec.h"

// STATIC: %0x00B44E34
const DBCMeta SpellMissileMotionRec::s_meta = { "DBFilesClient\\SpellMissileMotion.dbc", 5, 20, SpellMissileMotionRec::FixupStrings, 0 };

// OFFSET: %0x0097C1F0
void SpellMissileMotionRec::FixupStrings(void* record, const char* strings) {
    SpellMissileMotionRec* rec = static_cast<SpellMissileMotionRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_scriptBody = &strings[reinterpret_cast<uintptr_t>(rec->m_scriptBody)];
}

#if defined(WOWDECOMP_ARCH_64)
void SpellMissileMotionRec::ConvertRow(SpellMissileMotionRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    rec->m_scriptBody = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    memcpy(&rec->m_flags, &row[0x000C], sizeof(rec->m_flags));
    memcpy(&rec->m_missileCount, &row[0x0010], sizeof(rec->m_missileCount));
}
#endif
