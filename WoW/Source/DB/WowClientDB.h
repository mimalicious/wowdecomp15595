#ifndef WOW_SOURCE_DB_WOWCLIENTDB_H
#define WOW_SOURCE_DB_WOWCLIENTDB_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"

class WowClientDB_Common;

class DBCRelativeString {
    public:
    /* 0000 */ int32_t m_offset;

    DBCRelativeString();
    DBCRelativeString(const DBCRelativeString& other);
    DBCRelativeString& operator=(const DBCRelativeString& other);
    void Set(const DBCRelativeString& other);
    operator const char*() const;
};

template <size_t PointerSize>
struct DBCStringType {
    typedef const char* Type;
};

template <>
struct DBCStringType<8> {
    typedef DBCRelativeString Type;
};

typedef DBCStringType<sizeof(void*)>::Type DBCString;

struct DBCMeta {
    /* 0000 */ const char* m_filename;
    /* 0004 */ int32_t m_numColumns;
    /* 0008 */ int32_t m_rowSize;
    /* 000C */ void (*m_fixupStrings)(void* record, const char* strings);
    /* 0010 */ int32_t m_idOffset;
    /* 0014 */ const int32_t* m_fieldOffsets;
    /* 0018 */ const int32_t* m_fieldSizes;
    /* 001C */ const int32_t* m_fieldTypes;
};

struct ClientDBTable {
    /* 0000 */ WowClientDB_Common* m_db;
    /* 0004 */ const DBCMeta* m_meta;
};

extern bool g_clientDbPerformCompress;
extern ClientDBTable g_staticDBTables[];
extern ClientDBTable g_staticDBGlueTables[];
extern ClientDBTable g_staticDBWorldTables[];

void DBCFixupString(const char*& string, const char* strings);
void DBCFixupString(DBCRelativeString& string, const char* strings);
uint8_t* DecompressRow(const uint8_t* src, uint32_t size, uint8_t* dst);
uint32_t CompressRow(const uint8_t* src, uint32_t size, uint8_t* dst);
void StaticDBUnloadAll();

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

    T* GetRecord(int32_t id) const;
    T* GetRecordByIndex(int32_t index) const;
};

class WowClientDB_RLE_Base : public WowClientDB_Common {
    public:
    /* 0010 */ const DBCMeta* m_meta;
    /* 0014 */ uint8_t* m_data;
    /* 0018 */ char* m_strings;
    /* 001C */ uint8_t** m_records;
    /* 0020 */ uint8_t** m_recordsById;

    WowClientDB_RLE_Base();
    void LoadRecords(const DBCMeta* meta, uint8_t* data, uint32_t numRecords, uint8_t* rows, uint32_t stringSize, const char* strings) override;
    void Free() override;
    void CopyRecord(void* rec, const uint8_t* row, uint8_t* buffer) const;
};

template <class T>
class WowClientDB_RLE : public WowClientDB_RLE_Base {
    public:
    int32_t GetRecordByIndex(int32_t index, void* rec) const override;

    int32_t GetRecordByIndex(int32_t index, T* rec) const;
    int32_t GetRecord(int32_t id, T* rec) const;
};

#include "WoW/Source/DB/WowClientDB.inl"

#endif
