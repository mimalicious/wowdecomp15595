#include "WoW/Source/DB/Rec/JournalEncounterCreatureRec.h"

// STATIC: 0x00C23D38/0x140A191B0
static const int32_t s_fieldOffsets[7] = { 0, 4, 8, 12, 16, 20, 24 };

// STATIC: 0x00C23D54/0x140A191D0
static const int32_t s_fieldSizes[7] = { 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C23D70/0x140A191F0
static const int32_t s_fieldTypes[7] = { 0, 0, 0, 0, 0, 2, 2 };

// STATIC: 0x00C23D8C/0x140A19210
const DBCMeta JournalEncounterCreatureRec::s_meta = { "DBFilesClient\\JournalEncounterCreature.dbc", 7, 28, JournalEncounterCreatureRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EB30/0x1407D7E90
void JournalEncounterCreatureRec::FixupStrings(void* record, const char* strings) {
    JournalEncounterCreatureRec* rec = static_cast<JournalEncounterCreatureRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_description, strings);
}
