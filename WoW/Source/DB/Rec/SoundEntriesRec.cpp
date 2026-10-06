#include "WoW/Source/DB/Rec/SoundEntriesRec.h"

// STATIC: 0x00C28680/0x140A1EEA0
static const int32_t s_fieldOffsets[17] = { 0, 4, 8, 12, 52, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136 };

// STATIC: 0x00C286C8/0x140A1EEF0
static const int32_t s_fieldSizes[17] = { 4, 4, 4, 40, 40, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C28710/0x140A1EF40
static const int32_t s_fieldTypes[17] = { 0, 0, 2, 2, 0, 2, 3, 0, 3, 3, 0, 0, 3, 3, 3, 3, 0 };

// STATIC: 0x00C28754/0x140A1EF88
const DBCMeta SoundEntriesRec::s_meta = { "DBFilesClient\\SoundEntries.dbc", 17, 140, SoundEntriesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 0d956bfa IDENTICAL/NONE
// OFFSET: 0x00A3B770/0x1407E86B0
void SoundEntriesRec::FixupStrings(void* record, const char* strings) {
    SoundEntriesRec* rec = static_cast<SoundEntriesRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_file[0], strings);
    DBCFixupString(rec->m_file[1], strings);
    DBCFixupString(rec->m_file[2], strings);
    DBCFixupString(rec->m_file[3], strings);
    DBCFixupString(rec->m_file[4], strings);
    DBCFixupString(rec->m_file[5], strings);
    DBCFixupString(rec->m_file[6], strings);
    DBCFixupString(rec->m_file[7], strings);
    DBCFixupString(rec->m_file[8], strings);
    DBCFixupString(rec->m_file[9], strings);
    DBCFixupString(rec->m_directoryBase, strings);
}
