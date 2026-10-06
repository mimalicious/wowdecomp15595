#include "WoW/Source/DB/Rec/WorldMapContinentRec.h"

// STATIC: 0x00C1F378/0x140A138A0
const DBCMeta WorldMapContinentRec::s_meta = { "DBFilesClient\\WorldMapContinent.dbc", 11, 56, WorldMapContinentRec::FixupStrings, 0 };

// PROVEN: fde269c0 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void WorldMapContinentRec::FixupStrings(void* record, const char* strings) {
}
