#include "WoW/Source/DB/Rec/ZoneLightRec.h"

// STATIC: 0x00C27954/0x140A1DEC8
const DBCMeta ZoneLightRec::s_meta = { "DBFilesClient\\ZoneLight.dbc", 4, 16, ZoneLightRec::FixupStrings, 0 };

// PROVEN: 76d9f533 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ZoneLightRec::FixupStrings(void* record, const char* strings) {
    ZoneLightRec* rec = static_cast<ZoneLightRec*>(record);
    DBCFixupString(rec->unk_0004, strings);
}
