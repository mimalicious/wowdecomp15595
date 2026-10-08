#include <cfloat>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <new>
#include <xmmintrin.h>
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Base/SIMD.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/ViewSettings.h"
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Model2/CM2SceneRender.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/Os/Path.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffectManager.h"
#include "Engine/Source/Services/GfxSingletonManager.h"
#include "Engine/Source/Services/ParticleSystem2.h"
#include "Engine/Source/Services/RibbonEmitter.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/C3Ray.h"
#include "Engine/Source/Tempest/NTempest.h"

// STATIC: %0x00AF949C
const uint32_t s_localLighting[7] = { 1, 1, 1, 1, 1, 0, 0 };

// STATIC: %0x00AF94F0
const uint32_t s_blendModes[6][7] = { { 0, 1, 2, 10, 3, 4, 5 }, { 0, 1, 2, 10, 3, 4, 5 }, { 2, 2, 2, 10, 3, 4, 5 }, { 2, 2, 2, 10, 3, 4, 5 }, { 2, 2, 2, 10, 3, 4, 5 }, { 2, 2, 2, 10, 3, 4, 5 } };

// STATIC: %0x00C0D298
uint32_t CM2Scene::s_optFlags;

// STATIC: 0x00D07F14
static float s_lightGridScale = 0.05f;

// STATIC: %0x00D89EB0
static uint32_t s_doodadHash[1571];

// PROVEN: a9db6533 IDENTICAL/DIFFERENT
// OFFSET: 0x00827570/0x1405454E0
void CM2Scene::SetTime(uint32_t time) {
    this->m_time = time;
    this->m_cache->UpdateShared();
    this->m_cache->GarbageCollect(0);
}

// PROVEN: 6d1d632b IDENTICAL/UNPROVEN
// OFFSET: 0x00827550/0x1405454D0
void CM2Scene::ReloadModel(CM2Model* model, CM2Shared* shared) {
    model->Reload(this, shared);
}

// OFFSET: 0x0082F300/0x14054FB50
void CM2Scene::AdvanceTime(uint32_t delta) {
    if (!this->m_litSphereTexture) {
        CGxTexFlags texFlags;
        CStatus status;
        this->m_litSphereTexture = TextureCreate("XTextures\\FX\\litsphere.blp", texFlags, &status, 0, NULL, 2);
        this->m_litSphereEffect = CShaderEffectManager::GetEffect("LitSphere_Additive");
    }

    this->m_time += delta;
    this->m_cache->UpdateShared();
    this->m_cache->GarbageCollect(0);
    this->m_flags |= 4;
    this->m_timeDelta = delta;

    if (delta) {
        CM2Model* model = this->m_animateList;

        while (model) {
            CM2Model* next = model->m_animateNext;
            model->ProcessCallbacksRecursive();
            model = next;
        }
    }

    this->m_flags &= ~4u;
}

// INLINED
void CM2Scene::SetFloat18(float value) {
    this->m_float18 = value;
}

// OFFSET: 0x008275A0/0x140545510
uint32_t ParticleBlendToM2Blend(uint32_t blend) {
    switch (blend) {
        case 1:
            return 1;
        case 2:
            return 2;
        case 3:
            return 4;
        case 4:
            return 5;
        case 5:
            return 6;
        case 10:
            return 3;
        default:
            return 0;
    }
}

// OFFSET: 0x00827600
uint32_t ParticleFlagsToM2Flags(uint32_t flags) {
    uint32_t result = 4;

    if (!(flags & 1)) {
        result = 5;
    }

    if (!(flags & 2)) {
        result |= 2;
    }

    if (!(flags & 4)) {
        result |= 0x10;
    }

    return result;
}

// PROVEN: 5955f780 IDENTICAL/NONE
// OFFSET: 0x00827630/0x140545580
void CM2Scene::EndDraw() {
    this->m_drawing = 0;
}

// PROVEN: 118a4d83 IDENTICAL/IDENTICAL
// OFFSET: 0x00827640/0x140545590
void CM2Scene::PurgeModelCache() {
    if (this->m_cache) {
        this->m_cache->GarbageCollect(1);
    }
}

// PROVEN: 3d78f649 IDENTICAL/DIFFERENT
// OFFSET: 0x00827650/0x1405455B0
void CM2Scene::BeginHitTest() {
    this->m_flags |= 2;
}

// PROVEN: 75b851aa IDENTICAL/NONE
// OFFSET: 0x00827660/0x1405455C0
void CM2Scene::ClearHitTestList() {
    for (CM2Model* model = this->m_hitTestList; model; model = model->m_hitTestNext) {
        *model->m_hitTestPrev = NULL;
        model->m_hitTestPrev = NULL;
    }

    this->m_flags &= ~2u;
}

// OFFSET: 0x00827690/0x140545600
void CM2Scene::AllocateSpaceForHitList() {
    uint32_t count = 0;

    for (CM2Model* model = this->m_hitTestList; model; model = model->m_hitTestNext) {
        count++;
    }

    if (count <= this->m_hitCapacity) {
        return;
    }

    delete[] this->m_hitRecs;
    delete[] this->m_hitOrder;

    if (!this->m_hitCapacity) {
        this->m_hitCapacity = 1;
    }

    while (this->m_hitCapacity < count) {
        this->m_hitCapacity *= 2;
    }

    this->m_hitRecs = new (__FILE__, __LINE__) M2HitRec[this->m_hitCapacity];
    this->m_hitOrder = new (__FILE__, __LINE__) uint32_t[this->m_hitCapacity];
}

// OFFSET: 0x00827950/0x140545990
int32_t CM2Scene::SortHitNear(uint32_t a, uint32_t b, const void* param) {
    const M2HitRec* recs = static_cast<const M2HitRec*>(param);
    const M2HitRec* recA = &recs[a];
    const M2HitRec* recB = &recs[b];

    if (recB->m_tNear > recA->m_tNear) {
        return -1;
    }

    if (recB->m_tNear < recA->m_tNear) {
        return 1;
    }

    if (recB->m_tFar > recA->m_tFar) {
        return -1;
    }

    if (recB->m_tFar < recA->m_tFar) {
        return 1;
    }

    if (b > a) {
        return -1;
    }

    return b < a;
}

// PROVEN: 6a5aa96f IDENTICAL/NONE
// OFFSET: 0x00827B50/0x140545C20
void CM2Scene::SetProjectTextureCallback(void (*callback)(const CAaBox&, const CImVector&, int32_t, void*, int32_t), void* param) {
    this->m_projectTextureCallback = callback;
    this->m_projectTextureParam = param;
}

// OFFSET: 0x00827BA0/0x140545C50
static uint32_t __stdcall HashDoodadElement(const M2Element* element) {
    const CM2Model* model = element->m_model;
    const CM2Lighting* lighting = model->m_currentLighting;
    uint32_t colorIndex = element->m_batch->m_colorIndex;

    const uint32_t* light = reinterpret_cast<const uint32_t*>(lighting);
    uint32_t hash = light[23] + 19 * (light[22] + 19 * (light[21] + 19 * (light[26] + 19 * (light[25] + 19 * (19 * (light[46] + 19 * (light[45] + 19 * (19 * (light[41] + 19 * (light[40] + 19 * (element->m_index + 19 * static_cast<uint32_t>(reinterpret_cast<uintptr_t>(model->m_shared))))) + light[44]))) + light[24])))));

    if (colorIndex < model->m_shared->m_data->m_colors.m_count) {
        const uint32_t* color = reinterpret_cast<const uint32_t*>(&model->m_colors[colorIndex].m_colorTrack.m_currentValue);
        hash = color[2] + 19 * (color[1] + 19 * (19 * hash + color[0]));
    }

    const uint32_t* diffuse = reinterpret_cast<const uint32_t*>(&model->m_diffuseColor);
    const uint32_t* emissive = reinterpret_cast<const uint32_t*>(&model->m_emissiveColor);

    return reinterpret_cast<const uint32_t*>(&model->unk_01D0)[0] + 19 * (emissive[2] + 19 * (emissive[1] + 19 * (19 * (diffuse[2] + 19 * (diffuse[1] + 19 * (19 * hash + diffuse[0]))) + emissive[0])));
}


