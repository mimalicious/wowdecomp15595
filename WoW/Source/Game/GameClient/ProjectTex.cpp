#include "ProjectTex.h"
#include <cmath>
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/CGxVertexPC.h"
#include "Engine/Source/Gx/CGxVertexPN.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CiRect.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/World.h"

class ProjTex2DConstants {
    public:
    /* 0000 */ C44Matrix m_world;
    /* 0040 */ C44Matrix m_viewProj;
    /* 0080 */ C44Matrix m_tex0;
    /* 00C0 */ C44Matrix m_tex1;
    /* 0100 */ C4Vector m_color;

    ProjTex2DConstants();
};

// STATIC: 0x00D00EF8
static uint16_t s_quadIndices[12] = { 0, 1, 2, 0, 2, 3, 0, 2, 1, 0, 3, 2 };

// STATIC: 0x00D00F10
static uint32_t s_shadowTexelCount = 64 * 8;

// STATIC: 0x00D00F14
static CImVector s_shadowTexels[64 * 8];

// STATIC: 0x00E755E0/0x140D5F360
static CGxShader* s_psProjTex2D;

// STATIC: 0x00E755E4/0x140D5F368
static CGxShader* s_vsProjTex2D;

// STATIC: 0x00E755E8/0x140D5F370
static CGxShader* s_vsProjTex2DModel2;

// STATIC: 0x00E755EC/0x140D5F378
static CTexture* s_shadowModTexture;

// STATIC: 0x00E755F0/0x140D5F380
static CTexture* s_shadowAddTexture;

// STATIC: 0x00E755F4/0x140D5F388
static uint32_t s_doodadModelCount;

// STATIC: 0x00E755F8/0x140D5F390
static CM2Model* s_doodadModels[16];

// STATIC: 0x00E75638/0x140D5F410
static World::TriData s_triData;

// INLINED
CGxTex* ProjectTex2dGetAddFadeTex() {
    return TextureGetGxTex(s_shadowAddTexture);
}

// OFFSET: 0x00740620/0x14041E910
CTexture* ProjectTex2dGetModFadeTex() {
    return s_shadowModTexture;
}

// OFFSET: 0x007407B0/0x14041EA50
void ProjectTexDestroy() {
    if (s_shadowModTexture) {
        TextureRelease(s_shadowModTexture);
        s_shadowModTexture = NULL;
    }

    if (s_shadowAddTexture) {
        TextureRelease(s_shadowAddTexture);
        s_shadowAddTexture = NULL;
    }

    if (s_vsProjTex2DModel2) {
        g_theGxDevicePtr->ShaderRelease(s_vsProjTex2DModel2);
        s_vsProjTex2DModel2 = NULL;
    }

    if (s_vsProjTex2D) {
        g_theGxDevicePtr->ShaderRelease(s_vsProjTex2D);
        s_vsProjTex2D = NULL;
    }

    if (s_psProjTex2D) {
        g_theGxDevicePtr->ShaderRelease(s_psProjTex2D);
        s_psProjTex2D = NULL;
    }
}

