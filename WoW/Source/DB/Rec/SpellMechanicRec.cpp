#include "WoW/Source/DB/Rec/SpellMechanicRec.h"

// STATIC: 0x00C21018/0x140A15A98
const DBCMeta SpellMechanicRec::s_meta = { "DBFilesClient\\SpellMechanic.dbc", 2, 8, SpellMechanicRec::FixupStrings, 0 };

// PROVEN: 1cd50061 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void SpellMechanicRec::FixupStrings(void* record, const char* strings) {
    SpellMechanicRec* rec = static_cast<SpellMechanicRec*>(record);
    DBCFixupString(rec->m_stateName, strings);
}
