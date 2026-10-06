#include "WoW/Source/DB/Rec/SpellMissileMotionRec.h"

// STATIC: 0x00C20ECC/0x140A15910
const DBCMeta SpellMissileMotionRec::s_meta = { "DBFilesClient\\SpellMissileMotion.dbc", 5, 20, SpellMissileMotionRec::FixupStrings, 0 };

// PROVEN: 3f40c0b5 IDENTICAL/NONE
// OFFSET: 0x00A2E9B0/0x1407D8110
void SpellMissileMotionRec::FixupStrings(void* record, const char* strings) {
    SpellMissileMotionRec* rec = static_cast<SpellMissileMotionRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_scriptBody, strings);
}
