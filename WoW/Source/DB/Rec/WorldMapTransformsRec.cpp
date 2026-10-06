#include "WoW/Source/DB/Rec/WorldMapTransformsRec.h"

// STATIC: 0x00C1F1DC/0x140A136C0
const DBCMeta WorldMapTransformsRec::s_meta = { "DBFilesClient\\WorldMapTransforms.dbc", 9, 48, WorldMapTransformsRec::FixupStrings, 0 };

// PROVEN: c3931a06 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void WorldMapTransformsRec::FixupStrings(void* record, const char* strings) {
}
