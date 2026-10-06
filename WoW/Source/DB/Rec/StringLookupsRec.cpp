#include "WoW/Source/DB/Rec/StringLookupsRec.h"

// STATIC: 0x00C204EC/0x140A14D38
const DBCMeta StringLookupsRec::s_meta = { "DBFilesClient\\StringLookups.dbc", 2, 8, StringLookupsRec::FixupStrings, 0 };

// PROVEN: 1a1ad6c9 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void StringLookupsRec::FixupStrings(void* record, const char* strings) {
    StringLookupsRec* rec = static_cast<StringLookupsRec*>(record);
    DBCFixupString(rec->m_string, strings);
}
