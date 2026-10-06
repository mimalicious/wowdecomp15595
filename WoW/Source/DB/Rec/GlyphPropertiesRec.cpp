#include "WoW/Source/DB/Rec/GlyphPropertiesRec.h"

// STATIC: 0x00C252E0/0x140A1AE08
const DBCMeta GlyphPropertiesRec::s_meta = { "DBFilesClient\\GlyphProperties.dbc", 4, 16, GlyphPropertiesRec::FixupStrings, 0 };

// PROVEN: b9193c7d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GlyphPropertiesRec::FixupStrings(void* record, const char* strings) {
}
