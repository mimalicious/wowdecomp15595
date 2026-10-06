#include "WoW/Source/DB/Rec/AnimKitSegmentRec.h"

// STATIC: 0x00C1DC20/0x140A11C60
const DBCMeta AnimKitSegmentRec::s_meta = { "DBFilesClient\\AnimKitSegment.dbc", 16, 64, AnimKitSegmentRec::FixupStrings, 0 };

// PROVEN: ee1aba6e IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void AnimKitSegmentRec::FixupStrings(void* record, const char* strings) {
}
