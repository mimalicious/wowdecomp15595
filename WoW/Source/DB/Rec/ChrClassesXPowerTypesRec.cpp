#include "WoW/Source/DB/Rec/ChrClassesXPowerTypesRec.h"

// STATIC: 0x00C26428/0x140A1C320
static const int32_t s_fieldOffsets[2] = { 4, 8 };

// STATIC: 0x00C26430/0x140A1C328
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C26438/0x140A1C330
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C26440/0x140A1C338
const DBCMeta ChrClassesXPowerTypesRec::s_meta = { "DBFilesClient\\ChrClassesXPowerTypes.dbc", 2, 12, ChrClassesXPowerTypesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void ChrClassesXPowerTypesRec::FixupStrings(void* record, const char* strings) {
}
