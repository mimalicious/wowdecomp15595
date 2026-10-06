#include <cstddef>
#include <cstdint>
#include <cstring>
#include "WoW/Source/ObjectAlloc/ObjectAlloc.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/SLock.h"

// STATIC: 0x00D73258
static OBJALLOCGLOBALS s_globals;

// STATIC: 0x00D73268
static SCritSect s_globalsLock;

// INLINED
CObjectHeap::CObjectHeap() {
    this->m_obj = NULL;
    this->m_indexStack = NULL;
    this->m_allocated = 0;
}

// INLINED
CObjectHeap::CObjectHeap(const CObjectHeap& source) {
    this->m_obj = source.m_obj;
    this->m_indexStack = source.m_indexStack;
    const_cast<CObjectHeap&>(source).m_obj = NULL;
    const_cast<CObjectHeap&>(source).m_indexStack = NULL;
    this->m_allocated = source.m_allocated;
}

// INLINED
CObjectHeap::~CObjectHeap() {
    if (this->m_obj) {
        SMemFree(this->m_obj, __FILE__, __LINE__, 0);
    }
}

// INLINED
void CObjectHeap::Delete(uint32_t index, uint32_t objSize, uint32_t count) {
    if (this->m_allocated && this->m_indexStack) {
        this->m_indexStack[--this->m_allocated] = index;
    }
}

// INLINED
void* CObjectHeap::Ptr(uint32_t index, uint32_t objSize, uint32_t count) {
    return static_cast<char*>(this->m_obj) + index * objSize;
}

// INLINED
CObjectHeapList::CObjectHeapList() {
    this->m_objSize = 0;
    this->m_objsPerBlock = 128;
    this->m_numFullHeaps = 0;
    this->m_hasEmptyHeaps = 0;
    this->m_deleteCount = 0;
    this->m_fullestHeap = 0;
    this->m_freeEmptyHeaps = 1;
}

// OFFSET: 0x00486710
bool CObjectHeap::Allocate(uint32_t objSize, uint32_t count) {
    this->m_obj = SMemAlloc(count * (objSize + sizeof(uint32_t)), __FILE__, __LINE__, 0x2000000);

    if (!this->m_obj) {
        return false;
    }

    this->m_indexStack = reinterpret_cast<uint32_t*>(static_cast<char*>(this->m_obj) + count * objSize);

    for (uint32_t i = 0; i < count; i++) {
        this->m_indexStack[i] = i;
    }

    this->m_allocated = 0;

    return this->m_obj != NULL;
}

// OFFSET: %0x0048F420
OBJALLOCGLOBALS* GetObjAllocGlobals() {
    s_globalsLock.Enter();

    return &s_globals;
}

// OFFSET: %0x0048F430
void ReleaseObjAllocGlobals() {
    s_globalsLock.Leave();
}

// OFFSET: 0x00486910/0x1400A7980
void CObjectHeapList::FreeEmptyHeaps() {
    uint32_t count = this->m_heaps.Count();
    uint32_t allocated = 0;
    uint32_t capacity = 0;
    uint32_t emptyHeaps = 0;

    for (uint32_t i = 0; i < count; i++) {
        CObjectHeap& heap = this->m_heaps[i];

        if (heap.m_obj) {
            capacity += this->m_objsPerBlock;

            if (heap.m_allocated) {
                allocated += heap.m_allocated;
            } else {
                emptyHeaps++;
            }
        }
    }

    if (!emptyHeaps) {
        this->m_hasEmptyHeaps = 0;
        return;
    }

    uint32_t threshold = allocated + ((3 * this->m_objsPerBlock) >> 1);

    if (threshold < capacity) {
        for (uint32_t i = count; i > 0; i--) {
            if (threshold >= capacity || !emptyHeaps) {
                break;
            }

            CObjectHeap& heap = this->m_heaps[i - 1];

            if (heap.m_obj && !heap.m_allocated) {
                if (heap.m_obj) {
                    SMemFree(heap.m_obj, __FILE__, __LINE__, 0);
                }

                heap.m_obj = NULL;
                capacity -= this->m_objsPerBlock;
                emptyHeaps--;
                this->m_deleteCount = 0;
            }
        }
    }

    this->m_hasEmptyHeaps = emptyHeaps != 0;
}

