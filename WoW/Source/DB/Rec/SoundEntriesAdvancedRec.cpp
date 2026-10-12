#include "WoW/Source/DB/Rec/SoundEntriesAdvancedRec.h"

// STATIC: 0x00C287A0/0x140A1EFF0
static const int32_t s_fieldOffsets[26] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100 };

// STATIC: 0x00C28808/0x140A1F090
static const int32_t s_fieldSizes[26] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C28870/0x140A1F100
static const int32_t s_fieldTypes[26] = { 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 0, 0, 3 };

// STATIC: 0x00C288D8/0x140A1F058
const DBCMeta SoundEntriesAdvancedRec::s_meta = { "DBFilesClient\\SoundEntriesAdvanced.dbc", 26, 104, SoundEntriesAdvancedRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SoundEntriesAdvancedRec::FixupStrings(void* record, const char* strings) {
}
