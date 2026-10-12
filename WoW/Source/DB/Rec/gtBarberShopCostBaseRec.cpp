#include "WoW/Source/DB/Rec/gtBarberShopCostBaseRec.h"

// STATIC: 0x00C25018/0x140A1AA70
static const int32_t s_fieldOffsets[1] = { 4 };

// STATIC: 0x00C2501C/0x140A1AA74
static const int32_t s_fieldSizes[1] = { 4 };

// STATIC: 0x00C25020/0x140A1AA78
static const int32_t s_fieldTypes[1] = { 3 };

// STATIC: 0x00C25024/0x140A1AA80
const DBCMeta gtBarberShopCostBaseRec::s_meta = { "DBFilesClient\\gtBarberShopCostBase.dbc", 1, 8, gtBarberShopCostBaseRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void gtBarberShopCostBaseRec::FixupStrings(void* record, const char* strings) {
}
