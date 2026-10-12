#include "WoW/Source/DB/Rec/CreatureMovementInfoRec.h"

// STATIC: 0x00C2621C/0x140A1C098
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C26224/0x140A1C0A0
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C2622C/0x140A1C0A8
static const int32_t s_fieldTypes[2] = { 0, 3 };

// STATIC: 0x00C26234/0x140A1C0B0
const DBCMeta CreatureMovementInfoRec::s_meta = { "DBFilesClient\\CreatureMovementInfo.dbc", 2, 8, CreatureMovementInfoRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void CreatureMovementInfoRec::FixupStrings(void* record, const char* strings) {
}
