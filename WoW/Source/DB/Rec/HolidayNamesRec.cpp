#include "WoW/Source/DB/Rec/HolidayNamesRec.h"

// STATIC: 0x00C24AC0/0x140A1A340
const DBCMeta HolidayNamesRec::s_meta = { "DBFilesClient\\HolidayNames.dbc", 2, 8, HolidayNamesRec::FixupStrings, 0 };

// PROVEN: ff1137b0 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void HolidayNamesRec::FixupStrings(void* record, const char* strings) {
    HolidayNamesRec* rec = static_cast<HolidayNamesRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
