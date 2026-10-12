#ifndef WOW_SOURCE_DB_DBCLIENT_DBCACHE_H
#define WOW_SOURCE_DB_DBCLIENT_DBCACHE_H

#include <cstddef>
#include <cstdint>
#include <type_traits>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/System/Using/Using.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

class CHashKeyGUID;
class CWardenKey;

struct HASHKEY_INT {
    HASHKEY_INT();
    HASHKEY_INT(int32_t key);
    bool operator==(const HASHKEY_INT& key);
};

struct DBCACHECALLBACK : public TSLinkedNode<DBCACHECALLBACK> {
    /* 0008 */ void (*m_callback)(int32_t id, const WOWGUID& guid, void* param, bool success);
    /* 000C */ uint32_t m_keyCallback;
    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ void* m_param;
};

// INLINED
template <class TKey>
void DBCacheDispatch(DBCACHECALLBACK* node, const TKey& key, int32_t id, bool success) {
    if (node->m_keyCallback) {
        reinterpret_cast<void (*)(const TKey& key, const WOWGUID& guid, void* param, bool success)>(node->m_callback)(key, node->m_guid, node->m_param, success);
    } else {
        node->m_callback(id, node->m_guid, node->m_param, success);
    }
}

template <class T, class TKey, class THashKey>
class DBCache : public Blizzard::Using::GlobalUse {
    public:
    struct DBCACHEHASH : public TSHashObject<DBCACHEHASH, THashKey> {
        T m_record;
        TKey m_id;
        bool m_loaded;
        TSList<DBCACHECALLBACK> m_callbacks;
        bool m_noSave;
        bool m_dispatching;
        bool m_invalidatePending;
        TSLink<DBCACHEHASH> m_pendingLink;

        ~DBCACHEHASH();
    };

    struct REVERSEENTRY : public TSHashObject<REVERSEENTRY, HASHKEY_CONSTSTR> {
        T* m_record;
    };

    /* 0008 */ TSHashTable<DBCACHEHASH, THashKey> m_entries;
    /* 0030 */ uint32_t m_signature;
    /* 0034 */ const char* m_fileName;
    /* 0038 */ uint32_t m_version;
    /* 003C */ uint32_t m_queryOpcode;
    /* 0040 */ uint32_t m_multipleQueryOpcode;
    /* 0044 */ bool m_sendRequester;
    /* 0045 */ bool m_enabled;
    /* 0046 */ bool m_loaded;
    /* 0048 */ uint32_t m_maxQueriesPerPump;
    /* 004C */ uint32_t m_queriesSent;
    /* 0050 */ uint32_t m_nextQueryTime;
    /* 0054 */ TSExplicitList<DBCACHEHASH, offsetof(DBCACHEHASH, m_pendingLink)> m_pending;
    /* 0060 */ TSHashTable<REVERSEENTRY, HASHKEY_CONSTSTR> m_reverse;

    virtual void InvalidateIntKey(int32_t key);

    DBCache(uint32_t signature, const char* fileName, uint32_t queryOpcode, uint32_t multipleQueryOpcode, bool sendRequester, bool enabled, uint32_t queriesPerMinute);
    ~DBCache();
    void AddItem(T* item, TKey key);
    void AddItems(CDataStore* msg, bool single);
    void CancelCallback(TKey key, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success), void* param);
    void CancelCallback(TKey key, void (*callback)(const TKey& key, const WOWGUID& guid, void* param, bool success), void* param);
    void Clear();
    void ClearPending();
    void DenyItem(TKey key);
    void DispatchCallbacks(const TKey& key, DBCACHEHASH* hash, bool success);
    T* GetRecord(TKey key, const WOWGUID& guid, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success), void* param, bool unique);
    T* GetRecord2(TKey key, const WOWGUID& guid, void (*callback)(const TKey& key, const WOWGUID& guid, void* param, bool success), void* param, bool unique);
    T* GetRecordInternal(TKey key, const WOWGUID& guid, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success), uint32_t keyCallback, void* param, bool unique);
    void Destroy();
    void Invalidate(TKey key);
    REVERSEENTRY* FindReverseEntry(const char* name);
    T* GetRecordByName(const char* name);
    void Idle();
    void RemoveReverseEntry(const char* name);
    void ItemAdded(T* item);
    void Load();
    bool QueryLimitReached();
    void RetryItem(TKey key);
    void Save();
    void SendSingleQuery(DBCACHEHASH* hash);
    void SetTemporary(TKey key);
    void VerifyAndUpdateVersion(uint32_t version);
};

void DBCacheReadKey(CDataStore& msg, int32_t& key);
void DBCacheReadKey(CDataStore& msg, uint64_t& key);
void DBCacheReadKey(CDataStore& msg, CWardenKey& key);
void DBCacheWriteKey(CDataStore& msg, int32_t key);
void DBCacheWriteKey(CDataStore& msg, uint64_t key);
void DBCacheWriteKey(CDataStore& msg, const CWardenKey& key);
bool DBCacheIsNullKey(int32_t key);
bool DBCacheIsNullKey(uint64_t key);
bool DBCacheIsNullKey(const CWardenKey& key);
uint32_t DBCacheHashKey(int32_t key);
uint32_t DBCacheHashKey(uint64_t key);
uint32_t DBCacheHashKey(const CWardenKey& key);
void GetCacheDirectory(char* path);

