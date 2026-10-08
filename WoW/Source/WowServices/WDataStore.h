#ifndef WOW_SOURCE_WOWSERVICES_WDATASTORE_H
#define WOW_SOURCE_WOWSERVICES_WDATASTORE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Base/ObjectAlloc.h"

struct WDATASTOREBUFFERHEADER {
    /* 0000 */ uint32_t m_handle;
    /* 0004 */ uint8_t m_header[28];
};

template <uint32_t size>
struct WDATASTOREBUFFER : public WDATASTOREBUFFERHEADER {
    /* 0020 */ uint8_t m_data[size];

    WDATASTOREBUFFER();
};

class WDataStore : public CDataStore {
    public:
    static void* AllocBuffer(uint32_t size);
    static void FreeBuffer(void* buffer, uint32_t size);
    static void StaticDestroy();
    static void StaticInitialize();

    /* 0018 */ WDATASTOREBUFFERHEADER* m_bufferObj;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ uint32_t m_chunkSize;

    void InternalInitialize(uint8_t*& data, uint32_t& base, uint32_t& alloc) override;
    void InternalDestroy(uint8_t*& data, uint32_t& base, uint32_t& alloc) override;
    int32_t InternalFetchWrite(uint32_t pos, uint32_t bytes, uint8_t*& data, uint32_t& base, uint32_t& alloc, const char* fileName, int32_t lineNumber) override;
    ~WDataStore();
    uint8_t* GetHeaderSpace(uint32_t bytes) override;

    WDataStore();
};

#include "WoW/Source/WowServices/WDataStore.inl"

#endif
