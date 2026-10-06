#include "WoW/Source/DB/Rec/ImportPriceShieldRec.h"

// STATIC: 0x00C248BC/0x140A1A0C8
const DBCMeta ImportPriceShieldRec::s_meta = { "DBFilesClient\\ImportPriceShield.dbc", 2, 8, ImportPriceShieldRec::FixupStrings, 0 };

// PROVEN: 49f56832 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ImportPriceShieldRec::FixupStrings(void* record, const char* strings) {
}
