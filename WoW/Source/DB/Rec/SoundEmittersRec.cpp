#include "WoW/Source/DB/Rec/SoundEmittersRec.h"

// STATIC: 0x00C27CC8/0x140A1E2F8
const DBCMeta SoundEmittersRec::s_meta = { "DBFilesClient\\SoundEmitters.dbc", 7, 44, SoundEmittersRec::FixupStrings, 0 };

// PROVEN: dba5006f IDENTICAL/NONE
// OFFSET: 0x00A2EB70/0x1407D8310
void SoundEmittersRec::FixupStrings(void* record, const char* strings) {
    SoundEmittersRec* rec = static_cast<SoundEmittersRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
