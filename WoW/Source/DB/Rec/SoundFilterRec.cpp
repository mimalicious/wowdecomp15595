#include "WoW/Source/DB/Rec/SoundFilterRec.h"

// STATIC: 0x00C1DF18/0x140A11FE8
const DBCMeta SoundFilterRec::s_meta = { "DBFilesClient\\SoundFilter.dbc", 2, 8, SoundFilterRec::FixupStrings, 0 };

// PROVEN: 7e7ff3ec IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void SoundFilterRec::FixupStrings(void* record, const char* strings) {
    SoundFilterRec* rec = static_cast<SoundFilterRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
