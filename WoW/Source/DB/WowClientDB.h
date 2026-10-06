#ifndef WOW_SOURCE_DB_WOWCLIENTDB_H
#define WOW_SOURCE_DB_WOWCLIENTDB_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"

struct DBCMeta {
    /* 0000 */ const char* m_filename;
    /* 0004 */ int32_t m_numColumns;
    /* 0008 */ int32_t m_rowSize;
    /* 000C */ void (*m_fixupStrings)(void* record, const char* strings);
    /* 0010 */ int32_t m_idOffset;
};

extern bool g_clientDbPerformCompress;

uint8_t* DecompressRow(const uint8_t* src, uint32_t size, uint8_t* dst);
uint32_t CompressRow(const uint8_t* src, uint32_t size, uint8_t* dst);

class WowClientDB_Common {
    public:
    /* 0004 */ int32_t m_numRecords;
    /* 0008 */ int32_t m_maxID;
    /* 000C */ int32_t m_minID;

    virtual int32_t GetRecordByIndex(int32_t index, void* rec) const = 0;
    virtual void LoadRecords(const DBCMeta* meta, uint8_t* data, uint32_t numRecords, uint8_t* rows, uint32_t stringSize, const char* strings) = 0;
    virtual void Free() = 0;

    WowClientDB_Common();
    void Load(const DBCMeta* meta);
    void Unload(const DBCMeta* meta);
};

class WowClientDB_Base : public WowClientDB_Common {
    public:
    /* 0010 */ uint8_t* m_data;
    /* 0014 */ uint8_t* m_records;
    /* 0018 */ uint8_t** m_recordsById;

    WowClientDB_Base();
    void LoadRecords(const DBCMeta* meta, uint8_t* data, uint32_t numRecords, uint8_t* rows, uint32_t stringSize, const char* strings) override;
    void Free() override;
};

template <class T>
class WowClientDB : public WowClientDB_Base {
    public:
    int32_t GetRecordByIndex(int32_t index, void* rec) const override;
#if defined(WOWDECOMP_ARCH_64)
    void LoadRecords(const DBCMeta* meta, uint8_t* data, uint32_t numRecords, uint8_t* rows, uint32_t stringSize, const char* strings) override;
    void Free() override;
#endif

    T* GetRecord(int32_t id) const;
    T* GetRecordByIndex(int32_t index) const;
};

class WowClientDB_RLE_Base : public WowClientDB_Common {
    public:
    /* 0010 */ uint8_t* m_data;
    /* 0014 */ char* m_strings;
    /* 0018 */ uint8_t** m_records;
    /* 001C */ uint8_t** m_recordsById;

    WowClientDB_RLE_Base();
    void LoadRecords(const DBCMeta* meta, uint8_t* data, uint32_t numRecords, uint8_t* rows, uint32_t stringSize, const char* strings) override;
    void Free() override;
};

struct ClientDBTable {
    WowClientDB_Common* m_db;
    const DBCMeta* m_meta;
};

void StaticDBLoadAll();
void StaticDBUnloadAll();

template <class T>
class WowClientDB_RLE : public WowClientDB_RLE_Base {
    public:
    int32_t GetRecordByIndex(int32_t index, void* rec) const override;
#if defined(WOWDECOMP_ARCH_64)
    void LoadRecords(const DBCMeta* meta, uint8_t* data, uint32_t numRecords, uint8_t* rows, uint32_t stringSize, const char* strings) override;
#endif

    int32_t GetRecordByIndex(int32_t index, T* rec) const;
    int32_t GetRecord(int32_t id, T* rec) const;
};

#include "WoW/Source/DB/WowClientDB.inl"

#endif
