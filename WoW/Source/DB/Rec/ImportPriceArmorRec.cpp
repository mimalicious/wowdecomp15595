#include "WoW/Source/DB/Rec/ImportPriceArmorRec.h"

// STATIC: 0x00C2499C/0x140A1A1E8
const DBCMeta ImportPriceArmorRec::s_meta = { "DBFilesClient\\ImportPriceArmor.dbc", 5, 20, ImportPriceArmorRec::FixupStrings, 0 };

// PROVEN: fdc6507e IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ImportPriceArmorRec::FixupStrings(void* record, const char* strings) {
}
