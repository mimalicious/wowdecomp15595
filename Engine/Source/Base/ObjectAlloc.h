#ifndef ENGINE_SOURCE_BASE_OBJECTALLOC_H
#define ENGINE_SOURCE_BASE_OBJECTALLOC_H

#include <cstddef>
#include <cstdint>
#include <new>
#include "Engine/Source/Os/Atomic.h"
#include "Engine/Source/Storm/H/stpl.h"

class ObjectAllocPool {
    public:
    /* 0000 */ TSLink<ObjectAllocPool> m_link;
    /* 0008 */ int64_t m_liveCount;
    /* 0010 */ int64_t m_allocCount;
    /* 0018 */ size_t m_objectSize;
    /* 001C */ const char* m_name;
    /* 0020 */ uint32_t m_heapId;
};

template <class T>
class TSObjectAllocPool : public ObjectAllocPool {
    public:
    TSObjectAllocPool(const char* name, uint32_t objectsPerBlock);
    ~TSObjectAllocPool();
    void Delete(T* object);
    void Delete(T* object, const char* file, int32_t line);
    T* New(const char* file, int32_t line);
};

int32_t ObjectAlloc(uint32_t heapId, uint32_t* memHandle, void** objectPtr, bool zero, const char* file, int32_t line);
uint32_t ObjectAllocAddHeap(uint32_t objectSize, uint32_t objectsPerBlock, const char* name, bool zero);
void ObjectAllocRegisterPool(ObjectAllocPool* pool);
void ObjectAllocUnregisterPool(ObjectAllocPool* pool);
void ObjectFree(uint32_t heapId, uint32_t memHandle, void* object, const char* file, int32_t line);
void* ObjectPtr(uint32_t heapId, uint32_t memHandle);

#include "Engine/Source/Base/ObjectAlloc.inl"

#endif
