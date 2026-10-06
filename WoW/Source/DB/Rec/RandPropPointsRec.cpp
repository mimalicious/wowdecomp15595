#include "WoW/Source/DB/Rec/RandPropPointsRec.h"

// STATIC: 0x00C229A8/0x140A17930
const DBCMeta RandPropPointsRec::s_meta = { "DBFilesClient\\RandPropPoints.dbc", 4, 64, RandPropPointsRec::FixupStrings, 0 };

// PROVEN: 847b9dd3 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void RandPropPointsRec::FixupStrings(void* record, const char* strings) {
}
