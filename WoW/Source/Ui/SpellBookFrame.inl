#ifndef WOW_SOURCE_UI_SPELLBOOKFRAME_INL
#define WOW_SOURCE_UI_SPELLBOOKFRAME_INL

#include "WoW/Source/DB/DBClient/DBClient.h"

// INLINED
inline bool HasSpell(uint32_t spellId) {
    return spellId <= static_cast<uint32_t>(g_spellDB.m_maxID) && (s_knownSpellBits.m_bits.m_data[spellId >> 5] & (1 << (spellId & 0x1F)));
}

#endif