// OFFSET: 0x00740840/0x14041EB00
int32_t ProjectTex2dMakeMatrices(C44Matrix& texMatrix, C44Matrix& fadeMatrix, const CAaBox& box, const C3Vector* scale, const C44Matrix* rotation, float fade, int32_t local, int32_t flatDepth) {
    C44Matrix translation;
    float centerY = (box.t.y + box.b.y) * 0.5f;
    float centerZ = (box.t.z + box.b.z) * 0.5f;
    C3Vector move;

    if (local) {
        move.x = -((box.t.x + box.b.x) * 0.5f);
        move.y = -centerY;
        move.z = -centerZ;
    } else {
        C3Vector center((box.b.x + box.t.x) * 0.5f, centerY, centerZ);
        const C3Vector& camPos = World::GetCamPos();
        move.x = camPos.x - center.x;
        move.y = camPos.y - center.y;
        move.z = camPos.z - center.z;
    }

    translation.Translate(move);

    float sizeX = box.t.x - box.b.x;
    float sizeZ = box.t.z - box.b.z;
    float sizeY = box.t.y - box.b.y;

    if (sizeX < 0.001f || sizeY < 0.001f || sizeZ < 0.001f) {
        return 0;
    }

    float invX = 1.0f;
    float invY = 1.0f;

    if (!scale) {
        invX = 1.0f / sizeX;
        invY = 1.0f / sizeY;
    }

    C44Matrix scaleMatrix(invX, 0.0f, 0.0f, 0.0f, 0.0f, invY, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    C44Matrix spin = C44Matrix::Rotation(-1.5707964f, C3Vector(0.0f, 0.0f, 1.0f), true);
    texMatrix = translation * scaleMatrix * spin;

    if (rotation) {
        texMatrix *= *rotation;
    }

    if (scale) {
        scaleMatrix.a0 = scale->x;
        scaleMatrix.b1 = scale->y;
        texMatrix *= scaleMatrix;
    }

    texMatrix.d0 = texMatrix.d0 + 0.5f;
    texMatrix.d1 = texMatrix.d1 + 0.5f;
    texMatrix.d2 = texMatrix.d2;

    float depthScale = 1.0f / sizeZ;

    if (flatDepth) {
        depthScale = 0.0f;
    }

    C44Matrix fadeScale(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, depthScale, 0.0f, depthScale, 0.0f, fade, 0.5f, fade, 1.0f);
    fadeMatrix = translation * fadeScale;
    return 1;
}

// OFFSET: 0x00740BD0/0x14041F060
ProjTex2DConstants::ProjTex2DConstants() {
    this->m_color = C4Vector(0.0f, 0.0f, 0.0f, 0.0f);
}

// OFFSET: 0x00740DA0/0x14041F1E0
static void ProjectTexBuildVertexBuffer(const World::TriData::Batch* triBatch, CGxBatch& batch) {
    CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPN), triBatch->m_indexCount);
    CGxVertexPN* vertex = reinterpret_cast<CGxVertexPN*>(g_theGxDevicePtr->BufLock(buf));

    if (triBatch->m_heightList) {
        for (uint16_t i = 0; i < triBatch->m_indexCount; i++) {
            uint16_t index = triBatch->m_indices[i];
            C3Vector position = triBatch->m_vertexList[index];
            position.z = triBatch->m_heightList[index];
            vertex->p = position;
            vertex->n = triBatch->GetNormal(index);
            vertex++;
        }
    } else {
        batch.m_count = 0;

        for (uint16_t i = 0; i < triBatch->m_indexCount; i += 3) {
            uint16_t i0 = triBatch->m_indices[i];
            const C3Vector& v0 = triBatch->m_vertexList[i0];
            const C3Vector& v1 = triBatch->m_vertexList[triBatch->m_indices[i + 1]];
            const C3Vector& v2 = triBatch->m_vertexList[triBatch->m_indices[i + 2]];
            C3Vector n0 = triBatch->GetNormal(i0);
            C3Vector n1 = triBatch->GetNormal(triBatch->m_indices[i + 1]);
            C3Vector n2 = triBatch->GetNormal(triBatch->m_indices[i + 2]);
            vertex[0].p = v0;
            vertex[0].n = n0;
            vertex[1].p = v1;
            vertex[1].n = n1;
            vertex[2].p = v2;
            vertex[2].n = n2;
            batch.m_count += 3;
            vertex += 3;
        }
    }

    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->unk_001C = 1;
    g_theGxDevicePtr->PrimVertexPtr(buf, GxVBF_PN);
}

