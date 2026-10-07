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

// OFFSET: [0x007479A0/0x1404268C0<CMapObjOccluder>]
template <class T>
void TSObjectAllocPool<T>::Delete(T* object, const char* file, int32_t line) {
    object->~T();
    OsInterlockedAdd64(&this->m_liveCount, -1);
    ObjectFree(this->m_heapId, object->m_handle);
}

// OFFSET: [0x00747900<CMapObjOccluder>, 0x00775940/0x140460020<CMapObjGroup>, 0x007759C0<CMapObj>, 0x00775A40/0x140460100<CMapArea>, 0x00775AC0<CMapAreaLow>, 0x00775B40<CMapChunk>, 0x00775BC0<CMapRenderChunk>, 0x00775C40/0x1404602C0<CMapChunkBuf>, 0x00775CC0<CMapDoodadDef>, 0x00775D40/0x1404603A0<CMapEntity>, 0x00775DC0/0x140460410<CMapObjDefGroup>, 0x00775E40<CMapObjDef>, 0x00775EC0/0x1404604F0<CChunkLiquid>, 0x007765E0/0x140460C90<CMapBaseObjLink>, 0x00739330<CDetailDoodadInstBatch>, 0x00739C20<CDetailDoodadInst>]
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
