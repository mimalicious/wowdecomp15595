#include "WoW/Source/DB/Rec/SpellFlyoutItemRec.h"

// STATIC: 0x00C21390/0x140A15EE0
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C213A0/0x140A15EF0
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C213B0/0x140A15F00
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C213C0/0x140A15F10
const DBCMeta SpellFlyoutItemRec::s_meta = { "DBFilesClient\\SpellFlyoutItem.dbc", 4, 16, SpellFlyoutItemRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 164e5513 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SpellFlyoutItemRec::FixupStrings(void* record, const char* strings) {
}
