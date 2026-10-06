#include "WoW/Source/DB/Rec/ChrClassesRec.h"

// STATIC: 0x00C1E67C/0x140A128E0
const DBCMeta ChrClassesRec::s_meta = { "DBFilesClient\\ChrClasses.dbc", 14, 56, ChrClassesRec::FixupStrings, 0 };

// PROVEN: 26393df6 IDENTICAL/NONE
// OFFSET: 0x00A2E8C0/0x1407D7B10
void ChrClassesRec::FixupStrings(void* record, const char* strings) {
    ChrClassesRec* rec = static_cast<ChrClassesRec*>(record);
    DBCFixupString(rec->m_petNameToken, strings);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_nameFemale, strings);
    DBCFixupString(rec->m_nameMale, strings);
    DBCFixupString(rec->m_filename, strings);
}
