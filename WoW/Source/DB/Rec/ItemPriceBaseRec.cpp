#include "WoW/Source/DB/Rec/ItemPriceBaseRec.h"

// STATIC: 0x00C240D0/0x140A19648
const DBCMeta ItemPriceBaseRec::s_meta = { "DBFilesClient\\ItemPriceBase.dbc", 4, 16, ItemPriceBaseRec::FixupStrings, 0 };

// PROVEN: 0144331a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemPriceBaseRec::FixupStrings(void* record, const char* strings) {
}
