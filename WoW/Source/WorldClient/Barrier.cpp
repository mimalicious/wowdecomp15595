#include "WoW/Source/WorldClient/Barrier.h"
#include <cfloat>
#include <cmath>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Services/ParticleSystem2.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Ray.h"
#include "Engine/Source/Tempest/C3Segment.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/NTempest.h"

// STATIC: 0x00E89420
static CGxShader* s_pixelShader;

// STATIC: 0x00E89424
static CGxShader* s_vertexShader;

// STATIC: 0x00E89428
static CM2Model* s_contactModels[4];

// STATIC: 0x00E89438
static CTexture* s_texture;

// STATIC: 0x00E8943C
static CTexture* s_maskTexture;

// STATIC: 0x00E89654
static float s_scrollPhase;

// OFFSET: 0x00777190
void CBarrier::Destroy() {
    for (CM2Model** model = s_contactModels; model < s_contactModels + 4; model++) {
        if (*model) {
            (*model)->Release();
            *model = NULL;
        }
    }

    if (s_maskTexture) {
        TextureRelease(s_maskTexture);
        s_maskTexture = NULL;
    }

    if (s_texture) {
        TextureRelease(s_texture);
        s_texture = NULL;
    }

    if (s_vertexShader) {
        g_theGxDevicePtr->ShaderRelease(s_vertexShader);
        s_vertexShader = NULL;
    }

    if (s_pixelShader) {
        g_theGxDevicePtr->ShaderRelease(s_pixelShader);
        s_pixelShader = NULL;
    }
}

// OFFSET: 0x00777230
CGxVertexPCT0T1::CGxVertexPCT0T1() {
    this->m_color = CImVector(0);
    this->m_t0x = 0.0f;
    this->m_t0y = 0.0f;
    this->m_t1x = 0.0f;
    this->m_t1y = 0.0f;
}

// OFFSET: 0x00777610/0x1404624B0
CBarrier::CBarrier() {
    this->m_enabled = 1;
}

