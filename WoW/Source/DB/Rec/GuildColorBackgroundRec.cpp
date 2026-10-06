#include "WoW/Source/DB/Rec/GuildColorBackgroundRec.h"

// STATIC: 0x00C1D424/0x140A11320
static const int32_t s_fieldOffsets[4] = { 0, 4, 5, 6 };

// STATIC: 0x00C1D434/0x140A11330
static const int32_t s_fieldSizes[4] = { 4, 1, 1, 1 };

// STATIC: 0x00C1D444/0x140A11340
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C1D454/0x140A11350
const DBCMeta GuildColorBackgroundRec::s_meta = { "DBFilesClient\\GuildColorBackground.dbc", 4, 8, GuildColorBackgroundRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 3380f664 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GuildColorBackgroundRec::FixupStrings(void* record, const char* strings) {
}
