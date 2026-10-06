#include "WoW/Source/DB/Rec/GlyphPropertiesRec.h"

// STATIC: 0x00C252B0/0x140A1ADD8
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C252C0/0x140A1ADE8
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C252D0/0x140A1ADF8
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C252E0/0x140A1AE08
const DBCMeta GlyphPropertiesRec::s_meta = { "DBFilesClient\\GlyphProperties.dbc", 4, 16, GlyphPropertiesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: b9193c7d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GlyphPropertiesRec::FixupStrings(void* record, const char* strings) {
}