// PROVEN: 055bdfe9 IDENTICAL/DIFFERENT
// OFFSET: 0x00828470/0x1405462A0
static void AnimateThread(void* param) {
    CM2Scene* scene = static_cast<CM2Scene*>(param);

    for (CM2Model* model = scene->m_animateList; model; model = model->m_animateNext) {
        model = model->m_animateNext;

        if (!model) {
            break;
        }

        if (model->m_attachParent) {
            continue;
        }

        if (model->m_fFlags & 0x800) {
            model->AnimateMTSimple(&scene->m_view, C3Vector(1.0f, 1.0f, 1.0f), C3Vector(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, model->m_baseAlpha3);
        } else {
            model->AnimateMT(&scene->m_view, C3Vector(1.0f, 1.0f, 1.0f), C3Vector(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, model->m_baseAlpha3);
        }
    }
}

// OFFSET: 0x008287F0/0x140546850
int32_t CM2Scene::ComputeRayDirAndLen(const C3Vector& start, const C3Vector& end, float t, float* length, C3Vector* dir) {
    if (t >= 0.0000099999997f) {
        float dx = end.x - start.x;
        float dy = end.y - start.y;
        float dz = end.z - start.z;
        float len = sqrtf(dx * dx + dz * dz + dy * dy);
        *length = len;

        if (len >= 0.0000099999997f) {
            float inv = 1.0f / len;
            dir->x = dx * inv;
            dir->y = dy * inv;
            dir->z = inv * dz;
            return 1;
        }
    }

    for (CM2Model* model = this->m_hitTestList; model; model = model->m_hitTestNext) {
        *model->m_hitTestPrev = NULL;
        model->m_hitTestPrev = NULL;
    }

    this->m_flags &= ~2u;

    return 0;
}

// OFFSET: 0x00829210/0x140547540
uint32_t CM2Scene::SphereTestModels(const C3Vector& start, const C3Vector& dir, float length, int32_t frameCheck) {
    uint32_t count = 0;

    for (CM2Model* model = this->m_hitTestList; model; model = model->m_hitTestNext) {
        *model->m_hitTestPrev = NULL;
        model->m_hitTestPrev = NULL;

        if (!(model->m_fFlags & 1) || model->m_frameStamp == 0xFFFFFFFF) {
            continue;
        }

        if (frameCheck && model->m_frameStamp != this->m_frameStamp) {
            if (model->m_hitTestMode != 3 || model->m_attachParent) {
                continue;
            }

            model->m_matrixF4 = model->m_worldTransform * this->m_view;
        }

        M2Data* data = model->m_shared->m_data;
        const M2Bounds* bounds = model->m_hitTestMode == 3 ? &data->m_collisionBounds : &data->m_sequences[model->m_bones->m_sequence.m_sequenceIndex].m_bounds;

        if (fabsf(bounds->m_radius) < 0.00000023841858f) {
            continue;
        }

        const C44Matrix& matrix = model->m_matrixF4;
        C3Vector center((bounds->m_extent.t.x + bounds->m_extent.b.x) * 0.5f, (bounds->m_extent.t.y + bounds->m_extent.b.y) * 0.5f, 0.5f * (bounds->m_extent.t.z + bounds->m_extent.b.z));
        C3Vector position = center * matrix;
        float vx = position.x - start.x;
        float vy = position.y - start.y;
        float vz = position.z - start.z;
        float proj = dir.x * vx + dir.z * vz + dir.y * vy;
        float px = dir.x * proj - vx;
        float py = dir.y * proj - vy;
        float pz = dir.z * proj - vz;
        float radiusSq = bounds->m_radius * ((matrix.a2 * matrix.a2 + matrix.a1 * matrix.a1 + matrix.a0 * matrix.a0) * bounds->m_radius);
        float distSq = px * px + py * py + pz * pz;

        if (distSq > radiusSq) {
            continue;
        }

        float halfSq = radiusSq - distSq;

        if (proj < 0.0f && proj * proj > halfSq) {
            continue;
        }

        float over = proj - length;

        if (over > 0.0f && over * over > halfSq) {
            continue;
        }

        float half = sqrtf(halfSq);
        float tNear = proj - half;

        if ((tNear >= 0.0f ? tNear : 0.0f) > length) {
            tNear = length;
        } else if (tNear < 0.0f) {
            tNear = 0.0f;
        }

        float tFar = proj + half;

        if ((tFar >= 0.0f ? tFar : 0.0f) > length) {
            tFar = length;
        } else if (tFar < 0.0f) {
            tFar = 0.0f;
        }

        M2HitRec* rec = &this->m_hitRecs[count];
        rec->m_tNear = tNear;
        rec->m_model = model;
        rec->m_tFar = tFar;
        rec->m_priority = model->m_hitTestGroup;
        this->m_hitOrder[count] = count;
        count++;
    }

    return count;
}

// OFFSET: 0x00827780/0x140545720
M2HitRec* CM2Scene::IntersectHitTestTriangles(const uint16_t* indices, const uint16_t* end, uint32_t vertexStart, const C3Vector& start, uint32_t pass, M2HitRec* rec, float& t, M2HitRec* best) {
    for (; indices < end; indices += 3) {
        const C3Vector& v0 = this->m_hitVerts[indices[0] - vertexStart];
        const C3Vector& v1 = this->m_hitVerts[indices[1] - vertexStart];
        const C3Vector& v2 = this->m_hitVerts[indices[2] - vertexStart];
        float det = (v2.y - v0.y) * (v1.x - v0.x) - (v2.x - v0.x) * (v1.y - v0.y);

        if (fabsf(det) < 0.0000099999997f) {
            continue;
        }

        float inv = 1.0f / det;
        float ax = v1.x - start.x;
        float ay = v1.y - start.y;
        float bx = v2.x - start.x;
        float by = v2.y - start.y;
        float w0 = (by * ax - bx * ay) * inv;

        if (w0 < 0.0f) {
            continue;
        }

        float cy = v0.y - start.y;
        float cx = v0.x - start.x;
        float w1 = (bx * cy - by * cx) * inv;

        if (w1 < 0.0f) {
            continue;
        }

        float w2 = inv * (ay * cx - ax * cy);

        if (w2 < 0.0f) {
            continue;
        }

        float z = w1 * v1.z + w2 * v2.z + v0.z * w0;

        if (z >= 0.0f && ((pass && (!best || best->m_priority != rec->m_priority)) || z <= t)) {
            t = z;
            best = rec;
        }
    }

    return best;
}

// OFFSET: 0x00829570/0x140547A10
M2HitRec* CM2Scene::HitTestGeometry(const CM2Model* model, uint32_t pass, const C3Vector& dir, float d, const C3Vector& start, M2HitRec* rec, float& t, M2HitRec* best) {
    M2SkinProfile* skinData = model->m_shared->m_skinData;
    M2Data* data = model->m_shared->m_data;

    for (uint32_t i = 0; i < skinData->m_batches.m_count; i++) {
        M2Batch* batch = &skinData->m_batches[i];

        if (batch->m_materialLayer || (batch->m_flags & 8)) {
            continue;
        }

        M2Material* material = &data->m_materials[batch->m_materialIndex];

        if (model->m_hitTestMode == 2 && !material->m_blendMode) {
            continue;
        }

        if (model->m_hitTestMode == 1 && material->m_blendMode && !(material->m_flags & 0x20)) {
            continue;
        }

        if (!model->m_skinSections[batch->m_skinSectionIndex]) {
            continue;
        }

        float alpha = model->m_alpha;

        if (batch->m_colorIndex < data->m_colors.m_count) {
            alpha *= model->m_colors[batch->m_colorIndex].m_alphaTrack.m_currentValue;
        }

        if (batch->m_textureCount && !(batch->m_flags & 0x40)) {
            alpha *= model->m_textureWeights[data->m_textureWeightCombos[batch->m_textureWeightComboIndex]].m_currentValue;
        }

        if (alpha <= 0.0f) {
            continue;
        }

        M2SkinSection* section = &skinData->m_skinSections[batch->m_skinSectionIndex];

        if (section->m_vertexCount > this->m_hitVertCapacity) {
            delete[] this->m_hitVerts;

            if (!this->m_hitVertCapacity) {
                this->m_hitVertCapacity = 1;
            }

            while (this->m_hitVertCapacity < section->m_vertexCount) {
                this->m_hitVertCapacity *= 2;
            }

            this->m_hitVerts = new (__FILE__, __LINE__) C3Vector[this->m_hitVertCapacity];
        }

        if (section->m_boneInfluences > 1) {
            SIMD::s_functions.ProjectVertices(this->m_hitVerts, data->m_vertices.Data(), section->m_vertexStart, section->m_vertexCount, skinData->m_vertices.Data(), model->m_boneMatrices, pass, &dir, d);
        } else {
            SIMD::s_functions.ProjectVerticesOneBone(this->m_hitVerts, data->m_vertices.Data(), section->m_vertexStart, section->m_vertexCount, skinData->m_vertices.Data(), model->m_boneMatrices, pass, &dir, d);
        }

        const uint16_t* indices = &skinData->m_indices[section->m_indexStart];
        best = this->IntersectHitTestTriangles(indices, indices + section->m_indexCount, section->m_vertexStart, start, pass, rec, t, best);
    }

    return best;
}

// OFFSET: 0x008297F0/0x140547EB0
M2HitRec* CM2Scene::HitTestCollision(const CM2Model* model, uint32_t pass, const C3Vector& dir, float d, const C3Vector& start, M2HitRec* rec, float& t, M2HitRec* best) {
    M2Data* data = model->m_shared->m_data;

    if (data->m_collisionPositions.m_count > this->m_hitVertCapacity) {
        delete[] this->m_hitVerts;

        if (!this->m_hitVertCapacity) {
            this->m_hitVertCapacity = 1;
        }

        while (this->m_hitVertCapacity < data->m_collisionPositions.m_count) {
            this->m_hitVertCapacity *= 2;
        }

        this->m_hitVerts = new (__FILE__, __LINE__) C3Vector[this->m_hitVertCapacity];
    }

    for (uint32_t i = 0; i < data->m_collisionPositions.m_count; i++) {
        C3Vector p = data->m_collisionPositions[i] * model->m_matrixF4;
        C3Vector* out = &this->m_hitVerts[i];
        float dist = dir.y * p.y + dir.x * p.x + dir.z * p.z - d;
        out->x = p.x - dir.x * dist;
        out->y = p.y - dir.y * dist;
        out->z = dist;
    }

    const uint16_t* indices = data->m_collisionIndices.Data();

    return this->IntersectHitTestTriangles(indices, indices + data->m_collisionIndices.m_count, 0, start, pass, rec, t, best);
}

// OFFSET: 0x00829A50/0x1405481B0
void* CM2Scene::EndHitTest(const C3Vector& start, const C3Vector& end, float& t, int32_t twoPass) {
    C3Vector dir(0.0f, 0.0f, 0.0f);
    float length;

    if (!this->ComputeRayDirAndLen(start, end, t, &length, &dir)) {
        return NULL;
    }

    this->AllocateSpaceForHitList();

    uint32_t count = this->SphereTestModels(start, dir, length, 1);
    M2HeapSort(&CM2Scene::SortHitNear, this->m_hitOrder, count, this->m_hitRecs);

    M2HitRec* best = NULL;
    float d = start.z * dir.z + start.y * dir.y + start.x * dir.x;
    float tMax = length;

    if (t < 1.0f) {
        tMax = t * length;
    }

    for (uint32_t pass = 0; ;) {
        for (uint32_t i = 0; i < count; i++) {
            M2HitRec* rec = &this->m_hitRecs[this->m_hitOrder[i]];

            if (pass) {
                if (best && (best->m_priority > rec->m_priority || (best->m_priority == rec->m_priority && tMax <= rec->m_tNear))) {
                    continue;
                }
            } else if (tMax <= rec->m_tNear) {
                break;
            }

            if (rec->m_model->m_hitTestMode == 3) {
                best = this->HitTestCollision(rec->m_model, pass, dir, d, start, rec, tMax, best);
            } else {
                best = this->HitTestGeometry(rec->m_model, pass, dir, d, start, rec, tMax, best);
            }
        }

        if (best || !twoPass || ++pass >= 2) {
            break;
        }

        tMax = length;
    }

    this->m_flags &= ~2u;

    if (!best) {
        return NULL;
    }

    t = tMax / length;

    void* owner = NULL;

    for (CM2Model* model = best->m_model; model; model = model->m_attachParent) {
        if (model->m_hitTestOwner != reinterpret_cast<void*>(static_cast<intptr_t>(-1))) {
            owner = model->m_hitTestOwner;
            break;
        }
    }

    this->m_lastHit = *best;
    this->m_lastHitOwner = owner;

    return owner;
}

// OFFSET: 0x00829C70/0x1405484B0
void* CM2Scene::EndHitTestCollisionWorld(const C3Vector& start, const C3Vector& end, float& t) {
    C3Vector dir(0.0f, 0.0f, 0.0f);
    float length;

    if (!this->ComputeRayDirAndLen(start, end, t, &length, &dir)) {
        return NULL;
    }

    this->AllocateSpaceForHitList();

    C3Vector viewStart = start * this->m_view;
    C33Matrix rotation(this->m_view);
    C3Vector viewDir(rotation.a0 * dir.x + rotation.b0 * dir.y + rotation.c0 * dir.z, rotation.c1 * dir.z + rotation.b1 * dir.y + rotation.a1 * dir.x, dir.y * rotation.b2 + dir.z * rotation.c2 + dir.x * rotation.a2);
    float viewLength = sqrtf(this->m_view.a2 * this->m_view.a2 + this->m_view.a1 * this->m_view.a1 + this->m_view.a0 * this->m_view.a0) * length;

    uint32_t count = this->SphereTestModels(viewStart, viewDir, viewLength, 0);
    M2HeapSort(&CM2Scene::SortHitNear, this->m_hitOrder, count, this->m_hitRecs);

    float tMax = length;

    if (t < 1.0f) {
        tMax = length * t;
    }

    M2HitRec* best = NULL;

    for (uint32_t i = 0; i < count; i++) {
        M2HitRec* rec = &this->m_hitRecs[this->m_hitOrder[i]];

        if (tMax <= rec->m_tNear) {
            break;
        }

        CM2Model* model = rec->m_model;
        M2Data* data = model->m_shared->m_data;

        if (model->m_hitTestMode != 3) {
            continue;
        }

        C44Matrix inverse = model->m_worldTransform.Inverse(model->m_worldTransform.Determinant());
        C3Vector localStart = start * inverse;
        C3Vector localEnd = end * inverse;

        C3Ray ray;
        float dx = localEnd.x - localStart.x;
        float dy = localEnd.y - localStart.y;
        float dz = localEnd.z - localStart.z;
        float invLength = 1.0f / sqrtf(dy * dy + dz * dz + dx * dx);
        ray.origin = localStart;
        ray.dir.x = dx * invLength;
        ray.dir.y = dy * invLength;
        ray.dir.z = invLength * dz;

        const uint16_t* indices = data->m_collisionIndices.Data();
        const uint16_t* indicesEnd = indices + data->m_collisionIndices.m_count;

        for (; indices < indicesEnd; indices += 3) {
            float hitT;

            if (NTempest::Intersect(ray, data->m_collisionPositions.Data(), indices, &hitT, NULL, 0.000001f) && hitT >= 0.0f) {
                float dist = hitT * invLength * length;

                if (tMax >= dist) {
                    tMax = dist;
                    best = rec;
                }
            }
        }
    }

    this->m_flags &= ~2u;

    if (!best) {
        return NULL;
    }

    t = tMax / length;

    void* owner = NULL;

    for (CM2Model* model = best->m_model; model; model = model->m_attachParent) {
        if (model->m_hitTestOwner != reinterpret_cast<void*>(static_cast<intptr_t>(-1))) {
            owner = model->m_hitTestOwner;
            break;
        }
    }

    this->m_lastHit = *best;
    this->m_lastHitOwner = owner;

    return owner;
}

// PROVEN: a8182422 IDENTICAL/DIFFERENT
// OFFSET: 0x008279C0/0x1405459F0
void CM2Scene::SelectLights(CM2Lighting* lighting) {
    for (CM2Light* light = this->m_lightList; light; light = light->m_lightNext) {
        lighting->AddLight(light);
    }

    if (!this->m_lightGrid) {
        return;
    }

    const CAaSphere& sphere = lighting->m_sphere4;
    uint32_t maxX = static_cast<int32_t>(floorf((sphere.c.x + sphere.r) * s_lightGridScale + 0.5f)) & 0x1F;
    uint32_t minY = static_cast<int32_t>(floorf((sphere.c.y - sphere.r) * s_lightGridScale - 0.5f)) & 0x1F;
    uint32_t maxY = static_cast<int32_t>(floorf((sphere.r + sphere.c.y) * s_lightGridScale + 0.5f)) & 0x1F;
    uint32_t minX = static_cast<int32_t>(floorf((sphere.c.x - sphere.r) * s_lightGridScale - 0.5f)) & 0x1F;

    for (uint32_t x = minX; ; x = (x + 1) & 0x1F) {
        for (uint32_t y = minY; ; y = (y + 1) & 0x1F) {
            CM2Light* light = this->m_lightGrid[x + 32 * y];

            while (light) {
                CM2Light* next = light->m_lightNext;

                if (!light->m_scene || light->m_stamp == this->m_frameStamp) {
                    lighting->AddLight(light);
                } else {
                    light->SetVisible(0);
                }

                light = next;
            }

            if (y == maxY) {
                break;
            }
        }

        if (x == maxX) {
            break;
        }
    }
}

// PROVEN: 39cefc36 IDENTICAL/DIFFERENT
// OFFSET: 0x00827B70/0x140545C30
void CM2Scene::SetProjectPositionCallback(int32_t (*callback)(const C3Vector&, float&, void*), void* param) {
    this->m_projectPositionCallback = callback;
    this->m_projectPositionParam = param;
    ParticleSystemManager::sm_projectCallback = callback;
    ParticleSystemManager::sm_projectParam = param;
}

// OFFSET: 0x00827CA0/0x140545D60
int32_t CM2Scene::SortDoodad(uint32_t a, uint32_t b, const void* param) {
    const M2Element* elements = static_cast<const M2Element*>(param);
    const M2Element* elementA = &elements[a];
    const M2Element* elementB = &elements[b];
    const CM2Model* modelA = elementA->m_model;
    const CM2Model* modelB = elementB->m_model;

    if (modelA->m_shared < modelB->m_shared) {
        return -1;
    }

    if (modelA->m_shared > modelB->m_shared) {
        return 1;
    }

    if (static_cast<uint32_t>(elementA->m_index) < static_cast<uint32_t>(elementB->m_index)) {
        return -1;
    }

    if (static_cast<uint32_t>(elementA->m_index) > static_cast<uint32_t>(elementB->m_index)) {
        return 1;
    }

    const CM2Lighting* lightingA = modelA->m_currentLighting;
    const CM2Lighting* lightingB = modelB->m_currentLighting;

    if (lightingA != lightingB) {
        if (lightingB->m_fogStart > lightingA->m_fogStart) {
            return -1;
        }

        if (lightingB->m_fogStart < lightingA->m_fogStart) {
            return 1;
        }

        if (lightingB->m_fogEnd > lightingA->m_fogEnd) {
            return -1;
        }

        if (lightingB->m_fogEnd < lightingA->m_fogEnd) {
            return 1;
        }

        int32_t result = memcmp(&lightingA->m_fogColor, &lightingB->m_fogColor, sizeof(C3Vector));

        if (result < 0) {
            return -1;
        }

        if (result > 0) {
            return 1;
        }

        result = memcmp(&lightingA->m_sunDiffuse, &lightingB->m_sunDiffuse, sizeof(C3Vector));

        if (result < 0) {
            return -1;
        }

        if (result > 0) {
            return 1;
        }

        result = memcmp(&lightingA->m_sunAmbient, &lightingB->m_sunAmbient, sizeof(C3Vector));

        if (result < 0) {
            return -1;
        }

        if (result > 0) {
            return 1;
        }
    }

    uint32_t colorIndex = elementA->m_batch->m_colorIndex;

    if (colorIndex < modelA->m_shared->m_data->m_colors.m_count) {
        int32_t result = memcmp(&modelA->m_colors[colorIndex].m_colorTrack.m_currentValue, &modelB->m_colors[colorIndex].m_colorTrack.m_currentValue, sizeof(C3Vector));

        if (result < 0) {
            return -1;
        }

        if (result > 0) {
            return 1;
        }
    }

    int32_t result = memcmp(&modelA->m_diffuseColor, &modelB->m_diffuseColor, sizeof(C3Vector));

    if (result < 0) {
        return -1;
    }

    if (result > 0) {
        return 1;
    }

    result = memcmp(&modelA->m_emissiveColor, &modelB->m_emissiveColor, sizeof(C3Vector));

    if (result < 0) {
        return -1;
    }

    if (result > 0) {
        return 1;
    }

    if (modelB->unk_01D0 > modelA->unk_01D0) {
        return -1;
    }

    return modelA->unk_01D0 > modelB->unk_01D0;
}

// OFFSET: 0x00828050/0x140545F80
int32_t CM2Scene::SortDoodadKey(uint32_t a, uint32_t b, const void* param) {
    const M2Element* elements = static_cast<const M2Element*>(param);
    uint32_t keyA = elements[a].m_doodadKey;
    uint32_t keyB = elements[b].m_doodadKey;

    if (keyB > keyA) {
        return -1;
    }

    return keyB < keyA;
}

// PROVEN: dc83df4f IDENTICAL/IDENTICAL
// OFFSET: 0x00828080/0x140545FB0
int32_t CM2Scene::SortSortKey(uint32_t a, uint32_t b, const void* param) {
    const M2Element* elements = static_cast<const M2Element*>(param);

    return elements[a].m_typeKey - elements[b].m_typeKey;
}

// OFFSET: 0x0082A400
int32_t CM2Scene::SortOpaqueRibbons(M2Element* a, M2Element* b) {
    M2Ribbon* ribbonA = &a->m_model->m_shared->m_data->m_ribbons[a->m_index];
    M2Ribbon* ribbonB = &b->m_model->m_shared->m_data->m_ribbons[b->m_index];
    uint32_t count = ribbonA->m_textureIndices.m_count < ribbonB->m_textureIndices.m_count ? ribbonA->m_textureIndices.m_count : ribbonB->m_textureIndices.m_count;

    for (uint32_t i = 0; i < count; i++) {
        CTexture* textureA = a->m_model->m_textures[ribbonA->m_textureIndices[i]];
        CTexture* textureB = b->m_model->m_textures[ribbonB->m_textureIndices[i]];

        if (textureA < textureB) {
            return -1;
        }

        if (textureA > textureB) {
            return 1;
        }
    }

    if (ribbonA->m_textureIndices.m_count < ribbonB->m_textureIndices.m_count) {
        return -1;
    }

    if (ribbonA->m_textureIndices.m_count > ribbonB->m_textureIndices.m_count) {
        return 1;
    }

    if (static_cast<uint32_t>(b->m_index) > static_cast<uint32_t>(a->m_index)) {
        return -1;
    }

    return static_cast<uint32_t>(b->m_index) < static_cast<uint32_t>(a->m_index);
}

// OFFSET: 0x008288E0
int32_t CM2Scene::SortOpaqueParticles(M2Element* a, M2Element* b) {
    CParticleEmitter2* emitterA = a->m_emitter;
    CParticleEmitter2* emitterB = b->m_emitter;
    int32_t blendA = emitterA->m_params.m_materialBlend;
    int32_t blendB = emitterB->m_params.m_materialBlend;

    if (blendA < blendB) {
        return -1;
    }

    if (blendA > blendB) {
        return 1;
    }

    uint32_t flagsA = ParticleFlagsToM2Flags(emitterA->m_params.m_materialFlags);
    uint32_t flagsB = ParticleFlagsToM2Flags(emitterB->m_params.m_materialFlags);

    if (flagsA < flagsB) {
        return -1;
    }

    return flagsA > flagsB || emitterA->m_texture < emitterB->m_texture;
}

// OFFSET: 0x0082A1A0
int32_t CM2Scene::SortOpaqueGeoBatches(M2Element* a, M2Element* b) {
    M2Data* dataA = a->m_model->m_shared->m_data;
    M2Batch* batchA = a->m_batch;
    M2Data* dataB = b->m_model->m_shared->m_data;
    M2Batch* batchB = b->m_batch;

    if ((a->m_typeKey & 0xE0000000) == 0x20000000) {
        if (batchA->m_materialLayer < batchB->m_materialLayer) {
            return -1;
        }

        if (batchA->m_materialLayer > batchB->m_materialLayer) {
            return 1;
        }

        if (a->m_effect && b->m_effect) {
            CGxShaderPermute* vertexA = a->m_effect->GetVertexPermute(a->m_vertexPermute);
            CGxShaderPermute* vertexB = b->m_effect->GetVertexPermute(b->m_vertexPermute);
            CGxShaderPermute* pixelA = a->m_effect->GetPixelPermute(a->m_pixelPermute);
            CGxShaderPermute* pixelB = b->m_effect->GetPixelPermute(b->m_pixelPermute);

            if (vertexA < vertexB) {
                return -1;
            }

            if (vertexA > vertexB) {
                return 1;
            }

            if (pixelA < pixelB) {
                return -1;
            }

            if (pixelA > pixelB) {
                return 1;
            }
        }

        if (a->m_model->m_shared < b->m_model->m_shared) {
            return -1;
        }

        if (a->m_model->m_shared > b->m_model->m_shared) {
            return 1;
        }

        int32_t optimizedA = a->m_flags & 4;
        int32_t optimizedB = b->m_flags & 4;

        if (optimizedA < optimizedB) {
            return -1;
        }

        if (optimizedA > optimizedB) {
            return 1;
        }

        if (a->m_model < b->m_model) {
            return -1;
        }

        if (a->m_model > b->m_model) {
            return 1;
        }

        if (a->m_skinSection->m_boneComboIndex < b->m_skinSection->m_boneComboIndex) {
            return -1;
        }

        if (a->m_skinSection->m_boneComboIndex > b->m_skinSection->m_boneComboIndex) {
            return 1;
        }
    }

    M2Material* materialA = &dataA->m_materials[batchA->m_materialIndex];
    M2Material* materialB = &dataB->m_materials[batchB->m_materialIndex];

    if (materialA->m_blendMode < materialB->m_blendMode) {
        return -1;
    }

    if (materialA->m_blendMode > materialB->m_blendMode) {
        return 1;
    }

    uint32_t flagsA = materialA->m_flags & 0x1F;
    uint32_t flagsB = materialB->m_flags & 0x1F;

    if (flagsA < flagsB) {
        return -1;
    }

    if (flagsA > flagsB) {
        return 1;
    }

    uint16_t count = batchA->m_textureCount < batchB->m_textureCount ? batchA->m_textureCount : batchB->m_textureCount;

    for (uint32_t i = 0; i < count; i++) {
        uint32_t comboA = dataA->m_textureCombos[batchA->m_textureComboIndex + i];
        uint32_t comboB = dataB->m_textureCombos[batchB->m_textureComboIndex + i];
        CTexture* textureA = comboA >= dataA->m_textures.m_count ? NULL : a->m_model->m_textures[comboA];
        CTexture* textureB = comboB >= dataB->m_textures.m_count ? NULL : b->m_model->m_textures[comboB];

        if (textureA < textureB) {
            return -1;
        }

        if (textureA > textureB) {
            return 1;
        }
    }

    if (batchA->m_textureCount < batchB->m_textureCount) {
        return -1;
    }

    if (batchA->m_textureCount > batchB->m_textureCount) {
        return 1;
    }

    if (batchB > batchA) {
        return -1;
    }

    return batchB < batchA;
}

// OFFSET: 0x0082A4E0
int32_t CM2Scene::SortOpaque(uint32_t a, uint32_t b, const void* param) {
    M2Element* elements = const_cast<M2Element*>(static_cast<const M2Element*>(param));
    M2Element* elementA = &elements[a];
    M2Element* elementB = &elements[b];
    uint32_t typeA = elementA->m_typeKey >> 29;
    uint32_t typeB = elementB->m_typeKey >> 29;

    if (typeA < typeB) {
        return -1;
    }

    if (typeA > typeB) {
        return 1;
    }

    switch (typeA) {
        case 0:
        case 1:
            return CM2Scene::SortOpaqueGeoBatches(elementA, elementB);
        case 3:
            return CM2Scene::SortOpaqueRibbons(elementA, elementB);
        case 4:
            return CM2Scene::SortOpaqueParticles(elementA, elementB);
        default:
            return 0;
    }
}

// OFFSET: 0x0082A570
int32_t CM2Scene::SortTransparent(uint32_t a, uint32_t b, const void* param) {
    M2Element* elements = const_cast<M2Element*>(static_cast<const M2Element*>(param));
    M2Element* elementA = &elements[a];
    M2Element* elementB = &elements[b];
    uint32_t typeA = elementA->m_typeKey >> 29;

    if (!typeA && (elementB->m_typeKey & 0xE0000000)) {
        return -1;
    }

    uint32_t typeB = elementB->m_typeKey >> 29;

    if (!typeB && typeA) {
        return 1;
    }

    if (elementA->m_sortDepth > elementB->m_sortDepth) {
        return -1;
    }

    if (elementB->m_sortDepth > elementA->m_sortDepth) {
        return 1;
    }

    if (elementA->m_priorityPlane < elementB->m_priorityPlane) {
        return -1;
    }

    if (elementA->m_priorityPlane > elementB->m_priorityPlane) {
        return 1;
    }

    if (elementA->m_depth > elementB->m_depth) {
        return -1;
    }

    if (elementB->m_depth > elementA->m_depth) {
        return 1;
    }

    uint32_t splitA = elementA->m_flags & 1;
    uint32_t splitB = elementB->m_flags & 1;

    if (splitA > splitB) {
        return -1;
    }

    if (splitA < splitB) {
        return 1;
    }

    uint32_t litA = elementA->m_flags & 8;
    uint32_t litB = elementB->m_flags & 8;

    if (litA > litB) {
        return -1;
    }

    if (litA < litB) {
        return 1;
    }

    uint32_t shaderSort = CM2Scene::s_optFlags & 0x4000;

    if (shaderSort && (typeA != typeB || elementA->m_model != elementB->m_model) && elementA->m_effect && elementB->m_effect) {
        CGxShaderPermute* vertexA = elementA->m_effect->GetVertexPermute(elementA->m_vertexPermute);
        CGxShaderPermute* vertexB = elementB->m_effect->GetVertexPermute(elementB->m_vertexPermute);
        CGxShaderPermute* pixelA = elementA->m_effect->GetPixelPermute(elementA->m_pixelPermute);
        CGxShaderPermute* pixelB = elementB->m_effect->GetPixelPermute(elementB->m_pixelPermute);

        if (vertexA < vertexB) {
            return -1;
        }

        if (vertexA > vertexB) {
            return 1;
        }

        if (pixelA < pixelB) {
            return -1;
        }

        if (pixelA > pixelB) {
            return 1;
        }
    }

    if (elementA->m_model < elementB->m_model) {
        return -1;
    }

    if (elementA->m_model > elementB->m_model) {
        return 1;
    }

    if (typeA < typeB) {
        return -1;
    }

    if (typeA > typeB) {
        return 1;
    }

    if (typeA <= 2) {
        if (elementA->m_batch->m_materialLayer < elementB->m_batch->m_materialLayer) {
            return -1;
        }

        if (elementA->m_batch->m_materialLayer > elementB->m_batch->m_materialLayer) {
            return 1;
        }
    }

    if (!shaderSort || !elementA->m_effect || !elementB->m_effect) {
        return CM2Scene::SortOpaque(a, b, param);
    }

    CGxShaderPermute* vertexA = elementA->m_effect->GetVertexPermute(elementA->m_vertexPermute);
    CGxShaderPermute* vertexB = elementB->m_effect->GetVertexPermute(elementB->m_vertexPermute);
    CGxShaderPermute* pixelA = elementA->m_effect->GetPixelPermute(elementA->m_pixelPermute);
    CGxShaderPermute* pixelB = elementB->m_effect->GetPixelPermute(elementB->m_pixelPermute);

    if (vertexA < vertexB) {
        return -1;
    }

    if (vertexA > vertexB) {
        return 1;
    }

    if (pixelA < pixelB) {
        return -1;
    }

    if (pixelA > pixelB) {
        return 1;
    }

    return CM2Scene::SortOpaque(a, b, param);
}

// OFFSET: 0x0082A780
int32_t CM2Scene::SortAdditiveParticles(uint32_t a, uint32_t b, const void* param) {
    M2Element* elements = const_cast<M2Element*>(static_cast<const M2Element*>(param));
    M2Element* elementA = &elements[a];
    M2Element* elementB = &elements[b];

    if (elementA->m_sortKey > elementB->m_sortKey) {
        return 1;
    }

    if (elementA->m_sortKey < elementB->m_sortKey) {
        return -1;
    }

    uint32_t typeA = elementA->m_typeKey >> 29;

    if (typeA == 4 || (elementB->m_typeKey & 0xE0000000) == 0x80000000) {
        uint32_t typeB = elementB->m_typeKey >> 29;

        if (typeA > typeB) {
            return -1;
        }

        if (typeA < typeB) {
            return 1;
        }
    }

    if (typeA != 4) {
        return CM2Scene::SortTransparent(a, b, param);
    }

    CParticleEmitter2* emitterA = elementA->m_emitter;
    CParticleEmitter2* emitterB = elementB->m_emitter;
    int32_t blendA = emitterA->m_params.m_materialBlend;
    int32_t blendB = emitterB->m_params.m_materialBlend;

    if (blendA < blendB) {
        return -1;
    }

    if (blendA > blendB) {
        return 1;
    }

    uint32_t flagsA = ParticleFlagsToM2Flags(emitterA->m_params.m_materialFlags);
    uint32_t flagsB = ParticleFlagsToM2Flags(emitterB->m_params.m_materialFlags);

    if (flagsA < flagsB) {
        return -1;
    }

    if (flagsA > flagsB) {
        return 1;
    }

    return emitterA->m_texture < emitterB->m_texture;
}

// OFFSET: 0x0082A870/0x140549530
void CM2Scene::ComputeElementShaders(M2Element* element) {
    CM2Model* model = element->m_model;
    CM2Lighting* lighting = model->m_currentLighting;
    M2Material* material = &model->m_shared->m_data->m_materials[element->m_batch->m_materialIndex];
    uint32_t unlit = material->m_flags & 1;
    int32_t specular;
    uint32_t lightCount;

    if (unlit || !s_localLighting[material->m_blendMode]) {
        specular = 0;
        lightCount = unlit ? 0 : lighting->m_lightCount;
    } else {
        specular = 1;
        lightCount = lighting->m_lightCount;
    }

    uint32_t type = element->m_typeKey >> 29;
    uint32_t boneInfluences = element->m_skinSection->m_boneInfluences;

    if (type == 2 && !boneInfluences) {
        boneInfluences = 1;
    }

    int32_t alphaTest;

    if (!material->m_blendMode) {
        alphaTest = 0;
    } else if (material->m_blendMode == 1) {
        alphaTest = static_cast<int32_t>(static_cast<int64_t>(static_cast<double>(CGxDevice::s_alphaRef[GxBlend_AlphaKey]) * element->m_alpha));
    } else {
        alphaTest = 1;
    }

    uint32_t shadow = 0;

    if (!unlit && (lighting->m_flags & 0x10)) {
        if (material->m_flags & 0x100) {
            shadow = 0;
        } else {
            shadow = this->m_shadowValue;

            if (shadow) {
                if (lighting->m_flags & 8) {
                    shadow = 1;
                }

                if (type == 0) {
                    shadow = 0;
                }
            }
        }
    }

    int32_t useAlphaTest = alphaTest && (!g_theGxDevicePtr->Caps().unk_02F4 || (!CShaderEffect::s_forceAlphaTest && shadow));
    uint32_t shadowIndex = shadow > 1 ? 1 : shadow;
    uint32_t boneIndex = boneInfluences > 2 ? 2 : boneInfluences;
    uint32_t lights = specular ? (lightCount > 2 ? 2 : lightCount) + 1 : 0;

    element->m_vertexPermute = ((element->m_flags & 2) != 0) + 4 * (lights + 4 * (shadowIndex + boneIndex + 2 * shadowIndex));
    element->m_pixelPermute = shadow + 4 * (CShaderEffect::s_usePcfFiltering + 2 * useAlphaTest);
    element->m_hullPermute = 0xFFFFFFFF;
    element->m_domainPermute = 0xFFFFFFFF;
    element->m_shadowValue = shadow;

    uintptr_t vertex = reinterpret_cast<uintptr_t>(element->m_effect->GetVertexPermute(element->m_vertexPermute));
    uintptr_t pixel = reinterpret_cast<uintptr_t>(element->m_effect->GetPixelPermute(element->m_pixelPermute));
    uint32_t hash = static_cast<uint32_t>(vertex ^ ((vertex >> 2) + pixel + (vertex << 6) - 29918791));
    element->m_typeKey ^= (element->m_typeKey ^ hash) & 0x3FFFFFF;
}

// PROVEN: 0c827297 IDENTICAL/NONE
// OFFSET: 0x00827450
void M2PreloadRecord::Set(CM2Scene* scene, const char* fileName, const uint64_t& key, TSGrowableArray<CAsyncObject*>* waitList) {
    this->m_scene = scene;
    this->m_key = key;
    this->m_waitList = waitList;
    SStrCopy(this->m_fileName, fileName, sizeof(this->m_fileName));
}

// OFFSET: 0x00828260
void M2PreloadMap::Iterator::Next() {
    if (this->m_node && (this->m_node = this->m_node->m_next)) {
        return;
    }

    for (this->m_bucket++; this->m_bucket < this->m_end; this->m_bucket++) {
        this->m_node = *this->m_bucket;

        if (this->m_node) {
            break;
        }
    }
}

// INLINED
M2PreloadMap::M2PreloadMap() {
    this->m_bucketCount = 8;
    this->m_count = 0;
    this->m_buckets = static_cast<Node**>(Blizzard::Memory::s_allocHook(sizeof(Node*) * 8));

    for (uint32_t i = 0; i < 8; i++) {
        this->m_buckets[i] = NULL;
    }

    this->m_maxLoadFactor = 1.0f;
}

// PROVEN: 69d486a2 IDENTICAL/NONE
// OFFSET: 0x00829100/0x140547390
M2PreloadMap::~M2PreloadMap() {
    for (uint32_t i = 0; i < this->m_bucketCount; i++) {
        for (Node* node = this->m_buckets[i]; node; ) {
            Node* next = node->m_next;
            Blizzard::Memory::s_freeHook(node, sizeof(Node));
            node = next;
        }
    }

    Blizzard::Memory::s_freeHook(this->m_buckets, sizeof(Node*) * this->m_bucketCount);
}

// INLINED
M2PreloadMap::Iterator M2PreloadMap::Begin() {
    Iterator it;
    it.m_node = *this->m_buckets;
    it.m_bucket = this->m_buckets;
    it.m_end = &this->m_buckets[this->m_bucketCount];

    if (!it.m_node) {
        it.Next();
    }

    return it;
}

// INLINED
M2PreloadMap::Iterator M2PreloadMap::End() {
    Iterator it;
    it.m_node = NULL;
    it.m_bucket = &this->m_buckets[this->m_bucketCount];
    it.m_end = it.m_bucket;

    return it;
}

// OFFSET: 0x0082C9E0
M2PreloadMap::Iterator M2PreloadMap::Erase(Iterator where) {
    Node** link = where.m_bucket;

    for (Node* node = *link; node; link = &node->m_next, node = node->m_next) {
        if (node != where.m_node) {
            continue;
        }

        Iterator next;
        next.m_node = node;
        next.m_bucket = where.m_bucket;
        next.m_end = &this->m_buckets[this->m_bucketCount];
        next.Next();

        Node* following = node->m_next;
        Blizzard::Memory::s_freeHook(node, sizeof(Node));
        *link = following;
        this->m_count--;

        return next;
    }

    return this->End();
}

// OFFSET: 0x0082D230
int32_t M2PreloadMap::Erase(const uint64_t& key) {
    Node** link = &this->m_buckets[static_cast<uint32_t>(key) & (this->m_bucketCount - 1)];

    for (Node* node = *link; node; link = &node->m_next, node = node->m_next) {
        if (node->m_key != key) {
            continue;
        }

        Node* next = node->m_next;
        Blizzard::Memory::s_freeHook(node, sizeof(Node));
        *link = next;
        this->m_count--;

        return 1;
    }

    return 0;
}

// OFFSET: 0x0082CA80
M2PreloadMap::Iterator M2PreloadMap::Find(const uint64_t& key) {
    Node** bucket = &this->m_buckets[static_cast<uint32_t>(key) & (this->m_bucketCount - 1)];

    for (Node* node = *bucket; node; node = node->m_next) {
        if (node->m_key == key) {
            Iterator it;
            it.m_node = node;
            it.m_end = &this->m_buckets[this->m_bucketCount];
            it.m_bucket = bucket;

            return it;
        }
    }

    return this->End();
}

// OFFSET: 0x0082D120
M2PreloadMap::InsertResult M2PreloadMap::Insert(const Value& value) {
    Node** bucket = &this->m_buckets[static_cast<uint32_t>(value.m_key) & (this->m_bucketCount - 1)];
    Node** link = bucket;
    InsertResult result;

    for (Node* node = *bucket; node; link = &node->m_next, node = node->m_next) {
        if (node->m_key == value.m_key) {
            result.m_where.m_node = node;
            result.m_where.m_bucket = bucket;
            result.m_inserted = false;
            result.m_where.m_end = &this->m_buckets[this->m_bucketCount];

            return result;
        }
    }

    Node* node = static_cast<Node*>(Blizzard::Memory::s_allocHook(sizeof(Node)));

    if (node) {
        node->m_next = NULL;
        node->m_key = value.m_key;
        node->m_record = value.m_record;
    }

    *link = node;
    this->m_count++;

    if (static_cast<double>(this->m_count) / static_cast<double>(this->m_bucketCount) > this->m_maxLoadFactor) {
        this->Rehash(2 * this->m_bucketCount);
        bucket = &this->m_buckets[static_cast<uint32_t>(value.m_key) & (this->m_bucketCount - 1)];
    }

    result.m_where.m_node = node;
    result.m_where.m_bucket = bucket;
    result.m_inserted = true;
    result.m_where.m_end = &this->m_buckets[this->m_bucketCount];

    return result;
}

// OFFSET: 0x00829160
void M2PreloadMap::Rehash(uint32_t bucketCount) {
    uint32_t count = 8;

    while (count < bucketCount) {
        count *= 2;
    }

    Node** buckets = static_cast<Node**>(Blizzard::Memory::s_allocHook(sizeof(Node*) * count));

    if (count) {
        memset(buckets, 0, sizeof(Node*) * count);
    }

    for (uint32_t i = 0; i < this->m_bucketCount; i++) {
        for (Node* node = this->m_buckets[i]; node; ) {
            Node* next = node->m_next;
            uint32_t index = static_cast<uint32_t>(node->m_key) & (count - 1);
            node->m_next = buckets[index];
            buckets[index] = node;
            node = next;
        }
    }

    Blizzard::Memory::s_freeHook(this->m_buckets, sizeof(Node*) * this->m_bucketCount);
    this->m_bucketCount = count;
    this->m_buckets = buckets;
}

// OFFSET: 0x008283D0
uint64_t M2HashFileName(const char* fileName, uint32_t length) {
    uint64_t hash = 0xCBF29CE484222325ULL;

    for (const uint8_t* c = reinterpret_cast<const uint8_t*>(fileName); length; length--, c++) {
        hash = *c ^ (0x100000001B3ULL * hash);
    }

    return hash;
}

// OFFSET: 0x008274B0
int32_t CM2Scene::GetSkinProfileName(uint32_t numSkinProfiles, uint32_t useShaders, const char* fileName, char* result, uint32_t size) {
    uint32_t profile = 0;

    if (useShaders) {
        profile = CM2Shared::GetSkinProfileIndex(numSkinProfiles);

        if (profile == 0xFFFFFFFF) {
            return 0;
        }
    }

    SStrCopy(result, fileName, size);
    *const_cast<char*>(OsPathFindExtensionWithDot(result)) = '\0';
    uint32_t length = strlen(result);
    SStrPrintf(&result[length], size - length, "%02d.skin", profile);

    return 1;
}

// OFFSET: 0x0082D4A0
int32_t CM2Scene::AsyncLoad(const char* fileName, uint8_t priority, int32_t allocBuffer, TSGrowableArray<CAsyncObject*>* waitList, void (*succeeded)(CAsyncObject*), void (*failed)(CAsyncObject*), CM2Scene* scene) {
    SFileImpl* file;

    if (!SFile::OpenEx(NULL, fileName, 0, &file)) {
        return 0;
    }

    void* buffer = NULL;

    if (allocBuffer) {
        buffer = SIMD::AllocA16(SFileGetFileSize(file, NULL));
    }

    uint64_t key = M2HashFileName(fileName, strlen(fileName));
    M2PreloadMap::Iterator it = scene->m_preloads.Find(key);

    if (it.m_node || it.m_bucket != &scene->m_preloads.m_buckets[scene->m_preloads.m_bucketCount]) {
        if (buffer) {
            SIMD::FreeA16(buffer);
        }

        if (waitList && !it.m_node->m_record.m_waitList) {
            waitList->Add(1, &it.m_node->m_record.m_object);
            it.m_node->m_record.m_waitList = waitList;
        }

        SFileCloseFile(file);

        return 1;
    }

    M2PreloadMap::Value value;
    memset(&value.m_record, 0, sizeof(value.m_record));
    value.m_key = key;
    M2PreloadRecord* record = &scene->m_preloads.Insert(value).m_where.m_node->m_record;
    record->Set(scene, fileName, key, waitList);
    AsyncFileReadCreateObject(file, priority, 1, &record->m_object, buffer, record, succeeded, failed, 0, 0, 0);

    if (!record->m_object) {
        scene->m_preloads.Erase(key);
        return 0;
    }

    if (waitList) {
        waitList->Add(1, &record->m_object);
    }

    return 1;
}

// OFFSET: 0x0082D650/0x14054D300
void CM2Scene::PreloadCommonCallback(CAsyncObject* object) {
    if (object->m_buffer) {
        SIMD::FreeA16(object->m_buffer);
    }

    M2PreloadRecord* record = static_cast<M2PreloadRecord*>(object->m_userArg);

    if (record->m_waitList) {
        for (uint32_t i = 0; i < record->m_waitList->Count(); i++) {
            if ((*record->m_waitList)[i] == object) {
                (*record->m_waitList)[i] = NULL;
            }
        }
    }

    object->m_buffer = NULL;
    object->m_userArg = NULL;
    DefaultAsyncObjectCleanupCallback(object);
    record->m_scene->m_preloads.Erase(record->m_key);
}

// OFFSET: 0x0082D6D0
void CM2Scene::ModelLoadSucceededCallback(CAsyncObject* object) {
    M2Data* data = static_cast<M2Data*>(object->m_buffer);
    M2PreloadRecord* record = static_cast<M2PreloadRecord*>(object->m_userArg);
    char name[260];

    for (uint32_t i = 0; i < data->m_textures.m_count; i++) {
        const M2Texture* texture = reinterpret_cast<const M2Texture*>(reinterpret_cast<const uint8_t*>(data) + data->m_textures.m_offset) + i;
        SStrCopy(name, reinterpret_cast<const char*>(data) + texture->m_filename.m_offset, sizeof(name));
        _strlwr(name);

        if (name[0]) {
            CM2Scene::AsyncLoad(name, object->m_priority, 0, NULL, CM2Scene::PreloadCommonCallback, CM2Scene::PreloadCommonCallback, record->m_scene);
        }
    }

    if (CM2Scene::GetSkinProfileName(data->m_numSkinProfiles, record->m_scene->m_cache->m_flags & 8, record->m_fileName, name, sizeof(name))) {
        CM2Scene::AsyncLoad(name, object->m_priority, 0, record->m_waitList, CM2Scene::PreloadCommonCallback, CM2Scene::PreloadCommonCallback, record->m_scene);
    }

    CM2Scene::PreloadCommonCallback(object);
}

// OFFSET: 0x0082D800/0x14054D5D0
void CM2Scene::PreloadModel(const char* fileName, uint8_t priority, int32_t wait) {
    if (!fileName) {
        return;
    }

    CM2Shared* shared = this->m_cache->CreateShared(fileName, 0x100, priority);

    if (shared) {
        if (wait) {
            if (shared->m_asyncObject) {
                AsyncFileReadWait(shared->m_asyncObject);
            }

            if (shared->m_asyncObject) {
                AsyncFileReadWait(shared->m_asyncObject);
            }
        }

        shared->Release();

        return;
    }

    char path[260];

    if (!M2ConvertModelFileName(fileName, path, sizeof(path), 0)) {
        return;
    }

    TSGrowableArray<CAsyncObject*> waitList;

    if (CM2Scene::AsyncLoad(path, priority, 1, wait ? &waitList : NULL, CM2Scene::ModelLoadSucceededCallback, CM2Scene::PreloadCommonCallback, this) && wait) {
        for (uint32_t i = 0; i < waitList.Count(); i++) {
            if (waitList[i]) {
                AsyncFileReadWait(waitList[i]);
            }
        }

        waitList.SetCount(0);
    }
}

// OFFSET: 0x00830F00/0x140552380
int32_t CM2Scene::PrefetchModel(const char* fileName, uint8_t priority) {
    SFileImpl* file = NULL;
    int32_t local = 1;

    if (SFile::FileIsLocal(fileName) != 1 || !SFile::OpenEx(NULL, fileName, 0, &file)) {
        this->PreloadModel(fileName, priority, 0);
        return 0;
    }

    M2Data header;
    SFile::ReadAtPosition(file, &header, 0, sizeof(header), NULL);
    M2Texture texture;

    for (uint32_t i = 0; i < header.m_textures.m_count; i++) {
        char name[260];
        SFile::ReadAtPosition(file, &texture, header.m_textures.m_offset + sizeof(M2Texture) * i, sizeof(texture), NULL);
        SFile::ReadAtPosition(file, name, texture.m_filename.m_offset, texture.m_filename.m_count, NULL);

        if (name[0] && SFile::FileIsLocal(name) != 1) {
            AsyncFileReadCreateObject(name, priority, 0, 0, NULL, NULL, NULL, DefaultAsyncObjectCleanupCallback, DefaultAsyncObjectCleanupCallback, 0, 0, 0);
            local = 0;
        }
    }

    memset(&header, 0, sizeof(header));
    memset(&texture, 0, sizeof(texture));
    SFileCloseFile(file);

    return local;
}

// OFFSET: 0x0082CF00/0x14054C7E0
CM2Model* CM2Scene::CreateModel(const char* fileName, uint32_t flags, uint8_t priority) {
    if (!fileName) {
        return NULL;
    }

    CM2Shared* shared = this->m_cache->CreateShared(fileName, flags, priority);
    CM2Model* model = NULL;

    if (shared) {
        model = g_modelPool->New(__FILE__, __LINE__);

        if (model && !model->Initialize(this, shared, NULL, flags)) {
            g_modelPool->Delete(model, __FILE__, __LINE__);
            model = NULL;
        }

        shared->Release();
    }

    return model;
}

// OFFSET: 0x0082CF80/0x14054C880
CM2Model* CM2Scene::DuplicateModel(CM2Model* source, uint32_t flags) {
    if (!source) {
        return NULL;
    }

    CM2Model* model = g_modelPool->New(__FILE__, __LINE__);

    if (model && !model->Initialize(this, source->m_shared, source, flags)) {
        g_modelPool->Delete(model, __FILE__, __LINE__);
        return NULL;
    }

    return model;
}

// OFFSET: 0x0082CEB0/0x14054C760
void CM2Scene::GarbageCollectModels() {
    while (this->m_recycleList) {
        g_modelPool->Delete(this->m_recycleList, __FILE__, __LINE__);
    }
}

// OFFSET: 0x0082CBF0/0x14054C470
CM2Scene::CM2Scene(CM2Cache* cache) {
    this->m_refCount = 1;
    this->m_float18 = 0.0f;
    this->m_scenePrev = NULL;
    this->m_sceneNext = NULL;
    this->m_modelList = NULL;
    this->m_recycleList = NULL;
    this->m_time = 0;
    this->m_frameStamp = 0;
    this->m_flags = 0;
    this->m_lightList = NULL;
    this->m_lightGrid = NULL;
    this->m_animateList = NULL;
    this->m_drawList = NULL;
    this->m_particleList = NULL;
    this->m_cache = cache;
    this->m_projectTextureCallback = NULL;
    this->m_projectPositionCallback = NULL;
    this->m_hitTestList = NULL;
    this->m_hitRecs = NULL;
    this->m_hitOrder = NULL;
    this->m_hitCapacity = 0;
    this->m_hitVerts = NULL;
    this->m_hitVertCapacity = 0;
    this->m_liquidTypeId = 0;
    this->m_shadowValue = 0;
    this->m_litSphereTexture = NULL;
    this->m_litSphereEffect = NULL;
    CM2Scene::s_optFlags = 0;
    this->m_passMask = 0xFFFFFFFF;
    this->m_drawing = 0;
}

// OFFSET: 0x0082D290/0x14054CCF0
CM2Scene::~CM2Scene() {
    while (this->m_modelList) {
        CM2Model* model = this->m_modelList;

        while (model->Release()) {
        }
    }

    if (this->m_preloads.m_count) {
        for (M2PreloadMap::Iterator it = this->m_preloads.Begin(); it.m_node || it.m_bucket != &this->m_preloads.m_buckets[this->m_preloads.m_bucketCount]; ) {
            CAsyncObject* object = it.m_node->m_record.m_object;

            if (object->m_buffer) {
                SIMD::FreeA16(object->m_buffer);
            }

            AsyncFileReadCancel(object, DefaultAsyncObjectCleanupCallback);
            it = this->m_preloads.Erase(it);
        }
    }

    this->GarbageCollectModels();

    delete[] this->m_lightGrid;
    delete[] this->m_hitRecs;
    delete[] this->m_hitOrder;
    delete[] this->m_hitVerts;

    if (this->m_litSphereTexture) {
        TextureRelease(this->m_litSphereTexture);
        this->m_litSphereTexture = NULL;
    }
}

// OFFSET: 0x0082CFE0
void CM2Scene::SortAdditiveParticleElements(uint32_t pass) {
    TSGrowableArray<uint32_t>& list = this->m_passElements[pass];
    uint32_t group = 0;
    int32_t prevAdditive = 0;

    for (uint32_t i = 0; i < list.m_count; i++) {
        M2Element* element = &this->m_elements.m_data[list.m_data[i]];
        M2Data* data = element->m_model->m_shared->m_data;
        uint32_t blend = 0;

        switch (element->m_typeKey >> 29) {
            case 0:
            case 1:
            case 2:
                blend = data->m_materials[element->m_batch->m_materialIndex].m_blendMode;
                break;
            case 3:
                blend = data->m_materials[data->m_ribbons[element->m_index].m_materialIndices[0]].m_blendMode;
                break;
            case 4:
                blend = ParticleBlendToM2Blend(element->m_emitter->m_params.m_materialBlend);
                break;
            default:
                break;
        }

        uint32_t gxBlend = s_blendModes[1][blend];
        int32_t additive = gxBlend == 3 || gxBlend == 10;

        if (!additive || !prevAdditive) {
            group++;
        }

        element->m_sortKey = group;
        prevAdditive = additive;
    }

    M2HeapSort(&CM2Scene::SortAdditiveParticles, list.m_data, list.m_count, this->m_elements.m_data);
}

// OFFSET: 0x0082D930/0x14054D750
void CM2Scene::AddParticleElement(CParticleEmitter2* emitter, CM2Model* model, float depth, float alpha, int32_t above, int32_t below, uint32_t& index, uint32_t& additiveCount) {
    if (!emitter->HasLiveParticles() || emitter->m_params.m_hasModel == 1) {
        return;
    }

    M2Element* element = this->m_elements.New();

    if (!element) {
        return;
    }

    element->m_typeKey = 0x80000000;
    element->m_model = model;
    element->m_flags = 0;
    element->m_alpha = alpha;
    element->m_emitter = emitter;
    element->m_priorityPlane = emitter->m_priorityPlane;
    element->m_sortDepth = model->m_scaleSq;
    element->m_depth = depth;
    element->m_effect = NULL;
    element->m_vertexPermute = 0xFFFFFFFF;
    element->m_hullPermute = 0xFFFFFFFF;
    element->m_domainPermute = 0xFFFFFFFF;
    element->m_pixelPermute = 0xFFFFFFFF;
    element->m_shadowValue = 0;

    int32_t blend = emitter->m_params.m_materialBlend;

    if (blend == 10 || blend == 3) {
        additiveCount++;
    }

    if (blend <= 1 && alpha >= 0.99998999f) {
        if (model->m_flags & 0x40) {
            this->m_passElements[0].New(index);
        } else {
            this->m_passElements[1].New(index);
        }

        index++;
        return;
    }

    if (!(emitter->m_flags & 0x40000)) {
        if (model->m_fFlags & 0x4000000) {
            this->m_passElements[5].New(index);
            index++;
            return;
        }

        if (above) {
            if (below) {
                element->m_flags |= 2;
            }

            this->m_passElements[2].New(index);
        }

        if (!below) {
            index++;
            return;
        }
    }

    this->m_passElements[3].New(index);
    index++;
}

// OFFSET: 0x00828550/0x1405463E0
void CM2Scene::Animate(const C3Vector& cameraPos) {
    this->m_frameStamp++;

    uint32_t optFlags = this->m_cache->m_flags & 0xE000;

    if (CM2Scene::s_optFlags != optFlags) {
        CM2Scene::s_optFlags = optFlags;
    }

    CGxMatrixStack& viewStack = g_theGxDevicePtr->m_xforms[GxXform_View];
    this->m_view = viewStack.m_mtx[viewStack.m_level];
    C3Vector move(-cameraPos.x, -cameraPos.y, -cameraPos.z);
    this->m_view.Translate(move);
    this->m_viewInv = this->m_view.Inverse(this->m_view.Determinant());

    if (this->m_cache->m_flags & 4) {
        this->m_cache->BeginThread(&AnimateThread, this);

        for (CM2Model* model = this->m_animateList; model; ) {
            if (!model->m_attachParent) {
                if (model->m_fFlags & 0x800) {
                    model->AnimateMTSimple(&this->m_view, C3Vector(1.0f, 1.0f, 1.0f), C3Vector(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, model->m_baseAlpha3);
                } else {
                    model->AnimateMT(&this->m_view, C3Vector(1.0f, 1.0f, 1.0f), C3Vector(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, model->m_baseAlpha3);
                }
            }

            CM2Model* next = model->m_animateNext;

            if (!next) {
                break;
            }

            model = next->m_animateNext;
        }

        this->m_cache->WaitThread();
    } else {
        for (CM2Model* model = this->m_animateList; model; model = model->m_animateNext) {
            if (model->m_attachParent) {
                continue;
            }

            if (model->m_fFlags & 0x800) {
                model->AnimateMTSimple(&this->m_view, C3Vector(1.0f, 1.0f, 1.0f), C3Vector(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, model->m_baseAlpha3);
            } else {
                model->AnimateMT(&this->m_view, C3Vector(1.0f, 1.0f, 1.0f), C3Vector(0.0f, 0.0f, 0.0f), 1.0f, 1.0f, model->m_baseAlpha3);
            }
        }
    }

    for (CM2Model* model = this->m_animateList; model; model = model->m_animateNext) {
        if (!model->m_attachParent) {
            model->AnimateST();
        }
    }

    while (this->m_animateList) {
        CM2Model* model = this->m_animateList;
        this->m_animateList = model->m_animateNext;
        model->m_animatePrev = NULL;
        model->m_animateNext = NULL;
        model->SetupLighting();
    }
}

// OFFSET: 0x0082DA80/0x14054D8C0
int32_t CM2Scene::BeginDraw() {
    this->m_drawing = 1;
    this->m_doodadElements.m_count = 0;

    for (uint32_t i = 0; i < 6; i++) {
        this->m_passElements[i].m_count = 0;
    }

    this->m_elements.m_count = 0;

    uint32_t elementIndex = 0;

    while (this->m_drawList) {
        CM2Model* model = this->m_drawList;
        this->m_drawList = model->m_drawNext;
        model->m_fFlags &= ~0x8008u;
        model->m_drawPrev = NULL;
        model->m_drawNext = NULL;

        if (!model->IsDrawable() || (model->m_fFlags & 0x2000)) {
            continue;
        }

        CM2Lighting* lighting = model->m_currentLighting;
        M2Data* data = model->m_shared->m_data;
        int32_t above = lighting->m_flags & 0x20;
        int32_t below = lighting->m_flags & 0x40;

        if (above && below) {
            C3Vector center((data->m_bounds.m_extent.b.x + data->m_bounds.m_extent.t.x) * 0.5f, (data->m_bounds.m_extent.t.y + data->m_bounds.m_extent.b.y) * 0.5f, (data->m_bounds.m_extent.t.z + data->m_bounds.m_extent.b.z) * 0.5f);
            const C44Matrix& matrix = model->m_matrixF4;
            C3Vector position = center * matrix;
            const C4Plane& plane = lighting->m_liquidPlane;
            float dist = plane.n.z * position.z + plane.n.y * position.y + position.x * plane.n.x + plane.d;
            float radius = sqrtf(matrix.a2 * matrix.a2 + matrix.a1 * matrix.a1 + matrix.a0 * matrix.a0) * data->m_bounds.m_radius;

            above = dist >= -radius;
            below = radius >= dist;

            if (!(this->m_cache->m_flags & 2) && above && below) {
                int32_t submerged = ViewSettings::GetSubmerged();
                above = submerged == 0;
                below = submerged != 0;
            }
        }

        M2ModelOptGeo* optGeo = model->m_optGeo;
        M2SkinProfile* skinData = model->m_shared->m_skinData;
        int32_t optimized = optGeo != NULL;
        int32_t depthSort = (this->m_cache->m_flags & 1) && !(model->m_flags & 1) && (model->m_fFlags & 0x20);
        uint32_t batchCount = optGeo ? optGeo->m_batchCount : skinData->m_batches.m_count;

        for (uint32_t i = 0; i < batchCount; i++) {
            M2Batch* batch;
            M2SkinSection* section;

            if (optimized) {
                batch = &model->m_optGeo->m_batches[i];
                section = &model->m_optGeo->m_skinSections[batch->m_skinSectionIndex];
            } else {
                batch = &skinData->m_batches[i];
                section = &model->m_shared->m_skinSections[batch->m_skinSectionIndex];

                if (!model->m_skinSections[batch->m_skinSectionIndex]) {
                    continue;
                }
            }

            float alpha = model->m_alpha;
            float alpha3 = model->m_alpha3;

            if (batch->m_colorIndex < data->m_colors.m_count) {
                alpha *= model->m_colors[batch->m_colorIndex].m_alphaTrack.m_currentValue;
            }

            if (batch->m_textureCount && !(batch->m_flags & 0x40)) {
                alpha *= model->m_textureWeights[data->m_textureWeightCombos[batch->m_textureWeightComboIndex]].m_currentValue;
            }

            if (alpha < 0.000099999997f || (alpha3 > 0.0f && (batch->m_materialLayer || section->m_indexCount <= 6))) {
                continue;
            }

            M2Material* material = &data->m_materials[batch->m_materialIndex];
            int32_t projected = (batch->m_flags & 4) && this->m_projectTextureCallback;
            M2Material* baseMaterial = batch->m_materialLayer ? &data->m_materials[batch->m_materialIndex - batch->m_materialLayer] : material;
            uint32_t pass;

            if (baseMaterial->m_blendMode > 1 || alpha < 0.99998999f || alpha3 > 0.0000099999997f) {
                pass = 2;
            } else {
                pass = (model->m_flags & 0x40) ? 0 : 1;
            }

            CShaderEffect* effect = optimized ? model->m_optGeo->m_effects[i] : model->m_shared->m_effects[i];

            if (!effect) {
                continue;
            }

            M2Element* element = this->m_elements.New();

            if (!element) {
                return 0;
            }

            uint32_t kind;

            if (projected) {
                kind = 0;
                alpha3 = 0.0f;
            } else if (!model->IsBatchDoodadCompatible(batch) || pass) {
                kind = 1;
            } else {
                kind = 2;
            }

            element->m_typeKey = (batch->m_materialLayer | (8 * kind)) << 26;
            element->m_model = model;
            element->m_flags = 0;

            if (pass == 2 && above && below && !projected) {
                element->m_flags = 2;
            }

            if (optimized) {
                element->m_flags |= 4;
            }

            element->m_index = i;
            element->m_alpha = alpha;
            element->m_priorityPlane = batch->m_priorityPlane;
            element->m_batch = batch;
            element->m_skinSection = section;
            element->m_effect = effect;

            float sortDepth;

            if (model->m_flags & 0x80) {
                pass = 4;
            }

            if (pass < 2) {
                element->m_depth = model->m_scaleSq;
                sortDepth = model->m_scaleSq;
            } else if (data->m_flags & 0x10) {
                C3Vector position = section->m_sortCenterPosition * model->m_boneMatrices[section->m_centerBoneIndex];
                element->m_depth = position.z * position.z + position.y * position.y + position.x * position.x;
                sortDepth = model->m_scaleSq;
            } else {
                const C44Matrix& bone = model->m_boneMatrices[section->m_centerBoneIndex];
                float depth;

                if (batch->m_flags & 3) {
                    C3Vector position = section->m_sortCenterPosition * bone;
                    C3Vector normal = position;
                    normal.NormalizeSafe();
                    float radius = sqrtf(bone.a2 * bone.a2 + bone.a1 * bone.a1 + bone.a0 * bone.a0) * section->m_sortRadius;
                    normal.x = normal.x * radius;
                    normal.y = normal.y * radius;
                    normal.z = radius * normal.z;

                    if (batch->m_flags & 1) {
                        position.x = position.x - normal.x;
                        position.y = position.y - normal.y;
                        position.z = position.z - normal.z;
                    } else {
                        position.x = normal.x + position.x;
                        position.y = normal.y + position.y;
                        position.z = normal.z + position.z;
                    }

                    depth = position.y * position.y + position.z * position.z + position.x * position.x;

                    if (position.z < 0.0f) {
                        depth = -depth;
                    }
                } else {
                    C3Vector position = section->m_sortCenterPosition * bone;
                    depth = position.z * position.z + position.y * position.y + position.x * position.x;
                }

                element->m_depth = depth;

                if (!depthSort || projected || (material->m_flags & 0x10)) {
                    sortDepth = element->m_depth;
                } else {
                    sortDepth = model->m_scaleSq;
                }
            }

            element->m_sortDepth = sortDepth;

            if ((element->m_typeKey & 0xE0000000) == 0x40000000) {
                this->m_doodadElements.New(elementIndex);
            } else if (pass == 2) {
                if (projected) {
                    if (!below) {
                        this->m_passElements[2].New(elementIndex);
                    } else {
                        this->m_passElements[3].New(elementIndex);
                    }
                } else if (model->m_fFlags & 0x4000000) {
                    this->m_passElements[5].New(elementIndex);
                } else {
                    if (above) {
                        this->m_passElements[2].New(elementIndex);
                    }

                    if (below) {
                        this->m_passElements[3].New(elementIndex);
                    }
                }
            } else {
                this->m_passElements[pass].New(elementIndex);
            }

            this->ComputeElementShaders(element);
            elementIndex++;

            if (depthSort && !projected && pass >= 2 && !(material->m_flags & 0x10)) {
                element->m_depth = FLT_MAX;

                M2Element* copy = this->m_elements.New();

                if (!copy) {
                    return 0;
                }

                *copy = this->m_elements.m_data[elementIndex - 1];
                copy->m_flags |= 1;

                if (above) {
                    this->m_passElements[2].New(elementIndex);
                }

                if (below) {
                    this->m_passElements[3].New(elementIndex);
                }

                elementIndex++;
            }

            if (alpha3 > 0.0f) {
                if (alpha3 >= 0.99998999f) {
                    element->m_flags |= 8;
                    element->m_effect = this->m_litSphereEffect;
                } else {
                    element->m_alpha = (1.0f - alpha3) * element->m_alpha;

                    M2Element* copy = this->m_elements.New();

                    if (!copy) {
                        return 0;
                    }

                    *copy = this->m_elements.m_data[elementIndex - 1];
                    copy->m_flags = (copy->m_flags & ~9u) | 8;
                    copy->m_depth = copy->m_depth - 0.0099999998f;
                    copy->m_effect = this->m_litSphereEffect;
                    copy->m_alpha = alpha3;

                    if (above) {
                        this->m_passElements[2].New(elementIndex);
                    }

                    if (below) {
                        this->m_passElements[3].New(elementIndex);
                    }

                    elementIndex++;
                }
            }
        }

        for (uint32_t i = 0; i < data->m_ribbons.m_count; i++) {
            if (model->m_ribbonEmitters[i]->IsDead()) {
                continue;
            }

            M2Ribbon* ribbon = &data->m_ribbons[i];
            float alpha = model->m_effectiveAlpha;

            if (ribbon->m_alphaTrack.m_sequenceTimes.m_count) {
                alpha *= model->m_ribbons[i].m_alphaTrack.m_currentValue;
            }

            M2Material* material = &data->m_materials[ribbon->m_materialIndices[0]];
            M2Element* element = this->m_elements.New();

            if (!element) {
                continue;
            }

            element->m_index = i;
            element->m_typeKey = 0x60000000;
            element->m_model = model;
            element->m_flags = 0;
            element->m_alpha = alpha;
            element->m_priorityPlane = ribbon->m_priorityPlane;
            element->m_sortDepth = model->m_scaleSq;
            element->m_effect = NULL;
            element->m_depth = model->m_scaleSq;
            element->m_vertexPermute = 0xFFFFFFFF;
            element->m_hullPermute = 0xFFFFFFFF;
            element->m_domainPermute = 0xFFFFFFFF;
            element->m_pixelPermute = 0xFFFFFFFF;
            element->m_shadowValue = 0;

            if (material->m_blendMode > 1 || alpha < 0.99998999f) {
                if (above) {
                    this->m_passElements[2].New(elementIndex);
                } else {
                    this->m_passElements[3].New(elementIndex);
                }
            } else {
                this->m_passElements[0].New(elementIndex);
            }

            elementIndex++;
        }

        if (model->m_drawCallback) {
            M2Element* element = this->m_elements.New();

            if (element) {
                element->m_typeKey = 0xA0000000;
                element->m_model = model;
                element->m_flags = 0;
                element->m_alpha = 1.0f;
                element->m_index = 0;
                element->m_priorityPlane = 0;
                element->m_sortDepth = model->m_scaleSq;
                element->m_effect = NULL;
                element->m_depth = model->m_scaleSq;
                element->m_vertexPermute = 0xFFFFFFFF;
                element->m_hullPermute = 0xFFFFFFFF;
                element->m_domainPermute = 0xFFFFFFFF;
                element->m_pixelPermute = 0xFFFFFFFF;
                element->m_shadowValue = 0;

                uint32_t callbackPass = model->m_drawFlags & 7;

                if (!callbackPass) {
                    this->m_passElements[0].New(elementIndex);
                } else if (callbackPass == 5 || (model->m_fFlags & 0x4000000)) {
                    this->m_passElements[5].New(elementIndex);
                } else if (callbackPass == 4) {
                    this->m_passElements[4].New(elementIndex);
                } else if (above) {
                    this->m_passElements[2].New(elementIndex);
                } else {
                    this->m_passElements[3].New(elementIndex);
                }

                elementIndex++;
            }
        }
    }

    uint32_t additiveCount = 0;

    while (this->m_particleList) {
        CM2Model* model = this->m_particleList;
        this->m_particleList = model->m_particleNext;
        model->m_particlePrev = NULL;
        model->m_particleNext = NULL;

        if (!model->IsDrawable() || (model->m_fFlags & 0x2000)) {
            continue;
        }

        M2Data* data = model->m_shared->m_data;
        int32_t below = model->m_currentLighting->m_flags & 0x40;
        int32_t above = model->m_currentLighting->m_flags & 0x20;

        if (!(this->m_cache->m_flags & 2) && above && below) {
            int32_t submerged = ViewSettings::GetSubmerged();
            above = submerged == 0;
            below = submerged != 0;
        }

        for (uint32_t i = 0; i < data->m_particles.m_count; i++) {
            CParticleEmitter2* emitter = model->m_particleEmitters[i];

            if (((model->m_fFlags & 0x1000) && (emitter->m_flags & 0x200)) || (emitter->m_flags & 0x2000000)) {
                continue;
            }

            if (!model->m_particles[i].m_active || model->m_alpha3 > 0.0f) {
                continue;
            }

            float alpha = model->m_effectiveAlpha;

            if (alpha < 0.000099999997f) {
                continue;
            }

            M2Particle* particle = &data->m_particles[i];
            C3Vector position = particle->m_position * model->m_boneMatrices[particle->m_boneIndex];
            float depth = position.z * position.z + position.y * position.y + position.x * position.x;

            this->AddParticleElement(emitter, model, depth, alpha, above, below, elementIndex, additiveCount);

            for (uint32_t j = 0; j < emitter->m_childEmitterCount; j++) {
                this->AddParticleElement(emitter->m_childEmitters[j], model, depth, alpha, above, below, elementIndex, additiveCount);
            }
        }
    }

    uint32_t doodadCount = this->m_doodadElements.m_count;
    this->m_doodadCounts.SetCount(doodadCount);

    if (doodadCount > 1) {
        memset(s_doodadHash, 0xFF, sizeof(s_doodadHash));

        for (uint32_t i = 0; i < doodadCount; i++) {
            uint32_t index = this->m_doodadElements.m_data[i];
            uint32_t start = HashDoodadElement(&this->m_elements.m_data[index]) % (sizeof(s_doodadHash) / sizeof(s_doodadHash[0]));
            uint32_t slot = start;

            while (true) {
                if (++slot >= sizeof(s_doodadHash) / sizeof(s_doodadHash[0])) {
                    slot = 0;
                }

                uint32_t other = s_doodadHash[slot];

                if (other == 0xFFFFFFFF || slot == start) {
                    s_doodadHash[slot] = index;
                    break;
                }

                if (!CM2Scene::SortDoodad(index, other, this->m_elements.m_data)) {
                    break;
                }
            }

            this->m_elements.m_data[index].m_doodadKey = s_doodadHash[slot];
        }

        M2HeapSort(&CM2Scene::SortDoodadKey, this->m_doodadElements.m_data, doodadCount, this->m_elements.m_data);
    }

    uint32_t out = 0;

    for (uint32_t i = 0; i < doodadCount; ) {
        uint32_t index = this->m_doodadElements.m_data[i];
        uint32_t start = out;
        this->m_doodadElements.m_data[out++] = index;

        uint32_t j = i + 1;

        if (j < doodadCount) {
            uint32_t key = this->m_elements.m_data[index].m_doodadKey;

            do {
                uint32_t other = this->m_doodadElements.m_data[j];

                if (key != this->m_elements.m_data[other].m_doodadKey) {
                    break;
                }

                this->m_doodadElements.m_data[out++] = other;
                j++;
            } while (j < doodadCount);
        }

        if (j - i <= 1) {
            this->m_elements.m_data[index].m_typeKey = (this->m_elements.m_data[index].m_typeKey & 0x1FFFFFFF) | 0x20000000;
            this->ComputeElementShaders(&this->m_elements.m_data[index]);
            this->m_passElements[0].New(index);
            out--;
            i++;
        } else {
            this->m_doodadCounts.m_data[start] = j - i;
            i = j;
        }
    }

    this->m_doodadElements.SetCount(out);
    this->m_doodadCounts.SetCount(out);

    if (this->m_elements.m_count) {
        M2HeapSort(&CM2Scene::SortSortKey, this->m_passElements[0].m_data, this->m_passElements[0].m_count, this->m_elements.m_data);
        M2HeapSort(&CM2Scene::SortSortKey, this->m_passElements[1].m_data, this->m_passElements[1].m_count, this->m_elements.m_data);
        M2HeapSort(&CM2Scene::SortTransparent, this->m_passElements[2].m_data, this->m_passElements[2].m_count, this->m_elements.m_data);
        M2HeapSort(&CM2Scene::SortTransparent, this->m_passElements[3].m_data, this->m_passElements[3].m_count, this->m_elements.m_data);
    }

    if ((this->m_cache->m_flags & 0x40) && additiveCount > 1) {
        this->SortAdditiveParticleElements(2);
        this->SortAdditiveParticleElements(3);
    }

    return 1;
}

// OFFSET: 0x0082F1B0/0x14054F880
int32_t CM2Scene::Release() {
    if (this->m_refCount-- != 1) {
        return this->m_refCount;
    }

    if (this->m_scenePrev) {
        *this->m_scenePrev = this->m_sceneNext;
    }

    if (this->m_sceneNext) {
        this->m_sceneNext->m_scenePrev = this->m_scenePrev;
    }

    this->m_scenePrev = NULL;
    this->m_sceneNext = NULL;
    delete this;

    return 0;
}

// OFFSET: 0x0082FB60/0x140550690
void CM2Scene::Draw(M2PASS pass) {
    if (!((1 << pass) & this->m_passMask)) {
        return;
    }

    DebugLog("Model2 Pass %d", pass);

    uint32_t optFlags = this->m_cache->m_flags & 0xE000;

    if (CM2Scene::s_optFlags != optFlags) {
        CM2Scene::s_optFlags = optFlags;
    }

    CM2SceneRender render(this);
    render.m_sceneFlag8 = this->m_flags & 8;
    render.Draw(pass, this->m_elements.m_data, this->m_passElements[pass].m_data, NULL, this->m_passElements[pass].m_count);

    if (pass == M2PASS_0) {
        render.Draw(M2PASS_0, this->m_elements.m_data, this->m_doodadElements.m_data, this->m_doodadCounts.m_data, this->m_doodadElements.m_count);
    }
}
