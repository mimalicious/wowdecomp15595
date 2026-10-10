#ifndef WOW_SOURCE_UI_GLYPHINFO_H
#define WOW_SOURCE_UI_GLYPHINFO_H

#include <cstdint>

struct lua_State;

class CGGlyphInfo {
    public:
    static void InitializeGame();
    static void AddKnownGlyph(int32_t spellId);
    static void ClearSelectedGlyphBySpell(int32_t spellId);
};

void GlyphInfoRegisterScriptFunctions();
void GlyphInfoUnregisterScriptFunctions();

#endif