// OFFSET: 0x00777660
void CBarrier::AddBarrierConvexPoly(C3Vector* verts, uint32_t count, float radius, C44Matrix* matrix, int32_t flip, int32_t oneSided) {
    if (!this->m_enabled || this->m_vertices.Count() > 1000) {
        return;
    }

    static CGxVertexPCT0T1 s_polyVertices[12];
    static C3Vector s_transformed[12];

    C3Vector* points = verts;

    if (matrix) {
        s_transformed[0] = verts[0] * *matrix;
        s_transformed[1] = verts[1] * *matrix;
        s_transformed[2] = verts[2] * *matrix;
        points = s_transformed;
    }

    float x0 = points[0].x - this->m_playerPos.x;
    float x1 = points[1].x - this->m_playerPos.x;
    float x2 = points[2].x - this->m_playerPos.x;
    float y1 = points[1].y - this->m_playerPos.y;
    float y2 = points[2].y - this->m_playerPos.y;
    float z2 = points[2].z - this->m_playerPos.z;
    float z1 = points[1].z - this->m_playerPos.z;
    SContact contact;
    float y0 = points[0].y - this->m_playerPos.y;
    float ex = x2 - x0;
    float z0 = points[0].z - this->m_playerPos.z;
    float fy = y2 - y0;
    float ez = z2 - z0;
    float ey = y1 - y0;
    float fx = x1 - x0;
    float nz = fy * fx - ey * ex;
    float nx = ey * ez - (z1 - z0) * fy;
    float ny = (z1 - z0) * ex - ez * fx;
    float inv = 1.0f / sqrt(nx * nx + nz * nz + ny * ny);
    contact.m_plane.n.x = nx * inv;
    contact.m_plane.n.y = ny * inv;
    contact.m_plane.n.z = inv * nz;
    contact.m_plane.d = -((nx * inv) * x0 + z0 * (inv * nz) + (ny * inv) * y0);
    C3Vector up(0.0f, 0.0f, 1.0f);

    if (inv * nz > 0.99f || inv * nz < -0.99f) {
        up = C3Vector(0.0f, 1.0f, 0.0f);
    }

    float dist = ((ny * inv) + (nx * inv) + (inv * nz)) * 0.0f - ((nx * inv) * x0 + z0 * (inv * nz) + (ny * inv) * y0);

    if (flip) {
        dist = dist * -1.0f;
    }

    if (!((!oneSided || dist >= 0.0f) && dist <= radius)) {
        return;
    }

    if (matrix) {
        for (uint32_t i = 3; i < count; i++) {
            s_transformed[i] = verts[i] * *matrix;
        }
    }

    for (uint32_t i = 0; i < count; i++) {
        points[i].x = points[i].x - this->m_playerPos.x;
        points[i].y = points[i].y - this->m_playerPos.y;
        points[i].z = points[i].z - this->m_playerPos.z;
    }

    C3Vector origin(0.0f, 0.0f, 0.0f);
    float polyDist = NTempest::DistanceFromPolygon(origin, points, count, contact.m_plane);

    if (polyDist > radius) {
        return;
    }

    float fade = (radius - polyDist) * 2.0f;

    if (fade < 0.5f) {
        return;
    }

    float t = polyDist / radius;
    float clamped = t;

    if (clamped >= 0.0f) {
        if (clamped >= 1.0f) {
            clamped = 1.0f;
        }
    } else {
        clamped = 0.0f;
    }

    contact.m_alpha = 1.0f - clamped;
    C3Ray ray;
    ray.origin = C3Vector(0.0f, 0.0f, 0.0f);
    ray.dir = C3Vector(-contact.m_plane.n.x, -contact.m_plane.n.y, -contact.m_plane.n.z);

    if (!NTempest::Intersect(ray, contact.m_plane, NULL, &contact.m_pos, 0.0099999998f)) {
        return;
    }

    this->m_contacts.Add(1, &contact);
    C44Matrix view;
    C3Vector eye(0.0f, 0.0f, 0.0f);
    GxuXformCreateLookAtSgCompat(eye, contact.m_plane.n, up, view);
    C44Matrix tex = view;

    if (radius < 12.0f) {
        tex.Scale(4.0f);
    }

    if (count) {
        float invFade = 1.0f / fade;
        const C3Vector* point = points;
        CGxVertexPCT0T1* vertex = s_polyVertices;

        for (uint32_t i = 0; i < count; i++, point++, vertex++) {
            float a = t;
            vertex->m_pos = *point;

            if (a >= 0.0f) {
                if (a >= 1.0f) {
                    a = 1.0f;
                }
            } else {
                a = 0.0f;
            }

            vertex->m_color.a = static_cast<uint8_t>(static_cast<int32_t>((1.0f - a) * 255.0f));
            float v = invFade * (point->y * view.b1 + point->x * view.a1 + point->z * view.c1 + view.d1);
            vertex->m_t0x = invFade * (point->y * view.b0 + point->z * view.c0 + point->x * view.a0 + view.d0) + 0.5f;
            vertex->m_t0y = 0.5f - v;
            float wx = point->x + this->m_playerPos.x;
            float wz = this->m_playerPos.z + point->z;
            float wy = point->y + this->m_playerPos.y;
            vertex->m_t1x = tex.a0 * wx + tex.c0 * wz + tex.b0 * wy + tex.d0;
            vertex->m_t1y = tex.c1 * wz + tex.b1 * wy + tex.a1 * wx + tex.d1;
        }
    }

    if (count != 2) {
        for (uint32_t i = 0; i < count - 2; i++) {
            this->m_vertices.Add(1, &s_polyVertices[0]);
            this->m_vertices.Add(2, &s_polyVertices[i + 1]);
        }
    }
}

