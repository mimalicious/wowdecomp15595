#include "WoW/Source/DB/Rec/Cfg_CategoriesRec.h"

// STATIC: 0x00C1E9DC/0x140A12D00
const DBCMeta Cfg_CategoriesRec::s_meta = { "DBFilesClient\\Cfg_Categories.dbc", 6, 24, Cfg_CategoriesRec::FixupStrings, 0 };

// PROVEN: 486afd9d IDENTICAL/NONE
// OFFSET: 0x00A2EBF0/0x1407D84B0
void Cfg_CategoriesRec::FixupStrings(void* record, const char* strings) {
    Cfg_CategoriesRec* rec = static_cast<Cfg_CategoriesRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
