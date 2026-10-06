#include "WoW/Source/DB/Rec/CharSectionsRec.h"

// STATIC: 0x00C1E7FC/0x140A12AB8
const DBCMeta CharSectionsRec::s_meta = { "DBFilesClient\\CharSections.dbc", 8, 40, CharSectionsRec::FixupStrings, 0 };

// PROVEN: 3e0eb194 IDENTICAL/NONE
// OFFSET: 0x00A2E8E0/0x1407D7BA0
void CharSectionsRec::FixupStrings(void* record, const char* strings) {
    CharSectionsRec* rec = static_cast<CharSectionsRec*>(record);
    DBCFixupString(rec->m_textureName[0], strings);
    DBCFixupString(rec->m_textureName[1], strings);
    DBCFixupString(rec->m_textureName[2], strings);
}
