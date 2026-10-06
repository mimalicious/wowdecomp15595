#include <cstdint>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <malloc.h>
#include <new>
#include <typeinfo>
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
#include "Engine/Source/Os/OsMemory.h"

// STATIC: %0x00AF9700
static const uint32_t s_skinProfileBoneCounts[4] = { 256, 64, 53, 21 };

// STATIC: %0x00C0D300
static uint32_t s_sequenceIndex = 0xFFFFFFFF;

// STATIC: %0x00D8B73C
static uint8_t* s_sequenceData;

// STATIC: %0x00D8B740
static uint32_t s_sequenceSize;

// OFFSET: %0x00795650
static void AsyncFileReadFreeA16(CAsyncObject* object) {
    void* buffer = object->m_buffer;
    AsyncFileReadDestroyObject(object);
    SIMD::FreeA16(buffer);
}

#if defined(WOWDECOMP_ARCH_64)
static void AsyncFileReadFreeNear(CAsyncObject* object) {
    void* buffer = object->m_buffer;
    AsyncFileReadDestroyObject(object);
    OsFreeNear(buffer);
}
#endif

// OFFSET: %0x00795670
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

// OFFSET: %0x007956C0
int32_t CM2Shared::CallbackWhenLoaded(CM2Model* model) {
    if (!(model->m_flags & 0x20)) {
        if ((this->m_flags & 1) && (this->m_flags & 2)) {
            model->InitializeLoaded();
            return 1;
        }

        model->LinkToCallbackListTail();
    }

    return 1;
}

// OFFSET: %0x00795700
void CM2Shared::LoadFailedCallback(CAsyncObject* object) {
    CM2Shared* shared = static_cast<CM2Shared*>(object->m_userArg);
    AsyncFileReadDestroyObject(shared->m_asyncObject);
    shared->m_asyncObject = NULL;
}

// OFFSET: %0x00795720
static void MakeAnimFileName(const char* fileName, uint32_t id, uint32_t variation, char* result) {
    strcpy(result, fileName);
    *OsPathFindExtensionWithDot(result) = '\0';
    sprintf(&result[strlen(result)], "%04d-%02d.anim", id, variation);
}

// OFFSET: %0x00795780
static void MakeSkinFileName(const char* fileName, uint32_t profile, char* result) {
    strcpy(result, fileName);
    *OsPathFindExtensionWithDot(result) = '\0';
    sprintf(&result[strlen(result)], "%02d.skin", profile);
}

// OFFSET: [%0x00795CA0, %0x00795B20, %0x00795B70, %0x00795BC0, %0x00795AD0, %0x00795A80, %0x00795CF0]
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

// OFFSET: [%0x00796D30, %0x00796880, %0x00796B80, %0x00796C10, %0x00796CA0, %0x00796DC0, %0x00796E50]
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
                    track.m_sequenceTimes[i].Relocate(base);
                }
            }
        } else if (!M2Init(base, size, data, track.m_sequenceTimes)) {
            return 0;
        }
    } else if (s_sequenceIndex < track.m_sequenceTimes.m_count) {
        track.m_sequenceTimes[s_sequenceIndex].Relocate(s_sequenceData);
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
                    track.m_sequenceTimes[i].Relocate(base);
                }
            }
        } else if (!M2Init(base, size, data, track.m_sequenceTimes)) {
            return 0;
        }

        if (track.m_loopIndex == 0xFFFF) {
            track.m_sequenceKeys.Relocate(base);

            for (uint32_t i = 0; i < track.m_sequenceTimes.m_count; i++) {
                if (data.m_sequences[i].m_flags & 0x20) {
                    track.m_sequenceKeys[i].Relocate(base);
                }
            }
        } else if (!M2Init(base, size, data, track.m_sequenceKeys)) {
            return 0;
        }
    } else {
        if (s_sequenceIndex < track.m_sequenceTimes.m_count) {
            track.m_sequenceTimes[s_sequenceIndex].Relocate(s_sequenceData);
        }

        if (s_sequenceIndex < track.m_sequenceKeys.m_count) {
            track.m_sequenceKeys[s_sequenceIndex].Relocate(s_sequenceData);
        }
    }

    return 1;
}

// OFFSET: %0x00795C10
template <>
int32_t M2Init<M2Sequence>(uint8_t* base, uint32_t size, const M2Data& data, M2Array<M2Sequence>& array) {
    if (s_sequenceIndex == 0xFFFFFFFF) {
        if (array.m_offset > size || array.m_offset + sizeof(M2Sequence) * array.m_count > size) {
            return 0;
        }

        array.Relocate(base);
    }

    for (uint32_t i = 0; i < array.m_count; i++) {
        M2Sequence* sequence = &array[i];

        if (data.m_version < 0x109 && (sequence->m_flags & 1)) {
            sequence->m_flags |= 0x80;
        }
    }

    return 1;
}

// OFFSET: %0x00795DB0
void CM2Shared::ConvertTextureCombosToValues() {
    for (uint32_t i = 0; i < this->m_skinData->m_batches.m_count; i++) {
        M2Batch* batch = &this->m_skinData->m_batches[i];

        if (batch->m_textureCount == 2) {
            uint16_t* combos = &this->m_data->m_textureCombos[batch->m_textureComboIndex];
            uint16_t* transforms = &this->m_data->m_textureTransformCombos[batch->m_textureTransformComboIndex];
            uint8_t transform0 = transforms[0] == 0xFFFF ? 0 : static_cast<uint8_t>(transforms[0] + 1);
            uint8_t transform1 = transforms[1] == 0xFFFF ? 0 : static_cast<uint8_t>(transforms[1] + 1);
            batch->m_textureComboIndex = static_cast<uint16_t>((combos[0] & 0xFF) | (combos[1] << 8));
            batch->m_textureTransformComboIndex = static_cast<uint16_t>(transform0 | (transform1 << 8));
        } else {
            uint16_t combo = this->m_data->m_textureCombos[batch->m_textureComboIndex];
            uint16_t transform = this->m_data->m_textureTransformCombos[batch->m_textureTransformComboIndex];
            batch->m_textureComboIndex = combo;
            batch->m_textureTransformComboIndex = transform == 0xFFFF ? 0 : static_cast<uint16_t>(transform + 1);
        }
    }
}

