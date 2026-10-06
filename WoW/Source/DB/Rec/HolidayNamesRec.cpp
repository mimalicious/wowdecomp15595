#include "WoW/Source/DB/Rec/HolidayNamesRec.h"

// STATIC: %0x00B46448
const DBCMeta HolidayNamesRec::s_meta = { "DBFilesClient\\HolidayNames.dbc", 2, 8, HolidayNamesRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void HolidayNamesRec::FixupStrings(void* record, const char* strings) {
    HolidayNamesRec* rec = static_cast<HolidayNamesRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void HolidayNamesRec::ConvertRow(HolidayNamesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