// OFFSET: 0x00740FF0/0x14041F490
static void ProjectTexBuildVertexBufferVariant(const World::TriData::Batch* triBatch, CImVector color, CGxBatch& batch) {
    CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Vertex, sizeof(CGxVertexPC), triBatch->m_indexCount);
    CGxVertexPC* vertex = reinterpret_cast<CGxVertexPC*>(g_theGxDevicePtr->BufLock(buf));

    if (triBatch->m_heightList) {
        for (uint32_t i = 0; i < triBatch->m_indexCount; i++) {
            uint16_t index = triBatch->m_indices[i];
            vertex->p.x = triBatch->m_vertexList[index].x;
            vertex->p.y = triBatch->m_vertexList[index].y;
            vertex->p.z = triBatch->m_heightList[index];
            vertex->c = color;
            vertex++;
        }
    } else {
        batch.m_count = 0;

        for (uint16_t i = 0; i < triBatch->m_indexCount; i += 3) {
            const C3Vector& v0 = triBatch->m_vertexList[triBatch->m_indices[i]];
            const C3Vector& v1 = triBatch->m_vertexList[triBatch->m_indices[i + 1]];
            const C3Vector& v2 = triBatch->m_vertexList[triBatch->m_indices[i + 2]];
            vertex[0].p = v0;
            vertex[0].c = color;
            vertex[1].p = v1;
            vertex[1].c = color;
            vertex[2].p = v2;
            vertex[2].c = color;
            batch.m_count += 3;
            vertex += 3;
        }
    }

    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->unk_001C = 1;
    g_theGxDevicePtr->PrimVertexPtr(buf, GxVBF_PC);
}

// OFFSET: 0x00741190/0x14041F670
static void ProjectTexRenderIndices(const World::TriData::Batch* triBatch) {
    CGxBuf* buf = g_theGxDevicePtr->BufStream(GxPoolTarget_Index, sizeof(uint16_t), triBatch->m_indexCount);
    uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(buf));

    for (uint32_t i = 0; i < triBatch->m_indexCount; i++) {
        indices[i] = static_cast<uint16_t>(i);
    }

    g_theGxDevicePtr->BufUnlock(buf, 0);
    buf->unk_001C = 1;
    g_theGxDevicePtr->PrimIndexPtr(buf);
}

// OFFSET: 0x00741200/0x14041F710
static bool ProjectTexCenterOnSteepTri(const CAaBox& box, const C3Vector* tri) {
    float e1x = tri[1].x - tri[0].x;
    float e2x = tri[2].x - tri[0].x;
    float e2y = tri[2].y - tri[0].y;
    float e2z = tri[2].z - tri[0].z;
    float e1y = tri[1].y - tri[0].y;
    float e1z = tri[1].z - tri[0].z;
    C3Vector normal(e1y * e2z - e1z * e2y, e1z * e2x - e2z * e1x, e1x * e2y - e1y * e2x);
    float lengthSq = normal.z * normal.z + normal.y * normal.y + normal.x * normal.x;

    if (lengthSq <= 0.000001f) {
        return false;
    }

    float invLength = 1.0f / sqrt(lengthSq);
    C3Vector center((box.t.x + box.b.x) * 0.5f, (box.t.y + box.b.y) * 0.5f, (box.t.z + box.b.z) * 0.5f);
    normal.x = invLength * normal.x;
    normal.y = invLength * normal.y;
    normal.z = invLength * normal.z;
    float planeDist = tri[2].z * normal.z + tri[2].y * normal.y + normal.x * tri[2].x;
    float dist = center.z * normal.z - planeDist + center.y * normal.y + center.x * normal.x;

    if (dist >= 0.1f) {
        return false;
    }

    return dist > -0.1f && NTempest::Intersect(center, tri, 3, normal.MajorAxis()) && (normal.y + normal.x) * 0.0f + normal.z < 0.30000001f;
}

// OFFSET: 0x007413F0/0x14041F900
static void ShadowAddCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    if (command == GxTex_Lock) {
        s_shadowTexelCount = height * width;
        return;
    }

    if (command != GxTex_Latch || mipLevel) {
        return;
    }

    texelStrideInBytes = width * sizeof(CImVector);
    texels = s_shadowTexels;
    CImVector* row = s_shadowTexels;

    for (uint32_t y = 0; y < height; y++) {
        CImVector color;
        color.b = 0xFF;
        color.g = 0xFF;
        color.r = 0xFF;

        for (uint32_t x = 0; x < width; x++) {
            float t = static_cast<float>(static_cast<double>(x) / static_cast<double>(width - 1) * 12.0);
            float alpha;

            if (t < 2.0f) {
                alpha = t * 0.5f;
            } else if (t < 10.0f) {
                alpha = 1.0f;
            } else {
                alpha = (12.0f - t) * 0.5f;

                if (alpha <= 0.0f) {
                    alpha = 0.0f;
                }
            }

            color.a = static_cast<uint8_t>(static_cast<int32_t>(alpha * 255.0f));
            row[x] = color;
        }

        row += width;
    }
}

