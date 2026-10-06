#include "WoW/Source/DB/Rec/ItemPriceBaseRec.h"

// STATIC: 0x00C240A0/0x140A19618
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C240B0/0x140A19628
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C240C0/0x140A19638
static const int32_t s_fieldTypes[4] = { 0, 0, 3, 3 };

// STATIC: 0x00C240D0/0x140A19648
const DBCMeta ItemPriceBaseRec::s_meta = { "DBFilesClient\\ItemPriceBase.dbc", 4, 16, ItemPriceBaseRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 0144331a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemPriceBaseRec::FixupStrings(void* record, const char* strings) {
}