// OFFSET: %0x00795EA0
static void ConvertTextureComboEntry(TextureComboList* list, int32_t adjust, uint16_t value) {
    uint16_t first = value & 0xFF;
    uint16_t second = value >> 8;

    if (adjust) {
        first = first ? first - 1 : 0xFFFF;
        second = second ? second - 1 : 0xFFFF;
    }

    uint32_t count = list->m_count;
    int32_t last = static_cast<int32_t>(count) - 1;

    for (int32_t i = 0; i < last; i++) {
        if (list->m_data[i] == first && list->m_data[i + 1] == second) {
            return;
        }
    }

    uint32_t index = count;

    for (int32_t i = 0; i < last; i += 2) {
        if (list->m_data[i] > first || (list->m_data[i] == first && list->m_data[i + 1] > second)) {
            index = i;
            break;
        }
    }

    if (count + 2 != count) {
        list->m_count = count + 2;
    }

    memmove(&list->m_data[index + 2], &list->m_data[index], sizeof(uint16_t) * (count - index));
    list->m_data[index] = first;
    list->m_data[index + 1] = second;
}

// OFFSET: %0x00795FA0
static uint16_t FindTextureComboEntry(M2Array<uint16_t>& list, int32_t adjust, uint16_t value) {
    uint16_t first = value & 0xFF;
    uint16_t second = value >> 8;

    if (adjust) {
        first = first ? first - 1 : 0xFFFF;
        second = second ? second - 1 : 0xFFFF;
    }

    if (list.m_count == 1) {
        return 0;
    }

    uint16_t index = 0;

    for (uint16_t* entry = list.Data(); entry[0] != first || entry[1] != second; entry++) {
        if (++index >= list.m_count - 1) {
            return 0;
        }
    }

    return index;
}

// OFFSET: %0x00796010
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

        int32_t sharedVertices = (this->m_cache->m_flags & 8) || (this->m_data->m_bones.m_count == 1 && (this->m_cache->m_flags & 0x40));
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

// OFFSET: %0x00796220
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

// OFFSET: %0x007963A0
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

// OFFSET: %0x00796910
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

// OFFSET: %0x00796500
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

// OFFSET: %0x007965D0
void CM2Shared::SubstituteSimpleShaders() {
    for (uint32_t i = 0; i < this->m_skinData->m_batches.m_count; i++) {
        M2Batch* batch = &this->m_skinData->m_batches[i];

        if (batch->m_shader & 0x8000) {
            continue;
        }

        M2Material* material = &this->m_data->m_materials[batch->m_materialIndex];

        if (!(this->m_data->m_flags & 8)) {
            uint16_t coord = this->m_data->m_textureCoordCombos[batch->m_textureCoordComboIndex];
            uint16_t op = material->m_blendMode != 0;

            if (coord > 2) {
                op |= 8;
            }

            batch->m_shader = op << 4;

            if (coord == 1) {
                batch->m_shader |= 0x4000;
            }
        } else {
            uint16_t combiner = batch->m_shader;
            uint32_t ops[2] = { 0, 0 };
            batch->m_shader = 0;

            for (uint32_t j = 0; j < batch->m_textureCount; j++) {
                uint32_t op = this->m_data->m_textureCombinerCombos[combiner + j];

                if (j == 0 && !material->m_blendMode) {
                    op = 0;
                }

                uint16_t coord = this->m_data->m_textureCoordCombos[batch->m_textureCoordComboIndex + j];
                ops[j] = op;

                if (coord > 2) {
                    ops[j] = op | 8;
                }

                if (coord == 1 && j + 1 == batch->m_textureCount) {
                    batch->m_shader |= 0x4000;
                }
            }

            batch->m_shader |= ops[1] | (ops[0] << 4);
        }
    }
}

// OFFSET: %0x007967E0
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