// INLINED
template <class TKey>
TKey DBCacheIntKey(int32_t key) {
    return static_cast<TKey>(key);
}

template <>
CWardenKey DBCacheIntKey<CWardenKey>(int32_t key);

// OFFSET: [0x004A8F00/0x1400DA5A0<CreatureStats_C, int, HASHKEY_INT>, 0x004A9030/0x1400DA750<GameObjectStats_C, int, HASHKEY_INT>, 0x004A9160/0x1400DA900<NPCText, int, HASHKEY_INT>, 0x004A9290/0x1400DAAB0<NameCache, unsigned __int64, CHashKeyGUID>, 0x004A93C0/0x1400DAC60<GuildStats_C, unsigned __int64, CHashKeyGUID>, 0x004A94F0/0x1400DAE10<QuestCache, int, HASHKEY_INT>, 0x004A9620/0x1400DAFC0<PageTextCache_C, int, HASHKEY_INT>, 0x004A9750/0x1400DB170<PetNameCache, int, HASHKEY_INT>, 0x004A9880/0x1400DB320<CGPetition, int, HASHKEY_INT>, 0x004A99B0/0x1400DB4D0<ItemTextCache_C, unsigned __int64, CHashKeyGUID>, 0x004A9AE0/0x1400DB680<WardenCachedModule, CWardenKey, CWardenKey>, 0x004A9C10/0x1400DB830<ArenaTeamCache, int, HASHKEY_INT>, 0x004A9D40/0x1400DB9E0<DanceCache, int, HASHKEY_INT>]
template <class T, class TKey, class THashKey>
DBCache<T, TKey, THashKey>::DBCache(uint32_t signature, const char* fileName, uint32_t queryOpcode, uint32_t multipleQueryOpcode, bool sendRequester, bool enabled, uint32_t queriesPerMinute) : m_signature(signature), m_fileName(fileName), m_version(0), m_queryOpcode(queryOpcode), m_multipleQueryOpcode(multipleQueryOpcode), m_sendRequester(sendRequester), m_enabled(enabled), m_loaded(false), m_maxQueriesPerPump(30000 * queriesPerMinute / 60000), m_queriesSent(0), m_nextQueryTime(0) {
}

// OFFSET: [0x004A8FA0/0x1400DA680<CreatureStats_C, int, HASHKEY_INT>, 0x004A90D0/0x1400DA830<GameObjectStats_C, int, HASHKEY_INT>, 0x004A9200/0x1400DA9E0<NPCText, int, HASHKEY_INT>, 0x004A9330/0x1400DAB90<NameCache, unsigned __int64, CHashKeyGUID>, 0x004A9460/0x1400DAD40<GuildStats_C, unsigned __int64, CHashKeyGUID>, 0x004A9590/0x1400DAEF0<QuestCache, int, HASHKEY_INT>, 0x004A96C0/0x1400DB0A0<PageTextCache_C, int, HASHKEY_INT>, 0x004A97F0/0x1400DB250<PetNameCache, int, HASHKEY_INT>, 0x004A9920/0x1400DB400<CGPetition, int, HASHKEY_INT>, 0x004A9A50/0x1400DB5B0<ItemTextCache_C, unsigned __int64, CHashKeyGUID>, 0x004A9CB0/0x1400DB910<ArenaTeamCache, int, HASHKEY_INT>, 0x004A9DE0/0x1400DBAC0<DanceCache, int, HASHKEY_INT>, 0x004A9B80/0x1400DB760<WardenCachedModule, CWardenKey, CWardenKey>]
template <class T, class TKey, class THashKey>
DBCache<T, TKey, THashKey>::~DBCache() {
    this->m_entries.InternalClear(0);
    this->m_reverse.InternalClear(0);
}

// OFFSET: [0x004A73B0/0x1400D7E80<QuestCache, int, HASHKEY_INT>, 0x004A7650/0x1400D8200<PageTextCache_C, int, HASHKEY_INT>, 0x004A7900/0x1400D8570<PetNameCache, int, HASHKEY_INT>, 0x004A7BB0/0x1400D8910<CGPetition, int, HASHKEY_INT>, 0x004A80F0/0x1400D9010<WardenCachedModule, CWardenKey, CWardenKey>, 0x004A8390/0x1400D9390<ArenaTeamCache, int, HASHKEY_INT>, 0x004A6E50/0x1400D7760<NameCache, uint64_t, CHashKeyGUID>, 0x004A7110/0x1400D7B00<GuildStats_C, uint64_t, CHashKeyGUID>, 0x004A7E50/0x1400D8C90<ItemTextCache_C, uint64_t, CHashKeyGUID>, 0x004A6640/0x1400D6CA0<CreatureStats_C, int, HASHKEY_INT>, 0x004A68F0/0x1400D7020<GameObjectStats_C, int, HASHKEY_INT>, 0x004A6BA0/0x1400D73A0<NPCText, int, HASHKEY_INT>, 0x004A8630/0x1400D9710<DanceCache, int, HASHKEY_INT>]
template <class T, class TKey, class THashKey>
DBCache<T, TKey, THashKey>::DBCACHEHASH::~DBCACHEHASH() {
    this->m_callbacks.Clear();
}

// OFFSET: [0x004B1290/0x1400E7660<NameCache, unsigned __int64, CHashKeyGUID>]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::ItemAdded(T* item) {
}

