#include "WoW/Source/DB/Rec/SpellSpecialUnitEffectRec.h"

// STATIC: 0x00C209C0/0x140A152F8
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C209C8/0x140A15300
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C209D0/0x140A15308
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C209D8/0x140A15310
const DBCMeta SpellSpecialUnitEffectRec::s_meta = { "DBFilesClient\\SpellSpecialUnitEffect.dbc", 2, 8, SpellSpecialUnitEffectRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 91cb15e1 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellSpecialUnitEffectRec::FixupStrings(void* record, const char* strings) {
}
