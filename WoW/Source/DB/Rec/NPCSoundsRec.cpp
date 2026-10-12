#include "WoW/Source/DB/Rec/NPCSoundsRec.h"

// STATIC: 0x00C23270/0x140A18468
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C23278/0x140A18470
static const int32_t s_fieldSizes[2] = { 4, 16 };

// STATIC: 0x00C23280/0x140A18478
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C23288/0x140A18480
const DBCMeta NPCSoundsRec::s_meta = { "DBFilesClient\\NPCSounds.dbc", 2, 20, NPCSoundsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void NPCSoundsRec::FixupStrings(void* record, const char* strings) {
}