// OFFSET: [0x004B1480<NameCache, unsigned __int64, CHashKeyGUID>, 0x004ACE80<CreatureStats_C, int, HASHKEY_INT>, 0x004AD4C0<GameObjectStats_C, int, HASHKEY_INT>, 0x004ADB00<NPCText, int, HASHKEY_INT>, 0x004AE270<GuildStats_C, unsigned __int64, CHashKeyGUID>, 0x004AE7E0<QuestCache, int, HASHKEY_INT>, 0x004AEE00<PageTextCache_C, int, HASHKEY_INT>, 0x004AF330<PetNameCache, int, HASHKEY_INT>, 0x004AF870<CGPetition, int, HASHKEY_INT>, 0x004AFE20<ItemTextCache_C, unsigned __int64, CHashKeyGUID>, 0x004B0480<WardenCachedModule, CWardenKey, CWardenKey>, 0x004B0A60<ArenaTeamCache, int, HASHKEY_INT>, 0x004B0FA0<DanceCache, int, HASHKEY_INT>]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::Load() {
    this->m_loaded = true;

    if (!this->m_enabled) {
        return;
    }

    char cacheDirectory[260];
    GetCacheDirectory(cacheDirectory);

    char path[260];
    SStrPrintf(path, sizeof(path), "%s/%s", cacheDirectory, this->m_fileName);

    HOSFILE file = OsCreateFile(path, OS_GENERIC_READ, 1, OS_OPEN_EXISTING, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (file == OS_INVALID_FILE) {
        return;
    }

    uint8_t localBuffer[0x4000];
    uint8_t* buffer = localBuffer;
    uint32_t bufferSize = sizeof(localBuffer);
    uint32_t bytesRead;

    OsReadFile(file, buffer, 6 * sizeof(uint32_t), &bytesRead);

    if (bytesRead != 6 * sizeof(uint32_t)) {
        OsCloseFile(file);
        return;
    }

    CDataStore header(buffer, 6 * sizeof(uint32_t));

    uint32_t signature;
    header.Get(signature);

    if (signature != this->m_signature) {
        OsCloseFile(file);
        return;
    }

    uint32_t build;
    header.Get(build);

    if (build != 12340) {
        OsCloseFile(file);
        return;
    }

    uint32_t locale;
    header.Get(locale);

    if (locale != SFile::s_locale) {
        OsCloseFile(file);
        return;
    }

    uint32_t recordSize;
    header.Get(recordSize);

    if (recordSize != sizeof(T)) {
        OsCloseFile(file);
        return;
    }

    uint32_t cacheVersion;
    header.Get(cacheVersion);

    if (cacheVersion != T::s_cacheVersion) {
        OsCloseFile(file);
        return;
    }

    uint32_t version;
    header.Get(version);
    this->m_version = version;

    bool corrupt = false;
    OsReadFile(file, buffer, sizeof(TKey) + sizeof(uint32_t), &bytesRead);

    if (bytesRead == sizeof(TKey) + sizeof(uint32_t)) {
        while (true) {
            CDataStore entryHeader(buffer, sizeof(TKey) + sizeof(uint32_t));

            TKey key;
            DBCacheReadKey(entryHeader, key);

            uint32_t size;
            entryHeader.Get(size);

            if (DBCacheIsNullKey(key)) {
                break;
            }

            if (size > bufferSize) {
                if (bufferSize > sizeof(localBuffer)) {
                    SMemFree(buffer, __FILE__, __LINE__, 0);
                }

                bufferSize = size;
                buffer = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));
            }

            OsReadFile(file, buffer, size, &bytesRead);

            if (bytesRead != size) {
                corrupt = true;
                break;
            }

            CDataStore data(buffer, size);

            DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

            if (!hash) {
                hash = this->m_entries.New(DBCacheHashKey(key), key, 0, 0);
            }

            hash->m_record.Unpack(&data);
            hash->m_loaded = true;
            hash->m_id = key;
            this->ItemAdded(&hash->m_record);

            OsReadFile(file, buffer, sizeof(TKey) + sizeof(uint32_t), &bytesRead);

            if (bytesRead != sizeof(TKey) + sizeof(uint32_t)) {
                corrupt = true;
                break;
            }
        }
    } else {
        corrupt = true;
    }

    if (bufferSize > sizeof(localBuffer)) {
        SMemFree(buffer, __FILE__, __LINE__, 0);
    }

    OsCloseFile(file);

    if (corrupt) {
        this->Clear();
    }
}

// OFFSET: [0x004B12D0/0x1400E76C0<CreatureStats_C, int, HASHKEY_INT>, 0x004B1300/0x1400E76F0<GameObjectStats_C, int, HASHKEY_INT>, 0x004B1330/0x1400E7720<NPCText, int, HASHKEY_INT>, 0x004B17A0/0x1400E7E80<GuildStats_C, unsigned __int64, CHashKeyGUID>, 0x004B1800/0x1400E7EE0<QuestCache, int, HASHKEY_INT>, 0x004B1830/0x1400E7F10<PageTextCache_C, int, HASHKEY_INT>, 0x004B18C0/0x1400E7FA0<ItemTextCache_C, unsigned __int64, CHashKeyGUID>, 0x004B1960/0x1400E8060<DanceCache, int, HASHKEY_INT>, 0x004B1360/0x1400E7750<NameCache, unsigned __int64, CHashKeyGUID>, 0x004B1860/0x1400E7F40<PetNameCache, int, HASHKEY_INT>, 0x004B1890/0x1400E7F70<CGPetition, int, HASHKEY_INT>, 0x004B1930/0x1400E8030<ArenaTeamCache, int, HASHKEY_INT>, 0x004B18F0<WardenCachedModule, CWardenKey, CWardenKey>]
template <class T, class TKey, class THashKey>
T* DBCache<T, TKey, THashKey>::GetRecord(TKey key, const WOWGUID& guid, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success), void* param, bool unique) {
    return this->GetRecordInternal(key, guid, callback, 0, param, unique);
}

