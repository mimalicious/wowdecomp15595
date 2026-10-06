#include "WoW/Source/DB/Rec/DestructibleModelDataRec.h"

// STATIC: 0x00C25BD0/0x140A1B930
static const int32_t s_fieldOffsets[24] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92 };

// STATIC: 0x00C25C30/0x140A1B990
static const int32_t s_fieldSizes[24] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C25C90/0x140A1B9F0
static const int32_t s_fieldTypes[24] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C25CF0/0x140A1BA50
const DBCMeta DestructibleModelDataRec::s_meta = { "DBFilesClient\\DestructibleModelData.dbc", 24, 96, DestructibleModelDataRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: c6859269 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void DestructibleModelDataRec::FixupStrings(void* record, const char* strings) {
}
