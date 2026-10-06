#include "WoW/Source/DB/Rec/GuildColorBorderRec.h"

// STATIC: 0x00C1D3AC/0x140A11290
static const int32_t s_fieldOffsets[4] = { 0, 4, 5, 6 };

// STATIC: 0x00C1D3BC/0x140A112A0
static const int32_t s_fieldSizes[4] = { 4, 1, 1, 1 };

// STATIC: 0x00C1D3CC/0x140A112B0
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C1D3DC/0x140A112C0
const DBCMeta GuildColorBorderRec::s_meta = { "DBFilesClient\\GuildColorBorder.dbc", 4, 8, GuildColorBorderRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 7c943553 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GuildColorBorderRec::FixupStrings(void* record, const char* strings) {
}
