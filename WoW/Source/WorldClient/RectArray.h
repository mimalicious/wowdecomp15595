#ifndef WOW_SOURCE_WORLDCLIENT_RECTARRAY_H
#define WOW_SOURCE_WORLDCLIENT_RECTARRAY_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/CiRect.h"

template <class T>
class TSRectArray {
    public:
    /* 0000 */ CiRect m_rect;
    /* 0010 */ int32_t m_width;
    /* 0014 */ int32_t m_height;
    /* 0018 */ TSGrowableArray<T> m_data;

    TSRectArray();
    T* Get(int32_t x, int32_t y);
    void SetRect(const CiRect& rect);
};

#include "WoW/Source/WorldClient/RectArray.inl"

#endif
