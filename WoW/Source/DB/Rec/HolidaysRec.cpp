#include "WoW/Source/DB/Rec/HolidaysRec.h"

// STATIC: %0x00B46414
const DBCMeta HolidaysRec::s_meta = { "DBFilesClient\\Holidays.dbc", 12, 220, HolidaysRec::FixupStrings, 0 };

// OFFSET: %0x0097C3B0
void HolidaysRec::FixupStrings(void* record, const char* strings) {
    HolidaysRec* rec = static_cast<HolidaysRec*>(record);
    rec->m_textureFileName = &strings[reinterpret_cast<uintptr_t>(rec->m_textureFileName)];
}

#if defined(WOWDECOMP_ARCH_64)
void HolidaysRec::ConvertRow(HolidaysRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_duration, &row[0x0004], sizeof(rec->m_duration));
    memcpy(rec->m_date, &row[0x002C], sizeof(rec->m_date));
    memcpy(&rec->m_region, &row[0x0094], sizeof(rec->m_region));
    memcpy(&rec->m_looping, &row[0x0098], sizeof(rec->m_looping));
    memcpy(rec->m_calendarFlags, &row[0x009C], sizeof(rec->m_calendarFlags));
    memcpy(&rec->m_holidayNameID, &row[0x00C4], sizeof(rec->m_holidayNameID));
    memcpy(&rec->m_holidayDescriptionID, &row[0x00C8], sizeof(rec->m_holidayDescriptionID));
    rec->m_textureFileName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x00CC])];
    memcpy(&rec->m_priority, &row[0x00D0], sizeof(rec->m_priority));
    memcpy(&rec->m_calendarFilterType, &row[0x00D4], sizeof(rec->m_calendarFilterType));
    memcpy(&rec->m_flags, &row[0x00D8], sizeof(rec->m_flags));
}
#endif
