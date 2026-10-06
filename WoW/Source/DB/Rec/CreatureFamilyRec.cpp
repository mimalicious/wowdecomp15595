#include "WoW/Source/DB/Rec/CreatureFamilyRec.h"

// STATIC: 0x00C1D7BC/0x140A11740
const DBCMeta CreatureFamilyRec::s_meta = { "DBFilesClient\\CreatureFamily.dbc", 11, 48, CreatureFamilyRec::FixupStrings, 0 };

// PROVEN: c46a9769 IDENTICAL/NONE
// OFFSET: 0x00A2E820/0x1407D7910
void CreatureFamilyRec::FixupStrings(void* record, const char* strings) {
    CreatureFamilyRec* rec = static_cast<CreatureFamilyRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_iconFile, strings);
}
