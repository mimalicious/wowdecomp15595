#include "WoW/Source/DB/Rec/EmotesTextDataRec.h"

// STATIC: %0x00B46BD8
const DBCMeta EmotesTextDataRec::s_meta = { "DBFilesClient\\EmotesTextData.dbc", 2, 8, EmotesTextDataRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void EmotesTextDataRec::FixupStrings(void* record, const char* strings) {
    EmotesTextDataRec* rec = static_cast<EmotesTextDataRec*>(record);
    rec->m_text = &strings[reinterpret_cast<uintptr_t>(rec->m_text)];
}

#if defined(WOWDECOMP_ARCH_64)
void EmotesTextDataRec::ConvertRow(EmotesTextDataRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_text = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif
