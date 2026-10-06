#include "WoW/Source/DB/Rec/SpellChainEffectsRec.h"

// STATIC: 0x00C21A90/0x140A16730
static const int32_t s_fieldOffsets[48] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144, 148, 152, 156, 157, 158, 159, 160, 164, 168, 172, 176 };

// STATIC: 0x00C21B50/0x140A167F0
static const int32_t s_fieldSizes[48] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 1, 4, 4, 4, 4 };

// STATIC: 0x00C21C10/0x140A168B0
static const int32_t s_fieldTypes[48] = { 0, 3, 3, 3, 3, 0, 0, 2, 0, 0, 3, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 2, 0, 3, 3 };

// STATIC: 0x00C21CD0/0x140A16970
const DBCMeta SpellChainEffectsRec::s_meta = { "DBFilesClient\\SpellChainEffects.dbc", 48, 180, SpellChainEffectsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: f00c713f IDENTICAL/NONE
// OFFSET: 0x00A2EA00/0x1407D7FB0
void SpellChainEffectsRec::FixupStrings(void* record, const char* strings) {
    SpellChainEffectsRec* rec = static_cast<SpellChainEffectsRec*>(record);
    DBCFixupString(rec->m_texture, strings);
    DBCFixupString(rec->m_combo, strings);
}
