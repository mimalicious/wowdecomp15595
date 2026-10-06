#include "WoW/Source/DB/Rec/GlyphSlotRec.h"

// STATIC: 0x00C25248/0x140A1AD48
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C25254/0x140A1AD58
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C25260/0x140A1AD68
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C2526C/0x140A1AD78
const DBCMeta GlyphSlotRec::s_meta = { "DBFilesClient\\GlyphSlot.dbc", 3, 12, GlyphSlotRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: c2aed5e8 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GlyphSlotRec::FixupStrings(void* record, const char* strings) {
}
