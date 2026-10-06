#include "WoW/Source/DB/Rec/GroundEffectDoodadRec.h"

// STATIC: 0x00C281B4/0x140A1E910
const DBCMeta GroundEffectDoodadRec::s_meta = { "DBFilesClient\\GroundEffectDoodad.dbc", 3, 12, GroundEffectDoodadRec::FixupStrings, 0 };

// PROVEN: d1bb9e24 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void GroundEffectDoodadRec::FixupStrings(void* record, const char* strings) {
    GroundEffectDoodadRec* rec = static_cast<GroundEffectDoodadRec*>(record);
    DBCFixupString(rec->m_doodadpath, strings);
}
