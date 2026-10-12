#include <cstdint>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <malloc.h>
#include <new>
#include <typeinfo>
#include "Engine/Source/Base/BlzAlgorithm.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/SIMD.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Shader.h"
#include "Engine/Source/Os/Path.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/ShaderEffect/ShaderEffectManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CImVector.h"

// STATIC: 0x00BD1CA0
static const uint32_t s_skinProfileBoneCounts[4] = { 256, 64, 53, 21 };

// STATIC: 0x00D07F78/0x140BB9210
static uint32_t s_sequenceIndex = 0xFFFFFFFF;

// STATIC: 0x00EBEED4
static uint8_t* s_sequenceData;

// STATIC: 0x00EBEED8
static uint32_t s_sequenceSize;

// STATIC: 0x00EBEEDC
static void (*s_sequenceCallback)(CAsyncObject* object);

// STATIC: 0x00EBEEE0
static CM2Shared* s_sequenceShared;

// OFFSET: 0x00831090/0x140552660
static void AsyncFileReadFreeA16(CAsyncObject* object) {
    void* buffer = object->m_buffer;
    AsyncFileReadDestroyObject(object);
    SIMD::FreeA16(buffer);
}

// OFFSET: 0x008310C0/0x140552690
uint32_t CM2Shared::AddRef() {
    if (!this->m_refCount && this->m_cache) {
        if (this->m_freePrev) {
            *this->m_freePrev = this->m_freeNext;
        }

        if (this->m_freeNext) {
            this->m_freeNext->m_freePrev = this->m_freePrev;
        } else {
            this->m_cache->m_freeListTail = this->m_freePrev;
        }

        this->m_freePrev = NULL;
        this->m_freeNext = NULL;
        this->m_freeTime = 0;
    }

    return ++this->m_refCount;
}

// OFFSET: 0x00831110/0x140552700
int32_t CM2Shared::CallbackWhenLoaded(CM2Model* model) {
    if (!(model->m_flags & 0x10)) {
        if ((this->m_flags & 1) && (this->m_flags & 2)) {
            model->InitializeLoaded();
            return 1;
        }

        model->LinkToCallbackListTail();
    }

    return 1;
}

// OFFSET: 0x00831150/0x140552740
void CM2Shared::LoadFailedCallback(CAsyncObject* object) {
    CM2Shared* shared = static_cast<CM2Shared*>(object->m_userArg);
    DefaultAsyncObjectCleanupCallback(object);
    shared->m_asyncObject = NULL;
}

// OFFSET: 0x00831180
static void MakeAnimFileName(const char* fileName, uint32_t id, uint32_t variation, char* result) {
    strcpy(result, fileName);
    *OsPathFindExtensionWithDot(result) = '\0';
    sprintf(&result[strlen(result)], "%04d-%02d.anim", id, variation);
}

// OFFSET: 0x008311E0
static void MakeSkinFileName(const char* fileName, uint32_t profile, char* result) {
    strcpy(result, fileName);
    *OsPathFindExtensionWithDot(result) = '\0';
    sprintf(&result[strlen(result)], "%02d.skin", profile);
}

// OFFSET: 0x00831230/0x140552770
SFileImpl* CM2Shared::OpenLowPrioritySequenceFile(uint32_t id, uint32_t variation) {
    char fileName[260];
    MakeAnimFileName(this->m_fileName, id, variation, fileName);

    SFileImpl* file;

    return SFile::OpenEx(NULL, fileName, (this->m_flags >> 2) & 1, &file) ? file : NULL;
}

// OFFSET: 0x00831290/0x140552830
SFileImpl* CM2Shared::OpenSkinFile(uint32_t profile) {
    char fileName[260];
    char errorCube[] = "Spells\\ErrorCube.m2";
    MakeSkinFileName((this->m_flags & 0x80) ? errorCube : this->m_fileName, profile, fileName);

    SFileImpl* file;

    if (SFile::OpenEx(NULL, fileName, (this->m_flags & 4) >> 2, &file)) {
        return file;
    }

    this->m_flags |= 0x100;

    return NULL;
}

// OFFSET: [0x008318A0/0x140552FB0<char>, 0x008318F0/0x140552EA0<M2Loop>, 0x008318F0/0x140552EA0<M2Material>, 0x008318F0/0x140552EA0<ubyte4>, 0x008318F0/0x140552EA0<unsigned int>, 0x008318F0/0x140552EA0<float>, 0x00831940/0x140553060<M2Sequence>, 0x00831990/0x1405530C0<unsigned short>, 0x00831990/0x1405530C0<fixed16>, 0x008317B0/0x140552EF0<M2Vertex>, 0x008317B0/0x140552EF0<M2SkinSection>, 0x00831800/0x140552F50<M2Batch>, 0x00831850/0x140553000<C3Vector>, 0x00831850/0x140553000<M2ShadowBatch>, 0x00831850/0x140553000<M2SplineKey<float> >, 0x008319E0/0x140553110<C2Vector>, 0x008319E0/0x140553110<M2CompQuat>]
template <class T>
int32_t M2Init(uint8_t* base, uint32_t size, const M2Data& data, M2Array<T>& array) {
    if (s_sequenceIndex == 0xFFFFFFFF) {
        if (array.m_offset > size || array.m_offset + sizeof(T) * array.m_count > size) {
            return 0;
        }

        array.Relocate(base);
    }

    return 1;
}

// OFFSET: [0x008328C0/0x140554ED0<unsigned int>, 0x008328C0/0x140554ED0<float>, 0x00832460/0x140554D70<unsigned short>, 0x00832460/0x140554D70<fixed16>, 0x00832950<M2CompQuat>, 0x008329E0<C4Quaternion>, 0x00832A70/0x140554E20<unsigned char>, 0x00832B00<M2SplineKey<C3Vector> >, 0x00832B90<C3Vector>, 0x00832B90<M2SplineKey<float> >]
template <class T>
int32_t M2Init(uint8_t* base, uint32_t size, const M2Data& data, M2Array<M2Array<T> >& array) {
    if (s_sequenceIndex == 0xFFFFFFFF) {
        if (array.m_offset > size || array.m_offset + sizeof(M2Array<T>) * array.m_count > size) {
            return 0;
        }

        array.Relocate(base);
    }

    for (uint32_t i = 0; i < array.m_count; i++) {
        if (!M2Init(base, size, data, array[i])) {
            return 0;
        }
    }

    return 1;
}

