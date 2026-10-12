#include "SmartScreenRect.h"
#include <new>
#include <typeinfo>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Frame/CLayoutFrame.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CRect.h"
#include <cmath>

struct BFSNODE : public TSLinkedNode<BFSNODE> {
    /* 0008 */ CRect m_rect;
    /* 0018 */ int32_t m_direction;

    BFSNODE();
};

static CRect SmartScreenRectMoveUp(uint32_t grid, CRect rect);
static CRect SmartScreenRectMoveLeft(uint32_t grid, CRect rect);
static CRect SmartScreenRectMoveRight(uint32_t grid, CRect rect);
static CRect SmartScreenRectMoveDown(uint32_t grid, CRect rect);

static int32_t s_searchDirections[9][4] = {
    { 0, 2, 3, 1 },
    { 1, 0, 2, 0 },
    { 1, 3, 2, 3 },
    { 0, 1, 3, 1 },
    { 0, 2, 3, 2 },
    { 0, 1, 0, 1 },
    { 0, 2, 0, 2 },
    { 3, 1, 3, 1 },
    { 3, 2, 3, 2 },
};

static CRect (*s_moveFunctions[4])(uint32_t grid, CRect rect) = {
    SmartScreenRectMoveUp,
    SmartScreenRectMoveLeft,
    SmartScreenRectMoveRight,
    SmartScreenRectMoveDown,
};

// STATIC: 0x00C99DF4
static TSList<BFSNODE> s_searchQueue;

// STATIC: 0x00C99E00
static TSList<BFSNODE> s_freeNodes;

// STATIC: 0x00D3A774
static TSGrowableArray<CRect> s_grids[3];

// INLINED
inline BFSNODE::BFSNODE() {
    this->m_rect.minY = 0.0f;
    this->m_rect.minX = 0.0f;
    this->m_rect.maxY = 0.0f;
    this->m_rect.maxX = 0.0f;
}

// OFFSET: 0x00460650/0x14006BEA0
static uint32_t SmartScreenRectClampToScreen(CRect* out, CRect rect, int32_t testOnly) {
    float screenWidth = NDCToDDCWidth(1.0f);
    float top = NDCToDDCHeight(1.0f) - 0.0375f;

    static float s_screenWidth = screenWidth;
    static float s_bottom = top - 0.01875f;

    uint32_t flags = 0;
    float width = rect.maxX - rect.minX;
    float height = rect.minY - rect.maxY;

    if (rect.minX < 0.0f) {
        flags = 0x1;
    }

    if (screenWidth < rect.maxX) {
        flags |= 0x2;
    }

    if (rect.maxY < 0.01875f) {
        flags |= 0x8;
    }

    if (rect.minY > top) {
        flags |= 0x4;
    }

    if (testOnly) {
        return flags;
    }

    if (!flags) {
        *out = rect;
        return 0;
    }

    if (flags & 0xC) {
        if (flags & 0x4) {
            rect.minY = top;
            rect.maxY = top - height;
        } else if (flags & 0x8) {
            rect.maxY = 0.01875f;
            rect.minY = height + 0.01875f;
        }
    }

    if (flags & 0x3) {
        if (flags & 0x1) {
            rect.minX = 0.0f;
            rect.maxX = width;
        } else if (flags & 0x2) {
            rect.maxX = screenWidth;
            rect.minX = screenWidth - width;
        }
    }

    *out = rect;

    return flags;
}

// PROVEN: 5a066354 IDENTICAL/NONE
// OFFSET: 0x00460810
static uint32_t SmartScreenRectGridPos(const CRect& rect) {
    float screenWidth = NDCToDDCWidth(1.0f);
    float screenHeight = NDCToDDCHeight(1.0f);

    uint32_t columns = static_cast<uint32_t>(screenWidth / (rect.maxX - rect.minX)) + 1;
    int32_t rows = static_cast<int32_t>(screenHeight / (rect.minY - rect.maxY)) + 1;

    return columns * rows;
}

