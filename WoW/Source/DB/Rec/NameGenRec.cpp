#include "WoW/Source/DB/Rec/NameGenRec.h"

// STATIC: 0x00C1D098/0x140A10EC0
const DBCMeta NameGenRec::s_meta = { "DBFilesClient\\NameGen.dbc", 4, 16, NameGenRec::FixupStrings, 0 };

// PROVEN: 8eb17c2d IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void NameGenRec::FixupStrings(void* record, const char* strings) {
    NameGenRec* rec = static_cast<NameGenRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
