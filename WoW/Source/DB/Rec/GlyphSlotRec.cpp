#include "WoW/Source/DB/Rec/GlyphSlotRec.h"

// STATIC: 0x00C2526C/0x140A1AD78
const DBCMeta GlyphSlotRec::s_meta = { "DBFilesClient\\GlyphSlot.dbc", 3, 12, GlyphSlotRec::FixupStrings, 0 };

// PROVEN: c2aed5e8 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GlyphSlotRec::FixupStrings(void* record, const char* strings) {
}
