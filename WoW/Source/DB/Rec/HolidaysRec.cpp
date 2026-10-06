#include "WoW/Source/DB/Rec/HolidaysRec.h"

// STATIC: 0x00C24A68/0x140A1A2D0
const DBCMeta HolidaysRec::s_meta = { "DBFilesClient\\Holidays.dbc", 12, 220, HolidaysRec::FixupStrings, 0 };

// PROVEN: 7e16b22d IDENTICAL/NONE
// OFFSET: 0x00A2EB80/0x1407D8350
void HolidaysRec::FixupStrings(void* record, const char* strings) {
    HolidaysRec* rec = static_cast<HolidaysRec*>(record);
    DBCFixupString(rec->m_textureFileName, strings);
}
