#include "WoW/Source/DB/Rec/ScalingStatValuesRec.h"

// STATIC: 0x00C228AC/0x140A177F0
const DBCMeta ScalingStatValuesRec::s_meta = { "DBFilesClient\\ScalingStatValues.dbc", 23, 188, ScalingStatValuesRec::FixupStrings, 0 };

// PROVEN: 18d315ec IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ScalingStatValuesRec::FixupStrings(void* record, const char* strings) {
}
