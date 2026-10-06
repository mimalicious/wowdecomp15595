#include "WoW/Source/DB/Rec/JournalEncounterItemRec.h"

// STATIC: 0x00C23CB0/0x140A19100
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C23CC4/0x140A19118
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C23CD8/0x140A19130
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 0, 0 };

// STATIC: 0x00C23CEC/0x140A19148
const DBCMeta JournalEncounterItemRec::s_meta = { "DBFilesClient\\JournalEncounterItem.dbc", 5, 20, JournalEncounterItemRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: ac5914aa IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void JournalEncounterItemRec::FixupStrings(void* record, const char* strings) {
}
