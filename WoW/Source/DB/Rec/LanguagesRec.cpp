#include "WoW/Source/DB/Rec/LanguagesRec.h"

// STATIC: 0x00C239A4/0x140A18D58
const DBCMeta LanguagesRec::s_meta = { "DBFilesClient\\Languages.dbc", 2, 8, LanguagesRec::FixupStrings, 0 };

// PROVEN: 88852aa4 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void LanguagesRec::FixupStrings(void* record, const char* strings) {
    LanguagesRec* rec = static_cast<LanguagesRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
