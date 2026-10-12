#include "WoW/Source/DB/Rec/SpellMissileMotionRec.h"

// STATIC: 0x00C20E90/0x140A158C8
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C20EA4/0x140A158E0
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C20EB8/0x140A158F8
static const int32_t s_fieldTypes[5] = { 0, 2, 2, 0, 0 };

// STATIC: 0x00C20ECC/0x140A15910
const DBCMeta SpellMissileMotionRec::s_meta = { "DBFilesClient\\SpellMissileMotion.dbc", 5, 20, SpellMissileMotionRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 3f40c0b5 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2E9B0/0x1407D8110
void SpellMissileMotionRec::FixupStrings(void* record, const char* strings) {
    SpellMissileMotionRec* rec = static_cast<SpellMissileMotionRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_scriptBody, strings);
}
