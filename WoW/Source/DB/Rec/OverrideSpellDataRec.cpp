#include "WoW/Source/DB/Rec/OverrideSpellDataRec.h"

// STATIC: 0x00C231D8/0x140A18398
const DBCMeta OverrideSpellDataRec::s_meta = { "DBFilesClient\\OverrideSpellData.dbc", 4, 52, OverrideSpellDataRec::FixupStrings, 0 };

// PROVEN: 565e7c94 IDENTICAL/NONE
// OFFSET: 0x00A2EA80/0x1407D80B0
void OverrideSpellDataRec::FixupStrings(void* record, const char* strings) {
    OverrideSpellDataRec* rec = static_cast<OverrideSpellDataRec*>(record);
    DBCFixupString(rec->unk_0030, strings);
}
