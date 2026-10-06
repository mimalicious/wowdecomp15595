#include "WoW/Source/DB/Rec/ZoneIntroMusicTableRec.h"

// STATIC: 0x00C1EFC4/0x140A13448
const DBCMeta ZoneIntroMusicTableRec::s_meta = { "DBFilesClient\\ZoneIntroMusicTable.dbc", 5, 20, ZoneIntroMusicTableRec::FixupStrings, 0 };

// PROVEN: 46979e0c IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ZoneIntroMusicTableRec::FixupStrings(void* record, const char* strings) {
    ZoneIntroMusicTableRec* rec = static_cast<ZoneIntroMusicTableRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