// INLINED
int32_t M2Init(uint8_t* base, uint32_t size, const M2Data& data, M2TrackBase& track) {
    if (s_sequenceIndex == 0xFFFFFFFF) {
        if (track.m_loopIndex == 0xFFFF) {
            track.m_sequenceTimes.Relocate(base);

            for (uint32_t i = 0; i < track.m_sequenceTimes.m_count; i++) {
                if (data.m_sequences[i].m_flags & 0x20) {
                    if (track.m_sequenceTimes[i].m_offset >= size) {
                        return 0;
                    }

                    track.m_sequenceTimes[i].Relocate(base);
                }
            }
        } else if (!M2Init(base, size, data, track.m_sequenceTimes)) {
            return 0;
        }
    } else if (s_sequenceIndex < track.m_sequenceTimes.m_count) {
        M2Array<uint32_t>& times = track.m_sequenceTimes[s_sequenceIndex];

        if (times.m_offset >= s_sequenceSize) {
            return 0;
        }

        times.Relocate(s_sequenceData);
    }

    return 1;
}

// INLINED
template <class T>
int32_t M2Init(uint8_t* base, uint32_t size, const M2Data& data, M2Track<T>& track) {
    if (s_sequenceIndex == 0xFFFFFFFF) {
        if (track.m_loopIndex == 0xFFFF) {
            track.m_sequenceTimes.Relocate(base);

            for (uint32_t i = 0; i < track.m_sequenceTimes.m_count; i++) {
                if (data.m_sequences[i].m_flags & 0x20) {
                    if (track.m_sequenceTimes[i].m_offset >= size) {
                        return 0;
                    }

                    track.m_sequenceTimes[i].Relocate(base);
                }
            }
        } else if (!M2Init(base, size, data, track.m_sequenceTimes)) {
            return 0;
        }

        if (track.m_loopIndex == 0xFFFF) {
            track.m_sequenceKeys.Relocate(base);

            for (uint32_t i = 0; i < track.m_sequenceKeys.m_count; i++) {
                if (data.m_sequences[i].m_flags & 0x20) {
                    if (track.m_sequenceKeys[i].m_offset >= size) {
                        return 0;
                    }

                    track.m_sequenceKeys[i].Relocate(base);
                }
            }
        } else if (!M2Init(base, size, data, track.m_sequenceKeys)) {
            return 0;
        }
    } else {
        if (s_sequenceIndex < track.m_sequenceTimes.m_count) {
            M2Array<uint32_t>& times = track.m_sequenceTimes[s_sequenceIndex];

            if (times.m_offset >= s_sequenceSize) {
                return 0;
            }

            times.Relocate(s_sequenceData);
        }

        if (s_sequenceIndex < track.m_sequenceKeys.m_count) {
            M2Array<T>& keys = track.m_sequenceKeys[s_sequenceIndex];

            if (keys.m_offset >= s_sequenceSize) {
                return 0;
            }

            keys.Relocate(s_sequenceData);
        }
    }

    return 1;
}

// OFFSET: 0x00831BE0/0x1405533B0
int32_t CM2Shared::SetIndices() {
    if (!this->m_indexPool) {
        this->m_indexPool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Index, GxPoolUsage_Dynamic, sizeof(uint16_t) * this->m_instances * this->m_skinData->m_indices.m_count, GxPoolHintBit_Unk1, this->m_fileNameWithoutPath);
        this->m_indexBuf = g_theGxDevicePtr->BufCreate(this->m_indexPool, sizeof(uint16_t), this->m_instances * this->m_skinData->m_indices.m_count, 0);

        if (!this->m_indexPool || !this->m_indexBuf) {
            return 0;
        }
    }

    if (!this->m_indexBuf->unk_001C || !this->m_indexBuf->unk_001D) {
        uint16_t* indices = reinterpret_cast<uint16_t*>(g_theGxDevicePtr->BufLock(this->m_indexBuf));

        if (!indices) {
            return 0;
        }

        int32_t sharedVertices = (this->m_cache->m_flags & 8) || (this->m_data->m_bones.m_count == 1 && (this->m_cache->m_flags & 0x20));
        uint32_t indexStart = 0;

        for (uint32_t i = 0; i < this->m_skinData->m_skinSections.m_count; i++) {
            M2SkinSection* section = &this->m_skinSections[i];
            uint32_t start = this->m_skinData->m_skinSections[i].m_indexStart;
            uint16_t offset = sharedVertices ? 0 : static_cast<uint16_t>(-section->m_vertexStart);

            for (uint32_t instance = 0; instance < this->m_instances; instance++) {
                for (uint32_t j = 0; j < section->m_indexCount; j++) {
                    indices[j] = offset + this->m_skinData->m_indices[start + j];
                }

                indices += section->m_indexCount;
                offset += sharedVertices ? static_cast<uint16_t>(this->m_skinData->m_vertices.m_count) : section->m_vertexCount;
            }

            section->m_indexStart = static_cast<uint16_t>(indexStart);
            indexStart += this->m_instances * section->m_indexCount;
        }

        CGxBuf* buf = this->m_indexBuf;
        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
    }

    g_theGxDevicePtr->PrimIndexPtr(this->m_indexBuf);

    return 1;
}

// OFFSET: 0x00831DF0/0x140553660
int32_t CM2Shared::SetVerticesShader(EGxVertexBufferFormat format) {
    if (!this->m_vertexBuf->unk_001C || !this->m_vertexBuf->unk_001D) {
        M2Vertex* vertices = reinterpret_cast<M2Vertex*>(g_theGxDevicePtr->BufLock(this->m_vertexBuf));

        if (!vertices) {
            return 0;
        }

        for (uint32_t instance = 0; instance < this->m_instances; instance++) {
            for (uint32_t i = 0; i < this->m_skinData->m_skinSections.m_count; i++) {
                M2SkinSection* section = &this->m_skinData->m_skinSections[i];
                uint32_t boneOffset = 0x01010101 * instance * section->m_boneCount;
                uint32_t end = section->m_vertexStart + section->m_vertexCount;

                for (uint32_t j = section->m_vertexStart; j < end; j++) {
                    memcpy(&vertices[j], &this->m_data->m_vertices[this->m_skinData->m_vertices[j]], sizeof(M2Vertex));
                    *reinterpret_cast<uint32_t*>(&vertices[j].m_indices) = boneOffset + *reinterpret_cast<uint32_t*>(&this->m_skinData->m_bones[j]);
                }
            }

            vertices += this->m_skinData->m_vertices.m_count;
        }

        CGxBuf* buf = this->m_vertexBuf;
        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
    }

    GxPrimVertexPtr(this->m_vertexBuf, format);

    return 1;
}

