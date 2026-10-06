#include "WoW/Source/DB/Rec/SpellChainEffectsRec.h"

// STATIC: 0x00C21CD0/0x140A16970
const DBCMeta SpellChainEffectsRec::s_meta = { "DBFilesClient\\SpellChainEffects.dbc", 48, 180, SpellChainEffectsRec::FixupStrings, 0 };

// PROVEN: f00c713f IDENTICAL/NONE
// OFFSET: 0x00A2EA00/0x1407D7FB0
void SpellChainEffectsRec::FixupStrings(void* record, const char* strings) {
    SpellChainEffectsRec* rec = static_cast<SpellChainEffectsRec*>(record);
    DBCFixupString(rec->m_texture, strings);
    DBCFixupString(rec->m_combo, strings);
}
