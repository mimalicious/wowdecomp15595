#include "WoW/Source/DB/Rec/CurrencyTypesRec.h"

// STATIC: 0x00C25EB8/0x140A1BC98
static const int32_t s_fieldOffsets[10] = { 0, 4, 8, 12, 20, 24, 28, 32, 36, 40 };

// STATIC: 0x00C25EE0/0x140A1BCC0
static const int32_t s_fieldSizes[10] = { 4, 4, 4, 8, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C25F08/0x140A1BCE8
static const int32_t s_fieldTypes[10] = { 0, 0, 2, 2, 0, 0, 0, 0, 0, 2 };

// STATIC: 0x00C25F30/0x140A1BD10
const DBCMeta CurrencyTypesRec::s_meta = { "DBFilesClient\\CurrencyTypes.dbc", 10, 44, CurrencyTypesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: d4e9e096 IDENTICAL/NONE
// OFFSET: 0x00A2EC00/0x1407D84D0
void CurrencyTypesRec::FixupStrings(void* record, const char* strings) {
    CurrencyTypesRec* rec = static_cast<CurrencyTypesRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_inventoryIcon, strings);
    DBCFixupString(rec->m_inventoryIcon2, strings);
    DBCFixupString(rec->m_description, strings);
}
