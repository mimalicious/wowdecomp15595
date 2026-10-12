#include "WoW/Source/DB/Rec/LanguagesRec.h"

// STATIC: 0x00C2398C/0x140A18D40
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C23994/0x140A18D48
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C2399C/0x140A18D50
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C239A4/0x140A18D58
const DBCMeta LanguagesRec::s_meta = { "DBFilesClient\\Languages.dbc", 2, 8, LanguagesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 88852aa4 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBA0/0x1407D8330
void LanguagesRec::FixupStrings(void* record, const char* strings) {
    LanguagesRec* rec = static_cast<LanguagesRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
