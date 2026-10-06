#include "WoW/Source/DB/Rec/gtSpellScalingRec.h"

// STATIC: 0x00C24BCC/0x140A1A4A8
static const int32_t s_fieldOffsets[1] = { 4 };

// STATIC: 0x00C24BD0/0x140A1A4AC
static const int32_t s_fieldSizes[1] = { 4 };

// STATIC: 0x00C24BD4/0x140A1A4B0
static const int32_t s_fieldTypes[1] = { 3 };

// STATIC: 0x00C24BD8/0x140A1A4B8
const DBCMeta gtSpellScalingRec::s_meta = { "DBFilesClient\\gtSpellScaling.dbc", 1, 8, gtSpellScalingRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 286b55bc IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void gtSpellScalingRec::FixupStrings(void* record, const char* strings) {
}