// OFFSET: [0x004B17D0/0x1400E7EB0<GuildStats_C, unsigned __int64, CHashKeyGUID>]
template <class T, class TKey, class THashKey>
T* DBCache<T, TKey, THashKey>::GetRecord2(TKey key, const WOWGUID& guid, void (*callback)(const TKey& key, const WOWGUID& guid, void* param, bool success), void* param, bool unique) {
    return this->GetRecordInternal(key, guid, reinterpret_cast<void (*)(int32_t, const WOWGUID&, void*, bool)>(callback), 1, param, unique);
}

// OFFSET: [0x004ADFD0/0x1400E2380<GuildStats_C, unsigned __int64, CHashKeyGUID>, 0x004ACB30/0x1400E0360<CreatureStats_C, int, HASHKEY_INT>, 0x004AD170/0x1400E0D50<GameObjectStats_C, int, HASHKEY_INT>, 0x004ADDF0/0x1400E2130<NameCache, unsigned __int64, CHashKeyGUID>, 0x004AE590/0x1400E2C90<QuestCache, int, HASHKEY_INT>, 0x004AF0F0/0x1400E3F10<PetNameCache, int, HASHKEY_INT>, 0x004AF620/0x1400E47E0<CGPetition, int, HASHKEY_INT>, 0x004AD7B0/0x1400E1740<NPCText, int, HASHKEY_INT>, 0x004AEAD0/0x1400E3560<PageTextCache_C, int, HASHKEY_INT>, 0x004AFB60/0x1400E50B0<ItemTextCache_C, unsigned __int64, CHashKeyGUID>, 0x004B0810/0x1400E64B0<ArenaTeamCache, int, HASHKEY_INT>, 0x004B0D50/0x1400E6D90<DanceCache, int, HASHKEY_INT>, 0x004B0140<WardenCachedModule, CWardenKey, CWardenKey>]
template <class T, class TKey, class THashKey>
T* DBCache<T, TKey, THashKey>::GetRecordInternal(TKey key, const WOWGUID& guid, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success), uint32_t keyCallback, void* param, bool unique) {
    if (DBCacheIsNullKey(key)) {
        return NULL;
    }

    DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

    if (!hash) {
        if (callback) {
            hash = this->m_entries.New(DBCacheHashKey(key), key, 0, 0);
            hash->m_id = key;

            DBCACHECALLBACK* node = hash->m_callbacks.NewNode(2, 0, 0);
            node->m_callback = callback;
            node->m_keyCallback = keyCallback;
            node->m_guid = guid;
            node->m_param = param;

            if (this->m_maxQueriesPerPump && this->m_queriesSent >= this->m_maxQueriesPerPump) {
                this->m_pending.LinkToTail(hash);
            } else {
                this->SendSingleQuery(hash);
            }
        }

        return NULL;
    }

    if (hash->m_loaded) {
        return &hash->m_record;
    }

    if (callback) {
        if (unique) {
            for (DBCACHECALLBACK* node = hash->m_callbacks.Head(); node; node = hash->m_callbacks.Next(node)) {
                if (node->m_callback == callback && node->m_param == param) {
                    return NULL;
                }
            }
        }

        DBCACHECALLBACK* node = hash->m_callbacks.NewNode(2, 0, 0);
        node->m_callback = callback;
        node->m_keyCallback = keyCallback;
        node->m_guid = guid;
        node->m_param = param;
    }

    return NULL;
}

// OFFSET: [0x004A3C10<CreatureStats_C, int, HASHKEY_INT>, 0x004A3C90<GameObjectStats_C, int, HASHKEY_INT>, 0x004A3E30<PetNameCache, int, HASHKEY_INT>, 0x004A3D10<NameCache, unsigned __int64, CHashKeyGUID>]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::CancelCallback(TKey key, void (*callback)(int32_t id, const WOWGUID& guid, void* param, bool success), void* param) {
    DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

    if (!hash) {
        return;
    }

    if (hash->m_dispatching) {
        ConsolePrintf("DBCache::CancelCallback ignored for id %016I64X.", static_cast<int64_t>(key));
        return;
    }

    DBCACHECALLBACK* node = hash->m_callbacks.Head();

    while (node) {
        DBCACHECALLBACK* next = hash->m_callbacks.Next(node);

        if (node->m_callback == callback && node->m_param == param) {
            hash->m_callbacks.DeleteNode(node);
        }

        node = next;
    }
}

