#ifndef WOW_SOURCE_UI_GLYPHINFO_H
#define WOW_SOURCE_UI_GLYPHINFO_H

#include <cstdint>

struct lua_State;

class CGGlyphInfo {
    public:
    static void InitializeGame();
};

void GlyphInfoRegisterScriptFunctions();
void GlyphInfoUnregisterScriptFunctions();

#endif