// OFFSET: 0x00831F70/0x1405538B0
int32_t CM2Shared::SetVerticesFixedFunction(uint32_t texCoordIndex) {
    if (!this->m_vertexBuf->unk_001C || !this->m_vertexBuf->unk_001D) {
        M2VertexPNT2* vertices = reinterpret_cast<M2VertexPNT2*>(g_theGxDevicePtr->BufLock(this->m_vertexBuf));

        if (!vertices) {
            return 0;
        }

        for (uint32_t i = 0; i < this->m_skinData->m_skinSections.m_count; i++) {
            M2SkinSection* section = &this->m_skinData->m_skinSections[i];
            uint32_t end = section->m_vertexStart + section->m_vertexCount;

            for (uint32_t j = section->m_vertexStart; j < end; j++) {
                M2Vertex* source = &this->m_data->m_vertices[this->m_skinData->m_vertices[j]];
                M2VertexPNT2* vertex = &vertices[j];
                vertex->m_position = source->m_position;
                vertex->m_normal = source->m_normal;
                vertex->m_texcoord[0] = source->m_texcoord[texCoordIndex];
                vertex->m_texcoord[1] = source->m_texcoord[texCoordIndex == 0];
            }
        }

        CGxBuf* buf = this->m_vertexBuf;
        g_theGxDevicePtr->BufUnlock(buf, 0);
        buf->unk_001C = 1;
    }

    GxPrimVertexPtr(this->m_vertexBuf, GxVBF_PNT2);

    return 1;
}

// OFFSET: 0x008323B0/0x140554560
int32_t CM2Shared::SetVertices(uint32_t texCoordIndex, EGxVertexBufferFormat format) {
    if (!this->m_vertexPool) {
        this->m_vertexPool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Vertex, GxPoolUsage_Static, sizeof(M2Vertex) * this->m_instances * this->m_skinData->m_vertices.m_count, GxPoolHintBit_Unk1, this->m_fileNameWithoutPath);
        this->m_vertexBuf = g_theGxDevicePtr->BufCreate(this->m_vertexPool, sizeof(M2Vertex), this->m_instances * this->m_skinData->m_vertices.m_count, 0);

        if (!this->m_vertexPool || !this->m_vertexBuf) {
            return 0;
        }
    }

    if (CShaderEffect::s_enableShaders) {
        return this->SetVerticesShader(format);
    }

    return this->SetVerticesFixedFunction(texCoordIndex);
}

// OFFSET: 0x008316E0/0x140552D70
void CM2Shared::DestroyBuffers() {
    if (this->m_indexBuf) {
        CGxBuf* buf = this->m_indexBuf;

        if (buf != g_theGxDevicePtr->BufStream(buf->m_pool->m_target, 0, 0)) {
            g_theGxDevicePtr->BufDestroy(buf);
        }

        this->m_indexBuf = NULL;
    }

    if (this->m_indexPool) {
        g_theGxDevicePtr->PoolDestroy(this->m_indexPool);
        this->m_indexPool = NULL;
    }

    if (this->m_vertexBuf) {
        CGxBuf* buf = this->m_vertexBuf;

        if (buf != g_theGxDevicePtr->BufStream(buf->m_pool->m_target, 0, 0)) {
            g_theGxDevicePtr->BufDestroy(buf);
        }

        this->m_vertexBuf = NULL;
    }

    if (this->m_vertexPool) {
        g_theGxDevicePtr->PoolDestroy(this->m_vertexPool);
        this->m_vertexPool = NULL;
    }
}

