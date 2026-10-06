#include "WoW/Source/DB/Rec/FileDataRec.h"

// STATIC: 0x00C1E324/0x140A12518
const DBCMeta FileDataRec::s_meta = { "DBFilesClient\\FileData.dbc", 3, 12, FileDataRec::FixupStrings, 0 };

// PROVEN: 2850267a IDENTICAL/NONE
// OFFSET: 0x00A2E9B0/0x1407D8110
void FileDataRec::FixupStrings(void* record, const char* strings) {
    FileDataRec* rec = static_cast<FileDataRec*>(record);
    DBCFixupString(rec->m_filename, strings);
    DBCFixupString(rec->m_filepath, strings);
}
