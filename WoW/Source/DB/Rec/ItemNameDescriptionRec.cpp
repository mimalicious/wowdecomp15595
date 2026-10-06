#include "WoW/Source/DB/Rec/ItemNameDescriptionRec.h"

// STATIC: 0x00C24194/0x140A19748
const DBCMeta ItemNameDescriptionRec::s_meta = { "DBFilesClient\\ItemNameDescription.dbc", 3, 12, ItemNameDescriptionRec::FixupStrings, 0 };

// PROVEN: 350e8ea9 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ItemNameDescriptionRec::FixupStrings(void* record, const char* strings) {
    ItemNameDescriptionRec* rec = static_cast<ItemNameDescriptionRec*>(record);
    DBCFixupString(rec->unk_0004, strings);
}