// OFFSET: %0x007957D0
CShaderEffect* CM2Shared::GetEffect(M2Batch* batch) {
    uint16_t shader = batch->m_shader;

    if (shader == 0x801D) {
        return NULL;
    }

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

// OFFSET: %0x007969C0
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

// OFFSET: %0x00796A40
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

// OFFSET: %0x00796EE0
void CM2Shared::ConvertTextureValuesToCombos() {
    uint32_t count = this->m_skinData->m_batches.m_count;

    TextureComboList combos;
    combos.m_data = static_cast<uint16_t*>(_alloca(sizeof(uint16_t) * 2 * count));
    combos.m_count = 0;

    TextureComboList transforms;
    transforms.m_data = static_cast<uint16_t*>(_alloca(sizeof(uint16_t) * 2 * count));
    transforms.m_count = 0;

    uint32_t materialIndex = 0xFFFFFFFF;

    for (uint32_t i = 0; i < this->m_skinData->m_batches.m_count; i++) {
        M2Batch* batch = &this->m_skinData->m_batches[i];

        if (batch->m_materialIndex != materialIndex) {
            materialIndex = batch->m_materialIndex;

            if (batch->m_textureCount >= 2) {
                ConvertTextureComboEntry(&combos, 0, batch->m_textureComboIndex);
                ConvertTextureComboEntry(&transforms, 1, batch->m_textureTransformComboIndex);
            }
        }
    }

    materialIndex = 0xFFFFFFFF;

    for (uint32_t i = 0; i < this->m_skinData->m_batches.m_count; i++) {
        M2Batch* batch = &this->m_skinData->m_batches[i];

        if (batch->m_materialIndex == materialIndex) {
            continue;
        }

        materialIndex = batch->m_materialIndex;

        if (batch->m_textureCount > 1) {
            continue;
        }

        uint16_t* combo = combos.m_data;
        uint16_t* combosEnd = combos.m_data + combos.m_count;

        while (combo != combosEnd && *combo != batch->m_textureComboIndex) {
            combo++;
        }

        if (combo == combosEnd) {
            new (&combos.m_data[combos.m_count++]) uint16_t(batch->m_textureComboIndex);
        }

        uint16_t transform = batch->m_textureTransformComboIndex ? batch->m_textureTransformComboIndex - 1 : 0xFFFF;
        uint16_t* entry = transforms.m_data;
        uint16_t* transformsEnd = transforms.m_data + transforms.m_count;

        while (entry != transformsEnd && *entry != transform) {
            entry++;
        }

        if (entry == transformsEnd) {
            new (&transforms.m_data[transforms.m_count++]) uint16_t(transform);
        }
    }

#if defined(WOWDECOMP_ARCH_64)
    uint32_t comboBytes = combos.m_count > this->m_data->m_textureCombos.m_count ? sizeof(uint16_t) * combos.m_count : 0;
    uint32_t transformBytes = transforms.m_count > this->m_data->m_textureTransformCombos.m_count ? sizeof(uint16_t) * transforms.m_count : 0;

    if (comboBytes || transformBytes) {
        uint32_t size = (this->m_fileSize + 15) & ~15u;
        M2Data* data = static_cast<M2Data*>(SIMD::AllocA16(size + comboBytes + transformBytes));
        memcpy(data, this->m_data, this->m_fileSize);
        SIMD::FreeA16(this->m_data);
        this->m_data = data;
        this->m_fileSize = size + comboBytes + transformBytes;
        uint8_t* tail = reinterpret_cast<uint8_t*>(data) + size;

        if (comboBytes) {
            data->m_textureCombos.Set(reinterpret_cast<uint16_t*>(tail));
            tail += comboBytes;
        }

        if (transformBytes) {
            data->m_textureTransformCombos.Set(reinterpret_cast<uint16_t*>(tail));
        }
    }
#else
    if (combos.m_count > this->m_data->m_textureCombos.m_count) {
        this->m_data->m_textureCombos.Set(static_cast<uint16_t*>(SMemAlloc(sizeof(uint16_t) * combos.m_count, __FILE__, __LINE__, 0)));
        this->m_data->m_flags |= 0x20;
    }

    if (transforms.m_count > this->m_data->m_textureTransformCombos.m_count) {
        this->m_data->m_textureTransformCombos.Set(static_cast<uint16_t*>(SMemAlloc(sizeof(uint16_t) * transforms.m_count, __FILE__, __LINE__, 0)));
        this->m_data->m_flags |= 0x40;
    }
#endif

    memcpy(this->m_data->m_textureCombos.Data(), combos.m_data, sizeof(uint16_t) * combos.m_count);
    this->m_data->m_textureCombos.m_count = combos.m_count;

    memcpy(this->m_data->m_textureTransformCombos.Data(), transforms.m_data, sizeof(uint16_t) * transforms.m_count);
    this->m_data->m_textureTransformCombos.m_count = transforms.m_count;
}

// OFFSET: %0x00797120
void CM2Shared::AssignBatchTextureComboIndices() {
    uint32_t materialIndex = 0xFFFFFFFF;

    for (uint32_t i = 0; i < this->m_skinData->m_batches.m_count; i++) {
        M2Batch* batch = &this->m_skinData->m_batches[i];

        if (batch->m_materialIndex == materialIndex) {
            continue;
        }

        materialIndex = batch->m_materialIndex;

        uint16_t transformIndex;

        if (batch->m_textureCount == 2) {
            M2Array<uint16_t>& combos = this->m_data->m_textureCombos;
            uint16_t first = batch->m_textureComboIndex & 0xFF;
            uint16_t second = batch->m_textureComboIndex >> 8;
            uint16_t comboIndex = 0;
            uint32_t j;

            for (j = 0; j < combos.m_count - 1; j++) {
                if (combos[j] == first && combos[j + 1] == second) {
                    break;
                }
            }

            if (j < combos.m_count - 1) {
                comboIndex = static_cast<uint16_t>(j);
            } else {
                DebugLog("Failed to fix up texture combo!\n");
            }

            batch->m_textureComboIndex = comboIndex;

            transformIndex = FindTextureComboEntry(this->m_data->m_textureTransformCombos, 1, batch->m_textureTransformComboIndex);
        } else {
            uint16_t* combo = this->m_data->m_textureCombos.Data();
            uint16_t* combosEnd = combo + this->m_data->m_textureCombos.m_count;

            while (combo != combosEnd && *combo != batch->m_textureComboIndex) {
                combo++;
            }

            batch->m_textureComboIndex = static_cast<uint16_t>(combo - this->m_data->m_textureCombos.Data());

            uint16_t transform = batch->m_textureTransformComboIndex ? batch->m_textureTransformComboIndex - 1 : 0xFFFF;
            uint16_t* entry = this->m_data->m_textureTransformCombos.Data();
            uint16_t* transformsEnd = entry + this->m_data->m_textureTransformCombos.m_count;

            while (entry != transformsEnd && *entry != transform) {
                entry++;
            }

            transformIndex = static_cast<uint16_t>(entry - this->m_data->m_textureTransformCombos.Data());
        }

        batch->m_textureTransformComboIndex = transformIndex;
    }
}

// OFFSET: %0x007972A0
void CM2Shared::SubstituteSpecializedShaders() {
    M2SkinProfile* skinData = this->m_skinData;

    if (!skinData->m_indices.m_count) {
        return;
    }

    uint32_t layered;

    for (layered = 0; layered < skinData->m_batches.m_count; layered++) {
        if (skinData->m_batches[layered].m_materialLayer) {
            break;
        }
    }

    if (layered >= skinData->m_batches.m_count) {
        return;
    }

    int8_t state0 = 0;
    int8_t state1 = 0;

    this->ConvertTextureCombosToValues();

    M2Batch* base = NULL;
    uint32_t materialIndex = 0xFFFFFFFF;
    bool sharedMaterials = false;
    bool combined = false;

    for (uint32_t i = 0; i < this->m_skinData->m_batches.m_count; i++) {
        M2Batch* batch = &this->m_skinData->m_batches[i];

        if (batch->m_materialIndex == materialIndex) {
            sharedMaterials = true;
            continue;
        }

        uint32_t op = batch->m_shader & 7;
        M2Material* material = &this->m_data->m_materials[batch->m_materialIndex];
        materialIndex = batch->m_materialIndex;

        if (!batch->m_materialLayer) {
            state0 = 0;
            state1 = 0;

            if (batch->m_textureCount && !material->m_blendMode) {
                batch->m_shader &= 0xFF8F;
            }

            base = &this->m_skinData->m_batches[i];
        }

        switch (state0) {
            case 1:
                if ((material->m_blendMode == 2 || material->m_blendMode == 1) && batch->m_textureCount == 1 && !((material->m_flags ^ this->m_data->m_materials[base->m_materialIndex].m_flags) & 1) && batch->m_textureComboIndex == (base->m_textureComboIndex & 0xFF) && this->m_data->m_textureWeightCombos[base->m_textureWeightComboIndex] == this->m_data->m_textureWeightCombos[batch->m_textureWeightComboIndex]) {
                    batch->m_shader = 0x801D;
                    base->m_shader = 0x8000;
                    state0 = 3;
                    continue;
                }

                state0 = 0;
            case 0:
                if (!material->m_blendMode && batch->m_textureCount == 2 && (op == 4 || op == 6)) {
                    uint16_t* coords = &this->m_data->m_textureCoordCombos[batch->m_textureCoordComboIndex];

                    if (!coords[0] && coords[1] > 2) {
                        state0 = 1;
                    }
                }
                break;
            default:
                break;
        }

        switch (state1) {
            case 0:
                break;
            case 1:
                if ((material->m_blendMode == 4 || material->m_blendMode == 6) && batch->m_textureCount == 1 && this->m_data->m_textureCoordCombos[batch->m_textureCoordComboIndex] > 2 && this->m_data->m_textureWeightCombos[base->m_textureWeightComboIndex] == this->m_data->m_textureWeightCombos[batch->m_textureWeightComboIndex]) {
                    batch->m_shader = 0x801D;
                    combined = true;
                    base->m_shader = material->m_blendMode != 4 ? 0xE : 0x8001;
                    base->m_textureCount = 2;
                    state1 = 2;
                    base->m_textureComboIndex = static_cast<uint16_t>((base->m_textureComboIndex & 0xFF) | (batch->m_textureComboIndex << 8));
                    base->m_textureTransformComboIndex = static_cast<uint16_t>((base->m_textureTransformComboIndex & 0xFF) | (batch->m_textureTransformComboIndex << 8));
                    continue;
                }

                state1 = 0;
                break;
            case 2:
                if ((material->m_blendMode == 2 || material->m_blendMode == 1) && batch->m_textureCount == 1 && !((material->m_flags ^ this->m_data->m_materials[base->m_materialIndex].m_flags) & 1) && (base->m_textureComboIndex & 0xFF) == (batch->m_textureComboIndex & 0xFF) && this->m_data->m_textureWeightCombos[base->m_textureWeightComboIndex] == this->m_data->m_textureWeightCombos[batch->m_textureWeightComboIndex]) {
                    batch->m_shader = 0x801D;
                    state1 = 3;
                    base->m_shader = base->m_shader == 0x8001 ? 0x8002 : 0x8000;
                    continue;
                }

                state1 = 0;
                break;
            default:
                continue;
        }

        if (!material->m_blendMode && batch->m_textureCount == 1 && !this->m_data->m_textureCoordCombos[batch->m_textureCoordComboIndex]) {
            state1 = 1;
        }
    }

    if (combined) {
        this->ConvertTextureValuesToCombos();
    }

    this->AssignBatchTextureComboIndices();

    if (sharedMaterials) {
        materialIndex = 0xFFFFFFFF;

        for (uint32_t i = 0; i < this->m_skinData->m_batches.m_count; i++) {
            M2Batch* batch = &this->m_skinData->m_batches[i];

            if (batch->m_materialIndex == materialIndex) {
                batch->m_shader = batch[-1].m_shader;
                batch->m_textureCount = batch[-1].m_textureCount;
                batch->m_textureComboIndex = batch[-1].m_textureComboIndex;
                batch->m_textureTransformComboIndex = batch[-1].m_textureTransformComboIndex;
            } else {
                materialIndex = batch->m_materialIndex;
            }
        }
    }
}

// OFFSET: %0x007976C0
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

    if (this->m_data->m_version < 0x109) {
        this->SubstituteSimpleShaders();
        this->SubstituteSpecializedShaders();
    }

    if (this->m_skinData->m_indices.m_count) {
        for (uint32_t i = 0; i < this->m_skinData->m_batches.m_count; i++) {
            this->m_effects[i] = this->GetEffect(&this->m_skinData->m_batches[i]);
        }
    }

    return 1;
}

