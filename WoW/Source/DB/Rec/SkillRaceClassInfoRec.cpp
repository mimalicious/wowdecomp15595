#include "WoW/Source/DB/Rec/SkillRaceClassInfoRec.h"

// STATIC: 0x00C2233C/0x140A17180
static const int32_t s_fieldOffsets[9] = { 0, 4, 8, 12, 16, 20, 24, 28, 32 };

// STATIC: 0x00C22360/0x140A171A8
static const int32_t s_fieldSizes[9] = { 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C22384/0x140A171D0
static const int32_t s_fieldTypes[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C223A8/0x140A171F8
const DBCMeta SkillRaceClassInfoRec::s_meta = { "DBFilesClient\\SkillRaceClassInfo.dbc", 9, 36, SkillRaceClassInfoRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 9013b2aa IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SkillRaceClassInfoRec::FixupStrings(void* record, const char* strings) {
}
