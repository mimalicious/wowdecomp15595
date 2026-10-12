#include "WoW/Source/DB/Rec/HolidayNamesRec.h"

// STATIC: 0x00C24AA8/0x140A1A328
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C24AB0/0x140A1A330
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C24AB8/0x140A1A338
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C24AC0/0x140A1A340
const DBCMeta HolidayNamesRec::s_meta = { "DBFilesClient\\HolidayNames.dbc", 2, 8, HolidayNamesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void HolidayNamesRec::FixupStrings(void* record, const char* strings) {
    HolidayNamesRec* rec = static_cast<HolidayNamesRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
