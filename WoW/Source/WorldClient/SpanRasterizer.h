#ifndef WOW_SOURCE_WORLDCLIENT_SPANRASTERIZER_H
#define WOW_SOURCE_WORLDCLIENT_SPANRASTERIZER_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2iVector.h"
#include "Engine/Source/Tempest/C3Vector.h"

struct SMOLiquidVert;

namespace World {
    struct LiquidQueryResult;
}

struct SpanRange {
    /* 0000 */ int32_t m_min;
    /* 0004 */ int32_t m_max;

    SpanRange Merge(const SpanRange& range);
};

class SpanRasterizer {
    public:
    /* 0004 */ int32_t m_width;
    /* 0008 */ int32_t m_height;
    /* 000C */ SpanRange* m_ranges;

    virtual void VisitSpan(int32_t x0, int32_t x1, int32_t y) = 0;

    SpanRasterizer(const C2iVector& size, SpanRange* ranges);
    void AddRing(int32_t x, int32_t y, int32_t radius);
    void AddSpan(int32_t x0, int32_t x1, int32_t y);
    void VisitRange(int32_t x0, int32_t x1, int32_t y);
};

class LiquidSoundQuery : public SpanRasterizer {
    public:
    /* 0010 */ C3Vector m_pos;
    /* 001C */ uint32_t m_liquidType;
    /* 0020 */ const uint8_t* m_tileFlags;
    /* 0024 */ const SMOLiquidVert* m_verts;
    /* 0028 */ TSGrowableArray<World::LiquidQueryResult>* m_results;
    /* 002C */ float m_radiusSq;
    /* 0030 */ uint8_t m_found;

    virtual void VisitSpan(int32_t x0, int32_t x1, int32_t y);

    LiquidSoundQuery(const C2iVector& size, SpanRange* ranges, const C3Vector& pos, uint32_t liquidType, const uint8_t* tileFlags, const SMOLiquidVert* verts, TSGrowableArray<World::LiquidQueryResult>* results);
};

#endif
