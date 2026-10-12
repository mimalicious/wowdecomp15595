#include "WoW/Source/DB/Rec/BannedAddOnsRec.h"

// STATIC: 0x00C1EA1C/0x140A12D58
static const int32_t s_fieldOffsets[5] = { 0, 4, 20, 36, 40 };

// STATIC: 0x00C1EA30/0x140A12D70
static const int32_t s_fieldSizes[5] = { 4, 16, 16, 4, 4 };

// STATIC: 0x00C1EA44/0x140A12D88
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 0, 0 };

// STATIC: 0x00C1EA58/0x140A12DA0
const DBCMeta BannedAddOnsRec::s_meta = { "DBFilesClient\\BannedAddOns.dbc", 5, 44, BannedAddOnsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void BannedAddOnsRec::FixupStrings(void* record, const char* strings) {
}
