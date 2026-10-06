#include "WoW/Source/DB/Rec/SoundEntriesRec.h"

// STATIC: 0x00C28754/0x140A1EF88
const DBCMeta SoundEntriesRec::s_meta = { "DBFilesClient\\SoundEntries.dbc", 17, 140, SoundEntriesRec::FixupStrings, 0 };

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
