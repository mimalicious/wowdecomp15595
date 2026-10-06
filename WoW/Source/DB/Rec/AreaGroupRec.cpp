#include "WoW/Source/DB/Rec/AreaGroupRec.h"

// STATIC: 0x00C26CD4/0x140A1CDC0
static const int32_t s_fieldOffsets[3] = { 0, 4, 28 };

// STATIC: 0x00C26CE0/0x140A1CDD0
static const int32_t s_fieldSizes[3] = { 4, 24, 4 };

// STATIC: 0x00C26CEC/0x140A1CDE0
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C26CF8/0x140A1CDF0
const DBCMeta AreaGroupRec::s_meta = { "DBFilesClient\\AreaGroup.dbc", 3, 32, AreaGroupRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: c030281e IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void AreaGroupRec::FixupStrings(void* record, const char* strings) {
}
