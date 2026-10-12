#include "WoW/Source/DB/Rec/GlueScreenEmoteRec.h"

// STATIC: 0x00C1D498/0x140A113B0
static const int32_t s_fieldOffsets[8] = { 0, 4, 8, 12, 16, 20, 24, 28 };

// STATIC: 0x00C1D4B8/0x140A113D0
static const int32_t s_fieldSizes[8] = { 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1D4D8/0x140A113F0
static const int32_t s_fieldTypes[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C1D4F8/0x140A11410
const DBCMeta GlueScreenEmoteRec::s_meta = { "DBFilesClient\\GlueScreenEmote.dbc", 8, 32, GlueScreenEmoteRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void GlueScreenEmoteRec::FixupStrings(void* record, const char* strings) {
}
