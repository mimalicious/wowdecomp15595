#ifndef WOW_SOURCE_WORLDCLIENT_SHADOWQUERY_H
#define WOW_SOURCE_WORLDCLIENT_SHADOWQUERY_H

#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/WorldClient/WorldQuery.h"

class C44Matrix;
class CFrustum;
class CGxFrameBuffer;
class CVar;
class CWorldOcclusion;
class CWorldView;
struct CShadowCacheQueryData;

class CShadowQuery {
    public:
    static CShadowQuery* s_activeShadowQuery;
    static CVar* s_shadowCull;
    static CVar* s_shadowScissor;
    static CVar* s_shadowInstancing;

    static void Clear(CGxFrameBuffer* frameBuffer, const float* rect);
    static void Destroy();
    static void FrustumCallback(CFrustum& frustum, C44Matrix& matrix, const C3Vector& direction, CShadowCacheQueryData& data, int32_t band);
    static void GetSize(CShadowCacheQueryData& data, int32_t band, float& minRadius, float& maxRadius);
    static int32_t Initialize();
    static void MatrixCallback(C3Vector& center, float radius, C44Matrix& matrix, C3Vector& direction, int32_t band);
    static int32_t QueryCallback(CShadowCacheQueryData& data, uint32_t frame);
    static int32_t RenderCallback(CShadowCacheQueryData& data, int32_t band, CGxFrameBuffer* frameBuffer, C3Vector& direction);

    /* 0000 */ CWorldQuery m_queries[3];
    /* 00A8 */ CWorldOcclusion* m_occlusion;
    /* 00AC */ CWorldView* m_worldView;
    /* 00B0 */ float m_entityCutoff;
    /* 00B4 */ uint8_t m_frame;

    CShadowQuery();
    ~CShadowQuery();
    void AllocBuffers();
    void GetFrustum(CFrustum& frustum, C44Matrix& matrix, const C3Vector& direction, CShadowCacheQueryData& data, int32_t band);
    void GetMatrix(C3Vector& center, float radius, C44Matrix& matrix, C3Vector& direction, int32_t band);
    int32_t Query(CShadowCacheQueryData& data, uint32_t frame);
    int32_t Render(CShadowCacheQueryData& data, int32_t band, CGxFrameBuffer* frameBuffer, C3Vector& direction);
    void SetEntityCutoff(const C3Vector& pos, const C3Vector& camPos);
    void Update();
};

#endif
