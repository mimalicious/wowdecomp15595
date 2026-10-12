#include "WoW/Source/DB/Rec/SpamMessagesRec.h"

// STATIC: 0x00C1DE28/0x140A11EB8
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C1DE30/0x140A11EC0
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C1DE38/0x140A11EC8
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C1DE40/0x140A11ED0
const DBCMeta SpamMessagesRec::s_meta = { "DBFilesClient\\SpamMessages.dbc", 2, 8, SpamMessagesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: dba6a13e IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBA0/0x1407D8330
void SpamMessagesRec::FixupStrings(void* record, const char* strings) {
    SpamMessagesRec* rec = static_cast<SpamMessagesRec*>(record);
    DBCFixupString(rec->m_text, strings);
}