// OFFSET: %0x0048F700
uint32_t CObjectHeapList::BlocksAllocated() {
    uint32_t allocated = 0;

    for (uint32_t i = 0; i < this->m_heaps.Count(); i++) {
        allocated += this->m_heaps[i].m_allocated;
    }

    return allocated;
}

// OFFSET: %0x0048F760
uint32_t CObjectHeapList::TotalHeaps() {
    uint32_t heaps = 0;

    for (uint32_t i = 0; i < this->m_heaps.Count(); i++) {
        if (this->m_heaps[i].m_obj) {
            heaps++;
        }
    }

    return heaps;
}

// OFFSET: %0x0048F780
uint32_t CObjectHeapList::PercentUsed() {
    uint32_t capacity = 0;
    uint32_t allocated = 0;

    for (uint32_t i = 0; i < this->m_heaps.Count(); i++) {
        if (this->m_heaps[i].m_obj) {
            capacity += this->m_objsPerBlock;
            allocated += this->m_heaps[i].m_allocated;
        }
    }

    return capacity ? 100 * allocated / capacity : 0;
}

// INLINED
int32_t CObjectHeap::New(uint32_t objSize, uint32_t count, uint32_t* index, void** ptr, bool zero) {
    if (this->m_allocated >= count) {
        return 0;
    }

    if (!this->m_obj) {
        this->Allocate(objSize, count);
    }

    *index = this->m_indexStack[this->m_allocated++];

    if (zero) {
        memset(static_cast<char*>(this->m_obj) + objSize * *index, 0, objSize);
    }

    if (ptr) {
        *ptr = static_cast<char*>(this->m_obj) + objSize * *index;
    }

    return 1;
}

// OFFSET: %0x0048F840
void* ObjectPtr(uint32_t heapId, uint32_t memHandle) {
    s_globalsLock.Enter();

    CObjectHeapList& list = s_globals.objects[heapId];
    void* ptr = list.m_heaps[memHandle / list.m_objsPerBlock].Ptr(memHandle % list.m_objsPerBlock, list.m_objSize, list.m_objsPerBlock);

    s_globalsLock.Leave();

    return ptr;
}

// OFFSET: 0x00486AC0/0x1400A7A90
void CObjectHeapList::Delete(uint32_t index) {
    uint32_t heapIndex = index / this->m_objsPerBlock;
    CObjectHeap& heap = this->m_heaps[heapIndex];

    if (heap.m_allocated == this->m_objsPerBlock) {
        this->m_numFullHeaps--;
    }

    heap.Delete(index % this->m_objsPerBlock, this->m_objSize, this->m_objsPerBlock);

    if (!this->m_freeEmptyHeaps) {
        return;
    }

    if (this->m_heaps[heapIndex].m_obj && !this->m_heaps[heapIndex].m_allocated) {
        this->m_hasEmptyHeaps = 1;
    }

    if (this->m_hasEmptyHeaps && !(this->m_deleteCount++ & ((this->m_objsPerBlock - 1) >> 2))) {
        this->FreeEmptyHeaps();
    }
}

