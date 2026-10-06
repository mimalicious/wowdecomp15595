#include "WoW/Source/DB/Rec/FileDataRec.h"

// STATIC: 0x00C1E300/0x140A124E8
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C1E30C/0x140A124F8
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C1E318/0x140A12508
static const int32_t s_fieldTypes[3] = { 0, 2, 2 };

// STATIC: 0x00C1E324/0x140A12518
const DBCMeta FileDataRec::s_meta = { "DBFilesClient\\FileData.dbc", 3, 12, FileDataRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 2850267a IDENTICAL/NONE
// OFFSET: 0x00A2E9B0/0x1407D8110
void FileDataRec::FixupStrings(void* record, const char* strings) {
    FileDataRec* rec = static_cast<FileDataRec*>(record);
    DBCFixupString(rec->m_filename, strings);
    DBCFixupString(rec->m_filepath, strings);
}
