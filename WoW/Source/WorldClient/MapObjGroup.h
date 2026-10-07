#ifndef WOW_SOURCE_WORLDCLIENT_MAPOBJGROUP_H
#define WOW_SOURCE_WORLDCLIENT_MAPOBJGROUP_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"
#include "WoW/Source/WorldClient/MapObj.h"

class CAsyncObject;

class CMapObjGroup : public CMapBaseObj {
    public:
    static void Initialize();

    /* 0010 */ uint32_t unk_0010[7];
    /* 002C */ uint32_t unk_002C;
    /* 0030 */ uint32_t unk_0030[2];
    /* 0038 */ uint16_t unk_0038;
    /* 003A */ uint16_t unk_003A;
    /* 003C */ uint32_t unk_003C[8];
    /* 005C */ uint32_t m_frameStamp;
    /* 0060 */ uint32_t m_groupFlags;
    /* 0064 */ float m_distSq;
    /* 0068 */ CAsyncObject* m_asyncObject;
    /* 006C */ uint32_t unk_006C[3];
    /* 0078 */ TSExplicitList<CMapObjOccluder, offsetof(CMapObjOccluder, m_link)> m_occluders;
    /* 0084 */ uint32_t unk_0084[2];
    /* 008C */ uint32_t m_portalStart;
    /* 0090 */ uint32_t m_portalCount;
    /* 0094 */ uint32_t unk_0094[3];
    /* 00A0 */ uint8_t unk_00A0[188];
    /* 015C */ uint32_t unk_015C[7];
    /* 0178 */ uint8_t unk_0178[96];

    CMapObjGroup();
    void FreeLiquidVB();
    uint32_t GetLiquidType();
    int32_t QueryLiquid(const C3Vector& pos, int32_t a2, float* liquidType, float* height);
};

#endif
