#ifndef WOW_SOURCE_WORLDCLIENT_MAPFOOTPRINT_H
#define WOW_SOURCE_WORLDCLIENT_MAPFOOTPRINT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/CGxVertexPT2.h"
#include "Engine/Source/Storm/H/stpl.h"

class CTexture;

class CMapFootprint {
    public:
    /* 0000 */ int16_t m_life;
    /* 0002 */ uint8_t m_alpha;
    /* 0004 */ uint32_t m_textureIndex;
    /* 0008 */ TSGrowableArray<CGxVertexPT2> m_verts;
    /* 0018 */ TSLink<CMapFootprint> m_mapLink;
    /* 0020 */ TSLink<CMapFootprint> m_textureLink;
};

class CMapFootprintTexture {
    public:
    /* 0000 */ CTexture* m_texture;
    /* 0004 */ uint32_t m_vertexCount;
    /* 0008 */ TSExplicitList<CMapFootprint, offsetof(CMapFootprint, m_textureLink)> m_footprints;
};

#endif
