#ifndef ENGINE_SOURCE_MODEL2_M2SHARED_H
#define ENGINE_SOURCE_MODEL2_M2SHARED_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Storm/H/stpl.h"

class CAsyncObject;
struct CGxBuf;
struct CGxPool;
class CM2Cache;
class CM2Model;
class CM2SequencePlayBack;
class CM2Shared;
class CShaderEffect;
class CTexture;
class SFileImpl;

class TextureComboList {
    public:
    /* 0000 */ uint32_t m_count;
    /* 0004 */ uint16_t* m_data;
};

class CM2SequenceLoad : public TSLinkedNode<CM2SequenceLoad> {
    public:
    /* 0008 */ CAsyncObject* m_asyncObject;
    /* 000C */ CM2Shared* m_shared;
    /* 0010 */ uint16_t m_sequenceIndex;
    /* 0012 */ uint16_t m_sequenceBufferIndex;
    /* 0014 */ TSList<CM2SequencePlayBack> m_playbacks;

    CM2SequenceLoad();
    ~CM2SequenceLoad();
};

class CM2Shared {
    public:
    /* 0000 */ uint32_t m_refCount;
    /* 0004 */ CM2Cache* m_cache;
    /* 0008 */ uint32_t m_flags;
    /* 000C */ uint8_t m_priority;
    /* 0010 */ CAsyncObject* m_asyncObject;
    /* 0014 */ CM2Model* m_callbackList;
    /* 0018 */ CM2Model** m_callbackListTail;
    /* 001C */ TSList<CM2SequenceLoad> m_sequenceLoads;
    /* 0028 */ TSGrowableArray<uint8_t*> m_lowPrioritySequenceBuffers;
    /* 0038 */ uint32_t m_lowPrioritySequenceCount;
    /* 003C */ CM2Shared** m_freePrev;
    /* 0040 */ CM2Shared* m_freeNext;
    /* 0044 */ uint32_t m_freeTime;
    /* 0048 */ char m_fileName [260];
    /* 014C */ char* m_fileNameWithoutPath;
    /* 0150 */ CM2Shared** m_previous;
    /* 0154 */ CM2Shared* m_next;
    /* 0158 */ uint32_t m_fileNameHash;
    /* 015C */ M2Data* m_data;
    /* 0160 */ CAaBox m_boundingBox;
    /* 0178 */ uint32_t m_fileSize;
    /* 017C */ M2SkinProfile* m_skinData;
    /* 0180 */ CTexture** m_textures;
    /* 0184 */ CGxPool* m_indexPool;
    /* 0188 */ CGxBuf* m_indexBuf;
    /* 018C */ CGxPool* m_vertexPool;
    /* 0190 */ CGxBuf* m_vertexBuf;
    /* 0194 */ CShaderEffect** m_effects;
    /* 0198 */ M2SkinSection* m_skinSections;
    /* 019C */ uint32_t m_instances;
    /* 01A0 */ uint32_t m_maxInstances;
    /* 01A4 */ uint16_t m_billboardBoneCount;
    /* 01A8 */ uint32_t m_lastUsedTime;
    /* 01AC */ TSLink<CM2Shared> m_updateLink;

    static void LoadFailedCallback(CAsyncObject* object);
    static void LoadSucceededCallback(CAsyncObject* object);
    static void LowPrioritySequenceFailedCallback(CAsyncObject* object);
    static void LowPrioritySequenceLoadedCallback(CAsyncObject* object);
    static void SkinProfileLoadedCallback(CAsyncObject* object);

    CM2Shared(CM2Cache* cache);
    ~CM2Shared();
    uint32_t AddRef();
    uint32_t AllocInstances(uint32_t count, int32_t exact);
    int32_t CallbackWhenLoaded(CM2Model* model);
    void CancelAllDeferredSequences();
    void DestroyBuffers();
    int32_t FinishLoadingLowPrioritySequence(uint32_t sequenceIndex, uint8_t* buffer, uint32_t size);
    int32_t FinishLoadingLowPrioritySequence(uint32_t sequenceIndex, CAsyncObject* object);
    int32_t FinishLoadingSkinProfile(uint32_t size);
    static uint32_t GetSkinProfileIndex(uint32_t numSkinProfiles);
    CShaderEffect* GetEffect(M2Batch* batch);
    int32_t Initialize();
    int32_t InitializeSkinProfile();
    int32_t Load(SFileImpl* file, int32_t flags, CAaBox* boundingBox, uint8_t priority);
    CM2SequenceLoad* LoadLowPrioritySequence(uint16_t sequenceIndex);
    SFileImpl* OpenLowPrioritySequenceFile(uint32_t id, uint32_t variation);
    SFileImpl* OpenSkinFile(uint32_t profile);
    int32_t LoadSkinProfile(uint32_t profile);
    void Release();
    int32_t SetIndices();
    int32_t SetVertices(uint32_t texCoordIndex, EGxVertexBufferFormat format);
    int32_t SetVerticesFixedFunction(uint32_t texCoordIndex);
    int32_t SetVerticesShader(EGxVertexBufferFormat format);
};

template <class T>
int32_t M2Init(uint8_t* base, uint32_t size, const M2Data& data, M2Array<T>& array);

template <class T>
int32_t M2Init(uint8_t* base, uint32_t size, const M2Data& data, M2Array<M2Array<T> >& array);

template <class T>
int32_t M2Init(uint8_t* base, uint32_t size, const M2Data& data, M2Track<T>& track);

int32_t M2Init(uint8_t* base, uint32_t size, const M2Data& data, M2TrackBase& track);
int32_t M2Init(uint8_t* base, uint32_t size, const M2Data& data, M2SkinProfile& skinData);
int32_t M2Init(uint8_t* base, uint32_t size, M2Data& data);
void M2HeapSort(int32_t (*compare)(uint32_t, uint32_t, const void*), uint32_t* array, uint32_t count, const void* param);

#endif
