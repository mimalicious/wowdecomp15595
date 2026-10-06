#include "WoW/Source/DB/Rec/CurrencyCategoryRec.h"

// STATIC: 0x00C25E78/0x140A1BC40
const DBCMeta CurrencyCategoryRec::s_meta = { "DBFilesClient\\CurrencyCategory.dbc", 3, 12, CurrencyCategoryRec::FixupStrings, 0 };

// PROVEN: 265bd8ae IDENTICAL/NONE
// OFFSET: 0x00A2ECA0/0x1407D8230
void CurrencyCategoryRec::FixupStrings(void* record, const char* strings) {
    CurrencyCategoryRec* rec = static_cast<CurrencyCategoryRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
