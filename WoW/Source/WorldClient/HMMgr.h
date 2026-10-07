#ifndef WOW_SOURCE_WORLDCLIENT_HMMGR_H
#define WOW_SOURCE_WORLDCLIENT_HMMGR_H

#include <cstdint>
#include "Engine/Source/Tempest/C2iVector.h"
#include "Engine/Source/Tempest/C3Vector.h"

class WeatherGridCell {
    public:
    /* 0000 */ float m_heights[1024];
    /* 1000 */ int32_t m_cellX;
    /* 1004 */ int32_t m_cellY;

    WeatherGridCell();
    float GetHeight(const C3Vector& pos);
};

class CHMMgr {
    public:
    /* 0000 */ int32_t m_originX;
    /* 0004 */ int32_t m_originY;
    /* 0008 */ int32_t m_minCellX;
    /* 000C */ int32_t m_minCellY;
    /* 0010 */ int32_t m_minX;
    /* 0014 */ int32_t m_minY;
    /* 0018 */ WeatherGridCell* m_cells[5][5];

    CHMMgr();
    ~CHMMgr();
    float GetSubCellHeight(const C2iVector& cell, float z);
    void Invalidate();
    float Query(const C3Vector& pos);
    bool Query(const C3Vector& start, const C3Vector& end, C3Vector& hit);
    void Update(const C3Vector& pos);
};

#endif
