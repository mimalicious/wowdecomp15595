#ifndef WOW_SOURCE_GAME_GAMECLIENT_CDRAWBOWSTRING_H
#define WOW_SOURCE_GAME_GAMECLIENT_CDRAWBOWSTRING_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

class CGUnit_C;
class CM2Lighting;
class CM2Model;

class CDrawBowString {
    public:
    static void DrawBowString(CM2Model* model, const CM2Lighting* lighting, CGUnit_C* unit);
    static void GlueDrawBowStringCB(CM2Model* model, const CM2Lighting* lighting, void* arg);
    static void UnitDrawBowStringCB(CM2Model* model, const CM2Lighting* lighting, void* arg);
};


#endif
