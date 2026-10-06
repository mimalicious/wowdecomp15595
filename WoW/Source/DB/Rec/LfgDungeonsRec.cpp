#include "WoW/Source/DB/Rec/LfgDungeonsRec.h"

// STATIC: 0x00C237AC/0x140A18AE8
const DBCMeta LfgDungeonsRec::s_meta = { "DBFilesClient\\LfgDungeons.dbc", 21, 84, LfgDungeonsRec::FixupStrings, 0 };

// PROVEN: cc2f18a1 IDENTICAL/NONE
// OFFSET: 0x00A2EAD0/0x1407D81D0
void LfgDungeonsRec::FixupStrings(void* record, const char* strings) {
    LfgDungeonsRec* rec = static_cast<LfgDungeonsRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_textureFilename, strings);
    DBCFixupString(rec->m_description, strings);
}