// OFFSET: [0x004A3DA0/0x1400D2780<GuildStats_C, unsigned __int64, CHashKeyGUID>]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::CancelCallback(TKey key, void (*callback)(const TKey& key, const WOWGUID& guid, void* param, bool success), void* param) {
    DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

    if (!hash) {
        return;
    }

    if (hash->m_dispatching) {
        ConsolePrintf("DBCache::CancelCallback ignored for id %016I64X.", static_cast<int64_t>(key));
        return;
    }

    DBCACHECALLBACK* node = hash->m_callbacks.Head();

    while (node) {
        DBCACHECALLBACK* next = hash->m_callbacks.Next(node);

        if (node->m_callback == reinterpret_cast<void (*)(int32_t, const WOWGUID&, void*, bool)>(callback) && node->m_param == param) {
            hash->m_callbacks.DeleteNode(node);
        }

        node = next;
    }
}

// OFFSET: [0x0049BE30<CreatureStats_C, int, HASHKEY_INT>, 0x0049BF90<NPCText, int, HASHKEY_INT>, 0x0049C040<NameCache, unsigned __int64, CHashKeyGUID>, 0x0049C100<GuildStats_C, unsigned __int64, CHashKeyGUID>, 0x0049C1C0<QuestCache, int, HASHKEY_INT>, 0x0049C270<PageTextCache_C, int, HASHKEY_INT>, 0x0049C320<PetNameCache, int, HASHKEY_INT>, 0x0049C3D0<CGPetition, int, HASHKEY_INT>, 0x0049C480<ItemTextCache_C, unsigned __int64, CHashKeyGUID>, 0x0049C540<WardenCachedModule, CWardenKey, CWardenKey>, 0x0049C610<ArenaTeamCache, int, HASHKEY_INT>, 0x0049BEE0<GameObjectStats_C, int, HASHKEY_INT>, 0x0049C6C0<DanceCache, int, HASHKEY_INT>]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::SendSingleQuery(DBCACHEHASH* hash) {
    CDataStore msg;
    msg.Put(this->m_queryOpcode);
    DBCacheWriteKey(msg, hash->m_id);

    if (this->m_sendRequester) {
        DBCACHECALLBACK* node = hash->m_callbacks.Head();
        msg.Put(node ? node->m_guid.m_guid : 0);
    }

    msg.Finalize();
    ClientServices::Send2(&msg);
    this->m_queriesSent++;
}

// OFFSET: [0x004A0690<QuestCache, int, HASHKEY_INT>]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::Clear() {
    this->m_entries.InternalClear(0);
    this->m_reverse.InternalClear(0);
}

// OFFSET: [0x004A0620/0x1400CE2B0<CreatureStats_C, int, HASHKEY_INT>, 0x004A0B50/0x1400CE9A0<DanceCache, int, HASHKEY_INT>, 0x004A0720/0x1400CE3D0<NPCText, int, HASHKEY_INT>, 0x004A0790/0x1400CE460<NameCache, unsigned __int64, CHashKeyGUID>, 0x004A0800/0x1400CE4F0<GuildStats_C, unsigned __int64, CHashKeyGUID>, 0x004A0870/0x1400CE580<QuestCache, int, HASHKEY_INT>, 0x004A08E0/0x1400CE610<PageTextCache_C, int, HASHKEY_INT>, 0x004A0940/0x1400CE6A0<PetNameCache, int, HASHKEY_INT>, 0x004A09A0/0x1400CE760<CGPetition, int, HASHKEY_INT>, 0x004A0A10/0x1400CE7F0<ItemTextCache_C, unsigned __int64, CHashKeyGUID>, 0x004A0A80/0x1400CE880<WardenCachedModule, CWardenKey, CWardenKey>, 0x004A0AE0/0x1400CE910<ArenaTeamCache, int, HASHKEY_INT>, 0x004A06B0/0x1400CE340<GameObjectStats_C, int, HASHKEY_INT>]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::ClearPending() {
    DBCACHEHASH* hash = this->m_entries.m_fulllist.Head();

    while (hash) {
        if (hash->m_loaded) {
            hash = this->m_entries.Next(hash);
        } else {
            DBCACHEHASH* next = this->m_entries.Next(hash);
            this->m_entries.Delete(hash);
            hash = next;
        }
    }
}

