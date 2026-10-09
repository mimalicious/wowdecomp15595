#ifndef WOW_SOURCE_DB_WOWCLIENTDB2_H
#define WOW_SOURCE_DB_WOWCLIENTDB2_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/DB/WowClientDB.h"
#include "WoW/Source/Object/WGUID.h"

class CAsyncObject;
class ClientDBReply;
class ClientHotfixNotify;
class ClientHotfixNotifyBlob;
class SFileImpl;
class WDataStore;

struct DB2Meta {
    /* 0000 */ const char* m_filename;
    /* 0004 */ const char* m_cacheName;
    /* 0008 */ int32_t m_numFields;
    /* 000C */ int32_t m_recordSize;
    /* 0010 */ void (*m_fixupRecord)(void* record);
    /* 0014 */ int32_t m_idOffset;
    /* 0018 */ bool unk_0018;
    /* 0019 */ bool m_sparse;
    /* 001C */ const int32_t* m_fieldOffsets;
    /* 0020 */ const int32_t* m_fieldSizes;
    /* 0024 */ const int32_t* m_fieldTypes;
    /* 0028 */ uint32_t m_tableHash;
    /* 002C */ uint32_t m_relatedTableHash;
    /* 0030 */ int32_t m_numStringFields;
};

struct DBCALLBACK : public TSLinkedNode<DBCALLBACK> {
    /* 0008 */ void (*m_callback)(int32_t id, const WOWGUID& guid, void* param, bool success);
    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ void* m_param;
};

class WowClientDB2_Base {
    public:
    struct HOTFIX : public TSLinkedNode<HOTFIX> {
        /* 0008 */ bool m_confirmed;
        /* 000C */ uint32_t m_timestamp;
        /* 0010 */ int32_t m_recordID;

        HOTFIX();
    };

    struct DBTABLE : public TSHashObject<DBTABLE, HASHKEY_NONE> {
        /* 0018 */ WowClientDB2_Base* m_table;
    };

    typedef bool (*ITERATE_CALLBACK)(const uint8_t* record, WowClientDB2_Base* db, void* param);

    static TSHashTable<DBTABLE, HASHKEY_NONE> m_tableByHash;

    /* 0004 */ int32_t m_numRecords;
    /* 0008 */ int32_t m_numCachedRecords;
    /* 000C */ int32_t m_maxID;
    /* 0010 */ int32_t m_minID;
    /* 0014 */ int32_t m_numSparseRecords;
    /* 0018 */ SFileImpl* m_file;
    /* 001C */ SFileImpl* m_cacheFile;
    /* 0020 */ uint8_t* m_records;
    /* 0024 */ uint8_t* m_cachedRecords;
    /* 0028 */ uint8_t** m_recordsById;
    /* 002C */ uint32_t* m_sparseOffsets;
    /* 0030 */ uint16_t* m_sparseSizes;
    /* 0034 */ void (*m_recordUpdatedCallback)(uint32_t tableHash, int32_t id);
    /* 0038 */ const DB2Meta* m_meta;
    /* 003C */ uint8_t* m_data;
    /* 0040 */ TSList<HOTFIX, TSGetLink<HOTFIX> > m_hotfixes;

    static bool DBReplyMultiplex(ClientDBReply* reply);
    static void GetCacheDir(char* buffer, const char* fileName, bool create);
    static WowClientDB2_Base* GetTableByHash(uint32_t tableHash);
    static bool HotfixNotifyBlob(ClientHotfixNotifyBlob* notify);
    static bool HotfixNotifyMultiplex(ClientHotfixNotify* notify);

    virtual bool DBReplyHandler(ClientDBReply* reply) = 0;
    virtual const uint8_t* ExportCachedRecord(int32_t id) = 0;
    virtual void IterateOverCache(ITERATE_CALLBACK callback, void* param) = 0;
    virtual void Free();
    virtual ~WowClientDB2_Base();
    virtual void CancelAllCallbacks() = 0;
    virtual void Idle() = 0;
    virtual void ClearPendingRequests() = 0;
    virtual void* AppendDBRequest(int32_t id, WOWGUID guid, bool related) = 0;
    virtual void IterateOverRecords(ITERATE_CALLBACK callback, void* param) = 0;

