#include "WoW/Source/DB/Rec/JournalEncounterRec.h"

// STATIC: 0x00C23BF0/0x140A19028
static const int32_t s_fieldOffsets[10] = { 0, 4, 8, 12, 20, 24, 28, 32, 36, 40 };

// STATIC: 0x00C23C18/0x140A19050
static const int32_t s_fieldSizes[10] = { 4, 4, 4, 8, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C23C40/0x140A19078
static const int32_t s_fieldTypes[10] = { 0, 0, 0, 3, 0, 0, 0, 0, 2, 2 };

// STATIC: 0x00C23C68/0x140A190A0
const DBCMeta JournalEncounterRec::s_meta = { "DBFilesClient\\JournalEncounter.dbc", 10, 44, JournalEncounterRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 5643b054 IDENTICAL/NONE
// OFFSET: 0x00A2EB10/0x1407D8290
void JournalEncounterRec::FixupStrings(void* record, const char* strings) {
    JournalEncounterRec* rec = static_cast<JournalEncounterRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_description, strings);
}
