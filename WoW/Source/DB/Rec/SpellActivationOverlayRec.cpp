#include "WoW/Source/DB/Rec/SpellActivationOverlayRec.h"

// STATIC: 0x00C22154/0x140A16F20
static const int32_t s_fieldOffsets[9] = { 0, 4, 8, 12, 16, 20, 24, 36, 40 };

// STATIC: 0x00C22178/0x140A16F48
static const int32_t s_fieldSizes[9] = { 4, 4, 4, 4, 4, 4, 12, 4, 4 };

// STATIC: 0x00C2219C/0x140A16F70
static const int32_t s_fieldTypes[9] = { 0, 0, 0, 0, 0, 3, 0, 0, 0 };

// STATIC: 0x00C221C0/0x140A16F98
const DBCMeta SpellActivationOverlayRec::s_meta = { "DBFilesClient\\SpellActivationOverlay.dbc", 9, 44, SpellActivationOverlayRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: aab4ed50 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellActivationOverlayRec::FixupStrings(void* record, const char* strings) {
}