    WowClientDB2_Base();
    uint32_t Export(uint8_t* record, const uint8_t* source, char* strings, uint32_t offset);
    uint32_t GetFirstStringOffset(uint8_t* record);
    int32_t GetID(const uint8_t* record) const;
    uint32_t GetStringsSize(const uint8_t* record);
    bool HotfixNotifyHandler(int32_t id, uint32_t timestamp);
    void Load(uint32_t hotfixTime);
    void LoadRecord(uint8_t* record, const char* strings);
    void PurgeUnconfirmedHotfixes();
    void RestoreHotfix(uint32_t recordID, uint32_t timestamp);
    void Save(uint32_t hotfixTime);
    void StringsFixup(uint8_t* record);
    void Unload();
    uint8_t* Unpack(WDataStore* msg);
    void WriteHotfixes(HOSFILE file);
};

template <class T>
class WowClientDB2 : public WowClientDB2_Base {
    public:
    typedef void (*RECORD_CALLBACK)(int32_t id, const WOWGUID& guid, void* param, bool success);

    struct DBCACHE : public TSLinkedNode<DBCACHE> {
        /* 0008 */ uint8_t* m_record;
        /* 0010 */ WOWGUID m_guid;
        /* 0018 */ int32_t m_id;
        /* 001C */ TSList<DBCALLBACK, TSGetLink<DBCALLBACK> > m_callbacks;
        /* 0028 */ bool m_inCallback;
        /* 0029 */ bool m_deleted;
        /* 002A */ bool m_hasData;
        /* 002C */ TSLink<DBCACHE> m_pendingLink;
        /* 0034 */ TSLink<DBCACHE> m_sentLink;
        /* 003C */ TSLink<DBCACHE> m_receivedLink;
        /* 0044 */ TSLink<DBCACHE> m_requestLink;
        /* 004C */ TSLink<DBCACHE> m_recordLink;

        DBCACHE();
        ~DBCACHE();
    };

    struct DBREADINFO {
        /* 0000 */ bool m_hasStrings;
        /* 0001 */ bool unk_0001;
        /* 0004 */ uint32_t m_stringOffset;
        /* 0008 */ uint32_t m_stringSize;
        /* 000C */ WowClientDB2<T>* m_db;
        /* 0010 */ DBCACHE* m_cache;
    };

    static void DBReadCallback(CAsyncObject* object);
    static void DBReadFailCallback(CAsyncObject* object);
    static void DBReadStringsCallback(CAsyncObject* object);

    /* 004C */ TSExplicitList<DBCACHE, offsetof(DBCACHE, m_pendingLink)> m_pending;
    /* 0058 */ TSExplicitList<DBCACHE, offsetof(DBCACHE, m_sentLink)> m_sent;
    /* 0064 */ TSExplicitList<DBCACHE, offsetof(DBCACHE, m_receivedLink)> m_received;
    /* 0070 */ TSExplicitList<DBCACHE, offsetof(DBCACHE, m_requestLink)> m_requests;
    /* 007C */ TSExplicitList<DBCACHE, offsetof(DBCACHE, m_recordLink)> m_cachedRecordList;
    /* 0088 */ TSList<DBCACHE, TSGetLink<DBCACHE> > m_cache;
    /* 0094 */ uint32_t m_maxQueriesPerPump;
    /* 0098 */ uint32_t m_queriesSent;
    /* 009C */ uint32_t m_nextQueryTime;

    bool DBReplyHandler(ClientDBReply* reply) override;
    const uint8_t* ExportCachedRecord(int32_t id) override;
    void IterateOverCache(ITERATE_CALLBACK callback, void* param) override;
    ~WowClientDB2();
    void CancelAllCallbacks() override;
    void Idle() override;
    void ClearPendingRequests() override;
    void* AppendDBRequest(int32_t id, WOWGUID guid, bool related) override;
    void IterateOverRecords(ITERATE_CALLBACK callback, void* param) override;

    WowClientDB2();
    void CancelCallback(int32_t id, RECORD_CALLBACK callback, void* param);
    T* GetRecord(int32_t id, const WOWGUID& guid, RECORD_CALLBACK callback, void* param, bool unique);
};

extern WowClientDB2_Base* g_clientDB2Tables[];
extern WowClientDB2_Base* g_clientDB2GameTables[];

#include "WoW/Source/DB/WowClientDB2.inl"

#endif
