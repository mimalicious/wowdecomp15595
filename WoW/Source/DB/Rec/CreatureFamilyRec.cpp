#include "WoW/Source/DB/Rec/CreatureFamilyRec.h"

// STATIC: 0x00C1D738/0x140A116B0
static const int32_t s_fieldOffsets[11] = { 0, 4, 8, 12, 16, 20, 28, 32, 36, 40, 44 };

// STATIC: 0x00C1D764/0x140A116E0
static const int32_t s_fieldSizes[11] = { 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1D790/0x140A11710
static const int32_t s_fieldTypes[11] = { 0, 3, 0, 3, 0, 0, 0, 0, 0, 2, 2 };

// STATIC: 0x00C1D7BC/0x140A11740
const DBCMeta CreatureFamilyRec::s_meta = { "DBFilesClient\\CreatureFamily.dbc", 11, 48, CreatureFamilyRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: c46a9769 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2E820/0x1407D7910
void CreatureFamilyRec::FixupStrings(void* record, const char* strings) {
    CreatureFamilyRec* rec = static_cast<CreatureFamilyRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_iconFile, strings);
}
