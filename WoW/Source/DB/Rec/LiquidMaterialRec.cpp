#include "WoW/Source/DB/Rec/LiquidMaterialRec.h"

// STATIC: 0x00C27F54/0x140A1E610
const DBCMeta LiquidMaterialRec::s_meta = { "DBFilesClient\\LiquidMaterial.dbc", 3, 12, LiquidMaterialRec::FixupStrings, 0 };

// PROVEN: f3915b82 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void LiquidMaterialRec::FixupStrings(void* record, const char* strings) {
}
