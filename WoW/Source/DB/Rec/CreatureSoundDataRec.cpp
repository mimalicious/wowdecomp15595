#include "WoW/Source/DB/Rec/CreatureSoundDataRec.h"

// STATIC: 0x00C26040/0x140A1BE90
static const int32_t s_fieldOffsets[33] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 76, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144, 148, 152, 156 };

// STATIC: 0x00C260C8/0x140A1BF20
static const int32_t s_fieldSizes[33] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 20, 16, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C26150/0x140A1BFB0
static const int32_t s_fieldTypes[33] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C261D4/0x140A1C038
const DBCMeta CreatureSoundDataRec::s_meta = { "DBFilesClient\\CreatureSoundData.dbc", 33, 160, CreatureSoundDataRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void CreatureSoundDataRec::FixupStrings(void* record, const char* strings) {
}