// INLINED
static void SmartScreenRectClipRect(CRect& rect) {
    float height = rect.minY - rect.maxY;
    float width = rect.maxX - rect.minX;
    float screenWidth = NDCToDDCWidth(1.0f);
    float screenHeight = NDCToDDCHeight(1.0f);

    if (screenHeight <= rect.minY) {
        rect.minY = screenHeight;
        rect.maxY = screenHeight - height;
    }

    if (rect.maxY < 0.0f) {
        rect.maxY = 0.0f;
        rect.minY = height;
    }

    if (rect.minX < 0.0f) {
        rect.minX = 0.0f;
        rect.maxX = width;
    }

    if (screenWidth < rect.maxX) {
        rect.maxX = screenWidth;
        rect.minX = screenWidth - width;
    }
}

// INLINED
static int32_t SmartScreenRectFindOverlap(uint32_t grid, const CRect& rect, uint32_t direction, float* distance) {
    TSGrowableArray<CRect>& rects = s_grids[grid];

    for (uint32_t i = 0; i < rects.Count(); i++) {
        const CRect& other = rects.m_data[i];

        if (!(other.minX < rect.maxX && other.maxX > rect.minX && other.maxY < rect.minY && other.minY > rect.maxY)) {
            continue;
        }

        switch (direction) {
            case 0:
                *distance = other.minY - rect.maxY;
                return 1;

            case 1:
                *distance = rect.maxX - other.minX;
                return 1;

            case 2:
                *distance = other.maxX - rect.minX;
                return 1;

            case 3:
                *distance = rect.minY - other.maxY;
                return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00460940/0x14006C1E0
static CRect SmartScreenRectMoveUp(uint32_t grid, CRect rect) {
    float distance;

    if (SmartScreenRectFindOverlap(grid, rect, 0, &distance)) {
        rect.minY = rect.minY + distance;
        rect.maxY = distance + rect.maxY;
    }

    return rect;
}

// OFFSET: 0x004609D0/0x14006C280
static CRect SmartScreenRectMoveLeft(uint32_t grid, CRect rect) {
    float distance;

    if (SmartScreenRectFindOverlap(grid, rect, 1, &distance)) {
        rect.minX = rect.minX - distance;
        rect.maxX = rect.maxX - distance;
    }

    return rect;
}

// OFFSET: 0x00460A60/0x14006C320
static CRect SmartScreenRectMoveRight(uint32_t grid, CRect rect) {
    float distance;

    if (SmartScreenRectFindOverlap(grid, rect, 2, &distance)) {
        rect.minX = rect.minX + distance;
        rect.maxX = rect.maxX + distance;
    }

    return rect;
}

// OFFSET: 0x00460AF0/0x14006C3C0
static CRect SmartScreenRectMoveDown(uint32_t grid, CRect rect) {
    float distance;

    if (SmartScreenRectFindOverlap(grid, rect, 3, &distance)) {
        rect.minY = rect.minY - distance;
        rect.maxY = rect.maxY - distance;
    }

    return rect;
}

// OFFSET: 0x00460D30
static void SmartScreenRectRecycleNodes() {
    s_freeNodes.Combine(&s_searchQueue, 2, NULL);
}

// OFFSET: 0x00460DD0/0x14006C780
void SmartScreenRectClearAllGrids() {
    for (uint32_t i = 0; i < 2; i++) {
        s_grids[i].SetCount(0);
    }
}

// OFFSET: 0x00460E00/0x14006C7C0
void SmartScreenRectClearGrid(uint32_t grid) {
    if (grid <= 2) {
        s_grids[grid].SetCount(0);
    }
}

// OFFSET: 0x00460E30
static BFSNODE* SmartScreenRectAllocNode() {
    BFSNODE* node = s_freeNodes.Head();

    if (node) {
        s_searchQueue.LinkToHead(node);
        node->m_direction = -1;
        return node;
    }

    node = s_searchQueue.NewNode(2, 0, 0);
    node->m_direction = -1;

    return node;
}

// OFFSET: 0x004607B0
int32_t SmartScreenRectStartDirection(uint8_t flags) {
    if (flags & 0x1) {
        return (flags & 0x4) ? 8 : ((flags & 0x8) | 0x10) >> 2;
    }

    if (flags & 0x4) {
        return (flags & 0x2) ? 7 : 2;
    }

    if (flags & 0x2) {
        return (flags & 0x8) ? 5 : 3;
    }

    return (flags >> 3) & 1;
}

// OFFSET: 0x00460F00/0x14006C980
static CRect SmartScreenRectFindFreeRect(uint32_t grid, const CRect& rect) {
    CRect result;
    result.minY = 0.0f;
    result.minX = 0.0f;
    result.maxY = 0.0f;
    result.maxX = 0.0f;

    uint32_t flags = SmartScreenRectClampToScreen(&result, rect, 0);
    int32_t start = SmartScreenRectStartDirection(static_cast<uint8_t>(flags));

    BFSNODE* root = SmartScreenRectAllocNode();
    root->m_rect = result;

    uint32_t maxSteps = SmartScreenRectGridPos(rect);

    for (uint32_t step = 0; step < maxSteps; step++) {
        BFSNODE* node = s_searchQueue.Head();

        if (!node) {
            break;
        }

        for (uint32_t i = 0; i < 4; i++) {
            if (node->m_direction != -1 && node->m_direction == s_searchDirections[start][i]) {
                continue;
            }

            if (SmartScreenRectClampToScreen(NULL, node->m_rect, 1)) {
                continue;
            }

            int32_t direction = s_searchDirections[start][i];

            if (direction == -1) {
                continue;
            }

            CRect moved = s_moveFunctions[direction](grid, node->m_rect);

            if (node->m_rect.minY == moved.minY && node->m_rect.minX == moved.minX && node->m_rect.maxY == moved.maxY && node->m_rect.maxX == moved.maxX) {
                result = moved;
                SmartScreenRectRecycleNodes();
                return result;
            }

            BFSNODE* child = SmartScreenRectAllocNode();
            s_searchQueue.LinkToTail(child);
            child->m_rect = moved;
        }

        s_freeNodes.LinkToTail(node);
    }

    result = rect;
    SmartScreenRectRecycleNodes();

    return result;
}

// INLINED
static C2Vector SmartScreenRectFindPosition(uint32_t grid, const CRect& rect, int32_t* changed) {
    CRect found = SmartScreenRectFindFreeRect(grid, rect);

    *changed = found.minY != rect.minY || found.minX != rect.minX || found.maxY != rect.maxY || found.maxX != rect.maxX;

    C2Vector position;
    position.x = (found.maxX + found.minX) * 0.5f;
    position.y = found.minY;

    return position;
}

// OFFSET: 0x00461220/0x14006CE00
void SmartScreenRectPlace(uint32_t grid, CRect& rect) {
    float height = fabsf(rect.maxY - rect.minY);
    float halfWidth = fabsf(rect.maxX - rect.minX) * 0.5f;
    float halfHeight = height * 0.5f;
    float screenWidth = NDCToDDCWidth(1.0f);
    float screenHeight = NDCToDDCHeight(1.0f);
    SmartScreenRectClipRect(rect);

    int32_t changed;
    C2Vector point = SmartScreenRectFindPosition(grid, rect, &changed);

    float minX = halfWidth > point.x ? halfWidth : point.x;
    point.x = screenWidth - halfWidth <= minX ? screenWidth - halfWidth : minX;

    float minY = halfHeight > point.y ? halfHeight : point.y;
    point.y = screenHeight - halfHeight <= minY ? screenHeight - halfHeight : minY;

    rect.minY = point.y;
    rect.minX = point.x - halfWidth;
    rect.maxY = point.y - height;
    rect.maxX = point.x + halfWidth;
    SmartScreenRectClipRect(rect);

    *s_grids[grid].New() = rect;
}

// OFFSET: 0x00461510
void SmartScreenRectShutdown() {
    s_searchQueue.Clear();
    s_freeNodes.Clear();

    for (uint32_t i = 0; i < 2; i++) {
        s_grids[i].Clear();
    }
}
