#include "WoW/Source/DB/Rec/HolidayDescriptionsRec.h"

// STATIC: 0x00C24B08/0x140A1A3A0
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C24B10/0x140A1A3A8
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C24B18/0x140A1A3B0
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C24B20/0x140A1A3B8
const DBCMeta HolidayDescriptionsRec::s_meta = { "DBFilesClient\\HolidayDescriptions.dbc", 2, 8, HolidayDescriptionsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 083dc33e IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void HolidayDescriptionsRec::FixupStrings(void* record, const char* strings) {
    HolidayDescriptionsRec* rec = static_cast<HolidayDescriptionsRec*>(record);
    DBCFixupString(rec->m_description, strings);
}
