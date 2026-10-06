#include "WoW/Source/DB/Rec/ScreenLocationRec.h"

// STATIC: 0x00C226AC/0x140A175B8
const DBCMeta ScreenLocationRec::s_meta = { "DBFilesClient\\ScreenLocation.dbc", 2, 8, ScreenLocationRec::FixupStrings, 0 };

// PROVEN: 2dbf9922 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ScreenLocationRec::FixupStrings(void* record, const char* strings) {
    ScreenLocationRec* rec = static_cast<ScreenLocationRec*>(record);
    DBCFixupString(rec->unk_0004, strings);
}