// OFFSET: 0x007783F0
void CBarrier::AddAxisAlignedWall(const C2Vector& a1, const C2Vector& a2, float a3) {
    if (!this->m_enabled) {
        return;
    }

    float t = FLT_MAX;
    C3Segment seg;
    seg.b = C3Vector(a1.x, a1.y, this->m_playerPos.z);
    seg.t = C3Vector(a2.x, a2.y, this->m_playerPos.z);
    float dist = sqrt(NTempest::DistanceSquared(seg, this->m_playerPos, &t));

    if (dist > 0.99900001f * a3) {
        return;
    }

    float dz = seg.t.z - seg.b.z;
    float extent = a3 - dist;
    float dy = seg.t.y - seg.b.y;
    float dx = seg.t.x - seg.b.x;
    float frac = extent / sqrt(dz * dz + dy * dy + dx * dx);
    float t0 = 1.0f;

    if (t - frac >= 0.0f) {
        if (t - frac < 1.0f) {
            t0 = t - frac;
        }
    } else {
        t0 = 0.0f;
    }

    float sz = dz * t0 + seg.b.z;
    float sy = dy * t0 + seg.b.y;
    float sx = dx * t0 + seg.b.x;
    float t1 = 0.0f;

    if (frac + t >= 0.0f) {
        t1 = 1.0f;

        if (frac + t < 1.0f) {
            t1 = frac + t;
        }
    }

    float ez = dz * t1 + seg.b.z;
    C3Vector quad[4];
    quad[0] = C3Vector(sx, sy, extent + sz);
    quad[1] = C3Vector(sx, sy, sz - extent);
    quad[2] = C3Vector(dx * t1 + seg.b.x, dy * t1 + seg.b.y, ez - extent);
    quad[3] = C3Vector(dx * t1 + seg.b.x, dy * t1 + seg.b.y, extent + ez);
    this->AddBarrierConvexPoly(quad, 4, a3, NULL, 0, 0);
}

// OFFSET: 0x00778650/0x140463E60
void CBarrier::AddBarrier(const CAaBox& box, float a2) {
    if (!this->m_enabled || !(box.t.x > box.b.x && box.t.y > box.b.y && box.t.z > box.b.z)) {
        return;
    }

    CAaBox grown;
    grown.b.x = box.b.x - 0.050000001f;
    grown.b.y = box.b.y - 0.050000001f;
    grown.b.z = box.b.z - 0.050000001f;
    grown.t.x = box.t.x + 0.050000001f;
    grown.t.y = box.t.y + 0.050000001f;
    grown.t.z = box.t.z + 0.050000001f;

    if (NTempest::Distance(grown, this->m_playerPos) > a2) {
        return;
    }

    C3Vector quad[4];
    float bottom = grown.b.z - 1.0f;

    quad[0] = C3Vector(grown.b.x, grown.b.y, bottom);
    quad[1] = C3Vector(grown.b.x, grown.t.y, bottom);
    quad[2] = C3Vector(grown.b.x, grown.t.y, grown.t.z);
    quad[3] = C3Vector(grown.b.x, grown.b.y, grown.t.z);
    this->AddBarrierConvexPoly(quad, 4, a2, NULL, 0, 0);

    quad[0] = C3Vector(grown.b.x, grown.t.y, bottom);
    quad[1] = C3Vector(grown.t.x, grown.t.y, bottom);
    quad[2] = C3Vector(grown.t.x, grown.t.y, grown.t.z);
    quad[3] = C3Vector(grown.b.x, grown.t.y, grown.t.z);
    this->AddBarrierConvexPoly(quad, 4, a2, NULL, 0, 0);

    quad[0] = C3Vector(grown.t.x, grown.t.y, bottom);
    quad[1] = C3Vector(grown.t.x, grown.b.y, bottom);
    quad[2] = C3Vector(grown.t.x, grown.b.y, grown.t.z);
    quad[3] = C3Vector(grown.t.x, grown.t.y, grown.t.z);
    this->AddBarrierConvexPoly(quad, 4, a2, NULL, 0, 0);

    quad[0] = C3Vector(grown.t.x, grown.b.y, bottom);
    quad[1] = C3Vector(grown.b.x, grown.b.y, bottom);
    quad[2] = C3Vector(grown.b.x, grown.b.y, grown.t.z);
    quad[3] = C3Vector(grown.t.x, grown.b.y, grown.t.z);
    this->AddBarrierConvexPoly(quad, 4, a2, NULL, 0, 0);

    quad[0] = C3Vector(grown.b.x, grown.b.y, bottom);
    quad[1] = C3Vector(grown.b.x, grown.t.y, bottom);
    quad[2] = C3Vector(grown.t.x, grown.t.y, bottom);
    quad[3] = C3Vector(grown.t.x, grown.b.y, bottom);
    this->AddBarrierConvexPoly(quad, 4, a2, NULL, 0, 0);

    quad[0] = C3Vector(grown.b.x, grown.b.y, grown.t.z);
    quad[1] = C3Vector(grown.t.x, grown.b.y, grown.t.z);
    quad[2] = C3Vector(grown.t.x, grown.t.y, grown.t.z);
    quad[3] = C3Vector(grown.b.x, grown.t.y, grown.t.z);
    this->AddBarrierConvexPoly(quad, 4, a2, NULL, 0, 0);
}

