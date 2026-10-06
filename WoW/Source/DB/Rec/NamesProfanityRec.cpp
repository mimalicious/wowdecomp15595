#include "WoW/Source/DB/Rec/NamesProfanityRec.h"

// STATIC: 0x00C1E054/0x140A12188
const DBCMeta NamesProfanityRec::s_meta = { "DBFilesClient\\NamesProfanity.dbc", 3, 12, NamesProfanityRec::FixupStrings, 0 };

// PROVEN: 0350a707 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void NamesProfanityRec::FixupStrings(void* record, const char* strings) {
    NamesProfanityRec* rec = static_cast<NamesProfanityRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