// OFFSET: 0x00741530/0x14041FA70
static void ShadowModCallback(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipLevel, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    if (command == GxTex_Lock) {
        s_shadowTexelCount = height * width;
        return;
    }

    if (command != GxTex_Latch || mipLevel) {
        return;
    }

    texelStrideInBytes = width * sizeof(CImVector);
    texels = s_shadowTexels;
    CImVector* row = s_shadowTexels;

    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            float t = static_cast<float>(static_cast<double>(x) / static_cast<double>(width - 1) * 12.0);
            float alpha;

            if (t < 2.0f) {
                alpha = t * 0.5f;
            } else if (t < 10.0f) {
                alpha = 1.0f;
            } else {
                alpha = (12.0f - t) * 0.5f;

                if (alpha <= 0.0f) {
                    alpha = 0.0f;
                }
            }

            CImVector color;
            color.b = static_cast<uint8_t>(static_cast<int32_t>((1.0f - alpha) * 255.0f));
            color.g = color.b;
            color.r = color.b;
            color.a = color.b == 0xFF ? 0x00 : 0xFF;
            row[x] = color;
        }

        row += width;
    }
}

// OFFSET: [0x007416A0/0x14041FC10<0, 0>, 0x00741BF0/0x14041FD60<1, 0>, 0x00742140/0x14041FED0<2, 0>, 0x00742690/0x140420040<0, 1>, 0x00742BD0/0x140420190<1, 1>, 0x00743110/0x1404202F0<2, 1>]
template <uint32_t AXIS, int32_t MAX>
static void ProjectTexClipPolygon(uint32_t& outCount, C3Vector* out, const CAaBox& box, const C3Vector* in, uint32_t inCount) {
    outCount = 0;

    for (uint32_t i = 0; i < inCount; i++) {
        const C3Vector& a = in[i];
        const C3Vector& b = in[(i + 1) % inCount];
        float da = MAX ? (&box.t.x)[AXIS] - (&a.x)[AXIS] : (&a.x)[AXIS] - (&box.b.x)[AXIS];
        float db = MAX ? (&box.t.x)[AXIS] - (&b.x)[AXIS] : (&b.x)[AXIS] - (&box.b.x)[AXIS];

        if (da < 0.0f && db < 0.0f) {
            continue;
        }

        if (da >= 0.0f && db >= 0.0f) {
            out[outCount++] = a;
            continue;
        }

        float t = da / (da - db);
        C3Vector hit((b.x - a.x) * t + a.x, (b.y - a.y) * t + a.y, (b.z - a.z) * t + a.z);

        if (da >= 0.0f) {
            out[outCount++] = a;
        }

        out[outCount++] = hit;
    }
}

// OFFSET: 0x00743650/0x140420450
void ProjectTexInitialize() {
    s_vsProjTex2DModel2 = NULL;
    s_vsProjTex2D = NULL;
    s_psProjTex2D = NULL;

    if (CShaderEffect::s_enableShaders) {
        s_vsProjTex2DModel2 = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "ProjTex2D_Model2");
        s_vsProjTex2D = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "ProjTex2D");
        s_psProjTex2D = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "ProjTex2D");
    }

    CGxTexFlags flags;
    s_shadowAddTexture = TextureCreate(64, 8, GxTex_Argb8888, GxTex_Argb8888, flags, NULL, ShadowAddCallback, "ShadowAdd", 0);
    s_shadowModTexture = TextureCreate(64, 8, GxTex_Argb8888, GxTex_Argb8888, flags, NULL, ShadowModCallback, "ShadowMod", 0);

    CiRect rect(0, 0, 8, 64);
    GxTexUpdate(TextureGetGxTex(s_shadowAddTexture), rect, 0);
    GxTexUpdate(TextureGetGxTex(s_shadowModTexture), rect, 0);
}

