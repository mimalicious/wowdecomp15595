#include "WoW/Source/DB/Rec/LightRec.h"

// STATIC: 0x00C28064/0x140A1E778
static const int32_t s_fieldOffsets[8] = { 0, 4, 8, 12, 16, 20, 24, 28 };

// STATIC: 0x00C28084/0x140A1E798
static const int32_t s_fieldSizes[8] = { 4, 4, 4, 4, 4, 4, 4, 32 };

// STATIC: 0x00C280A4/0x140A1E7B8
static const int32_t s_fieldTypes[8] = { 0, 0, 3, 3, 3, 3, 3, 0 };

// STATIC: 0x00C280C4/0x140A1E7D8
const DBCMeta LightRec::s_meta = { "DBFilesClient\\Light.dbc", 8, 60, LightRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void LightRec::FixupStrings(void* record, const char* strings) {
}
