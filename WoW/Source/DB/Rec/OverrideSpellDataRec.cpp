#include "WoW/Source/DB/Rec/OverrideSpellDataRec.h"

// STATIC: 0x00C231A8/0x140A18368
static const int32_t s_fieldOffsets[4] = { 0, 4, 44, 48 };

// STATIC: 0x00C231B8/0x140A18378
static const int32_t s_fieldSizes[4] = { 4, 40, 4, 4 };

// STATIC: 0x00C231C8/0x140A18388
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 2 };

// STATIC: 0x00C231D8/0x140A18398
const DBCMeta OverrideSpellDataRec::s_meta = { "DBFilesClient\\OverrideSpellData.dbc", 4, 52, OverrideSpellDataRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: c922f1fa IDENTICAL/NONE
// OFFSET: 0x00A2EA80/0x1407D80B0
void OverrideSpellDataRec::FixupStrings(void* record, const char* strings) {
    OverrideSpellDataRec* rec = static_cast<OverrideSpellDataRec*>(record);
    DBCFixupString(rec->m_overrideBarType, strings);
}
