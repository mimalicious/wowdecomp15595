// OFFSET: [0x004C6180<WDATASTOREBUFFER<768>>, 0x004C61D0<WDATASTOREBUFFER<16384>>, 0x004C6220<WDATASTOREBUFFER<262208>>, 0x00704130<CACHEENTRY>, 0x009A3670<EQUIPMENT_SET>]
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

// OFFSET: [0x007479A0/0x1404268C0<CMapObjOccluder>, 0x00825DE0/0x1405437E0<CM2Model>, 0x004C5F70<WDATASTOREBUFFER<16384>>]
template <class T>
void TSObjectAllocPool<T>::Delete(T* object, const char* file, int32_t line) {
    object->~T();
    OsInterlockedAdd64(&this->m_liveCount, -1);
    ObjectFree(this->m_heapId, object->m_handle, object, file, line);
}

// OFFSET: [0x00747900/0x140426500<CMapObjOccluder>, 0x00775940/0x140460020<CMapObjGroup>, 0x007759C0<CMapObj>, 0x00775A40/0x140460100<CMapArea>, 0x00775AC0/0x140460170<CMapAreaLow>, 0x00775B40/0x1404601E0<CMapChunk>, 0x00775BC0/0x140460250<CMapRenderChunk>, 0x00775C40/0x1404602C0<CMapChunkBuf>, 0x00775CC0/0x140460330<CMapDoodadDef>, 0x00775D40/0x1404603A0<CMapEntity>, 0x00775DC0/0x140460410<CMapObjDefGroup>, 0x00775E40/0x140460480<CMapObjDef>, 0x00775EC0/0x1404604F0<CChunkLiquid>, 0x007765E0/0x140460C90<CMapBaseObjLink>, 0x00739330/0x140415D90<CDetailDoodadInstBatch>, 0x00739C20/0x140417910<CDetailDoodadInst>, 0x008282A0/0x140547400<CM2Model>, 0x004C5E80/0x1400A7FE0<WDATASTOREBUFFER<768>>, 0x005EB310/0x14026C7B0<CVehicle_C>, 0x00701960/0x1403CEC00<CCharacterComponent>, 0x00704180/0x1403D0B50<CACHEENTRY>, 0x00588DC0/0x1401F2490<CMissile>, 0x009A1530/0x140726650<EQUIPMENT_SET>, 0x009F49E0<CGResearchFrame::BRANCH>, 0x009F4A50<CGResearchFrame::PROJECT>, 0x0057D6F0/0x1401E3940<CMissileCollision>, 0x005CAC50/0x1402483D0<UnitThreat>, 0x005DF7D0/0x14025CB70<CGObject_C::FadeoutInfo>, 0x005E92F0/0x14026A130<CVehicleCamera_C>, 0x005F1270/0x1402738D0<CVehiclePassenger_C>]
template <class T>
T* TSObjectAllocPool<T>::New(const char* file, int32_t line) {
    uint32_t memHandle;
    void* memory = NULL;

    if (!ObjectAlloc(this->m_heapId, &memHandle, &memory, false, file, line)) {
        return NULL;
    }

    T* object = static_cast<T*>(memory);

    if (object) {
        new (object) T();
    }

    OsInterlockedAdd64(&this->m_liveCount, 1);
    OsInterlockedAdd64(&this->m_allocCount, 1);
    object->m_handle = memHandle;

    return object;
}

// OFFSET: [0x00814F30/0x14052D100<CM2Model>, 0x005F16F0<CVehiclePassenger_C>]
template <class T>
void TSObjectAllocPool<T>::Delete(T* object) {
    this->Delete(object, "", 0);
}
