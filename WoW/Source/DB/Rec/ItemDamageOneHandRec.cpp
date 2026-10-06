#include "WoW/Source/DB/Rec/ItemDamageOneHandRec.h"

// STATIC: 0x00C245C4/0x140A19CE0
static const int32_t s_fieldOffsets[3] = { 0, 4, 32 };

// STATIC: 0x00C245D0/0x140A19CF0
static const int32_t s_fieldSizes[3] = { 4, 28, 4 };

// STATIC: 0x00C245DC/0x140A19D00
static const int32_t s_fieldTypes[3] = { 0, 3, 0 };

// STATIC: 0x00C245E8/0x140A19D10
const DBCMeta ItemDamageOneHandRec::s_meta = { "DBFilesClient\\ItemDamageOneHand.dbc", 3, 36, ItemDamageOneHandRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 0fb8b618 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemDamageOneHandRec::FixupStrings(void* record, const char* strings) {
}