// OFFSET: 0x00743760/0x1404205C0
static void ProjectTexClipTriangle(const C3Vector* tri, const CAaBox& box, uint32_t& quadrants, float& minZ, float& maxZ) {
    C3Vector polyA[16];
    C3Vector polyB[16];
    uint32_t countA;
    uint32_t countB;

    ProjectTexClipPolygon<0, 0>(countA, polyA, box, tri, 3);

    if (countA < 3) {
        return;
    }

    ProjectTexClipPolygon<1, 0>(countB, polyB, box, polyA, countA);

    if (countB < 3) {
        return;
    }

    ProjectTexClipPolygon<2, 0>(countA, polyA, box, polyB, countB);

    if (countA < 3) {
        return;
    }

    ProjectTexClipPolygon<0, 1>(countB, polyB, box, polyA, countA);

    if (countB < 3) {
        return;
    }

    ProjectTexClipPolygon<1, 1>(countA, polyA, box, polyB, countB);

    if (countA < 3) {
        return;
    }

    ProjectTexClipPolygon<2, 1>(countB, polyB, box, polyA, countA);

    if (countB < 3) {
        return;
    }

    float centerX = (box.b.x + box.t.x) * 0.5f;
    float centerY = (box.t.y + box.b.y) * 0.5f;

    for (uint32_t i = 0; i < countB; i++) {
        const C3Vector& v = polyB[i];
        minZ = v.z <= minZ ? v.z : minZ;
        maxZ = maxZ > v.z ? maxZ : v.z;
        uint32_t shift = centerY <= v.y ? 2 : 0;
        uint32_t bit = centerX <= v.x ? 2 : 1;
        quadrants |= bit << shift;
    }
}

