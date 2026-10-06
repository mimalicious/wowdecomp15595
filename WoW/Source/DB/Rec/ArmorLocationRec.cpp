#include "WoW/Source/DB/Rec/ArmorLocationRec.h"

// STATIC: 0x00C26A18/0x140A1CA58
const DBCMeta ArmorLocationRec::s_meta = { "DBFilesClient\\ArmorLocation.dbc", 6, 24, ArmorLocationRec::FixupStrings, 0 };

// PROVEN: 708698ed IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ArmorLocationRec::FixupStrings(void* record, const char* strings) {
}
