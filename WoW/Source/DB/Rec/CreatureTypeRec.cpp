#include "WoW/Source/DB/Rec/CreatureTypeRec.h"

// STATIC: 0x00C25F94/0x140A1BD98
const DBCMeta CreatureTypeRec::s_meta = { "DBFilesClient\\CreatureType.dbc", 3, 12, CreatureTypeRec::FixupStrings, 0 };

// PROVEN: 1a525b3e IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void CreatureTypeRec::FixupStrings(void* record, const char* strings) {
    CreatureTypeRec* rec = static_cast<CreatureTypeRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
