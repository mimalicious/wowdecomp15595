#include "WoW/Source/DB/Rec/HolidayDescriptionsRec.h"

// STATIC: 0x00C24B20/0x140A1A3B8
const DBCMeta HolidayDescriptionsRec::s_meta = { "DBFilesClient\\HolidayDescriptions.dbc", 2, 8, HolidayDescriptionsRec::FixupStrings, 0 };

// PROVEN: 083dc33e IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void HolidayDescriptionsRec::FixupStrings(void* record, const char* strings) {
    HolidayDescriptionsRec* rec = static_cast<HolidayDescriptionsRec*>(record);
    DBCFixupString(rec->m_description, strings);
}
