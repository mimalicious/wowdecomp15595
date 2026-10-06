// INLINED
template <class T>
TSObjectAllocPool<T>::TSObjectAllocPool(const char* name, uint32_t objectsPerBlock) {
    this->m_liveCount = 0;
    this->m_allocCount = 0;
    ObjectAllocRegisterPool(this);
    this->m_objectSize = sizeof(T);
    this->m_name = name;
    this->m_heapId = ObjectAllocAddHeap(sizeof(T), objectsPerBlock, name, true);
}

// INLINED
template <class T>
TSObjectAllocPool<T>::~TSObjectAllocPool() {
    ObjectAllocUnregisterPool(this);
}

// INLINED
template <class T>
void TSObjectAllocPool<T>::Delete(T* object, const char* file, int32_t line) {
    object->~T();
    OsInterlockedAdd64(&this->m_liveCount, -1);
    ObjectFree(this->m_heapId, object->m_handle);
}

// INLINED
template <class T>
T* TSObjectAllocPool<T>::New(const char* file, int32_t line) {
    uint32_t memHandle;
    void* memory = NULL;

    if (!ObjectAlloc(this->m_heapId, &memHandle, &memory, false)) {
        return NULL;
    }

    T* object = memory ? new (memory) T() : NULL;
    OsInterlockedAdd64(&this->m_liveCount, 1);
    OsInterlockedAdd64(&this->m_allocCount, 1);
    object->m_handle = memHandle;

    return object;
}
