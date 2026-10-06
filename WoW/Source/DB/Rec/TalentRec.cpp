#include "WoW/Source/DB/Rec/TalentRec.h"

// STATIC: 0x00C20390/0x140A14BA8
static const int32_t s_fieldOffsets[10] = { 0, 4, 8, 12, 16, 36, 48, 60, 64, 68 };

// STATIC: 0x00C203B8/0x140A14BD0
static const int32_t s_fieldSizes[10] = { 4, 4, 4, 4, 20, 12, 12, 4, 4, 8 };

// STATIC: 0x00C203E0/0x140A14BF8
static const int32_t s_fieldTypes[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C20408/0x140A14C20
const DBCMeta TalentRec::s_meta = { "DBFilesClient\\Talent.dbc", 10, 76, TalentRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: bc046477 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void TalentRec::FixupStrings(void* record, const char* strings) {
}
