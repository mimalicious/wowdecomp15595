#include "WoW/Source/DB/Rec/JournalEncounterSectionRec.h"

// STATIC: 0x00C23B04/0x140A18F20
static const int32_t s_fieldOffsets[14] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52 };

// STATIC: 0x00C23B3C/0x140A18F58
static const int32_t s_fieldSizes[14] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C23B74/0x140A18F90
static const int32_t s_fieldTypes[14] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0 };

// STATIC: 0x00C23BAC/0x140A18FC8
const DBCMeta JournalEncounterSectionRec::s_meta = { "DBFilesClient\\JournalEncounterSection.dbc", 14, 56, JournalEncounterSectionRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: e7bf6c12 IDENTICAL/NONE
// OFFSET: 0x00A2EB10/0x1407D8290
void JournalEncounterSectionRec::FixupStrings(void* record, const char* strings) {
    JournalEncounterSectionRec* rec = static_cast<JournalEncounterSectionRec*>(record);
    DBCFixupString(rec->m_title, strings);
    DBCFixupString(rec->m_bodyText, strings);
}
