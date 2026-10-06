#include "WoW/Source/DB/Rec/SpellEffectCameraShakesRec.h"

// STATIC: 0x00C21518/0x140A160C0
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C21520/0x140A160C8
static const int32_t s_fieldSizes[2] = { 4, 12 };

// STATIC: 0x00C21528/0x140A160D0
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C21530/0x140A160D8
const DBCMeta SpellEffectCameraShakesRec::s_meta = { "DBFilesClient\\SpellEffectCameraShakes.dbc", 2, 16, SpellEffectCameraShakesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: b53220ba IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellEffectCameraShakesRec::FixupStrings(void* record, const char* strings) {
}
