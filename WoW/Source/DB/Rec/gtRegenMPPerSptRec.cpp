#include "WoW/Source/DB/Rec/gtRegenMPPerSptRec.h"

// STATIC: 0x00C24C1C/0x140A1A518
static const int32_t s_fieldOffsets[1] = { 4 };

// STATIC: 0x00C24C20/0x140A1A51C
static const int32_t s_fieldSizes[1] = { 4 };

// STATIC: 0x00C24C24/0x140A1A520
static const int32_t s_fieldTypes[1] = { 3 };

// STATIC: 0x00C24C28/0x140A1A528
const DBCMeta gtRegenMPPerSptRec::s_meta = { "DBFilesClient\\gtRegenMPPerSpt.dbc", 1, 8, gtRegenMPPerSptRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: c4ebab3d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void gtRegenMPPerSptRec::FixupStrings(void* record, const char* strings) {
}