// OFFSET: 0x007438D0/0x1404207A0
static int32_t ProjectTexNeedsFallback(const CAaBox& box, int32_t testSteep) {
    float minZ = 1000000.0f;
    float maxZ = -1000000.0f;
    uint32_t quadrants = 0;
    C3Vector tri[3];

    for (uint32_t b = 0; b < World::TriData::s_nBatches; b++) {
        const World::TriData::Batch& batch = World::TriData::s_batches[b];
        uint16_t indexCount = batch.m_indexCount;

        for (uint16_t i = 0; i < indexCount; i += 3) {
            uint16_t i0 = batch.m_indices[i];
            uint16_t i1 = batch.m_indices[i + 1];
            uint16_t i2 = batch.m_indices[i + 2];

            if (batch.m_heightList) {
                tri[0].x = batch.m_vertexList[i0].x;
                tri[0].y = batch.m_vertexList[i0].y;
                tri[0].z = batch.m_heightList[i0];
                tri[1].x = batch.m_vertexList[i1].x;
                tri[1].y = batch.m_vertexList[i1].y;
                tri[1].z = batch.m_heightList[i1];
                tri[2].x = batch.m_vertexList[i2].x;
                tri[2].y = batch.m_vertexList[i2].y;
                tri[2].z = batch.m_heightList[i2];
            } else {
                tri[0] = batch.m_vertexList[i0];
                tri[1] = batch.m_vertexList[i1];
                tri[2] = batch.m_vertexList[i2];
            }

            if (batch.m_matrix) {
                tri[0] = tri[0] * *batch.m_matrix;
                tri[1] = tri[1] * *batch.m_matrix;
                tri[2] = tri[2] * *batch.m_matrix;
            }

            if (testSteep && ProjectTexCenterOnSteepTri(box, tri)) {
                return 1;
            }

            ProjectTexClipTriangle(tri, box, quadrants, minZ, maxZ);
        }
    }

    if (World::TriData::s_nBatches && minZ <= maxZ && quadrants) {
        uint32_t count = 0;

        for (uint32_t bits = quadrants; bits; bits >>= 1) {
            if (bits & 0x1) {
                count++;
            }
        }

        if (count >= 3) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x00743EF0/0x140420E60
static int32_t ProjectTexRenderGetQuery2d(const CAaBox& box, uint32_t triFlags, uint32_t flags, int32_t testCoverage) {
    s_doodadModelCount = 0;
    World::TriData::s_statusFlags = 0;
    World::TriData::s_nBatches = 0;
    World::TriData::s_faceIndexCursor = 0;
    World::TriData::s_indexCursor = 0;
    World::TriData::s_nMatrices = 0;

    if (flags & 0x1) {
        s_doodadModelCount = World::GetIntersectingDoodadModels(s_doodadModels, 16, box, 0x2000000);
    }

    if (triFlags) {
        World::GetTris(box, s_triData, triFlags);
    }

    if (testCoverage) {
        if (s_doodadModelCount) {
            return 1;
        }

        if (ProjectTexNeedsFallback(box, flags & 0x10)) {
            static C3Vector s_quadVertices[4] = { C3Vector(1.0f, 1.0f, 0.0f), C3Vector(-1.0f, 1.0f, 0.0f), C3Vector(-1.0f, -1.0f, 0.0f), C3Vector(1.0f, -1.0f, 0.0f) };

            World::TriData::s_statusFlags = 0;
            World::TriData::s_nBatches = 0;
            World::TriData::s_faceIndexCursor = 0;
            World::TriData::s_indexCursor = 0;
            World::TriData::s_nMatrices = 0;
            World::TriData::Batch* batch = World::TriData::AllocBatch();

            if (batch) {
                C44Matrix* matrix = World::TriData::AllocMatrix();
                *matrix = C44Matrix();
                matrix->Scale(C3Vector((box.t.x - box.b.x) * 0.5f, (box.t.y - box.b.y) * 0.5f, (box.t.z - box.b.z) * 0.5f));
                matrix->d0 = (box.b.x + box.t.x) * 0.5f;
                matrix->d1 = (box.t.y + box.b.y) * 0.5f;
                matrix->d2 = (box.t.z + box.b.z) * 0.5f;
                batch->m_indexCount = 12;
                batch->m_faceCount = 4;
                batch->m_matrix = matrix;
                batch->m_vertexList = s_quadVertices;
                batch->m_normalList = NULL;
                batch->m_heightList = NULL;
                batch->m_indices = s_quadIndices;
                batch->m_faceIndices = NULL;
                batch->m_minVertexIndex = 0;
                batch->m_maxVertexIndex = 3;
                batch->m_def = NULL;
            }
        }
    }

    return s_doodadModelCount || World::TriData::s_nBatches;
}

// OFFSET: 0x007441B0/0x140421200
void ProjectTex2dDraw(const CAaBox& box, const CImVector& color, uint32_t triFlags, uint32_t flags, EGxPolygonOffset polygonOffset) {
    if (triFlags & 0x30000) {
        flags &= ~0x8;
    }

    if (!ProjectTexRenderGetQuery2d(box, triFlags, flags, flags & 0x8)) {
        return;
    }

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_PolygonOffset, static_cast<int32_t>(polygonOffset));

    int32_t blendingMode = g_theGxDevicePtr->m_appRenderStates[GxRs_BlendingMode].m_value.m_data.i[0];
    int32_t texOp;

    if (blendingMode == GxBlend_Mod) {
        g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(s_shadowModTexture));
        texOp = 2;
    } else {
        g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(s_shadowAddTexture));
        texOp = 0;
    }

    if (!CShaderEffect::s_enableShaders) {
        GxRsSet(GxRs_TexGen0, 2);
        GxRsSet(GxRs_TexGen1, 2);
        GxRsSet(GxRs_TextureShader0, 1);
        GxRsSet(GxRs_TextureShader1, 1);
        GxRsSet(GxRs_MatDiffuse, *reinterpret_cast<const int32_t*>(&color));
        GxRsSet(GxRs_ColorOp1, texOp);
        GxRsSet(GxRs_AlphaOp1, texOp);
    }

    C44Matrix view;
    const C3Vector& camPos = World::GetCamPos();
    view.d0 = -camPos.x;
    view.d1 = -camPos.y;
    view.d2 = -camPos.z;

    ProjTex2DConstants constants;
    constants.m_color.Set(color);

    for (uint32_t i = 0; i < World::TriData::s_nBatches; i++) {
        World::TriData::Batch* triBatch = &World::TriData::s_batches[i];

        if (3 * triBatch->m_indexCount > 0x10000 || triBatch->m_indexCount < 3 || !triBatch->m_matrix || !triBatch->m_vertexList || !triBatch->m_indices) {
            continue;
        }

        C44Matrix world = *triBatch->m_matrix * view;
        GxXformSet(GxXform_World, world);

        CGxBatch batch;
        batch.m_primType = GxPrim_Triangles;
        batch.m_start = 0;
        batch.m_count = triBatch->m_indexCount;
        batch.m_minIndex = 0;
        batch.m_maxIndex = triBatch->m_indexCount - 1;
        ProjectTexBuildVertexBuffer(triBatch, batch);
        ProjectTexRenderIndices(triBatch);

        if (CShaderEffect::s_enableShaders) {
            constants.m_world = world;
            GxXformViewProjNative(constants.m_viewProj);
            GxXform(GxXform_Tex0, constants.m_tex0);
            GxXform(GxXform_Tex1, constants.m_tex1);
            GxShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(&constants), 17);
            GxRsSet(GxRs_VertexShader, s_vsProjTex2D, 0);
            GxRsSet(GxRs_PixelShader, s_psProjTex2D, ((flags & 0x2) == 0) + 2 * (blendingMode == GxBlend_Mod));
        }

        GxDraw(batch);
    }

    if (s_doodadModelCount) {
        GxRsSet(GxRs_VertexShader, 0);
        GxRsSet(GxRs_PixelShader, 0);

        for (uint32_t i = 0; i < s_doodadModelCount; i++) {
            CM2Model* model = s_doodadModels[i];
            GxXformSet(GxXform_World, model->m_worldTransform * view);
            GxRsSet(GxRs_MatDiffuse, *reinterpret_cast<const int32_t*>(&color));

            if (CShaderEffect::s_enableShaders) {
                constants.m_world = model->m_worldTransform * view;
                GxXformViewProjNative(constants.m_viewProj);
                GxXform(GxXform_Tex0, constants.m_tex0);
                GxXform(GxXform_Tex1, constants.m_tex1);
                GxShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(&constants), 17);
                GxRsSet(GxRs_VertexShader, s_vsProjTex2DModel2, 0);
                GxRsSet(GxRs_PixelShader, s_psProjTex2D, ((flags & 0x2) == 0) + 2 * (blendingMode == GxBlend_Mod));
            }

            model->RenderModelBatchesForProjectedTexture();
        }
    }

    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x00744850/0x140422220
