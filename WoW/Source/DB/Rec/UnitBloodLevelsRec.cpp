#include "WoW/Source/DB/Rec/UnitBloodLevelsRec.h"

// STATIC: 0x00C1FDE4/0x140A144A0
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C1FDEC/0x140A144A8
static const int32_t s_fieldSizes[2] = { 4, 12 };

// STATIC: 0x00C1FDF4/0x140A144B0
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C1FDFC/0x140A144B8
const DBCMeta UnitBloodLevelsRec::s_meta = { "DBFilesClient\\UnitBloodLevels.dbc", 2, 16, UnitBloodLevelsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void UnitBloodLevelsRec::FixupStrings(void* record, const char* strings) {
}
