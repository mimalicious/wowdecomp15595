#include "WDataStore.h"
#include <cstring>
#include "Engine/Source/Base/ObjectAlloc.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00DBE220
static TSObjectAllocPool<WDATASTOREBUFFER<0x300> >* s_smallBufferHeap;

// STATIC: 0x00DBE224
static TSObjectAllocPool<WDATASTOREBUFFER<0x4000> >* s_largeBufferHeap;

// STATIC: 0x00DBE228
static TSObjectAllocPool<WDATASTOREBUFFER<0x40040> >* s_superBufferHeap;

// STATIC: 0x00DBE22C/0x140EE2558
static bool s_initialized;

// OFFSET: 0x0044EC60
WDataStore::WDataStore() {
    this->InternalInitialize(this->m_data, this->m_base, this->m_alloc);
}

// OFFSET: 0x0044ECB0
WDataStore::~WDataStore() {
    if (this->m_alloc != -1) {
        this->InternalDestroy(this->m_data, this->m_base, this->m_alloc);
    }
}

// PROVEN: 4e102741 IDENTICAL/DIFFERENT
// OFFSET: 0x004C5D50/0x14000A800
void WDataStore::InternalInitialize(uint8_t*& data, uint32_t& base, uint32_t& alloc) {
    this->m_bufferObj = NULL;
    alloc = 0;
    data = NULL;
    base = 0;
    this->m_chunkSize = 256;
}

// OFFSET: 0x004C5D80/0x140100F50
uint8_t* WDataStore::GetHeaderSpace(uint32_t bytes) {
    if (!this->m_bufferObj || bytes > sizeof(this->m_bufferObj->m_header)) {
        return NULL;
    }

    return reinterpret_cast<uint8_t*>(this->m_bufferObj + 1) - bytes;
}

// OFFSET: 0x004C5EF0/0x140101040
void* WDataStore::AllocBuffer(uint32_t size) {
    if (size <= 0x300) {
        return s_smallBufferHeap->New(__FILE__, __LINE__)->m_data;
    }

    if (size <= 0x4000) {
        return s_largeBufferHeap->New(__FILE__, __LINE__)->m_data;
    }

    if (size <= 0x40040) {
        return s_superBufferHeap->New(__FILE__, __LINE__)->m_data;
    }

    return SMemAlloc(size, __FILE__, __LINE__, 0);
}

// OFFSET: 0x004C5FB0/0x140101180
void WDataStore::InternalDestroy(uint8_t*& data, uint32_t& base, uint32_t& alloc) {
    if (this->m_bufferObj) {
        if (alloc == 0x300) {
            s_smallBufferHeap->Delete(static_cast<WDATASTOREBUFFER<0x300>*>(this->m_bufferObj), __FILE__, __LINE__);
            this->m_bufferObj = NULL;
        } else if (alloc == 0x4000) {
            s_largeBufferHeap->Delete(static_cast<WDATASTOREBUFFER<0x4000>*>(this->m_bufferObj), __FILE__, __LINE__);
            this->m_bufferObj = NULL;
        } else if (alloc == 0x40040) {
            s_superBufferHeap->Delete(static_cast<WDATASTOREBUFFER<0x40040>*>(this->m_bufferObj), __FILE__, __LINE__);
            this->m_bufferObj = NULL;
        }
    } else if (data) {
        SMemFree(data, __FILE__, __LINE__, 0);
    }

    data = NULL;
    base = 0;
    alloc = 0;
}

// OFFSET: 0x004C60A0
void WDataStore::FreeBuffer(void* buffer, uint32_t size) {
    if (size <= 0x300) {
        s_smallBufferHeap->Delete(reinterpret_cast<WDATASTOREBUFFER<0x300>*>(static_cast<uint8_t*>(buffer) - sizeof(WDATASTOREBUFFERHEADER)), __FILE__, __LINE__);
    } else if (size <= 0x4000) {
        s_largeBufferHeap->Delete(reinterpret_cast<WDATASTOREBUFFER<0x4000>*>(static_cast<uint8_t*>(buffer) - sizeof(WDATASTOREBUFFERHEADER)), __FILE__, __LINE__);
    } else if (size <= 0x40040) {
        s_superBufferHeap->Delete(reinterpret_cast<WDATASTOREBUFFER<0x40040>*>(static_cast<uint8_t*>(buffer) - sizeof(WDATASTOREBUFFERHEADER)), __FILE__, __LINE__);
    } else {
        SMemFree(buffer, __FILE__, __LINE__, 0);
    }
}

