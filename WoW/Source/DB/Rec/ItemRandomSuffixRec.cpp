#include "WoW/Source/DB/Rec/ItemRandomSuffixRec.h"

// STATIC: 0x00C23F80/0x140A194A0
const DBCMeta ItemRandomSuffixRec::s_meta = { "DBFilesClient\\ItemRandomSuffix.dbc", 5, 52, ItemRandomSuffixRec::FixupStrings, 0 };

// PROVEN: 1211172c IDENTICAL/NONE
// OFFSET: 0x00A2E9B0/0x1407D8110
void ItemRandomSuffixRec::FixupStrings(void* record, const char* strings) {
    ItemRandomSuffixRec* rec = static_cast<ItemRandomSuffixRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_internalName, strings);
}
