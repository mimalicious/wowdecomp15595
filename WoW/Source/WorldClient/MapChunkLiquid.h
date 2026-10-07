#ifndef WOW_SOURCE_WORLDCLIENT_MAPCHUNKLIQUID_H
#define WOW_SOURCE_WORLDCLIENT_MAPCHUNKLIQUID_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C2iVector.h"
#include "Engine/Source/Tempest/C3Vector.h"

class C3Ray;
class CMapChunk;
namespace Liquid { class CInstance; }

class CChunkLiquid {
    public:
    static C3Vector s_vertXY[81];
    static int32_t s_vertXYInitialized;

    static void CreateVertXY();

    /* 0000 */ uint32_t m_handle;
    /* 0004 */ uint32_t m_liquidType;
    /* 0008 */ uint32_t m_liquidObject;
    /* 000C */ C3Vector m_position;
    /* 0018 */ uint32_t unk_0018[3];
    /* 0024 */ float unk_0024;
    /* 0028 */ float m_minHeight;
    /* 002C */ float m_maxHeight;
    /* 0030 */ uint32_t m_frameStamp;
    /* 0034 */ int32_t m_xOffset;
    /* 0038 */ int32_t m_yOffset;
    /* 003C */ int32_t m_width;
    /* 0040 */ int32_t m_height;
    /* 0044 */ uint32_t m_vertexCount;
    /* 0048 */ const void* m_vertexData;
    /* 004C */ const void* m_tileData;
    /* 0050 */ const void* m_liquidHeader;
    /* 0054 */ Liquid::CInstance* m_instance;
    /* 0058 */ CMapChunk* m_owner;
    /* 005C */ TSLink<CChunkLiquid> m_chunkLink;
    /* 0064 */ TSLink<CChunkLiquid> m_updateLink;
    /* 006C */ float m_heights[81];

    CChunkLiquid();
    ~CChunkLiquid();
    void Batch();
    void CreateVertZ();
    bool GetHeight(const C2Vector& frac, const C2iVector& cell, float& height) const;
    float GetVertexHeight(uint32_t index) const;
    bool HasData() const;
    bool Intersect(int32_t x, int32_t y, C3Ray& ray, float& t) const;
    void Purge();
    void RenderPrep();
    bool TileExists(uint32_t x, uint32_t y) const;
    void UpdateResourcePurging();
};

#endif
