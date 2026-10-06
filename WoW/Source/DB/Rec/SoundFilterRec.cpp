#include "WoW/Source/DB/Rec/SoundFilterRec.h"

// STATIC: %0x00B43630
const DBCMeta SoundFilterRec::s_meta = { "DBFilesClient\\SoundFilter.dbc", 2, 8, SoundFilterRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void SoundFilterRec::FixupStrings(void* record, const char* strings) {
    SoundFilterRec* rec = static_cast<SoundFilterRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void SoundFilterRec::ConvertRow(SoundFilterRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
