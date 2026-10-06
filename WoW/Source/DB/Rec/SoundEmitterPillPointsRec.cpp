#include "WoW/Source/DB/Rec/SoundEmitterPillPointsRec.h"

// STATIC: 0x00C27D14/0x140A1E360
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C27D20/0x140A1E370
static const int32_t s_fieldSizes[3] = { 4, 4, 12 };

// STATIC: 0x00C27D2C/0x140A1E380
static const int32_t s_fieldTypes[3] = { 0, 0, 3 };

// STATIC: 0x00C27D38/0x140A1E390
const DBCMeta SoundEmitterPillPointsRec::s_meta = { "DBFilesClient\\SoundEmitterPillPoints.dbc", 3, 20, SoundEmitterPillPointsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 02659281 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SoundEmitterPillPointsRec::FixupStrings(void* record, const char* strings) {
}