// OFFSET: [0x0049EBF0<PageTextCache_C, int, HASHKEY_INT>, 0x0049EF30<PetNameCache, int, HASHKEY_INT>, 0x0049FD20<ArenaTeamCache, int, HASHKEY_INT>, 0x0049D7B0<GameObjectStats_C, int, HASHKEY_INT>, 0x0049DB00, 0x0049DE50<NPCText, int, HASHKEY_INT>, 0x0049E210<NameCache, unsigned __int64, CHashKeyGUID>, 0x0049E540<GuildStats_C, unsigned __int64, CHashKeyGUID>, 0x0049E8B0<QuestCache, int, HASHKEY_INT>, 0x0049F270<CGPetition, int, HASHKEY_INT>, 0x0049F5D0<ItemTextCache_C, unsigned __int64, CHashKeyGUID>, 0x0049F970<WardenCachedModule, CWardenKey, CWardenKey>, 0x004A0070<DanceCache, int, HASHKEY_INT>]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::Save() {
    if (!this->m_enabled || !this->m_loaded) {
        return;
    }

    char cacheDirectory[260];
    GetCacheDirectory(cacheDirectory);

    char path[260];
    SStrPrintf(path, sizeof(path), "%s/%s", cacheDirectory, this->m_fileName);

    HOSFILE file = OsCreateFile(path, OS_GENERIC_WRITE, 1, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (file == OS_INVALID_FILE) {
        DebugLog("OsCreateFile failed for cache file: %s\n", path);
        return;
    }

    void* data;
    uint32_t written;

    CDataStore header;
    header.Put(this->m_signature);
    header.Put(static_cast<uint32_t>(12340));
    header.Put(SFile::s_locale);
    header.Put(static_cast<uint32_t>(sizeof(T)));
    header.Put(T::s_cacheVersion);
    header.Put(this->m_version);
    header.Finalize();
    header.GetDataInSitu(data, header.m_size);
    OsWriteFile(file, data, header.m_size, &written);

    CDataStore entry;

    for (DBCACHEHASH* hash = this->m_entries.m_fulllist.Head(); hash; hash = this->m_entries.Next(hash)) {
        if (!hash->m_loaded || hash->m_noSave) {
            continue;
        }

        entry.Reset();
        DBCacheWriteKey(entry, hash->m_id);
        uint32_t sizePos = entry.m_size;
        entry.Put(static_cast<uint32_t>(0));
        hash->m_record.Pack(&entry);
        entry.Set(sizePos, static_cast<uint32_t>(entry.m_size - sizeof(TKey) - sizeof(uint32_t)));
        entry.Finalize();
        entry.GetDataInSitu(data, entry.m_size);
        OsWriteFile(file, data, entry.m_size, &written);
    }

    TKey nullKey = TKey();
    uint32_t nullSize = 0;
    OsWriteFile(file, &nullKey, sizeof(nullKey), &written);
    OsWriteFile(file, &nullSize, sizeof(nullSize), &written);
    OsCloseFile(file);
}

// OFFSET: [0x004A33A0<CreatureStats_C, int, HASHKEY_INT>, 0x004A3410<GameObjectStats_C, int, HASHKEY_INT>, 0x004A3480<NPCText, int, HASHKEY_INT>, 0x004A34F0<NameCache, unsigned __int64, CHashKeyGUID>, 0x004A3600<GuildStats_C, unsigned __int64, CHashKeyGUID>, 0x004A3670<QuestCache, int, HASHKEY_INT>, 0x004A36E0<PageTextCache_C, int, HASHKEY_INT>, 0x004A3750<PetNameCache, int, HASHKEY_INT>, 0x004A37C0<CGPetition, int, HASHKEY_INT>, 0x004A3830<ItemTextCache_C, unsigned __int64, CHashKeyGUID>, 0x004A38A0<WardenCachedModule, CWardenKey, CWardenKey>, 0x004A3910<ArenaTeamCache, int, HASHKEY_INT>, 0x004A3980<DanceCache, int, HASHKEY_INT>]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::Destroy() {
    this->Save();
    this->ClearPending();
    this->m_entries.Clear();
    this->m_reverse.Clear();
    this->m_loaded = false;
}

// OFFSET: [0x004A33D0/0x1400D19F0<CreatureStats_C, int, HASHKEY_INT>, 0x004A34B0/0x1400D1B10<NPCText, int, HASHKEY_INT>, 0x004A35C0/0x1400D1C60<NameCache, unsigned __int64, CHashKeyGUID>, 0x004A3630/0x1400D1CF0<GuildStats_C, unsigned __int64, CHashKeyGUID>, 0x004A36A0/0x1400D1D80<QuestCache, int, HASHKEY_INT>, 0x004A3710/0x1400D1E10<PageTextCache_C, int, HASHKEY_INT>, 0x004A3780/0x1400D1EA0<PetNameCache, int, HASHKEY_INT>, 0x004A37F0/0x1400D1F30<CGPetition, int, HASHKEY_INT>, 0x004A3860/0x1400D1FC0<ItemTextCache_C, unsigned __int64, CHashKeyGUID>, 0x004A38D0/0x1400D2050<WardenCachedModule, CWardenKey, CWardenKey>, 0x004A3940/0x1400D20E0<ArenaTeamCache, int, HASHKEY_INT>, 0x004A3440/0x1400D1A80<GameObjectStats_C, int, HASHKEY_INT>, 0x004A39B0/0x1400D2170<DanceCache, int, HASHKEY_INT>]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::VerifyAndUpdateVersion(uint32_t version) {
    if (!this->m_enabled || this->m_version == version) {
        return;
    }

    this->ClearPending();
    this->m_entries.InternalClear(0);
    this->m_reverse.InternalClear(0);
    this->m_version = version;
}

// OFFSET: [0x0049C840<CreatureStats_C, int, HASHKEY_INT>, 0x0049C970<GameObjectStats_C, int, HASHKEY_INT>, 0x0049CAA0<NPCText, int, HASHKEY_INT>, 0x0049CBD0<NameCache, unsigned __int64, CHashKeyGUID>, 0x0049CD00<GuildStats_C, unsigned __int64, CHashKeyGUID>, 0x0049CE30<QuestCache, int, HASHKEY_INT>, 0x0049CF60<PageTextCache_C, int, HASHKEY_INT>, 0x0049D090<PetNameCache, int, HASHKEY_INT>, 0x0049D1C0<CGPetition, int, HASHKEY_INT>, 0x0049D2F0<ItemTextCache_C, unsigned __int64, CHashKeyGUID>, 0x0049D420/0x1400C9390<WardenCachedModule, CWardenKey, CWardenKey>, 0x0049D580<ArenaTeamCache, int, HASHKEY_INT>, 0x0049D5E0<DanceCache, int, HASHKEY_INT>]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::DispatchCallbacks(const TKey& key, DBCACHEHASH* hash, bool success) {
    for (DBCACHECALLBACK* node = hash->m_callbacks.Head(); node;) {
        DBCACHECALLBACK* next = hash->m_callbacks.Next(node);

        if (node->m_keyCallback == 1) {
            reinterpret_cast<void (*)(const TKey& key, const WOWGUID& guid, void* param, bool success)>(node->m_callback)(key, node->m_guid, node->m_param, success);
        } else {
            node->m_callback(*reinterpret_cast<const int32_t*>(&key), node->m_guid, node->m_param, success);
        }

        node = next;
    }
}