// OFFSET: 0x00778BA0/0x140464550
void CBarrier::Render(const C3Vector& pos, float a2) {
    if (!this->m_enabled || !this->m_vertices.Count()) {
        return;
    }

    uint32_t contactCount = this->m_contacts.Count();

    if (contactCount > 4) {
        contactCount = 4;
    }

    for (uint32_t i = 0; i < contactCount; i++) {
        CM2Model* model = s_contactModels[i];

        if (!model) {
            continue;
        }

        const SContact& contact = this->m_contacts.m_data[i];
        const C3Vector& n = contact.m_plane.n;
        float zero = n.z * 0.0f;
        C3Vector right(n.y - zero, zero - n.x, n.x * 0.0f - n.y * 0.0f);
        C3Vector up(right.y * n.z - right.z * n.y, right.z * n.x - n.z * right.x, n.y * right.x - right.y * n.x);
        C44Matrix transform(n, right, up, contact.m_pos);
        model->SetAnimating(1);

        if (model->m_attachParent) {
            model->m_fFlags |= 0x40;
        } else {
            model->m_fFlags |= 0x8;
        }

        if (model->m_attachParent) {
            model->m_fFlags |= 0x10000;
        } else {
            model->m_fFlags |= 0x8000;
        }

        model->m_fFlags |= 0x4000;
        model->m_worldTransform = transform;
        model->m_fadeAlpha = contact.m_alpha;

        for (uint32_t j = 0; ; j++) {
            if (!(model->m_fFlags & 0x1)) {
                model->WaitForLoad(NULL);
            }

            if (j >= model->m_shared->m_data->m_particles.m_count) {
                break;
            }

            if (!(model->m_fFlags & 0x1)) {
                model->WaitForLoad(NULL);
            }

            CParticleEmitter2* emitter = model->m_particleEmitters[j];
            emitter->SetWidth(100.0f);
            emitter->SetHeight(100.0f);
        }
    }

    CGxTex* maskTex = TextureGetGxTex(s_maskTexture);

    if (!maskTex) {
        return;
    }

    CGxTex* tex = TextureGetGxTex(s_texture);

    if (!tex) {
        return;
    }

    C44Matrix world;
    world.Translate(C3Vector(this->m_playerPos.x - pos.x, this->m_playerPos.y - pos.y, this->m_playerPos.z - pos.z));
    g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);
    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_DepthTest, 1);
    GxRsSet(GxRs_BlendingMode, GxBlend_Add);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, maskTex);
    GxRsSet(GxRs_TextureCoord0, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture1, tex);
    GxRsSet(GxRs_TextureCoord1, 1);
    C44Matrix identity;
    g_theGxDevicePtr->XformPushAndSet(GxXform_Tex0, identity);
    g_theGxDevicePtr->XformPushAndSet(GxXform_Tex1, identity);
    GxRsSet(GxRs_TextureShader0, 1);
    GxRsSet(GxRs_TextureShader1, 1);
    GxRsSet(GxRs_ColorOp1, 0);
    GxRsSet(GxRs_AlphaOp1, 1);

    uint32_t triCount = this->m_vertices.Count() / 3;
    uint32_t vertexCount = 2 * this->m_vertices.Count();
    uint32_t indexCount = 6 * triCount;
    CGxBuf* vertexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPCT0T1), vertexCount);
    CGxBuf* indexBuf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), indexCount);
    CGxVertexPCT0T1* vertex = reinterpret_cast<CGxVertexPCT0T1*>(g_theGxDevicePtr->BufLock(vertexBuf));
    uint16_t* index = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(indexBuf));
    s_scrollPhase = a2 * 0.1f + s_scrollPhase;
    float phase = s_scrollPhase;

    if (s_scrollPhase > 3.1415927f) {
        phase = s_scrollPhase - 3.1415927f;
        s_scrollPhase = s_scrollPhase - 3.1415927f;
    }

    C2Vector rotation(cos(phase), sin(phase));

    if (vertex && index) {
        CImVector color(0);
        color.b = 0xFF;
        float scroll = static_cast<float>(fmod(static_cast<double>(s_scrollPhase), 3.141592741012573) * 1.5915494);

        for (uint32_t i = 0; static_cast<int32_t>(i) < static_cast<int32_t>(2 * triCount); i++) {
            index[0] = 3 * i;
            index[2] = 3 * i + 2;
            index[1] = 3 * i + 1;
            index += 3;
            const CGxVertexPCT0T1* source = &this->m_vertices.m_data[3 * (i / 2)];
            uint32_t layer = i & 1;

            for (uint32_t j = 0; j < 3; j++, vertex++, source++) {
                vertex->m_pos = source->m_pos;
                vertex->m_t0x = source->m_t0x;
                vertex->m_t0y = source->m_t0y;
                vertex->m_t1x = source->m_t1x;
                vertex->m_t1y = source->m_t1y;
                uint8_t green = layer ? 0xFF : 0;
                color.g = green;
                CImVector out;

                if (g_theGxDevicePtr->m_caps.m_colorFormat == GxCF_rgba) {
                    out.g = green;
                    out.b = 0;
                    out.r = 0xFF;
                    out.a = source->m_color.a;
                } else {
                    color.a = source->m_color.a;
                    out = color;
                }

                vertex->m_color = out;

                if (layer) {
                    vertex->m_t1x = source->m_t1x * 0.050000001f + 0.5f;
                    vertex->m_t1y = source->m_t1y * 0.050000001f - scroll;
                } else {
                    vertex->m_t1x = source->m_t1x * 0.1f;
                    vertex->m_t1y = source->m_t1y * 0.1f - scroll;
                }
            }
        }
    }

    g_theGxDevicePtr->BufUnlock(vertexBuf, 0);
    vertexBuf->unk_001C = 1;
    g_theGxDevicePtr->BufUnlock(indexBuf, 0);
    indexBuf->unk_001C = 1;
    g_theGxDevicePtr->PrimVertexPtr(vertexBuf, GxVBF_PCT2);
    g_theGxDevicePtr->PrimIndexPtr(indexBuf);

    if (CShaderEffect::s_enableShaders) {
        C44Matrix viewProj;
        GxXformViewProjNative(viewProj);
        C44Matrix transform = world * viewProj;
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(&transform), 4);
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 4, reinterpret_cast<const float*>(&identity), 4);
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &s_vertexShader->m_permutes[0]);
        g_theGxDevicePtr->RsSet(GxRs_PixelShader, &s_pixelShader->m_permutes[0]);
    }

    GxRsSet(GxRs_DepthWrite, 1);
    GxRsSet(GxRs_ColorWrite, 0);

    CGxBatch batch;
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    batch.m_count = indexCount;
    batch.m_minIndex = 0;
    batch.m_maxIndex = vertexCount - 1;
    g_theGxDevicePtr->Draw(batch);

    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_ColorWrite, 15);
    batch.m_primType = GxPrim_Triangles;
    batch.m_start = 0;
    batch.m_count = indexCount;
    batch.m_minIndex = 0;
    batch.m_maxIndex = vertexCount - 1;
    g_theGxDevicePtr->Draw(batch);

    GxXformPop(GxXform_Tex0);
    GxXformPop(GxXform_Tex1);
    g_theGxDevicePtr->RsPop();
    GxXformPop(GxXform_World);
    this->m_vertices.SetCount(0);
    this->m_contacts.SetCount(0);
    this->m_chunks.m_count = 0;
    this->m_mapObjDefGroups.m_count = 0;
}

// OFFSET: 0x007796F0/0x1404653F0
void CBarrier::Initialize() {
    CGxTexFlags flags;
    CStatus status;
    flags.m_filter = 1;
    flags.m_wrapU = 0;
    flags.m_wrapV = 0;
    s_maskTexture = TextureCreate("XTextures\\FX\\LBTBarrierMask", flags, &status, 0, NULL, 2);
    flags.m_wrapU = 1;
    flags.m_wrapV = 1;
    s_texture = TextureCreate("XTextures\\FX\\LBTBarrier", flags, &status, 0, NULL, 2);

    if (CShaderEffect::s_enableShaders) {
        s_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "Barrier");
        s_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "Barrier");
    }
}