// OFFSET: 0x00486B40/0x1400A7B30
static void HeapUse() {
    s_globalsLock.Enter();

    uint32_t count = s_globals.objects.Count();
    uint32_t used = 0;
    uint32_t allocated = 0;

    DebugLog("%u Heaps in use:\n", count);

    for (uint32_t i = 0; i < count; i++) {
        CObjectHeapList& list = s_globals.objects[i];

        DebugLog("    Heap %2u: %3u blocks, %5u objs per block (%6u bytes per obj): %6u objs allocated (%2d%%)\n", i, list.TotalHeaps(), list.m_objsPerBlock, list.m_objSize, list.BlocksAllocated(), list.PercentUsed());

        used += s_globals.objects[i].m_objSize * s_globals.objects[i].BlocksAllocated();
        allocated += s_globals.objects[i].TotalHeaps() * s_globals.objects[i].m_objSize * s_globals.objects[i].m_objsPerBlock;
    }

    DebugLog("%u total object bytes used, %u bytes allocated for heaps\n", used, allocated);

    s_globalsLock.Leave();
}

// PROVEN: 71ce0149 IDENTICAL/NONE
// OFFSET: 0x00486D40/0x1400A7D90
void ObjectFree(uint32_t heapId, uint32_t memHandle) {
    s_globalsLock.Enter();
    s_globals.objects[heapId].Delete(memHandle);
    s_globalsLock.Leave();
}

// OFFSET: 0x00486DE0
int32_t CObjectHeapList::New(uint32_t* index, void** ptr, bool zero) {
    CObjectHeap* heap = NULL;
    uint32_t count = this->m_heaps.Count();

    if (this->m_fullestHeap < count && this->m_heaps[this->m_fullestHeap].m_allocated != this->m_objsPerBlock && this->m_heaps[this->m_fullestHeap].m_obj) {
        heap = &this->m_heaps[this->m_fullestHeap];
    } else if (count == this->m_numFullHeaps) {
        this->m_fullestHeap = count;
        heap = this->m_heaps.New();

        if (!heap->Allocate(this->m_objSize, this->m_objsPerBlock)) {
            return 0;
        }
    } else {
        for (uint32_t i = 0; i < count; i++) {
            if (this->m_heaps[i].m_allocated != this->m_objsPerBlock) {
                this->m_fullestHeap = i;
                heap = &this->m_heaps[i];

                if (heap->m_obj) {
                    break;
                }
            }
        }
    }

    if (!heap->New(this->m_objSize, this->m_objsPerBlock, index, ptr, zero)) {
        return 0;
    }

    *index += this->m_fullestHeap * this->m_objsPerBlock;

    if (heap->m_allocated == this->m_objsPerBlock) {
        this->m_numFullHeaps++;
    }

    return 1;
}

// OFFSET: 0x00486F00
int32_t ObjectAlloc(uint32_t heapId, uint32_t* memHandle, void** objectPtr, bool zero) {
    if (memHandle) {
        *memHandle = 0;
        s_globalsLock.Enter();

        if (heapId < s_globals.objects.Count()) {
            uint32_t handle;
            void* ptr;

            if (s_globals.objects[heapId].New(&handle, &ptr, zero)) {
                if (objectPtr) {
                    *objectPtr = ptr;
                }

                s_globalsLock.Leave();
                *memHandle = handle;

                return 1;
            }

            s_globalsLock.Leave();

            return 0;
        }
    }

    SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);

    return 0;
}

// PROVEN: bae5ef52 IDENTICAL/NONE
// OFFSET: 0x00487110/0x1400A8420
void ObjectAllocDestroy() {
    HeapUse();

    s_globalsLock.Enter();
    s_globals.objects.Clear();
    s_globalsLock.Leave();
}

// OFFSET: 0x004871D0/0x1400A8530
uint32_t ObjectAllocAddHeap(uint32_t objectSize, uint32_t objectsPerBlock, const char* name, bool freeEmptyHeaps) {
    if (objectSize) {
        s_globalsLock.Enter();

        uint32_t heapId = s_globals.objects.Count();
        CObjectHeapList* list = s_globals.objects.New();
        list->m_objsPerBlock = objectsPerBlock;
        list->m_objSize = objectSize;
        list->m_freeEmptyHeaps = freeEmptyHeaps;

        s_globalsLock.Leave();

        return heapId;
    }

    SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);

    return 0;
}
