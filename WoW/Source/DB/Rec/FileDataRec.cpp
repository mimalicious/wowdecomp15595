#include "WoW/Source/DB/Rec/FileDataRec.h"

// STATIC: %0x00B43CAC
const DBCMeta FileDataRec::s_meta = { "DBFilesClient\\FileData.dbc", 3, 12, FileDataRec::FixupStrings, 0 };

// OFFSET: %0x0097C1F0
void FileDataRec::FixupStrings(void* record, const char* strings) {
    FileDataRec* rec = static_cast<FileDataRec*>(record);
    rec->m_filename = &strings[reinterpret_cast<uintptr_t>(rec->m_filename)];
    rec->m_filepath = &strings[reinterpret_cast<uintptr_t>(rec->m_filepath)];
}

#if defined(WOWDECOMP_ARCH_64)
void FileDataRec::ConvertRow(FileDataRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_filename = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    rec->m_filepath = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
}
#endif