// OFFSET: %0x007978D0
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

// OFFSET: %0x00797C90
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

// OFFSET: %0x00797E80
int32_t CM2Shared::FinishLoadingSkinProfile(uint32_t size) {
    if (this->m_flags & 2) {
        return 1;
    }

    M2Data* data = this->m_data;
    M2SkinProfile* skinData = this->m_skinData;
    uint8_t* base = reinterpret_cast<uint8_t*>(skinData);

    if (!M2Init(base, size, *data, skinData->m_vertices) || !M2Init(base, size, *data, skinData->m_indices) || !M2Init(base, size, *data, skinData->m_bones) || !M2Init(base, size, *data, skinData->m_skinSections) || !M2Init(base, size, *data, skinData->m_batches) || ((data->m_flags & 0x80) && !M2Init(base, size, *data, skinData->m_shadowBatches))) {
        DebugLog("Corrupt skin profile data: %s", this->m_fileName);
        return 0;
    }

    if (!this->InitializeSkinProfile()) {
        DebugLog("Failed to initialize model skin profile: %s", this->m_fileName);
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

// OFFSET: %0x00797F20
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

// OFFSET: %0x00798490
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

// OFFSET: %0x00798A10
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

// OFFSET: %0x00798C00
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

// OFFSET: %0x00799890
template <>
int32_t M2Init<M2CameraOld>(uint8_t* base, uint32_t size, const M2Data& data, M2Array<M2CameraOld>& array) {
    if (s_sequenceIndex == 0xFFFFFFFF) {
        if (array.m_offset > size || array.m_offset + sizeof(M2CameraOld) * array.m_count > size) {
            return 0;
        }

        array.Relocate(base);
    }

    for (uint32_t i = 0; i < array.m_count; i++) {
        M2CameraOld* camera = &array[i];

        if (!M2Init(base, size, data, camera->m_positions) || !M2Init(base, size, data, camera->m_targetPositions) || !M2Init(base, size, data, camera->m_roll)) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: %0x00799E00
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

// OFFSET: %0x0079A520
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
int32_t M2Init(uint8_t* base, uint32_t size, const M2Data& data, M2ParticleOld& particle) {
    return M2Init(base, size, data, particle.m_geometryMdl) && M2Init(base, size, data, particle.m_recursionMdl) && M2Init(base, size, data, particle.m_speedTrack) && M2Init(base, size, data, particle.m_variationTrack) && M2Init(base, size, data, particle.m_latitudeTrack) && M2Init(base, size, data, particle.m_longitudeTrack) && M2Init(base, size, data, particle.m_gravityTrack) && M2Init(base, size, data, particle.m_lifeTrack) && M2Init(base, size, data, particle.m_emissionRateTrack) && M2Init(base, size, data, particle.m_widthTrack) && M2Init(base, size, data, particle.m_lengthTrack) && M2Init(base, size, data, particle.m_zsourceTrack) && M2Init(base, size, data, particle.m_colorTrack.m_times) && M2Init(base, size, data, particle.m_colorTrack.m_values) && M2Init(base, size, data, particle.m_alphaTrack.m_times) && M2Init(base, size, data, particle.m_alphaTrack.m_values) && M2Init(base, size, data, particle.m_scaleTrack.m_times) && M2Init(base, size, data, particle.m_scaleTrack.m_values) && M2Init(base, size, data, particle.m_headCellTrack.m_times) && M2Init(base, size, data, particle.m_headCellTrack.m_values) && M2Init(base, size, data, particle.m_tailCellTrack.m_times) && M2Init(base, size, data, particle.m_tailCellTrack.m_values) && M2Init(base, size, data, particle.m_spline) && M2Init(base, size, data, particle.m_visibilityTrack);
}

// OFFSET: %0x0079B080
template <>
int32_t M2Init<M2ParticleOld>(uint8_t* base, uint32_t size, const M2Data& data, M2Array<M2ParticleOld>& array) {
    if (s_sequenceIndex == 0xFFFFFFFF) {
        if (array.m_offset > size || array.m_offset + sizeof(M2ParticleOld) * array.m_count > size) {
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
int32_t M2Init(uint8_t* base, uint32_t size, const M2Data& data, M2Particle& particle) {
    return M2Init(base, size, data, particle.m_geometryMdl) && M2Init(base, size, data, particle.m_recursionMdl) && M2Init(base, size, data, particle.m_speedTrack) && M2Init(base, size, data, particle.m_variationTrack) && M2Init(base, size, data, particle.m_latitudeTrack) && M2Init(base, size, data, particle.m_longitudeTrack) && M2Init(base, size, data, particle.m_gravityTrack) && M2Init(base, size, data, particle.m_lifeTrack) && M2Init(base, size, data, particle.m_emissionRateTrack) && M2Init(base, size, data, particle.m_widthTrack) && M2Init(base, size, data, particle.m_lengthTrack) && M2Init(base, size, data, particle.m_zsourceTrack) && M2Init(base, size, data, particle.m_colorTrack.m_times) && M2Init(base, size, data, particle.m_colorTrack.m_values) && M2Init(base, size, data, particle.m_alphaTrack.m_times) && M2Init(base, size, data, particle.m_alphaTrack.m_values) && M2Init(base, size, data, particle.m_scaleTrack.m_times) && M2Init(base, size, data, particle.m_scaleTrack.m_values) && M2Init(base, size, data, particle.m_headCellTrack.m_times) && M2Init(base, size, data, particle.m_headCellTrack.m_values) && M2Init(base, size, data, particle.m_tailCellTrack.m_times) && M2Init(base, size, data, particle.m_tailCellTrack.m_values) && M2Init(base, size, data, particle.m_spline) && M2Init(base, size, data, particle.m_visibilityTrack);
}

// OFFSET: %0x0079C6D0
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

// OFFSET: %0x0079DD20
CM2Shared::CM2Shared(CM2Cache* cache) {
    this->m_flags &= ~0x7Fu;
    this->m_cache = cache;
    this->m_callbackList = NULL;
    this->m_refCount = 1;
    this->m_asyncObject = NULL;
    this->m_callbackListTail = &this->m_callbackList;
    this->m_lowPrioritySequenceCapacity = 0;
    this->m_lowPrioritySequenceBuffers = NULL;
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

// INLINED
template <class T>
static void M2CopyArray(M2Array<T>& dest, const M2Array<T>& source) {
    dest.m_count = source.m_count;
    dest.Set(source.m_count ? source.Data() : NULL);
}

// INLINED
template <class T>
static void M2CopyTrack(M2Track<T>& dest, const M2Track<T>& source) {
    dest.m_trackType = source.m_trackType;
    dest.m_loopIndex = source.m_loopIndex;
    M2CopyArray(dest.m_sequenceTimes, source.m_sequenceTimes);
    M2CopyArray(dest.m_sequenceKeys, source.m_sequenceKeys);
}

// INLINED
template <class T>
static void M2CopyPartTrack(M2PartTrack<T>& dest, const M2PartTrack<T>& source) {
    M2CopyArray(dest.m_times, source.m_times);
    M2CopyArray(dest.m_values, source.m_values);
}

// INLINED
static void M2CopyParticleArrays(M2ParticleOld& dest, const M2ParticleOld& source) {
    M2CopyArray(dest.m_geometryMdl, source.m_geometryMdl);
    M2CopyArray(dest.m_recursionMdl, source.m_recursionMdl);
    M2CopyTrack(dest.m_speedTrack, source.m_speedTrack);
    M2CopyTrack(dest.m_variationTrack, source.m_variationTrack);
    M2CopyTrack(dest.m_latitudeTrack, source.m_latitudeTrack);
    M2CopyTrack(dest.m_longitudeTrack, source.m_longitudeTrack);
    M2CopyTrack(dest.m_gravityTrack, source.m_gravityTrack);
    M2CopyTrack(dest.m_lifeTrack, source.m_lifeTrack);
    M2CopyTrack(dest.m_emissionRateTrack, source.m_emissionRateTrack);
    M2CopyTrack(dest.m_widthTrack, source.m_widthTrack);
    M2CopyTrack(dest.m_lengthTrack, source.m_lengthTrack);
    M2CopyTrack(dest.m_zsourceTrack, source.m_zsourceTrack);
    M2CopyPartTrack(dest.m_colorTrack, source.m_colorTrack);
    M2CopyPartTrack(dest.m_alphaTrack, source.m_alphaTrack);
    M2CopyPartTrack(dest.m_scaleTrack, source.m_scaleTrack);
    M2CopyPartTrack(dest.m_headCellTrack, source.m_headCellTrack);
    M2CopyPartTrack(dest.m_tailCellTrack, source.m_tailCellTrack);
    M2CopyArray(dest.m_spline, source.m_spline);
    M2CopyTrack(dest.m_visibilityTrack, source.m_visibilityTrack);
}

// OFFSET: %0x0079E160
int32_t M2Init(uint8_t* base, uint32_t size, M2Data& data) {
    if (reinterpret_cast<uint8_t*>(&data) < base || reinterpret_cast<uint8_t*>(&data + 1) > base + size) {
        return 0;
    }

    if (data.m_magic != 0x3032444D) {
        return 0;
    }

    if (data.m_version < 0x108 || data.m_version > 0x110) {
        return 0;
    }

    if (!M2Init(base, size, data, data.m_name) || !M2Init(base, size, data, data.m_loops) || !M2Init(base, size, data, data.m_sequences) || !M2Init(base, size, data, data.m_sequenceIdxHashById) || !M2Init(base, size, data, data.m_bones) || !M2Init(base, size, data, data.m_boneIndicesById) || !M2Init(base, size, data, data.m_vertices) || !M2Init(base, size, data, data.m_colors) || !M2Init(base, size, data, data.m_textures) || !M2Init(base, size, data, data.m_textureWeights) || !M2Init(base, size, data, data.m_textureTransforms) || !M2Init(base, size, data, data.m_textureIndicesById) || !M2Init(base, size, data, data.m_materials) || !M2Init(base, size, data, data.m_boneCombos) || !M2Init(base, size, data, data.m_textureCombos) || (data.m_version < 0x109 && !M2Init(base, size, data, data.m_textureCoordCombos))) {
        return 0;
    }

    if (!M2Init(base, size, data, data.m_textureWeightCombos) || !M2Init(base, size, data, data.m_textureTransformCombos) || !M2Init(base, size, data, data.m_collisionIndices) || !M2Init(base, size, data, data.m_collisionPositions) || !M2Init(base, size, data, data.m_collisionFaceNormals) || !M2Init(base, size, data, data.m_attachments) || !M2Init(base, size, data, data.m_attachmentIndicesById) || !M2Init(base, size, data, data.m_events) || !M2Init(base, size, data, data.m_lights)) {
        return 0;
    }

    if (data.m_version >= 0x110 || (data.m_flags & 0x100)) {
        if (!M2Init(base, size, data, data.m_cameras)) {
            return 0;
        }
    } else {
        M2Array<M2CameraOld>& oldCameras = *reinterpret_cast<M2Array<M2CameraOld>*>(&data.m_cameras);

        if (!M2Init(base, size, data, oldCameras)) {
            return 0;
        }

        if (oldCameras.m_count) {
            uint32_t bytes = (sizeof(M2Camera) + 32) * oldCameras.m_count;
#if defined(WOWDECOMP_ARCH_64)
            uint8_t* memory = static_cast<uint8_t*>(OsAllocNear(&data, bytes));
#else
            uint8_t* memory = static_cast<uint8_t*>(SMemAlloc(bytes, __FILE__, __LINE__, 0));
#endif
            data.m_flags |= 0x100;

            M2Camera* cameras = reinterpret_cast<M2Camera*>(memory);
            uint8_t* tracks = memory + sizeof(M2Camera) * oldCameras.m_count;

            for (uint32_t i = 0; i < oldCameras.m_count; i++) {
                M2CameraOld* source = &oldCameras[i];
                M2Camera* camera = &cameras[i];

                camera->m_type = source->m_type;
                camera->m_farClip = source->m_farClip;
                camera->m_nearClip = source->m_nearClip;
                M2CopyTrack(camera->m_positions, source->m_positions);
                camera->m_positionBase = source->m_positionBase;
                M2CopyTrack(camera->m_targetPositions, source->m_targetPositions);
                camera->m_targetPositionBase = source->m_targetPositionBase;
                M2CopyTrack(camera->m_roll, source->m_roll);

                M2Array<uint32_t>* times = reinterpret_cast<M2Array<uint32_t>*>(tracks);
                M2Array<M2SplineKey<float> >* keys = reinterpret_cast<M2Array<M2SplineKey<float> >*>(tracks + 8);
                M2SplineKey<float>* key = reinterpret_cast<M2SplineKey<float>*>(tracks + 20);

                camera->m_fieldOfView.m_trackType = 0;
                camera->m_fieldOfView.m_loopIndex = 0xFFFF;
                camera->m_fieldOfView.m_sequenceTimes.m_count = 1;
                camera->m_fieldOfView.m_sequenceTimes.Set(times);
                camera->m_fieldOfView.m_sequenceKeys.m_count = 1;
                camera->m_fieldOfView.m_sequenceKeys.Set(keys);

                times->m_count = 1;
                times->Set(reinterpret_cast<uint32_t*>(tracks + 16));
                keys->m_count = 1;
                keys->Set(key);

                key->m_value = source->m_fieldOfView;
                key->m_inTan = 0.0f;
                key->m_outTan = 0.0f;

                tracks += 32;
            }

            data.m_cameras.Set(cameras);
        }
    }

    if (!M2Init(base, size, data, data.m_cameraIndicesById) || !M2Init(base, size, data, data.m_ribbons)) {
        return 0;
    }

    if (data.m_version >= 0x110 || (data.m_flags & 0x200)) {
        if (!M2Init(base, size, data, data.m_particles)) {
            return 0;
        }

        return !(data.m_flags & 8) || M2Init(base, size, data, data.m_textureCombinerCombos);
    }

    M2Array<M2ParticleOld>& oldParticles = *reinterpret_cast<M2Array<M2ParticleOld>*>(&data.m_particles);

    if (!M2Init(base, size, data, oldParticles)) {
        return 0;
    }

    if (oldParticles.m_count) {
        uint32_t bytes = sizeof(M2Particle) * oldParticles.m_count;
#if defined(WOWDECOMP_ARCH_64)
        M2Particle* particles = static_cast<M2Particle*>(OsAllocNear(&data, bytes));
#else
        M2Particle* particles = static_cast<M2Particle*>(SMemAlloc(bytes, __FILE__, __LINE__, 0));
#endif
        data.m_flags |= 0x200;

        for (uint32_t i = 0; i < oldParticles.m_count; i++) {
            M2Particle* particle = &particles[i];
            memcpy(particle, &oldParticles[i], sizeof(M2ParticleOld));
            M2CopyParticleArrays(*particle, oldParticles[i]);
            particle->m_multiTextureParam0[2] = 0;
            particle->m_multiTextureParam0[3] = 0;
            particle->m_multiTextureParam1[0] = 0;
            particle->m_multiTextureParam1[1] = 0;
            particle->m_multiTextureParam1[2] = 0;
            particle->m_multiTextureParam1[3] = 0;
        }

        data.m_particles.Set(particles);
    }

    return !(data.m_flags & 8) || M2Init(base, size, data, data.m_textureCombinerCombos);
}

// OFFSET: %0x0079E740
int32_t CM2Shared::FinishLoadingLowPrioritySequence(uint32_t sequenceIndex, uint8_t* buffer, uint32_t size) {
    s_sequenceData = buffer;
    s_sequenceIndex = sequenceIndex;
    s_sequenceSize = size;

    int32_t result = M2Init(reinterpret_cast<uint8_t*>(this->m_data), this->m_fileSize, *this->m_data);
    M2Sequence* sequence = &this->m_data->m_sequences[sequenceIndex];

    s_sequenceIndex = 0xFFFFFFFF;
    s_sequenceData = NULL;
    s_sequenceSize = 0;

    if (!result) {
        DebugLog("Corrupt model sequence data: %s (%d)", this->m_fileName, sequence->m_id);
        return 0;
    }

    sequence->m_flags = (sequence->m_flags & ~0x30u) | 0x20;

    return 1;
}

// OFFSET: %0x0079E7E0
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

// OFFSET: %0x0079DE20
void CM2Shared::SkinProfileLoadedCallback(CAsyncObject* object) {
    CM2Shared* shared = static_cast<CM2Shared*>(object->m_userArg);
    shared->FinishLoadingSkinProfile(shared->m_asyncObject->m_size);
    AsyncFileReadDestroyObject(shared->m_asyncObject);
    shared->m_asyncObject = NULL;
}

// OFFSET: %0x0079DE50
int32_t CM2Shared::LoadSkinProfile(uint32_t profile) {
    char fileName[260];
    MakeSkinFileName(this->m_fileName, profile, fileName);

    SFileImpl* file;

    if (!SFile::OpenEx(NULL, fileName, (this->m_flags & 4) >> 2, &file)) {
        DebugLog("Model2: File not found: %s\n", fileName);
        return 0;
    }

    uint32_t size = SFileGetFileSize(file, NULL);
    this->m_skinData = static_cast<M2SkinProfile*>(SIMD::AllocA16(size));

    if (!this->m_skinData) {
        SFileCloseFile(file);
        return 0;
    }

    this->m_asyncObject = AsyncFileReadAllocObject();

    if (!this->m_asyncObject) {
        SFileCloseFile(file);
        SIMD::FreeA16(this->m_skinData);
        this->m_skinData = NULL;
        return 0;
    }

    this->m_asyncObject->m_file = file;
    this->m_asyncObject->m_buffer = this->m_skinData;
    this->m_asyncObject->m_size = size;
    this->m_asyncObject->m_userArg = this;
    this->m_asyncObject->m_userPostloadCallback = &CM2Shared::SkinProfileLoadedCallback;
    this->m_asyncObject->m_userPostUnloadCallback = &CM2Shared::LoadFailedCallback;
    this->m_asyncObject->m_isRead = 0;
    this->m_asyncObject->m_isProcessed = 0;
    this->m_asyncObject->m_priority = 125;
    AsyncFileReadObject(this->m_asyncObject, 1);

    return 1;
}

// OFFSET: %0x00795D40
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

// OFFSET: %0x0079DF70
int32_t CM2Shared::Initialize() {
    this->m_skinData = NULL;

    uint32_t profile = 0;

    if (this->m_cache->m_flags & 8) {
        profile = CM2Shared::GetSkinProfileIndex(this->m_data->m_numSkinProfiles);

        if (profile == 0xFFFFFFFF) {
            DebugLog("Failed to choose skin profile: %s", this->m_fileName);
            return 0;
        }
    }

    if (!this->LoadSkinProfile(profile)) {
        DebugLog("Failed to load skin profile: %s", this->m_fileName);
        return 0;
    }

    this->m_textures = static_cast<CTexture**>(SMemAlloc(sizeof(CTexture*) * this->m_data->m_textures.m_count, __FILE__, __LINE__, 8));

    if (!this->m_textures) {
        DebugLog("Failed to allocate texture array: %s", this->m_fileName);
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
                flags |= 0x30;
            }

            this->m_textures[i] = TextureCreate(texture->m_filename.Data(), texFlags, &GetGlobalStatusObj(), flags, NULL, 2);
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

// INLINED
int32_t CM2Shared::FinishLoading() {
    if (this->m_flags & 1) {
        return 1;
    }

    if (!M2Init(reinterpret_cast<uint8_t*>(this->m_data), this->m_fileSize, *this->m_data)) {
        DebugLog("Corrupt model data: %s", this->m_fileName);
        return 0;
    }

    if (!this->Initialize()) {
        DebugLog("Failed to initialize model: %s", this->m_fileName);
        return 0;
    }

    uint32_t count = 0;

    for (uint32_t i = 0; i < this->m_data->m_sequences.m_count; i++) {
        if (!(this->m_data->m_sequences[i].m_flags & 0x20)) {
            count++;
        }
    }

    this->m_lowPrioritySequenceBuffers = static_cast<uint8_t**>(SMemAlloc(sizeof(uint8_t*) * count, __FILE__, __LINE__, 8));
    this->m_lowPrioritySequenceCapacity = count;
    this->m_flags |= 1;
    this->m_lowPrioritySequenceCount = 0;

    return 1;
}

// OFFSET: %0x0079E990
void CM2Shared::LoadSucceededCallback(CAsyncObject* object) {
    CM2Shared* shared = static_cast<CM2Shared*>(object->m_userArg);
    AsyncFileReadDestroyObject(shared->m_asyncObject);
    shared->m_asyncObject = NULL;
    shared->FinishLoading();
}

// INLINED
CM2SequenceLoad::CM2SequenceLoad() {
    this->m_asyncObject = NULL;
    this->m_shared = NULL;
    this->m_sequenceIndex = 0;
    this->m_sequenceBufferIndex = 0;
}

// OFFSET: %0x0079E9B0
CM2SequenceLoad::~CM2SequenceLoad() {
    this->m_playbacks.Clear();

    if (this->m_asyncObject) {
        AsyncFileReadDestroyObject(this->m_asyncObject);
    }
}

// OFFSET: %0x0079EA50
int32_t CM2Shared::Load(SFileImpl* file, int32_t flags, CAaBox* boundingBox) {
    this->m_flags = (this->m_flags & ~4u) | (flags ? 4 : 0);
    this->m_fileSize = SFileGetFileSize(file, NULL);
    this->m_data = static_cast<M2Data*>(SIMD::AllocA16(this->m_fileSize));

    if (!this->m_data) {
        return 0;
    }

    if (boundingBox) {
        this->m_boundingBox = *boundingBox;
    }

    this->m_asyncObject = AsyncFileReadAllocObject();

    if (!this->m_asyncObject) {
        return 0;
    }

    this->m_asyncObject->m_file = file;
    this->m_asyncObject->m_buffer = this->m_data;
    this->m_asyncObject->m_size = this->m_fileSize;
    this->m_asyncObject->m_userArg = this;
    this->m_asyncObject->m_userPostloadCallback = &CM2Shared::LoadSucceededCallback;
    this->m_asyncObject->m_userPostUnloadCallback = &CM2Shared::LoadFailedCallback;
    this->m_asyncObject->m_isRead = 0;
    this->m_asyncObject->m_isProcessed = 0;
    AsyncFileReadObject(this->m_asyncObject, 0);

    return 1;
}

// OFFSET: %0x0079EB80
void CM2Shared::CancelAllDeferredSequences() {
    for (CM2SequenceLoad* load = this->m_sequenceLoads.Head(); !this->m_sequenceLoads.IsEnd(load); load = this->m_sequenceLoads.RawNext(load)) {
        if (load->m_asyncObject) {
#if defined(WOWDECOMP_ARCH_64)
            if (!AsyncFileReadCancel(load->m_asyncObject, AsyncFileReadFreeNear)) {
#else
            if (!AsyncFileReadCancel(load->m_asyncObject, AsyncFileReadFreeA16)) {
#endif
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

// OFFSET: %0x0079EBF0
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

    if (this->m_lowPrioritySequenceCapacity) {
        for (uint32_t i = 0; i < this->m_lowPrioritySequenceCount; i++) {
#if defined(WOWDECOMP_ARCH_64)
            OsFreeNear(this->m_lowPrioritySequenceBuffers[i]);
#else
            SIMD::FreeA16(this->m_lowPrioritySequenceBuffers[i]);
#endif
        }

        SMemFree(this->m_lowPrioritySequenceBuffers, __FILE__, __LINE__, 0);
        this->m_lowPrioritySequenceBuffers = NULL;
        this->m_lowPrioritySequenceCapacity = 0;
    }

    if (this->m_flags & 8) {
#if defined(WOWDECOMP_ARCH_64)
        OsFreeNear(this->m_data->m_vertices.Data());
#else
        delete[] this->m_data->m_vertices.Data();
#endif
    }

    if (this->m_flags & 1) {
#if defined(WOWDECOMP_ARCH_64)
        if (this->m_data->m_flags & 0x100) {
            OsFreeNear(this->m_data->m_cameras.Data());
        }

        if (this->m_data->m_flags & 0x200) {
            OsFreeNear(this->m_data->m_particles.Data());
        }
#else
        if (this->m_data->m_flags & 0x20) {
            SMemFree(this->m_data->m_textureCombos.Data(), __FILE__, __LINE__, 0);
        }

        if (this->m_data->m_flags & 0x40) {
            SMemFree(this->m_data->m_textureTransformCombos.Data(), __FILE__, __LINE__, 0);
        }

        if (this->m_data->m_flags & 0x100) {
            SMemFree(this->m_data->m_cameras.Data(), __FILE__, __LINE__, 0);
        }

        if (this->m_data->m_flags & 0x200) {
            SMemFree(this->m_data->m_particles.Data(), __FILE__, __LINE__, 0);
        }
#endif
    }

    if (!pending) {
        SIMD::FreeA16(this->m_data);
    }
}

// OFFSET: %0x0079EEB0
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
                model->SetPrimaryBoneSequence(load->m_sequenceIndex, playback->m_boneIndex, playback->m_fallback, playback->m_time, playback->m_speed, playback->m_flags & 1, playback->m_repetitions);
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

// OFFSET: %0x0079F040
void CM2Shared::LowPrioritySequenceFailedCallback(CAsyncObject* object) {
    CM2SequenceLoad* load = static_cast<CM2SequenceLoad*>(object->m_userArg);
    load->~CM2SequenceLoad();
    SMemFree(load, typeid(CM2SequenceLoad).raw_name(), SLOG_OBJECT, 0);
}

// OFFSET: %0x0079F070
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

    char fileName[260];
    MakeAnimFileName(this->m_fileName, source->m_id, source->m_variationIndex, fileName);

    SFileImpl* file;

    if (!SFile::OpenEx(NULL, fileName, (this->m_flags & 4) >> 2, &file)) {
        DebugLog("Model2: File not found: %s\n", fileName);
        this->m_sequenceLoads.DeleteNode(load);
        return NULL;
    }

    uint32_t size = SFileGetFileSize(file, NULL);
#if defined(WOWDECOMP_ARCH_64)
    uint8_t* buffer = static_cast<uint8_t*>(OsAllocNear(this->m_data, size));
#else
    uint8_t* buffer = static_cast<uint8_t*>(SIMD::AllocA16(size));
#endif

    if (!buffer) {
        this->m_sequenceLoads.DeleteNode(load);
        SFileCloseFile(file);
        return NULL;
    }

    load->m_asyncObject = AsyncFileReadAllocObject();

    if (!load->m_asyncObject) {
        SFileCloseFile(file);
#if defined(WOWDECOMP_ARCH_64)
        OsFreeNear(buffer);
#else
        SIMD::FreeA16(buffer);
#endif
        this->m_sequenceLoads.DeleteNode(load);
        return NULL;
    }

    load->m_sequenceBufferIndex = static_cast<uint16_t>(this->m_lowPrioritySequenceCount);
    this->m_lowPrioritySequenceBuffers[this->m_lowPrioritySequenceCount++] = buffer;

    sequence->m_flags |= 0x10;

    for (uint16_t i = sequence->m_aliasNext; i != sequenceIndex; i = this->m_data->m_sequences[i].m_aliasNext) {
        this->m_data->m_sequences[i].m_flags |= 0x10;
    }

    load->m_asyncObject->m_file = file;
    load->m_asyncObject->m_buffer = buffer;
    load->m_asyncObject->m_size = size;
    load->m_asyncObject->m_userArg = load;
    load->m_asyncObject->m_userPostloadCallback = &CM2Shared::LowPrioritySequenceLoadedCallback;
    load->m_asyncObject->m_userPostUnloadCallback = &CM2Shared::LowPrioritySequenceFailedCallback;
    load->m_asyncObject->m_isRead = 0;
    load->m_asyncObject->m_isProcessed = 0;
    load->m_asyncObject->m_priority = 125;
    AsyncFileReadObject(load->m_asyncObject, 0);

    return load;
}

// OFFSET: %0x0079F2D0
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

// OFFSET: %0x0079F9D0
void M2HeapSort(int32_t (*compare)(uint32_t, uint32_t, const void*), uint32_t* array, uint32_t count, const void* param) {
    M2SortCompare predicate;
    predicate.m_compare = compare;
    predicate.m_param = param;
    std::sort(array, array + count, predicate);
}
