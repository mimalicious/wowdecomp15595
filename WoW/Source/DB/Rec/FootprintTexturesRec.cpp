#include "WoW/Source/DB/Rec/FootprintTexturesRec.h"

// STATIC: 0x00C28210/0x140A1E988
const DBCMeta FootprintTexturesRec::s_meta = { "DBFilesClient\\FootprintTextures.dbc", 2, 8, FootprintTexturesRec::FixupStrings, 0 };

// PROVEN: 721f256f IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void FootprintTexturesRec::FixupStrings(void* record, const char* strings) {
    FootprintTexturesRec* rec = static_cast<FootprintTexturesRec*>(record);
    DBCFixupString(rec->m_footstepFilename, strings);
}
