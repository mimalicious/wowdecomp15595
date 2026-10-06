#include "WoW/Source/DB/Rec/HolidayDescriptionsRec.h"

// STATIC: %0x00B46484
const DBCMeta HolidayDescriptionsRec::s_meta = { "DBFilesClient\\HolidayDescriptions.dbc", 2, 8, HolidayDescriptionsRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void HolidayDescriptionsRec::FixupStrings(void* record, const char* strings) {
    HolidayDescriptionsRec* rec = static_cast<HolidayDescriptionsRec*>(record);
    rec->m_description = &strings[reinterpret_cast<uintptr_t>(rec->m_description)];
}

#if defined(WOWDECOMP_ARCH_64)
void HolidayDescriptionsRec::ConvertRow(HolidayDescriptionsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_description = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
