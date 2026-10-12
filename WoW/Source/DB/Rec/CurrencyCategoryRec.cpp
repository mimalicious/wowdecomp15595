#include "WoW/Source/DB/Rec/CurrencyCategoryRec.h"

// STATIC: 0x00C25E54/0x140A1BC10
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C25E60/0x140A1BC20
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C25E6C/0x140A1BC30
static const int32_t s_fieldTypes[3] = { 0, 0, 2 };

// STATIC: 0x00C25E78/0x140A1BC40
const DBCMeta CurrencyCategoryRec::s_meta = { "DBFilesClient\\CurrencyCategory.dbc", 3, 12, CurrencyCategoryRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2ECA0/0x1407D8230
void CurrencyCategoryRec::FixupStrings(void* record, const char* strings) {
    CurrencyCategoryRec* rec = static_cast<CurrencyCategoryRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
