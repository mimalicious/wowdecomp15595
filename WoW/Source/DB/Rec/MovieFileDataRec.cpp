#include "WoW/Source/DB/Rec/MovieFileDataRec.h"

// STATIC: 0x00C1E0FC/0x140A12270
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C1E104/0x140A12278
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C1E10C/0x140A12280
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C1E114/0x140A12288
const DBCMeta MovieFileDataRec::s_meta = { "DBFilesClient\\MovieFileData.dbc", 2, 8, MovieFileDataRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 9faf1e1c IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void MovieFileDataRec::FixupStrings(void* record, const char* strings) {
}
