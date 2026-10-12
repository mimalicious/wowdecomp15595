#include "WoW/Source/DB/Rec/LiquidMaterialRec.h"

// STATIC: 0x00C27F30/0x140A1E5E0
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C27F3C/0x140A1E5F0
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C27F48/0x140A1E600
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C27F54/0x140A1E610
const DBCMeta LiquidMaterialRec::s_meta = { "DBFilesClient\\LiquidMaterial.dbc", 3, 12, LiquidMaterialRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void LiquidMaterialRec::FixupStrings(void* record, const char* strings) {
}
