#include "WoW/Source/DB/Rec/ZoneMusicRec.h"

// STATIC: 0x00C1EF40/0x140A133A0
const DBCMeta ZoneMusicRec::s_meta = { "DBFilesClient\\ZoneMusic.dbc", 5, 32, ZoneMusicRec::FixupStrings, 0 };

// PROVEN: 747ce2dd IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ZoneMusicRec::FixupStrings(void* record, const char* strings) {
    ZoneMusicRec* rec = static_cast<ZoneMusicRec*>(record);
    DBCFixupString(rec->m_setName, strings);
}
