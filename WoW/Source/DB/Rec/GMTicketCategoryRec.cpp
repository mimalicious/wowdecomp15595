#include "WoW/Source/DB/Rec/GMTicketCategoryRec.h"

// STATIC: 0x00C25068/0x140A1AAE0
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C25070/0x140A1AAE8
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C25078/0x140A1AAF0
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C25080/0x140A1AAF8
const DBCMeta GMTicketCategoryRec::s_meta = { "DBFilesClient\\GMTicketCategory.dbc", 2, 8, GMTicketCategoryRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 5d85c455 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void GMTicketCategoryRec::FixupStrings(void* record, const char* strings) {
    GMTicketCategoryRec* rec = static_cast<GMTicketCategoryRec*>(record);
    DBCFixupString(rec->m_category, strings);
}
