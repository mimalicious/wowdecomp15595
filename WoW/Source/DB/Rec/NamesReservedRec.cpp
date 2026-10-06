#include "WoW/Source/DB/Rec/NamesReservedRec.h"

// STATIC: 0x00C1DFEC/0x140A120F8
const DBCMeta NamesReservedRec::s_meta = { "DBFilesClient\\NamesReserved.dbc", 3, 12, NamesReservedRec::FixupStrings, 0 };

// PROVEN: 3b64fdf5 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void NamesReservedRec::FixupStrings(void* record, const char* strings) {
    NamesReservedRec* rec = static_cast<NamesReservedRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
