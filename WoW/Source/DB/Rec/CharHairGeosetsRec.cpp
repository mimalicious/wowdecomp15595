#include "WoW/Source/DB/Rec/CharHairGeosetsRec.h"

// STATIC: 0x00C1E888/0x140A12B60
const DBCMeta CharHairGeosetsRec::s_meta = { "DBFilesClient\\CharHairGeosets.dbc", 6, 24, CharHairGeosetsRec::FixupStrings, 0 };

// PROVEN: a21b8b23 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CharHairGeosetsRec::FixupStrings(void* record, const char* strings) {
}