void ProjectTex2dDrawShaderEffect(const CAaBox& box, const CImVector& color, uint32_t triFlags, uint32_t flags, EGxPolygonOffset polygonOffset) {
    s_doodadModelCount = 0;
    World::TriData::s_statusFlags = 0;
    World::TriData::s_nBatches = 0;
    World::TriData::s_faceIndexCursor = 0;
    World::TriData::s_indexCursor = 0;
    World::TriData::s_nMatrices = 0;

    if (flags & 0x1) {
        s_doodadModelCount = World::GetIntersectingDoodadModels(s_doodadModels, 16, box, 0x2000000);
    }

    if (triFlags) {
        World::GetTris(box, s_triData, triFlags);
    }

    if (!s_doodadModelCount && !World::TriData::s_nBatches) {
        return;
    }

    g_theGxDevicePtr->RsPush();

    if (g_theGxDevicePtr->m_appRenderStates[GxRs_BlendingMode].m_value.m_data.i[0] == GxBlend_Mod) {
        g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(s_shadowModTexture));
    } else if (!g_theGxDevicePtr->m_appRenderStates[GxRs_Texture1].m_value.m_data.i[0]) {
        g_theGxDevicePtr->RsSet(GxRs_Texture1, TextureGetGxTex(s_shadowAddTexture));
    }

    GxRsSet(GxRs_PolygonOffset, static_cast<int32_t>(polygonOffset));

    uint32_t shaderColors = flags & 0x4;
    CImVector packed = color;

    if (shaderColors) {
        CShaderEffect::SetDiffuse(C4Vector(color));
    } else {
        GxFormatColor(packed);
    }

    C44Matrix view;
    const C3Vector& camPos = World::GetCamPos();
    view.d0 = -camPos.x;
    view.d1 = -camPos.y;
    view.d2 = -camPos.z;
    g_theGxDevicePtr->XformPush(GxXform_World);

    for (uint32_t i = 0; i < World::TriData::s_nBatches; i++) {
        World::TriData::Batch* triBatch = &World::TriData::s_batches[i];

        if (3 * triBatch->m_indexCount > 0x10000 || triBatch->m_indexCount < 3 || !triBatch->m_matrix || !triBatch->m_vertexList || !triBatch->m_indices) {
            continue;
        }

        GxXformSet(GxXform_World, *triBatch->m_matrix * view);

        CGxBatch batch;
        batch.m_primType = GxPrim_Triangles;
        batch.m_start = 0;
        batch.m_count = triBatch->m_indexCount;
        batch.m_minIndex = 0;
        batch.m_maxIndex = triBatch->m_indexCount - 1;

        if (shaderColors) {
            ProjectTexBuildVertexBuffer(triBatch, batch);
        } else {
            ProjectTexBuildVertexBufferVariant(triBatch, packed, batch);
        }

        ProjectTexRenderIndices(triBatch);
        CShaderEffect::SetDefaultShaders(0, 0);
        CShaderEffect::SetAlphaRefDefault();
        CShaderEffect::UpdateWorldViewMatrix();
        GxDraw(batch);
    }

    for (uint32_t i = 0; i < s_doodadModelCount; i++) {
        CM2Model* model = s_doodadModels[i];
        GxXformSet(GxXform_World, model->m_worldTransform * view);
        CShaderEffect::UpdateWorldViewMatrix();
        CShaderEffect::SetDiffuse(C4Vector(color));
        model->RenderModelBatchesForProjectedTexture();
    }

    GxXformPop(GxXform_World);
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x00744C50/0x140422860
void ProjectTex2d(const CAaBox& box, const CImVector& color, const C44Matrix* rotation, float fade, uint32_t triFlags, uint32_t flags, EGxPolygonOffset polygonOffset) {
    C44Matrix texMatrix;
    C44Matrix fadeMatrix;

    if (!ProjectTex2dMakeMatrices(texMatrix, fadeMatrix, box, NULL, rotation, fade, 0, 0)) {
        return;
    }

    g_theGxDevicePtr->XformPushAndSet(GxXform_Tex0, texMatrix);
    g_theGxDevicePtr->XformPushAndSet(GxXform_Tex1, fadeMatrix);
    g_theGxDevicePtr->XformPush(GxXform_World);
    ProjectTex2dDraw(box, color, triFlags, flags, polygonOffset);
    GxXformPop(GxXform_World);
    GxXformPop(GxXform_Tex0);
    GxXformPop(GxXform_Tex1);
}

// OFFSET: 0x00744DD0/0x140422A40
void ProjectTex2d(const CAaBox& box, const CImVector& color, const C3Vector* scale, const C44Matrix* rotation, float fade, uint32_t triFlags, uint32_t flags, EGxPolygonOffset polygonOffset) {
    C44Matrix texMatrix;
    C44Matrix fadeMatrix;

    if (!ProjectTex2dMakeMatrices(texMatrix, fadeMatrix, box, scale, rotation, fade, 0, 0)) {
        return;
    }

    g_theGxDevicePtr->XformPushAndSet(GxXform_Tex0, texMatrix);
    g_theGxDevicePtr->XformPushAndSet(GxXform_Tex1, fadeMatrix);
    g_theGxDevicePtr->XformPush(GxXform_World);
    ProjectTex2dDraw(box, color, triFlags, flags, polygonOffset);
    GxXformPop(GxXform_World);
    GxXformPop(GxXform_Tex0);
    GxXformPop(GxXform_Tex1);
}
