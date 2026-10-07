#ifndef WOW_SOURCE_WORLDCLIENT_MAPLIGHT_H
#define WOW_SOURCE_WORLDCLIENT_MAPLIGHT_H

#include <cstdint>
#include "WoW/Source/WorldClient/MapBaseObj.h"

class CMapLight : public CMapBaseObj {
    public:
    /* 0010 */ uint8_t unk_0010[176];
};

#endif