// OFFSET: 0x00832250/0x140553CD0
template <>
int32_t M2Init<M2Texture>(uint8_t* base, uint32_t size, const M2Data& data, M2Array<M2Texture>& array) {
    if (s_sequenceIndex == 0xFFFFFFFF) {
        if (array.m_offset > size || array.m_offset + sizeof(M2Texture) * array.m_count > size) {
            return 0;
        }

        array.Relocate(base);
    }

    for (uint32_t i = 0; i < array.m_count; i++) {
        if (!M2Init(base, size, data, array[i].m_filename)) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x00831330/0x140552920
CShaderEffect* CM2Shared::GetEffect(M2Batch* batch) {
    uint16_t shader = batch->m_shader;

    const char* vertexShader = M2GetVertexShaderName(batch->m_textureCount, shader);
    const char* pixelShader = M2GetPixelShaderName(batch->m_textureCount, batch->m_shader);

    char hullShader[256];
    char domainShader[256];
    hullShader[0] = '\0';
    domainShader[0] = '\0';

    if (CShaderEffect::TesselationEnabled()) {
        const char* hull = M2GetHullShaderName(batch->m_textureCount, batch->m_shader);
        const char* domain = M2GetDomainShaderName(batch->m_textureCount, batch->m_shader);

        if (hull) {
            SStrPrintf(hullShader, sizeof(hullShader), "Model2_%s", hull);
        }

        if (domain) {
            SStrPrintf(domainShader, sizeof(domainShader), "Model2_%s", domain);
        }
    } else if (CShaderEffect::DisplacementEnabled()) {
        const char* hull = M2GetHullShaderName(batch->m_textureCount, batch->m_shader);
        const char* domain = M2GetDomainShaderName(batch->m_textureCount, batch->m_shader);

        if (hull) {
            SStrPrintf(hullShader, sizeof(hullShader), "Model2Displ_%s", hull);
        }

        if (domain) {
            SStrPrintf(domainShader, sizeof(domainShader), "Model2Displ_%s", domain);
        }
    }

    char effectName[256];

    if (batch->m_shader & 0x8000) {
        SStrPrintf(effectName, sizeof(effectName), "M2Effect %d", batch->m_shader & 0x7FFF);
    } else {
        strcpy(effectName, vertexShader);
        strcat(effectName, pixelShader);
    }

    CShaderEffect* effect = CShaderEffectManager::GetEffect(effectName);

    if (effect) {
        effect->AddRef();
    } else {
        effect = CShaderEffectManager::CreateEffect(effectName);
        effect->InitEffect(vertexShader, hullShader, domainShader, pixelShader);

        if (batch->m_shader & 0x8000) {
            EGxTexOp colorOp;
            EGxTexOp alphaOp;
            M2GetFixedFunctionFallback(batch->m_shader, &colorOp, &alphaOp);
            effect->InitFixedFuncPass(&colorOp, &alphaOp, 1);
        } else {
            EGxTexOp colorOps[2];
            EGxTexOp alphaOps[2];
            M2GetCombinerOps(batch->m_shader, batch->m_textureCount, colorOps, alphaOps);
            effect->InitFixedFuncPass(colorOps, alphaOps, batch->m_textureCount);
        }
    }

    if (!effect) {
        effect = CShaderEffectManager::GetEffect("Diffuse_T1Combiners_Opaque");

        if (!effect) {
            effect = CShaderEffectManager::CreateEffect("Diffuse_T1Combiners_Opaque");
            effect->InitEffect("Diffuse_T1", NULL, NULL, "Combiners_Opaque");

            EGxTexOp colorOp = static_cast<EGxTexOp>(0);
            EGxTexOp alphaOp = static_cast<EGxTexOp>(3);
            effect->InitFixedFuncPass(&colorOp, &alphaOp, 1);

            return effect;
        }

        effect->AddRef();
    }

    return effect;
}

// OFFSET: 0x008320D0/0x140553A70
uint32_t CM2Shared::AllocInstances(uint32_t count, int32_t exact) {
    if (exact) {
        uint32_t instances = count > this->m_maxInstances ? this->m_maxInstances : count;

        if (instances != this->m_instances) {
            this->m_instances = instances;
            this->DestroyBuffers();
        }

        return this->m_instances;
    }

    if (this->m_instances < count && this->m_instances < this->m_maxInstances) {
        this->m_instances = (count + 15) & ~15u;

        if (this->m_instances > this->m_maxInstances) {
            this->m_instances = this->m_maxInstances;
        }

        this->DestroyBuffers();
    }

    return this->m_instances;
}

// OFFSET: 0x00832C90/0x140555220
template <>
int32_t M2Init<M2Event>(uint8_t* base, uint32_t size, const M2Data& data, M2Array<M2Event>& array) {
    if (s_sequenceIndex == 0xFFFFFFFF) {
        if (array.m_offset > size || array.m_offset + sizeof(M2Event) * array.m_count > size) {
            return 0;
        }

        array.Relocate(base);
    }

    for (uint32_t i = 0; i < array.m_count; i++) {
        if (!M2Init(base, size, data, array[i].m_enabled)) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x00831A30/0x140553190
int32_t CM2Shared::InitializeSkinProfile() {
    uint32_t maxBones = 256;

    if (this->m_cache->m_flags & 8) {
        uint32_t constants = g_theGxDevicePtr->Caps().m_shaderConstants[0];
        maxBones = (constants > 0xC0 ? constants - 64 : constants - 32) / 3;

        if (maxBones >= 256) {
            maxBones = 256;
        }
    }

    M2SkinProfile* skinData = this->m_skinData;

    this->m_maxInstances = skinData->m_indices.m_count ? 0x10000 / skinData->m_indices.m_count : 1;

    for (uint32_t i = 0; i < skinData->m_skinSections.m_count; i++) {
        uint32_t instances = maxBones / skinData->m_skinSections[i].m_boneCount;

        if (this->m_maxInstances > instances) {
            this->m_maxInstances = instances;
        }
    }

    if (!this->m_maxInstances) {
        this->m_maxInstances = 1;
    }

    this->m_instances = 1;

    uint32_t size = sizeof(M2SkinSection) * skinData->m_skinSections.m_count;

    if (skinData->m_indices.m_count) {
        size += sizeof(CShaderEffect*) * skinData->m_batches.m_count;
    }

    uint8_t* memory = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));

    if (!memory) {
        return 0;
    }

    this->m_skinSections = reinterpret_cast<M2SkinSection*>(memory);

    size_t sectionsSize = 0;

    if (this->m_skinData->m_skinSections.m_count) {
        sectionsSize = sizeof(M2SkinSection) * this->m_skinData->m_skinSections.m_count;
        memcpy(memory, this->m_skinData->m_skinSections.Data(), sectionsSize);
    }

    if (this->m_skinData->m_indices.m_count) {
        this->m_effects = reinterpret_cast<CShaderEffect**>(&memory[sectionsSize]);
        memset(this->m_effects, 0, sizeof(CShaderEffect*) * this->m_skinData->m_batches.m_count);
    }

    if (this->m_skinData->m_indices.m_count) {
        for (uint32_t i = 0; i < this->m_skinData->m_batches.m_count; i++) {
            this->m_effects[i] = this->GetEffect(&this->m_skinData->m_batches[i]);
        }
    }

    return 1;
}

// OFFSET: 0x00833520
template <>
int32_t M2Init<M2Color>(uint8_t* base, uint32_t size, const M2Data& data, M2Array<M2Color>& array) {
    if (s_sequenceIndex == 0xFFFFFFFF) {
        if (array.m_offset > size || array.m_offset + sizeof(M2Color) * array.m_count > size) {
            return 0;
        }

        array.Relocate(base);
    }

    for (uint32_t i = 0; i < array.m_count; i++) {
        M2Color* color = &array[i];

        if (!M2Init(base, size, data, color->m_color) || !M2Init(base, size, data, color->m_alpha)) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x008339A0/0x1405571F0
template <>
int32_t M2Init<M2TextureWeight>(uint8_t* base, uint32_t size, const M2Data& data, M2Array<M2TextureWeight>& array) {
    if (s_sequenceIndex == 0xFFFFFFFF) {
        if (array.m_offset > size || array.m_offset + sizeof(M2TextureWeight) * array.m_count > size) {
            return 0;
        }

        array.Relocate(base);
    }

    for (uint32_t i = 0; i < array.m_count; i++) {
        if (!M2Init(base, size, data, array[i].m_weight)) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x00832150/0x140553B00
int32_t M2Init(uint8_t* base, uint32_t size, const M2Data& data, M2SkinProfile& skinData) {
    return M2Init(base, size, data, skinData.m_vertices) && M2Init(base, size, data, skinData.m_indices) && M2Init(base, size, data, skinData.m_bones) && M2Init(base, size, data, skinData.m_skinSections) && M2Init(base, size, data, skinData.m_batches) && M2Init(base, size, data, skinData.m_shadowBatches);
}

// OFFSET: 0x008322F0/0x140554480
int32_t CM2Shared::FinishLoadingSkinProfile(uint32_t size) {
    if (this->m_flags & 2) {
        return 1;
    }

    if (!M2Init(reinterpret_cast<uint8_t*>(this->m_skinData), size, *this->m_data, *this->m_skinData)) {
        CM2Model::ErrorSetFileLine(__FILE__, __LINE__);
        CM2Model::ErrorSet("Corrupt skin profile data: %s", this->m_fileName);
        this->m_flags |= 0x100;
        return 0;
    }

    if (!this->InitializeSkinProfile()) {
        CM2Model::ErrorSetFileLine(__FILE__, __LINE__);
        CM2Model::ErrorSet("Failed to initialize model skin profile: %s", this->m_fileName);
        this->m_flags |= 0x100;
        return 0;
    }

    this->m_flags |= 2;

    while (this->m_callbackList) {
        CM2Model* model = this->m_callbackList;
        model->UnlinkFromCallbackList();
        model->InitializeLoaded();
    }

    return 1;
}

// OFFSET: 0x00832E90
template <>
int32_t M2Init<M2CompBone>(uint8_t* base, uint32_t size, const M2Data& data, M2Array<M2CompBone>& array) {
    if (s_sequenceIndex == 0xFFFFFFFF) {
        if (array.m_offset > size || array.m_offset + sizeof(M2CompBone) * array.m_count > size) {
            return 0;
        }

        array.Relocate(base);
    }

    for (uint32_t i = 0; i < array.m_count; i++) {
        M2CompBone* bone = &array[i];

        if (!M2Init(base, size, data, bone->m_translationTrack) || !M2Init(base, size, data, bone->m_rotationTrack) || !M2Init(base, size, data, bone->m_scaleTrack)) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x00833BF0
template <>
int32_t M2Init<M2TextureTransform>(uint8_t* base, uint32_t size, const M2Data& data, M2Array<M2TextureTransform>& array) {
    if (s_sequenceIndex == 0xFFFFFFFF) {
        if (array.m_offset > size || array.m_offset + sizeof(M2TextureTransform) * array.m_count > size) {
            return 0;
        }

        array.Relocate(base);
    }

    for (uint32_t i = 0; i < array.m_count; i++) {
        M2TextureTransform* transform = &array[i];

        if (!M2Init(base, size, data, transform->m_translation) || !M2Init(base, size, data, transform->m_rotation) || !M2Init(base, size, data, transform->m_scaling)) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x00834280/0x140558830
template <>
int32_t M2Init<M2Attachment>(uint8_t* base, uint32_t size, const M2Data& data, M2Array<M2Attachment>& array) {
    if (s_sequenceIndex == 0xFFFFFFFF) {
        if (array.m_offset > size || array.m_offset + sizeof(M2Attachment) * array.m_count > size) {
            return 0;
        }

        array.Relocate(base);
    }

    for (uint32_t i = 0; i < array.m_count; i++) {
        if (!M2Init(base, size, data, array[i].m_animateAttached)) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x008344E0
template <>
int32_t M2Init<M2Light>(uint8_t* base, uint32_t size, const M2Data& data, M2Array<M2Light>& array) {
    if (s_sequenceIndex == 0xFFFFFFFF) {
        if (array.m_offset > size || array.m_offset + sizeof(M2Light) * array.m_count > size) {
            return 0;
        }

        array.Relocate(base);
    }

    for (uint32_t i = 0; i < array.m_count; i++) {
        M2Light* light = &array[i];

        if (!M2Init(base, size, data, light->m_ambientColor) || !M2Init(base, size, data, light->m_ambientIntensity) || !M2Init(base, size, data, light->m_diffuseColor) || !M2Init(base, size, data, light->m_diffuseIntensity) || !M2Init(base, size, data, light->m_attenuationStart) || !M2Init(base, size, data, light->m_attenuationEnd) || !M2Init(base, size, data, light->m_visibility)) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x008353B0
template <>
int32_t M2Init<M2Camera>(uint8_t* base, uint32_t size, const M2Data& data, M2Array<M2Camera>& array) {
    if (s_sequenceIndex == 0xFFFFFFFF) {
        if (array.m_offset > size || array.m_offset + sizeof(M2Camera) * array.m_count > size) {
            return 0;
        }

        array.Relocate(base);
    }

    for (uint32_t i = 0; i < array.m_count; i++) {
        M2Camera* camera = &array[i];

        if (!M2Init(base, size, data, camera->m_positions) || !M2Init(base, size, data, camera->m_targetPositions) || !M2Init(base, size, data, camera->m_roll) || !M2Init(base, size, data, camera->m_fieldOfView)) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x00835C30
template <>
int32_t M2Init<M2Ribbon>(uint8_t* base, uint32_t size, const M2Data& data, M2Array<M2Ribbon>& array) {
    if (s_sequenceIndex == 0xFFFFFFFF) {
        if (array.m_offset > size || array.m_offset + sizeof(M2Ribbon) * array.m_count > size) {
            return 0;
        }

        array.Relocate(base);
    }

    for (uint32_t i = 0; i < array.m_count; i++) {
        M2Ribbon* ribbon = &array[i];

        if (!M2Init(base, size, data, ribbon->m_textureIndices) || !M2Init(base, size, data, ribbon->m_materialIndices) || !M2Init(base, size, data, ribbon->m_colorTrack) || !M2Init(base, size, data, ribbon->m_alphaTrack) || !M2Init(base, size, data, ribbon->m_heightAboveTrack) || !M2Init(base, size, data, ribbon->m_heightBelowTrack) || !M2Init(base, size, data, ribbon->m_textureSlotTrack) || !M2Init(base, size, data, ribbon->m_visibilityTrack)) {
            return 0;
        }
    }

    return 1;
}

// INLINED
int32_t M2Init(uint8_t* base, uint32_t size, const M2Data& data, M2Particle& particle) {
    return M2Init(base, size, data, particle.m_geometryMdl) && M2Init(base, size, data, particle.m_recursionMdl) && M2Init(base, size, data, particle.m_speedTrack) && M2Init(base, size, data, particle.m_variationTrack) && M2Init(base, size, data, particle.m_latitudeTrack) && M2Init(base, size, data, particle.m_longitudeTrack) && M2Init(base, size, data, particle.m_gravityTrack) && M2Init(base, size, data, particle.m_lifeTrack) && M2Init(base, size, data, particle.m_emissionRateTrack) && M2Init(base, size, data, particle.m_widthTrack) && M2Init(base, size, data, particle.m_lengthTrack) && M2Init(base, size, data, particle.m_zsourceTrack) && M2Init(base, size, data, particle.m_colorTrack.m_times) && M2Init(base, size, data, particle.m_colorTrack.m_values) && M2Init(base, size, data, particle.m_alphaTrack.m_times) && M2Init(base, size, data, particle.m_alphaTrack.m_values) && M2Init(base, size, data, particle.m_scaleTrack.m_times) && M2Init(base, size, data, particle.m_scaleTrack.m_values) && M2Init(base, size, data, particle.m_headCellTrack.m_times) && M2Init(base, size, data, particle.m_headCellTrack.m_values) && M2Init(base, size, data, particle.m_tailCellTrack.m_times) && M2Init(base, size, data, particle.m_tailCellTrack.m_values) && M2Init(base, size, data, particle.m_spline) && M2Init(base, size, data, particle.m_visibilityTrack);
}

// OFFSET: 0x00836970/0x14055E720
template <>
int32_t M2Init<M2Particle>(uint8_t* base, uint32_t size, const M2Data& data, M2Array<M2Particle>& array) {
    if (s_sequenceIndex == 0xFFFFFFFF) {
        if (array.m_offset > size || array.m_offset + sizeof(M2Particle) * array.m_count > size) {
            return 0;
        }

        array.Relocate(base);
    }

    for (uint32_t i = 0; i < array.m_count; i++) {
        if (!M2Init(base, size, data, array[i])) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x008324F0/0x140554660
CM2Shared::CM2Shared(CM2Cache* cache) {
    this->m_flags &= ~0x1FFu;
    this->m_cache = cache;
    this->m_callbackList = NULL;
    this->m_refCount = 1;
    this->m_asyncObject = NULL;
    this->m_callbackListTail = &this->m_callbackList;
    this->m_lowPrioritySequenceCount = 0;
    this->m_freePrev = NULL;
    this->m_freeNext = NULL;
    this->m_freeTime = 0;
    this->m_fileNameWithoutPath = NULL;
    this->m_previous = NULL;
    this->m_next = NULL;
    this->m_fileNameHash = 0;
    this->m_data = NULL;
    this->m_fileSize = 0;
    this->m_skinData = NULL;
    this->m_textures = NULL;
    this->m_indexPool = NULL;
    this->m_indexBuf = NULL;
    this->m_vertexPool = NULL;
    this->m_vertexBuf = NULL;
    this->m_effects = NULL;
    this->m_skinSections = NULL;
    this->m_instances = 0;
    this->m_maxInstances = 0;
    this->m_billboardBoneCount = 0;
    this->m_lastUsedTime = 0;
    this->m_fileName[0] = '\0';
}

// OFFSET: 0x00838360/0x140561F60
int32_t M2Init(uint8_t* base, uint32_t size, M2Data& data) {
    if (reinterpret_cast<uint8_t*>(&data) < base || reinterpret_cast<uint8_t*>(&data + 1) > base + size || data.m_magic != 0x3032444D) {
        return 0;
    }

    if (data.m_version < 0x109 || data.m_version > 0x110) {
        return 0;
    }

    return M2Init(base, size, data, data.m_name) && M2Init(base, size, data, data.m_loops) && M2Init(base, size, data, data.m_sequences) && M2Init(base, size, data, data.m_sequenceIdxHashById) && M2Init(base, size, data, data.m_bones) && M2Init(base, size, data, data.m_boneIndicesById) && M2Init(base, size, data, data.m_vertices) && M2Init(base, size, data, data.m_colors) && M2Init(base, size, data, data.m_textures) && M2Init(base, size, data, data.m_textureWeights) && M2Init(base, size, data, data.m_textureTransforms) && M2Init(base, size, data, data.m_textureIndicesById) && M2Init(base, size, data, data.m_materials) && M2Init(base, size, data, data.m_boneCombos) && M2Init(base, size, data, data.m_textureCombos) && M2Init(base, size, data, data.m_textureWeightCombos) && M2Init(base, size, data, data.m_textureTransformCombos) && M2Init(base, size, data, data.m_collisionIndices) && M2Init(base, size, data, data.m_collisionPositions) && M2Init(base, size, data, data.m_collisionFaceNormals) && M2Init(base, size, data, data.m_attachments) && M2Init(base, size, data, data.m_attachmentIndicesById) && M2Init(base, size, data, data.m_events) && M2Init(base, size, data, data.m_lights) && M2Init(base, size, data, data.m_cameras) && M2Init(base, size, data, data.m_cameraIndicesById) && M2Init(base, size, data, data.m_ribbons) && M2Init(base, size, data, data.m_particles);
}

// OFFSET: 0x008389E0/0x140562880
int32_t CM2Shared::FinishLoadingLowPrioritySequence(uint32_t sequenceIndex, uint8_t* buffer, uint32_t size) {
    s_sequenceShared = this;
    s_sequenceData = buffer;
    s_sequenceIndex = sequenceIndex;
    s_sequenceSize = size;
    s_sequenceCallback = AsyncFileReadFreeA16;

    int32_t result = M2Init(reinterpret_cast<uint8_t*>(this->m_data), this->m_fileSize, *this->m_data);
    M2Sequence* sequence = &this->m_data->m_sequences[sequenceIndex];

    s_sequenceIndex = 0xFFFFFFFF;
    s_sequenceData = NULL;
    s_sequenceSize = 0;
    s_sequenceCallback = NULL;
    s_sequenceShared = NULL;

    if (!result) {
        CM2Model::ErrorSetFileLine(__FILE__, __LINE__);
        CM2Model::ErrorSet("Corrupt model sequence data: %s (%d)", this->m_fileName, sequence->m_id);
        return 0;
    }

    sequence->m_flags = (sequence->m_flags & ~0x30u) | 0x20;

    return 1;
}

// OFFSET: 0x00838A90/0x140562970
int32_t CM2Shared::FinishLoadingLowPrioritySequence(uint32_t sequenceIndex, CAsyncObject* object) {
    M2Sequence* sequence = &this->m_data->m_sequences[sequenceIndex];

    if (!this->FinishLoadingLowPrioritySequence(sequenceIndex, static_cast<uint8_t*>(object->m_buffer), object->m_size)) {
        return 0;
    }

    for (uint16_t* alias = &sequence->m_aliasNext; *alias != sequenceIndex; alias = &this->m_data->m_sequences[*alias].m_aliasNext) {
        if (!this->FinishLoadingLowPrioritySequence(*alias, static_cast<uint8_t*>(object->m_buffer), object->m_size)) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x00832600/0x140554790
void CM2Shared::SkinProfileLoadedCallback(CAsyncObject* object) {
    CM2Shared* shared = static_cast<CM2Shared*>(object->m_userArg);
    shared->FinishLoadingSkinProfile(shared->m_asyncObject->m_size);
    DefaultAsyncObjectCleanupCallback(shared->m_asyncObject);
    shared->m_asyncObject = NULL;
}

// OFFSET: 0x00832640/0x1405547D0
int32_t CM2Shared::LoadSkinProfile(uint32_t profile) {
    SFileImpl* file = this->OpenSkinFile(profile);

    if (!file) {
        return 0;
    }

    this->m_skinData = static_cast<M2SkinProfile*>(SIMD::AllocA16(SFileGetFileSize(file, NULL)));

    if (!this->m_skinData) {
        SFileCloseFile(file);
        return 0;
    }

    AsyncFileReadCreateObject(file, this->m_priority, 0, &this->m_asyncObject, this->m_skinData, this, CM2Shared::SkinProfileLoadedCallback, CM2Shared::LoadFailedCallback, 0, 0, 0);

    return 1;
}

// OFFSET: 0x00831670/0x140552D00
uint32_t CM2Shared::GetSkinProfileIndex(uint32_t numSkinProfiles) {
    uint32_t constants = g_theGxDevicePtr->Caps().m_shaderConstants[0];
    uint32_t maxBones = (constants > 0xC0 ? constants - 64 : constants - 32) / 3;

    if (maxBones >= 256) {
        maxBones = 256;
    }

    if (!numSkinProfiles) {
        return 0xFFFFFFFF;
    }

    for (uint32_t i = 0; i < numSkinProfiles; i++) {
        if (s_skinProfileBoneCounts[4 - numSkinProfiles + i] <= maxBones) {
            return i;
        }
    }

    return 0xFFFFFFFF;
}

// OFFSET: 0x008326B0/0x140554870
int32_t CM2Shared::Initialize() {
    this->m_skinData = NULL;

    uint32_t profile = 0;

    if (this->m_cache->m_flags & 8) {
        profile = CM2Shared::GetSkinProfileIndex(this->m_data->m_numSkinProfiles);

        if (profile == 0xFFFFFFFF) {
            CM2Model::ErrorSetFileLine(__FILE__, __LINE__);
            CM2Model::ErrorSet("Failed to choose skin profile: %s", this->m_fileName);
            return 0;
        }
    }

    if (!this->LoadSkinProfile(profile)) {
        return 0;
    }

    this->m_textures = static_cast<CTexture**>(SMemAlloc(sizeof(CTexture*) * this->m_data->m_textures.m_count, __FILE__, __LINE__, 8));

    if (!this->m_textures) {
        CM2Model::ErrorSetFileLine(__FILE__, __LINE__);
        CM2Model::ErrorSet("Failed to allocate texture array: %s", this->m_fileName);
        return 0;
    }

    for (uint32_t i = 0; i < this->m_data->m_materials.m_count; i++) {
        M2Material* material = &this->m_data->m_materials[i];

        if (material->m_blendMode >= 5) {
            material->m_flags |= 1;
        }
    }

    for (uint32_t i = 0; i < this->m_data->m_textures.m_count; i++) {
        M2Texture* texture = &this->m_data->m_textures[i];

        if (texture->m_filename.m_count > 1) {
            CGxTexFlags texFlags;
            texFlags.m_filter = GxTex_LinearMipLinear;
            texFlags.m_wrapU = 1;
            texFlags.m_wrapV = 1;

            uint32_t flags = (this->m_flags & 4) ? 2 : 0;

            if (this->m_flags & 0x40) {
                flags |= 0x10;
            }

            this->m_textures[i] = TextureCreate(texture->m_filename.Data(), texFlags, &GetGlobalStatusObj(), flags, this->m_fileName, this->m_priority);
        } else {
            static const uint32_t white = 0xFFFFFFFF;
            this->m_textures[i] = TextureCreateSolid(*reinterpret_cast<const CImVector*>(&white));
        }
    }

    for (uint32_t i = 0; i < this->m_data->m_bones.m_count; i++) {
        if (this->m_data->m_bones[i].m_flags & 0x2F8) {
            this->m_billboardBoneCount++;
        }
    }

    return 1;
}

// OFFSET: 0x00838C60/0x140562A40
void CM2Shared::LoadSucceededCallback(CAsyncObject* object) {
    CM2Shared* shared = static_cast<CM2Shared*>(object->m_userArg);
    DefaultAsyncObjectCleanupCallback(object);
    shared->m_asyncObject = NULL;

    if (shared->m_flags & 1) {
        return;
    }

    if (!M2Init(reinterpret_cast<uint8_t*>(shared->m_data), shared->m_fileSize, *shared->m_data)) {
        DebugLog("Corrupt model data: %s", shared->m_fileName);
        CM2Model::ErrorSetFileLine(__FILE__, __LINE__);
        CM2Model::ErrorSet("Failed to initialize model: %s", shared->m_fileName);
        return;
    }

    if (!shared->Initialize()) {
        CM2Model::ErrorSetFileLine(__FILE__, __LINE__);
        CM2Model::ErrorSet("Failed to initialize model: %s", shared->m_fileName);
        return;
    }

    uint32_t count = 0;

    for (uint32_t i = 0; i < shared->m_data->m_sequences.m_count; i++) {
        if (!(shared->m_data->m_sequences[i].m_flags & 0x20)) {
            count++;
        }
    }

    if (count) {
        shared->m_lowPrioritySequenceBuffers.SetCount(count);
        memset(shared->m_lowPrioritySequenceBuffers.Ptr(), 0, sizeof(uint8_t*) * shared->m_lowPrioritySequenceBuffers.Count());
    }

    shared->m_flags |= 1;
    shared->m_lowPrioritySequenceCount = 0;
}

// INLINED
CM2SequenceLoad::CM2SequenceLoad() {
    this->m_asyncObject = NULL;
    this->m_shared = NULL;
    this->m_sequenceIndex = 0;
    this->m_sequenceBufferIndex = 0;
}

// OFFSET: 0x00832DF0/0x140555580
CM2SequenceLoad::~CM2SequenceLoad() {
    this->m_playbacks.Clear();

    if (this->m_asyncObject) {
        AsyncFileReadDestroyObject(this->m_asyncObject);
    }
}

// OFFSET: 0x00839050/0x1405631A0
int32_t CM2Shared::Load(SFileImpl* file, int32_t flags, CAaBox* boundingBox, uint8_t priority) {
    this->m_flags ^= (this->m_flags ^ (flags ? 4 : 0)) & 4;
    this->m_fileSize = SFileGetFileSize(file, NULL);
    this->m_data = static_cast<M2Data*>(SIMD::AllocA16(this->m_fileSize));
    this->m_priority = priority;

    if (!this->m_data) {
        return 0;
    }

    if (boundingBox) {
        this->m_boundingBox = *boundingBox;
    }

    AsyncFileReadCreateObject(file, priority, 0, &this->m_asyncObject, this->m_data, this, CM2Shared::LoadSucceededCallback, CM2Shared::LoadFailedCallback, 0, 0, 0);

    return 1;
}

// OFFSET: 0x008386E0/0x140562310
void CM2Shared::CancelAllDeferredSequences() {
    for (CM2SequenceLoad* load = this->m_sequenceLoads.Head(); !this->m_sequenceLoads.IsEnd(load); load = this->m_sequenceLoads.RawNext(load)) {
        if (load->m_asyncObject) {
            if (!AsyncFileReadCancel(load->m_asyncObject, AsyncFileReadFreeA16)) {
                this->m_lowPrioritySequenceBuffers[load->m_sequenceBufferIndex] = NULL;
            }

            load->m_asyncObject = NULL;
        }
    }

    if (this->m_flags & 0x10) {
        this->m_flags |= 0x20;
    } else {
        this->m_sequenceLoads.Clear();
    }
}

// OFFSET: 0x00838760/0x140562400
CM2Shared::~CM2Shared() {
    this->CancelAllDeferredSequences();

    int32_t pending = 0;

    if (this->m_asyncObject) {
        pending = !AsyncFileReadCancel(this->m_asyncObject, AsyncFileReadFreeA16);
    }

    if (this->m_cache) {
        if (this->m_previous) {
            *this->m_previous = this->m_next;
        }

        if (this->m_next) {
            this->m_next->m_previous = this->m_previous;
        }

        this->m_previous = NULL;
        this->m_next = NULL;
        this->m_updateLink.Unlink();
    }

    if (this->m_textures) {
        for (uint32_t i = 0; i < this->m_data->m_textures.m_count; i++) {
            if (this->m_textures[i]) {
                TextureRelease(this->m_textures[i]);
            }
        }

        SMemFree(this->m_textures, __FILE__, __LINE__, 0);
    }

    this->DestroyBuffers();

    if (this->m_skinSections) {
        SMemFree(this->m_skinSections, __FILE__, __LINE__, 0);
    }

    if (this->m_skinData) {
        if (pending) {
            pending = 0;
        } else {
            SIMD::FreeA16(this->m_skinData);
        }
    }

    if (this->m_lowPrioritySequenceBuffers.Count()) {
        for (uint32_t i = 0; i < this->m_lowPrioritySequenceCount; i++) {
            SIMD::FreeA16(this->m_lowPrioritySequenceBuffers[i]);
        }

        this->m_lowPrioritySequenceBuffers.Clear();
    }

    if (this->m_flags & 8) {
        delete[] this->m_data->m_vertices.Data();
    }

    if (!pending) {
        SIMD::FreeA16(this->m_data);
    }
}

// OFFSET: 0x00838C90/0x140562C00
void CM2Shared::LowPrioritySequenceLoadedCallback(CAsyncObject* object) {
    CM2SequenceLoad* load = static_cast<CM2SequenceLoad*>(object->m_userArg);
    CM2Shared* shared = load->m_shared;

    shared->m_flags |= 0x10;
    shared->FinishLoadingLowPrioritySequence(load->m_sequenceIndex, load->m_asyncObject);

    CM2SequencePlayBack* playback = load->m_playbacks.Head();

    while (!load->m_playbacks.IsEnd(playback)) {
        CM2SequencePlayBack* next = load->m_playbacks.RawNext(playback);

        if (playback->m_flags & 8) {
            load->m_playbacks.DeleteNode(playback);
            playback = next;
            continue;
        }

        CM2Model* model = playback->m_model;

        if (model->OnSequenceInterrupted(shared->m_data->m_bones[playback->m_boneIndex].m_boneId, playback->m_boneIndex)) {
            uint16_t boneIndex = playback->m_boneIndex;
            M2ModelBone* bone = &model->m_bones[boneIndex];

            if (playback->m_flags & 2) {
                M2Sequence* sequence = &shared->m_data->m_sequences[load->m_sequenceIndex];
                bone->m_sequenceId = sequence->m_id;
                bone->m_variationIndex = sequence->m_variationIndex;
                model->SetPrimaryBoneSequence(load->m_sequenceIndex, playback->m_boneIndex, playback->m_fallback, playback->m_time, playback->m_speed, playback->m_flags & 1, playback->m_repetitions, playback->m_blendTimeMode, playback->m_blendTime);
                bone->m_sequence.m_pickRandomVariation = playback->m_flags & 4;
            } else {
                model->SetSecondaryBoneSequence(load->m_sequenceIndex, boneIndex, playback->m_fallback, playback->m_time, playback->m_speed, playback->m_repetitions);
                bone->m_secondarySequence.m_pickRandomVariation = playback->m_flags & 4;
            }

            playback->m_model = NULL;
        }

        playback = next;
    }

    if (shared->m_flags & 0x20) {
        shared->m_sequenceLoads.Clear();
        shared->m_flags &= ~0x30u;
    } else {
        load->~CM2SequenceLoad();
        SMemFree(load, typeid(CM2SequenceLoad).raw_name(), SLOG_OBJECT, 0);
        shared->m_flags &= ~0x10u;
    }
}

// OFFSET: 0x008389B0/0x140562840
void CM2Shared::LowPrioritySequenceFailedCallback(CAsyncObject* object) {
    CM2SequenceLoad* load = static_cast<CM2SequenceLoad*>(object->m_userArg);
    load->~CM2SequenceLoad();
    SMemFree(load, typeid(CM2SequenceLoad).raw_name(), SLOG_OBJECT, 0);
}

// OFFSET: 0x00838E50/0x140562EB0
CM2SequenceLoad* CM2Shared::LoadLowPrioritySequence(uint16_t sequenceIndex) {
    M2Sequence* sequence = &this->m_data->m_sequences[sequenceIndex];
    CM2SequenceLoad* load = this->m_sequenceLoads.NewNode(2, 0, 0);

    if (!load) {
        return NULL;
    }

    load->m_sequenceIndex = sequenceIndex;
    load->m_shared = this;

    M2Sequence* source = sequence;

    while (source->m_flags & 0x40) {
        source = &this->m_data->m_sequences[source->m_aliasNext];
    }

    uint32_t id = source->m_id;
    uint32_t variation = source->m_variationIndex;
    char fileName[260];
    MakeAnimFileName(this->m_fileName, id, variation, fileName);

    SFileImpl* file = this->OpenLowPrioritySequenceFile(id, variation);

    if (!file) {
        this->m_sequenceLoads.DeleteNode(load);
        return NULL;
    }

    uint8_t* buffer = static_cast<uint8_t*>(SIMD::AllocA16(SFileGetFileSize(file, NULL)));

    if (!buffer) {
        this->m_sequenceLoads.DeleteNode(load);
        SFileCloseFile(file);
        return NULL;
    }

    load->m_sequenceBufferIndex = static_cast<uint16_t>(this->m_lowPrioritySequenceCount);
    this->m_lowPrioritySequenceBuffers[this->m_lowPrioritySequenceCount++] = buffer;

    sequence->m_flags |= 0x10;

    for (uint16_t i = sequence->m_aliasNext; i != sequenceIndex; i = this->m_data->m_sequences[i].m_aliasNext) {
        this->m_data->m_sequences[i].m_flags |= 0x10;
    }

    AsyncFileReadCreateObject(file, 1, 0, &load->m_asyncObject, buffer, load, CM2Shared::LowPrioritySequenceLoadedCallback, CM2Shared::LowPrioritySequenceFailedCallback, 0, 0, 0);

    return load;
}

// OFFSET: 0x00838C00/0x140562B40
void CM2Shared::Release() {
    if (this->m_refCount-- != 1) {
        return;
    }

    if (this->m_cache && this->m_previous) {
        this->m_freeTime = OsGetAsyncTimeMs();
        this->m_freePrev = this->m_cache->m_freeListTail;
        this->m_freeNext = NULL;
        *this->m_cache->m_freeListTail = this;
        this->m_cache->m_freeListTail = &this->m_freeNext;
    } else {
        delete this;
    }
}

struct M2SortCompare {
    int32_t (*m_compare)(uint32_t, uint32_t, const void*);
    const void* m_param;

    bool operator()(uint32_t a, uint32_t b) const;
};

// INLINED
bool M2SortCompare::operator()(uint32_t a, uint32_t b) const {
    return this->m_compare(a, b, this->m_param) < 0;
}

// OFFSET: 0x008395E0/0x140563A20
void M2HeapSort(int32_t (*compare)(uint32_t, uint32_t, const void*), uint32_t* array, uint32_t count, const void* param) {
    M2SortCompare predicate;
    predicate.m_compare = compare;
    predicate.m_param = param;
    blz::sort(array, array + count, predicate);
}