// OFFSET: 0x004C6270/0x140101370
void WDataStore::StaticInitialize() {
    if (s_initialized) {
        return;
    }

    s_smallBufferHeap = new (__FILE__, __LINE__) TSObjectAllocPool<WDATASTOREBUFFER<0x300> >("WDataStoreSmallBuffer", 256);
    s_largeBufferHeap = new (__FILE__, __LINE__) TSObjectAllocPool<WDATASTOREBUFFER<0x4000> >("WDataStoreLargeBuffer", 16);
    s_superBufferHeap = new (__FILE__, __LINE__) TSObjectAllocPool<WDATASTOREBUFFER<0x40040> >("WDataStoreSuperBuffer", 4);
    s_initialized = true;
}

// OFFSET: 0x004C6330/0x1401014F0
int32_t WDataStore::InternalFetchWrite(uint32_t pos, uint32_t bytes, uint8_t*& data, uint32_t& base, uint32_t& alloc, const char* fileName, int32_t lineNumber) {
    if (!s_initialized) {
        WDataStore::StaticInitialize();
    }

    uint32_t size = pos + bytes;
    uint32_t oldAlloc = alloc;

    if (size < oldAlloc) {
        return 1;
    }

    WDATASTOREBUFFERHEADER* oldBuffer = this->m_bufferObj;

    if (size <= 0x300) {
        WDATASTOREBUFFER<0x300>* buffer = s_smallBufferHeap->New(__FILE__, __LINE__);

        if (data) {
            memcpy(buffer->m_data, data, alloc);
        }

        data = buffer->m_data;
        alloc = 0x300;
        this->m_bufferObj = buffer;
    } else if (size <= 0x4000) {
        WDATASTOREBUFFER<0x4000>* buffer = s_largeBufferHeap->New(__FILE__, __LINE__);

        if (data) {
            memcpy(buffer->m_data, data, alloc);
        }

        data = buffer->m_data;
        alloc = 0x4000;
        this->m_bufferObj = buffer;
    } else if (size <= 0x40040) {
        WDATASTOREBUFFER<0x40040>* buffer = s_superBufferHeap->New(__FILE__, __LINE__);

        if (data) {
            memcpy(buffer->m_data, data, alloc);
        }

        data = buffer->m_data;
        alloc = 0x40040;
        this->m_bufferObj = buffer;
    } else {
        uint32_t newAlloc = ~(this->m_chunkSize - 1) & (this->m_chunkSize + size - 1);

        if (oldBuffer) {
            uint8_t* newData = static_cast<uint8_t*>(CDataStore::Alloc(newAlloc, fileName ? fileName : __FILE__, lineNumber ? lineNumber : __LINE__));
            memcpy(newData, data, alloc);
            data = newData;
            this->m_bufferObj = NULL;
        } else {
            data = static_cast<uint8_t*>(CDataStore::ReAlloc(data, newAlloc, fileName ? fileName : __FILE__, lineNumber ? lineNumber : __LINE__));
        }

        alloc = newAlloc;
    }

    if (oldBuffer) {
        switch (oldAlloc) {
            case 0x300:
                s_smallBufferHeap->Delete(static_cast<WDATASTOREBUFFER<0x300>*>(oldBuffer), __FILE__, __LINE__);
                return 1;

            case 0x4000:
                s_largeBufferHeap->Delete(static_cast<WDATASTOREBUFFER<0x4000>*>(oldBuffer), __FILE__, __LINE__);
                return 1;

            case 0x40040:
                s_superBufferHeap->Delete(static_cast<WDATASTOREBUFFER<0x40040>*>(oldBuffer), __FILE__, __LINE__);
                break;
        }
    }

    return 1;
}

// OFFSET: 0x004C6580/0x140101800
void WDataStore::StaticDestroy() {
    delete s_smallBufferHeap;
    delete s_largeBufferHeap;
    delete s_superBufferHeap;
    s_initialized = false;
}
