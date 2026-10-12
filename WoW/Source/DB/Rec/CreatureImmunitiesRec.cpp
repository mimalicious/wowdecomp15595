#include "WoW/Source/DB/Rec/CreatureImmunitiesRec.h"

// STATIC: 0x00C2627C/0x140A1C110
static const int32_t s_fieldOffsets[10] = { 0, 4, 8, 12, 16, 20, 24, 48, 52, 100 };

// STATIC: 0x00C262A4/0x140A1C138
static const int32_t s_fieldSizes[10] = { 4, 4, 4, 4, 4, 4, 24, 4, 48, 4 };

// STATIC: 0x00C262CC/0x140A1C160
static const int32_t s_fieldTypes[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C262F4/0x140A1C188
const DBCMeta CreatureImmunitiesRec::s_meta = { "DBFilesClient\\CreatureImmunities.dbc", 10, 104, CreatureImmunitiesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void CreatureImmunitiesRec::FixupStrings(void* record, const char* strings) {
}
