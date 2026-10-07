#ifndef WOW_SOURCE_WORLDCLIENT_BARRIER_H
#define WOW_SOURCE_WORLDCLIENT_BARRIER_H

#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"

class C2Vector;
class C44Matrix;
class CAaBox;
class CMapChunk;
class CMapObjDefGroup;

class CGxVertexPCT0T1 {
    public:
    /* 0000 */ C3Vector m_pos;
    /* 000C */ CImVector m_color;
    /* 0010 */ float m_t0x;
    /* 0014 */ float m_t0y;
    /* 0018 */ float m_t1x;
    /* 001C */ float m_t1y;
};

class CBarrier {
    public:
    struct SContact {
        /* 0000 */ C3Vector m_pos;
        /* 000C */ float m_radius;
    };

    /* 0000 */ int32_t m_enabled;
    /* 0004 */ TSGrowableArray<CGxVertexPCT0T1> m_vertices;
    /* 0014 */ TSGrowableArray<SContact> m_contacts;
    /* 0024 */ TSGrowableArray<CMapChunk*> m_chunks;
    /* 0034 */ TSGrowableArray<CMapObjDefGroup*> m_mapObjDefGroups;
    /* 0044 */ C3Vector m_playerPos;

    static void Destroy();
    static void Initialize();

    CBarrier();
    void AddAxisAlignedWall(const C2Vector& a1, const C2Vector& a2, float a3);
    void AddBarrier(const CAaBox& box, float a2);
    void AddBarrierConvexPoly(C3Vector* verts, uint32_t count, float a3, C44Matrix* matrix, int32_t a5, int32_t a6);
    void Clear();
    void Render(const C3Vector& pos, float a2);
};

#endif
