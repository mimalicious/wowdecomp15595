#include "WoW/Source/DB/Rec/JournalInstanceRec.h"

// STATIC: 0x00C23A4C/0x140A18E40
static const int32_t s_fieldOffsets[9] = { 0, 4, 8, 12, 16, 20, 24, 28, 32 };

// STATIC: 0x00C23A70/0x140A18E68
static const int32_t s_fieldSizes[9] = { 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C23A94/0x140A18E90
static const int32_t s_fieldTypes[9] = { 0, 0, 0, 0, 0, 0, 0, 2, 2 };

// STATIC: 0x00C23AB8/0x140A18EB8
const DBCMeta JournalInstanceRec::s_meta = { "DBFilesClient\\JournalInstance.dbc", 9, 36, JournalInstanceRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 08a491d4 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EAF0/0x1407D8250
void JournalInstanceRec::FixupStrings(void* record, const char* strings) {
    JournalInstanceRec* rec = static_cast<JournalInstanceRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_description, strings);
}