// OFFSET: [0x004ACCE0<CreatureStats_C, int, HASHKEY_INT>, 0x004AD320<GameObjectStats_C, int, HASHKEY_INT>, 0x004AD960<NPCText, int, HASHKEY_INT>, 0x004AEC70<PageTextCache_C, int, HASHKEY_INT>]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::AddItems(CDataStore* msg, bool single) {
    uint8_t count;

    if (single) {
        count = 1;
    } else {
        msg->Get(count);

        if (!count) {
            return;
        }
    }

    do {
        bool denied = false;
        uint32_t id;
        msg->Get(id);

        if (static_cast<int32_t>(id) < 0) {
            id &= 0x7FFFFFFF;
            denied = true;
        }

        TKey key = id;

        DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

        if (denied) {
            if (hash) {
                hash->m_dispatching = true;
                this->m_entries.Unlink(hash);
                this->DispatchCallbacks(key, hash, false);
                hash->m_dispatching = false;
                this->m_entries.Delete(hash);
            }
        } else {
            if (!hash) {
                hash = this->m_entries.New(DBCacheHashKey(key), key, 0, 0);
            }

            hash->m_dispatching = true;
            hash->m_record.Unpack(msg);
            hash->m_loaded = true;
            hash->m_id = key;
            this->DispatchCallbacks(key, hash, true);
            hash->m_callbacks.Clear();
            bool invalidate = hash->m_invalidatePending;
            hash->m_dispatching = false;

            if (invalidate) {
                this->Invalidate(key);
            }
        }
    } while (--count);
}

// OFFSET: [0x004AE1B0<GuildStats_C, unsigned __int64, CHashKeyGUID>, 0x004AE740<QuestCache, int, HASHKEY_INT>, 0x004AF2A0<PetNameCache, int, HASHKEY_INT>, 0x004AF7D0<CGPetition, int, HASHKEY_INT>, 0x004AFD40<ItemTextCache_C, unsigned __int64, CHashKeyGUID>, 0x004B09C0<ArenaTeamCache, int, HASHKEY_INT>, 0x004B0F00<DanceCache, int, HASHKEY_INT>, 0x004B1390<NameCache, uint64_t, CHashKeyGUID>, 0x004B0360<WardenCachedModule, CWardenKey, CWardenKey>]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::AddItem(T* item, TKey key) {
    DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

    if (!hash) {
        hash = this->m_entries.New(DBCacheHashKey(key), key, 0, 0);
    }

    hash->m_dispatching = true;
    hash->m_record = *item;
    hash->m_loaded = true;
    hash->m_id = key;
    this->DispatchCallbacks(key, hash, true);
    hash->m_callbacks.Clear();
    bool invalidate = hash->m_invalidatePending;
    hash->m_dispatching = false;

    if (invalidate) {
        this->Invalidate(key);
    }
}

// OFFSET: [0x0049DDF0<NPCText, int, HASHKEY_INT>, 0x0049E160<NameCache, unsigned __int64, CHashKeyGUID>, 0x0049E4D0<GuildStats_C, unsigned __int64, CHashKeyGUID>, 0x0049E850<QuestCache, int, HASHKEY_INT>, 0x0049EED0<PetNameCache, int, HASHKEY_INT>, 0x0049F210<CGPetition, int, HASHKEY_INT>, 0x0049F560<ItemTextCache_C, unsigned __int64, CHashKeyGUID>, 0x0049F8E0/0x1400CCEA0<WardenCachedModule, CWardenKey, CWardenKey>, 0x0049FCC0<ArenaTeamCache, int, HASHKEY_INT>, 0x004A0010<DanceCache, int, HASHKEY_INT>, 0x0049D750<CreatureStats_C, int, HASHKEY_INT>, 0x0049DAA0<GameObjectStats_C, int, HASHKEY_INT>, 0x0049EBA0<PageTextCache_C, int, HASHKEY_INT>]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::DenyItem(TKey key) {
    DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

    if (!hash) {
        return;
    }

    hash->m_dispatching = true;
    this->DispatchCallbacks(key, hash, false);
    hash->m_dispatching = false;
    this->m_entries.Delete(hash);
}

