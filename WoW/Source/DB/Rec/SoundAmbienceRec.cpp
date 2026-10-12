#include "WoW/Source/DB/Rec/SoundAmbienceRec.h"

// STATIC: 0x00C22278/0x140A17080
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C22280/0x140A17088
static const int32_t s_fieldSizes[2] = { 4, 8 };

// STATIC: 0x00C22288/0x140A17090
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C22290/0x140A17098
const DBCMeta SoundAmbienceRec::s_meta = { "DBFilesClient\\SoundAmbience.dbc", 2, 12, SoundAmbienceRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SoundAmbienceRec::FixupStrings(void* record, const char* strings) {
}
