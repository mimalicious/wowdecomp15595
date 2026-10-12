#include "WoW/Source/DB/Rec/MovieVariationRec.h"

// STATIC: 0x00C1E098/0x140A121E8
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C1E0A4/0x140A121F8
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C1E0B0/0x140A12208
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C1E0BC/0x140A12218
const DBCMeta MovieVariationRec::s_meta = { "DBFilesClient\\MovieVariation.dbc", 3, 12, MovieVariationRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void MovieVariationRec::FixupStrings(void* record, const char* strings) {
}
