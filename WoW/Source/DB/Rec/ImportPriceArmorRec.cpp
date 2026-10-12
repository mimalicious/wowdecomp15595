#include "WoW/Source/DB/Rec/ImportPriceArmorRec.h"

// STATIC: 0x00C24960/0x140A1A1A0
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C24974/0x140A1A1B8
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C24988/0x140A1A1D0
static const int32_t s_fieldTypes[5] = { 0, 3, 3, 3, 3 };

// STATIC: 0x00C2499C/0x140A1A1E8
const DBCMeta ImportPriceArmorRec::s_meta = { "DBFilesClient\\ImportPriceArmor.dbc", 5, 20, ImportPriceArmorRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void ImportPriceArmorRec::FixupStrings(void* record, const char* strings) {
}
