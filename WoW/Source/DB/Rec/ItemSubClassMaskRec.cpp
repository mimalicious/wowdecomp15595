#include "WoW/Source/DB/Rec/ItemSubClassMaskRec.h"

// STATIC: 0x00C23DF4/0x140A192A0
const DBCMeta ItemSubClassMaskRec::s_meta = { "DBFilesClient\\ItemSubClassMask.dbc", 3, 16, ItemSubClassMaskRec::FixupStrings, 0 };

// PROVEN: 852bdd1f IDENTICAL/NONE
// OFFSET: 0x00A2ECE0/0x1407D8380
void ItemSubClassMaskRec::FixupStrings(void* record, const char* strings) {
    ItemSubClassMaskRec* rec = static_cast<ItemSubClassMaskRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
