#include "WoW/Source/DB/Rec/SpellVisualEffectNameRec.h"

// STATIC: 0x00C1CBFC/0x140A10948
const DBCMeta SpellVisualEffectNameRec::s_meta = { "DBFilesClient\\SpellVisualEffectName.dbc", 9, 36, SpellVisualEffectNameRec::FixupStrings, 0 };

// PROVEN: 422f2cd3 IDENTICAL/NONE
// OFFSET: 0x00A2E9B0/0x1407D8110
void SpellVisualEffectNameRec::FixupStrings(void* record, const char* strings) {
    SpellVisualEffectNameRec* rec = static_cast<SpellVisualEffectNameRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_fileName, strings);
}
