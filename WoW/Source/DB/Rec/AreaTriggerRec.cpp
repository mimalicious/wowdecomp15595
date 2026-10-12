#include "WoW/Source/DB/Rec/AreaTriggerRec.h"

// STATIC: 0x00C26A58/0x140A1CAB0
static const int32_t s_fieldOffsets[11] = { 0, 4, 8, 20, 24, 28, 32, 36, 40, 44, 48 };

// STATIC: 0x00C26A84/0x140A1CAE0
static const int32_t s_fieldSizes[11] = { 4, 4, 12, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C26AB0/0x140A1CB10
static const int32_t s_fieldTypes[11] = { 0, 0, 3, 0, 0, 0, 3, 3, 3, 3, 3 };

// STATIC: 0x00C26ADC/0x140A1CB40
const DBCMeta AreaTriggerRec::s_meta = { "DBFilesClient\\AreaTrigger.dbc", 11, 52, AreaTriggerRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void AreaTriggerRec::FixupStrings(void* record, const char* strings) {
}
