#include "WoW/Source/DB/Rec/BannedAddOnsRec.h"

// STATIC: 0x00C1EA58/0x140A12DA0
const DBCMeta BannedAddOnsRec::s_meta = { "DBFilesClient\\BannedAddOns.dbc", 5, 44, BannedAddOnsRec::FixupStrings, 0 };

// PROVEN: daa07007 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void BannedAddOnsRec::FixupStrings(void* record, const char* strings) {
}
