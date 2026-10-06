#include "WoW/Source/DB/Rec/SpellShapeshiftFormRec.h"

// STATIC: 0x00C20AA4/0x140A15400
const DBCMeta SpellShapeshiftFormRec::s_meta = { "DBFilesClient\\SpellShapeshiftForm.dbc", 11, 84, SpellShapeshiftFormRec::FixupStrings, 0 };

// PROVEN: 7e1267de IDENTICAL/NONE
// OFFSET: 0x00A2ECA0/0x1407D8230
void SpellShapeshiftFormRec::FixupStrings(void* record, const char* strings) {
    SpellShapeshiftFormRec* rec = static_cast<SpellShapeshiftFormRec*>(record);
    DBCFixupString(rec->m_name, strings);
}