// OFFSET: [0x0049E0D0/0x1400CA6A0<NPCText, int, HASHKEY_INT>, 0x0049E7D0/0x1400CB210<GuildStats_C, unsigned __int64, CHashKeyGUID>, 0x0049EB30/0x1400CB7B0<QuestCache, int, HASHKEY_INT>, 0x0049F4F0/0x1400CC850<CGPetition, int, HASHKEY_INT>, 0x0049F860/0x1400CCDE0<ItemTextCache_C, unsigned __int64, CHashKeyGUID>, 0x0049FC00/0x1400CD370<WardenCachedModule, CWardenKey, CWardenKey>, 0x0049FFA0/0x1400CD950<ArenaTeamCache, int, HASHKEY_INT>, 0x004A02F0/0x1400CDEE0<DanceCache, int, HASHKEY_INT>, 0x0049DA30/0x1400C9B80<CreatureStats_C, int, HASHKEY_INT>, 0x0049DD80/0x1400CA110<GameObjectStats_C, int, HASHKEY_INT>, 0x0049EE60/0x1400CBD30<PageTextCache_C, int, HASHKEY_INT>, 0x0049F1A0/0x1400CC2C0<PetNameCache, int, HASHKEY_INT>, 0x004A3520/0x1400D1BA0<NameCache, unsigned __int64, CHashKeyGUID>]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::Invalidate(TKey key) {
    DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

    if (!hash) {
        return;
    }

    if (!hash->m_loaded) {
        this->DenyItem(key);
    } else if (hash->m_dispatching) {
        hash->m_invalidatePending = true;
    } else {
        this->m_entries.Delete(hash);
    }
}

// OFFSET: [0x004A0380/0x1400CE020<NPCText, int, HASHKEY_INT>, 0x004A03E0/0x1400CE030<GuildStats_C, unsigned __int64, CHashKeyGUID>, 0x004A0400/0x1400CE040<QuestCache, int, HASHKEY_INT>, 0x004A0420/0x1400CE060<PetNameCache, int, HASHKEY_INT>, 0x004A0430/0x1400CE070<CGPetition, int, HASHKEY_INT>, 0x004A0440/0x1400CE080<ItemTextCache_C, unsigned __int64, CHashKeyGUID>, 0x004A0490/0x1400CE0C0<ArenaTeamCache, int, HASHKEY_INT>, 0x004A04A0/0x1400CE0D0<DanceCache, int, HASHKEY_INT>, 0x004A0360/0x1400CE000<CreatureStats_C, int, HASHKEY_INT>, 0x004A0370/0x1400CE010<GameObjectStats_C, int, HASHKEY_INT>, 0x004A0410/0x1400CE050<PageTextCache_C, int, HASHKEY_INT>, 0x004A0460/0x1400CE090<WardenCachedModule, CWardenKey, CWardenKey>, 0x004A3A70/0x1400D2260<NameCache, unsigned __int64, CHashKeyGUID>]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::InvalidateIntKey(int32_t key) {
    this->Invalidate(DBCacheIntKey<TKey>(key));
}

// OFFSET: [0x0049E1D0<NameCache, unsigned __int64, CHashKeyGUID>]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::RetryItem(TKey key) {
    DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

    if (hash) {
        this->m_pending.LinkToTail(hash);
    }
}

// OFFSET: 0x0049E4A0
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::SetTemporary(TKey key) {
    DBCACHEHASH* hash = this->m_entries.Ptr(DBCacheHashKey(key), key);

    if (hash) {
        hash->m_noSave = true;
    }
}

// PROVEN: 0c278f98 IDENTICAL/NONE
// OFFSET: 0x0049B360/0x1400C7060
template <class T, class TKey, class THashKey>
bool DBCache<T, TKey, THashKey>::QueryLimitReached() {
    return this->m_maxQueriesPerPump && this->m_queriesSent >= this->m_maxQueriesPerPump;
}

// OFFSET: [0x0049C770<CreatureStats_C, int, HASHKEY_INT>, 0x0049C8A0<GameObjectStats_C, int, HASHKEY_INT>, 0x0049C9D0<NPCText, int, HASHKEY_INT>, 0x0049CB00<NameCache, unsigned __int64, CHashKeyGUID>, 0x0049CC30<GuildStats_C, unsigned __int64, CHashKeyGUID>, 0x0049CD60<QuestCache, int, HASHKEY_INT>, 0x0049CE90<PageTextCache_C, int, HASHKEY_INT>, 0x0049CFC0<PetNameCache, int, HASHKEY_INT>, 0x0049D0F0<CGPetition, int, HASHKEY_INT>, 0x0049D220<ItemTextCache_C, unsigned __int64, CHashKeyGUID>, 0x0049D350<WardenCachedModule, CWardenKey, CWardenKey>, 0x0049D4B0<ArenaTeamCache, int, HASHKEY_INT>]
template <class T, class TKey, class THashKey>
void DBCache<T, TKey, THashKey>::Idle() {
    if (!this->m_maxQueriesPerPump || !this->m_loaded) {
        return;
    }

    if (this->m_nextQueryTime && static_cast<int32_t>(OsGetAsyncTimeMs() - this->m_nextQueryTime) < 0) {
        return;
    }

    this->m_queriesSent = 0;
    this->m_nextQueryTime = OsGetAsyncTimeMs() + 30000;

    DBCACHEHASH* next;

    for (DBCACHEHASH* hash = this->m_pending.Head(); !this->m_pending.IsEnd(hash); hash = next) {
        next = this->m_pending.RawNext(hash);
        this->m_pending.UnlinkNode(hash);
        this->SendSingleQuery(hash);

        if (this->QueryLimitReached()) {
            break;
        }
    }
}

#include "WoW/Source/DB/DBClient/DBCache.inl"

#endif
