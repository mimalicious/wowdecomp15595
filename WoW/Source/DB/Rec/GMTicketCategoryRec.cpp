#include "WoW/Source/DB/Rec/GMTicketCategoryRec.h"

// STATIC: 0x00C25080/0x140A1AAF8
const DBCMeta GMTicketCategoryRec::s_meta = { "DBFilesClient\\GMTicketCategory.dbc", 2, 8, GMTicketCategoryRec::FixupStrings, 0 };

// PROVEN: 5d85c455 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void GMTicketCategoryRec::FixupStrings(void* record, const char* strings) {
    GMTicketCategoryRec* rec = static_cast<GMTicketCategoryRec*>(record);
    DBCFixupString(rec->m_category, strings);
}
