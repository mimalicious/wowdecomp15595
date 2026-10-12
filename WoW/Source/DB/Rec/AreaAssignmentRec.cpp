#include "WoW/Source/DB/Rec/AreaAssignmentRec.h"

// STATIC: 0x00C26B20/0x140A1CBA0
static const int32_t s_fieldOffsets[4] = { 4, 8, 12, 16 };

// STATIC: 0x00C26B30/0x140A1CBB0
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C26B40/0x140A1CBC0
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C26B50/0x140A1CBD0
const DBCMeta AreaAssignmentRec::s_meta = { "DBFilesClient\\AreaAssignment.dbc", 4, 20, AreaAssignmentRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void AreaAssignmentRec::FixupStrings(void* record, const char* strings) {
}
