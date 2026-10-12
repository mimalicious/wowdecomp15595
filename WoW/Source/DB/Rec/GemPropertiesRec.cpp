#include "WoW/Source/DB/Rec/GemPropertiesRec.h"

// STATIC: 0x00C25320/0x140A1AE60
static const int32_t s_fieldOffsets[6] = { 0, 4, 8, 12, 16, 20 };

// STATIC: 0x00C25338/0x140A1AE78
static const int32_t s_fieldSizes[6] = { 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C25350/0x140A1AE90
static const int32_t s_fieldTypes[6] = { 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C25368/0x140A1AEA8
const DBCMeta GemPropertiesRec::s_meta = { "DBFilesClient\\GemProperties.dbc", 6, 24, GemPropertiesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void GemPropertiesRec::FixupStrings(void* record, const char* strings) {
}
