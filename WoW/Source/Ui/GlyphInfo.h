#ifndef WOW_SOURCE_UI_GLYPHINFO_H
#define WOW_SOURCE_UI_GLYPHINFO_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

struct GlyphItem {
    /* 0000 */ int32_t m_spellID;
    /* 0004 */ int32_t m_type;
    /* 0008 */ bool m_known;
    /* 0009 */ bool m_hidden;
};

class CGGlyphInfo {
    public:
    static uint32_t s_filter;
    static char* s_nameFilter;
    static TSGrowableArray<GlyphItem*> s_glyphs;
    static int32_t s_numFiltered;
    static int32_t s_clearGlyphSpell;
    static int32_t s_selectedGlyph;

    static void InitializeGame();
    static void ShutdownGame();
    static void ClearGlyphs();
    static void ListGlyphs();
    static void AddKnownGlyph(int32_t spellId);
    static bool IsGlyphKnown(int32_t spellId);
    static void SelectGlyph(int32_t index);
    static void ClearSelectedGlyphBySpell(int32_t spellId);
    static int32_t GetGlyphSpellIDByName(const char* name);
    static void FilterGlyphs();
    static void SetNameFilter(const char* name);
    static void CastGlyph(uint32_t index);
};

void ClearGlyphReagentCallback(int32_t id, const WOWGUID& guid, void* param, bool success);
void GlyphInfoRegisterScriptFunctions();
void GlyphInfoUnregisterScriptFunctions();

#endif
