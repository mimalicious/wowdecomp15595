#include "WoW/Source/DB/Rec/LoadingScreensRec.h"

// STATIC: %0x00B43954
const DBCMeta LoadingScreensRec::s_meta = { "DBFilesClient\\LoadingScreens.dbc", 4, 16, LoadingScreensRec::FixupStrings, 0 };

// OFFSET: %0x0097C1F0
void LoadingScreensRec::FixupStrings(void* record, const char* strings) {
    LoadingScreensRec* rec = static_cast<LoadingScreensRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_fileName = &strings[reinterpret_cast<uintptr_t>(rec->m_fileName)];
}

#if defined(WOWDECOMP_ARCH_64)
void LoadingScreensRec::ConvertRow(LoadingScreensRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    rec->m_fileName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    memcpy(&rec->m_hasWideScreen, &row[0x000C], sizeof(rec->m_hasWideScreen));
}
#endif
