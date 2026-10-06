#include "WoW/Source/DB/Rec/ExhaustionRec.h"

// STATIC: %0x00B46AF8
const DBCMeta ExhaustionRec::s_meta = { "DBFilesClient\\Exhaustion.dbc", 7, 28, ExhaustionRec::FixupStrings, 0 };

// OFFSET: %0x0097C410
void ExhaustionRec::FixupStrings(void* record, const char* strings) {
    ExhaustionRec* rec = static_cast<ExhaustionRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void ExhaustionRec::ConvertRow(ExhaustionRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_xp, &row[0x0004], sizeof(rec->m_xp));
    memcpy(&rec->m_factor, &row[0x0008], sizeof(rec->m_factor));
    memcpy(&rec->m_outdoorHours, &row[0x000C], sizeof(rec->m_outdoorHours));
    memcpy(&rec->m_innHours, &row[0x0010], sizeof(rec->m_innHours));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0014])];
    memcpy(&rec->m_threshold, &row[0x0018], sizeof(rec->m_threshold));
}
#endif
