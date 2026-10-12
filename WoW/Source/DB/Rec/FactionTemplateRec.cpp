#include "WoW/Source/DB/Rec/FactionTemplateRec.h"

// STATIC: 0x00C1E368/0x140A12578
static const int32_t s_fieldOffsets[8] = { 0, 4, 8, 12, 16, 20, 24, 40 };

// STATIC: 0x00C1E388/0x140A12598
static const int32_t s_fieldSizes[8] = { 4, 4, 4, 4, 4, 4, 16, 16 };

// STATIC: 0x00C1E3A8/0x140A125B8
static const int32_t s_fieldTypes[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C1E3C8/0x140A125D8
const DBCMeta FactionTemplateRec::s_meta = { "DBFilesClient\\FactionTemplate.dbc", 8, 56, FactionTemplateRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void FactionTemplateRec::FixupStrings(void* record, const char* strings) {
}
