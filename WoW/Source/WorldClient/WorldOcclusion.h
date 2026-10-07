#ifndef WOW_SOURCE_WORLDCLIENT_WORLDOCCLUSION_H
#define WOW_SOURCE_WORLDCLIENT_WORLDOCCLUSION_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaSphere.h"

class CWorldOcclusion;

class CWorldOcclusionVolume {
    public:
    /* 0000 */ uint32_t m_firstPlane;
    /* 0004 */ uint32_t m_planeCount;
};

class CWorldAntiOccluder {
    public:
    /* 0000 */ uint32_t unk_0000[4];
    /* 0010 */ TSLink<CWorldAntiOccluder> m_link;
};

class CWorldOccluder {
    public:
    static void Destroy();
    static void Initialize();
    static CWorldOccluder* Alloc();

    /* 0000 */ C3Vector m_start;
    /* 000C */ C3Vector m_end;
    /* 0018 */ TSLink<CWorldOccluder> m_link;

    void ClipBufferUpdate(CWorldOcclusion* occlusion, int32_t exterior);
    void Free();
};

class CWorldOcclusion {
    public:
    /* 0000 */ float m_buffer[2][256];
    /* 0800 */ uint8_t m_bufferFlags[256];
    /* 0900 */ C44Matrix m_viewProj;
    /* 0940 */ C44Matrix m_proj;
    /* 0980 */ C3Vector m_camDir;
    /* 098C */ int32_t m_horizon;
    /* 0990 */ uint32_t unk_0990;
    /* 0994 */ TSGrowableArray<C4Plane> m_planes;
    /* 09A4 */ TSGrowableArray<CWorldOcclusionVolume> m_volumes;

    CWorldOcclusion();
    ~CWorldOcclusion();
    void AddVolume(const C3Vector& pos, const C3Vector* verts, int32_t count, int32_t clip, float extend);
    void Clear();
    void ClearVolumes();
    int32_t QueryBuffer(const CAaBox& box, uint32_t flags);
    int32_t QueryBuffer(const CAaSphere& sphere, uint32_t flags);
    int32_t QueryVolumes(const C3Vector* verts, uint32_t count);
    int32_t QueryVolumes(const CAaBox& box);
    int32_t QueryVolumes(const CAaSphere& sphere);
    void Update(const C44Matrix& viewProj, const C44Matrix& proj, const C3Vector& camDir, int32_t horizon);
    void UpdateBuffer(const C3Vector* segment, uint32_t flags);
};

#endif
