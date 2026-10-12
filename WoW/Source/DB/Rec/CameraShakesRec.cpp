#include "WoW/Source/DB/Rec/CameraShakesRec.h"

// STATIC: 0x00C265EC/0x140A1C560
static const int32_t s_fieldOffsets[9] = { 0, 4, 8, 12, 16, 20, 24, 28, 32 };

// STATIC: 0x00C26610/0x140A1C588
static const int32_t s_fieldSizes[9] = { 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C26634/0x140A1C5B0
static const int32_t s_fieldTypes[9] = { 0, 0, 0, 3, 3, 3, 3, 3, 0 };

// STATIC: 0x00C26658/0x140A1C5D8
const DBCMeta CameraShakesRec::s_meta = { "DBFilesClient\\CameraShakes.dbc", 9, 36, CameraShakesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void CameraShakesRec::FixupStrings(void* record, const char* strings) {
}
