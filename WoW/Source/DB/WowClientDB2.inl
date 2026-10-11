#include <cstring>
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// INLINED
inline WowClientDB2_Base::HOTFIX::HOTFIX() {
    this->m_confirmed = false;
    this->m_timestamp = 0;
    this->m_recordID = 0;
}

// INLINED
inline WowClientDB2_Base::WowClientDB2_Base() : m_recordUpdatedCallback(NULL) {
}

// INLINED
inline int32_t WowClientDB2_Base::GetID(const uint8_t* record) const {
    return *reinterpret_cast<const int32_t*>(&record[this->m_meta->m_idOffset]);
}

// INLINED
template <class T>
WowClientDB2<T>::DBCACHE::DBCACHE() {
    this->m_record = NULL;
    this->m_inCallback = false;
    this->m_deleted = false;
    this->m_hasData = false;
}

// OFFSET: [0x00499C50<ItemRec>]
template <class T>
WowClientDB2<T>::DBCACHE::~DBCACHE() {
    if (this->m_record) {
        SMemFree(this->m_record, __FILE__, __LINE__, 0);
    }

    this->m_callbacks.Clear();
}

// OFFSET: [0x00498330/0x1400C23B0<ItemRec>, 0x004984E0/0x1400C2680<ItemRecSparse>, 0x004985B0/0x1400C27A0<ItemCurrencyCostRec>, 0x00498710/0x1400C2900<ItemExtendedCostRec>, 0x004987E0/0x1400C2AA0<KeyChainRec>]
template <class T>
WowClientDB2<T>::WowClientDB2() : m_maxQueriesPerPump(100), m_queriesSent(0), m_nextQueryTime(0) {
    this->m_meta = &T::s_meta;
}

// OFFSET: [0x00499F90<ItemRec>, 0x0049A770<ItemRecSparse>, 0x0049A990<ItemCurrencyCostRec>, 0x0049ABB0<ItemExtendedCostRec>, 0x0049ADD0<KeyChainRec>]
template <class T>
WowClientDB2<T>::~WowClientDB2() {
    this->m_pending.Clear();
    this->m_received.Clear();
    this->m_sent.Clear();
    this->m_requests.Clear();
    this->m_cachedRecordList.Clear();
    this->m_cache.Clear();
}

// OFFSET: [0x00498A30<ItemRec>, 0x00498DA0<ItemRecSparse>, 0x00499110<ItemCurrencyCostRec>, 0x00499480<ItemExtendedCostRec>, 0x004997F0<KeyChainRec>]
template <class T>
void* WowClientDB2<T>::AppendDBRequest(int32_t id, WOWGUID guid, bool related) {
    DBCACHE* cache = this->m_requests.Head();

    while (!this->m_requests.IsEnd(cache)) {
        if (cache->m_id == id) {
            return cache;
        }

        cache = this->m_requests.RawNext(cache);
    }

    cache = this->m_cache.NewNode(2, 0, 0);
    cache->m_id = id;
    cache->m_guid = guid;
    this->m_pending.LinkToTail(cache);
    this->m_requests.LinkToTail(cache);

    if (this->m_meta->m_relatedTableHash && !related) {
        WowClientDB2_Base* table = WowClientDB2_Base::GetTableByHash(this->m_meta->m_relatedTableHash);

        if (table) {
            table->AppendDBRequest(id, guid, true);
        }
    }

    return cache;
}

// OFFSET: [0x00499DF0<ItemRec>]
template <class T>
void WowClientDB2<T>::CancelAllCallbacks() {
    DBCACHE* cache = this->m_cache.Head();

    while (!this->m_cache.IsEnd(cache)) {
        DBCACHE* next = this->m_cache.RawNext(cache);

        if (!cache->m_inCallback) {
            DBCALLBACK* callback = cache->m_callbacks.Head();

            while (!cache->m_callbacks.IsEnd(callback)) {
                DBCALLBACK* nextCallback = cache->m_callbacks.RawNext(callback);
                cache->m_callbacks.DeleteNode(callback);
                callback = nextCallback;
            }
        }

        cache = next;
    }
}

// OFFSET: [0x00582DA0<ItemRec>]
template <class T>
void WowClientDB2<T>::CancelCallback(int32_t id, RECORD_CALLBACK callback, void* param) {
    DBCACHE* cache = this->m_cache.Head();

    while (!this->m_cache.IsEnd(cache)) {
        DBCACHE* next = this->m_cache.RawNext(cache);

        if (cache->m_inCallback) {
            ConsolePrintf("WowClientDB2::CancelCallback ignored for id %016llX.", static_cast<int64_t>(id));
            return;
        }

        DBCALLBACK* node = cache->m_callbacks.Head();

        while (!cache->m_callbacks.IsEnd(node)) {
            DBCALLBACK* nextNode = cache->m_callbacks.RawNext(node);

            if (node->m_callback == callback && node->m_param == param) {
                cache->m_callbacks.DeleteNode(node);
            }

            node = nextNode;
        }

        cache = next;
    }
}

// OFFSET: [0x0049A750<ItemRec>, 0x0049A970<ItemRecSparse>, 0x0049AB90<ItemCurrencyCostRec>, 0x0049ADB0<ItemExtendedCostRec>, 0x0049AFD0<KeyChainRec>]
template <class T>
void WowClientDB2<T>::ClearPendingRequests() {
    this->m_pending.Clear();
    this->m_sent.Clear();
    this->PurgeUnconfirmedHotfixes();
}

// OFFSET: [0x00465ED0<ItemRec>]
template <class T>
void WowClientDB2<T>::DBReadFailCallback(CAsyncObject* object) {
    DBREADINFO* info = static_cast<DBREADINFO*>(object->m_userArg);

    if (info) {
        delete info;
    }

    if (!object->m_cancelled) {
        AsyncFileReadSetStreamingError();
    }

    AsyncFileReadDestroyObject(object);
}

// OFFSET: [0x00582360<ItemRec>]
template <class T>
void WowClientDB2<T>::DBReadCallback(CAsyncObject* object) {
    DBREADINFO* info = static_cast<DBREADINFO*>(object->m_userArg);
    uint8_t* record = info->m_cache->m_record;
    info->m_db->LoadRecord(record, NULL);

    if (info->m_hasStrings) {
        uint32_t size = info->m_stringSize;
        uint32_t offset = info->m_db->GetFirstStringOffset(record) + info->m_stringOffset;
        AsyncFileReadCreateObject(object->m_file, 2, 0, NULL, &record[info->m_db->m_meta->m_recordSize], object->m_userArg, WowClientDB2<T>::DBReadStringsCallback, WowClientDB2<T>::DBReadFailCallback, 0, offset, size);
    } else {
        info->m_db->m_received.LinkToTail(info->m_cache);
        delete info;
    }

    AsyncFileReadDestroyObject(object);
}

// OFFSET: [0x00459510<ItemRec>]
template <class T>
void WowClientDB2<T>::DBReadStringsCallback(CAsyncObject* object) {
    DBREADINFO* info = static_cast<DBREADINFO*>(object->m_userArg);
    info->m_db->StringsFixup(info->m_cache->m_record);
    info->m_db->m_received.LinkToTail(info->m_cache);
    delete info;
    AsyncFileReadDestroyObject(object);
}

// OFFSET: [0x00498C10<ItemRec>, 0x00498F80<ItemRecSparse>, 0x004992F0<ItemCurrencyCostRec>, 0x00499660<ItemExtendedCostRec>, 0x004999D0<KeyChainRec>]
template <class T>
bool WowClientDB2<T>::DBReplyHandler(ClientDBReply* reply) {
    int32_t id = reply->m_recordID;
    bool deleted = false;

    if (id < 0) {
        id = -id;
        deleted = true;
    }

    DBCACHE* cache = this->m_sent.Head();

    while (!this->m_sent.IsEnd(cache)) {
        if (cache->m_id == id) {
            goto found;
        }

        cache = this->m_sent.RawNext(cache);
    }

    cache = this->m_cache.NewNode(2, 0, 0);

found:
    if (deleted) {
        cache->m_deleted = true;
    }

    cache->m_id = id;
    cache->m_hasData = true;

    if (!deleted) {
        cache->m_record = this->Unpack(&reply->m_data);
    }

    this->m_received.LinkToTail(cache);

    return true;
}

// OFFSET: [0x004986C0<ItemRec>]
template <class T>
const uint8_t* WowClientDB2<T>::ExportCachedRecord(int32_t id) {
    DBCACHE* cache = this->m_cachedRecordList.Head();

    while (!this->m_cachedRecordList.IsEnd(cache)) {
        DBCACHE* next = this->m_cachedRecordList.RawNext(cache);

        if (cache->m_id == id) {
            return cache->m_record;
        }

        cache = next;
    }

    return NULL;
}

// OFFSET: [0x00582970/0x1401EA090<ItemRec>, 0x0045AC30/0x14006B460<ItemRecSparse>, 0x00525410<KeyChainRec>, 0x0046EC90<ItemExtendedCostRec>]
template <class T>
T* WowClientDB2<T>::GetRecord(int32_t id, const WOWGUID& guid, RECORD_CALLBACK callback, void* param, bool unique) {
    if (id <= 0) {
        return NULL;
    }

    if (id >= this->m_minID && id <= this->m_maxID) {
        T* record = reinterpret_cast<T*>(this->m_recordsById[id - this->m_minID]);

        if (this->m_meta->m_sparse) {
            if (record) {
                return record;
            }

            uint32_t offset = this->m_sparseOffsets[id - this->m_minID];

            if (offset) {
                DBCACHE* cache = this->m_requests.Head();

                while (!this->m_requests.IsEnd(cache)) {
                    DBCACHE* next = this->m_requests.RawNext(cache);

                    if (cache->m_id == id) {
                        if (!callback || cache->m_deleted) {
                            return NULL;
                        }

                        if (unique) {
                            DBCALLBACK* node = cache->m_callbacks.Head();

                            while (!cache->m_callbacks.IsEnd(node)) {
                                DBCALLBACK* nextNode = cache->m_callbacks.RawNext(node);

                                if (node->m_callback == callback && node->m_param == param) {
                                    return NULL;
                                }

                                node = nextNode;
                            }
                        }

                        DBCALLBACK* node = cache->m_callbacks.NewNode(2, 0, 0);
                        node->m_callback = callback;
                        node->m_guid = guid;
                        node->m_param = param;

                        return NULL;
                    }

                    cache = next;
                }

                cache = this->m_cache.NewNode(2, 0, 0);
                cache->m_id = id;
                cache->m_guid = guid;
                this->m_requests.LinkToTail(cache);

                DBREADINFO* info = static_cast<DBREADINFO*>(SMemAlloc(sizeof(DBREADINFO), __FILE__, __LINE__, 0));
                info->m_cache = cache;
                info->m_db = this;
                info->unk_0001 = false;
                info->m_hasStrings = this->m_sparseSizes[id - this->m_minID] != 0;
                info->m_cache->m_record = static_cast<uint8_t*>(SMemAlloc(this->m_meta->m_recordSize + this->m_sparseSizes[id - this->m_minID], __FILE__, __LINE__, 0));

                if (info->m_hasStrings) {
                    info->m_stringOffset = this->m_numRecords * this->m_meta->m_recordSize + 6 * (this->m_numSparseRecords + 8);
                    info->m_stringSize = this->m_sparseSizes[id - this->m_minID];
                } else {
                    info->m_stringOffset = 0;
                    info->m_stringSize = 0;
                }

                if (callback) {
                    DBCALLBACK* node = cache->m_callbacks.NewNode(2, 0, 0);
                    node->m_callback = callback;
                    node->m_guid = guid;
                    node->m_param = param;
                }

                AsyncFileReadCreateObject(this->m_file, 2, 0, NULL, cache->m_record, info, WowClientDB2<T>::DBReadCallback, WowClientDB2<T>::DBReadFailCallback, 0, (offset - 1) * this->m_meta->m_recordSize + 6 * (this->m_numSparseRecords + 8), this->m_meta->m_recordSize);

                return NULL;
            }
        }

        if (record) {
            return record;
        }
    }

    DBCACHE* cache = this->m_requests.Head();

    while (!this->m_requests.IsEnd(cache)) {
        if (cache->m_id == id) {
            if (callback && !cache->m_deleted) {
                if (unique) {
                    DBCALLBACK* node = cache->m_callbacks.Head();

                    while (!cache->m_callbacks.IsEnd(node)) {
                        if (node->m_callback == callback && node->m_param == param) {
                            return NULL;
                        }

                        node = cache->m_callbacks.RawNext(node);
                    }
                }

                DBCALLBACK* node = cache->m_callbacks.NewNode(2, 0, 0);
                node->m_callback = callback;
                node->m_guid = guid;
                node->m_param = param;
            }

            return NULL;
        }

        cache = this->m_requests.RawNext(cache);
    }

    cache = static_cast<DBCACHE*>(this->AppendDBRequest(id, WOWGUID(), false));

    if (!callback) {
        return NULL;
    }

    DBCALLBACK* node = cache->m_callbacks.NewNode(2, 0, 0);
    node->m_callback = callback;
    node->m_guid = guid;
    node->m_param = param;

    return NULL;
}

// OFFSET: [0x0049A190/0x1400C59B0<ItemRec>]
template <class T>
void WowClientDB2<T>::Idle() {
    DBCACHE* cache = this->m_received.Head();

    while (!this->m_received.IsEnd(cache)) {
        DBCACHE* next = this->m_received.RawNext(cache);
        this->m_received.UnlinkNode(cache);

        if (!cache->m_deleted) {
            this->m_requests.UnlinkNode(cache);

            if (cache->m_hasData) {
                this->m_cachedRecordList.LinkToTail(cache);
                this->m_sent.UnlinkNode(cache);
            }
        }

        int32_t id = cache->m_id;
        cache->m_inCallback = true;

        HOTFIX* hotfix = NULL;

        for (HOTFIX* node = this->m_hotfixes.Head(); !this->m_hotfixes.IsEnd(node); node = this->m_hotfixes.RawNext(node)) {
            if (node->m_recordID == id) {
                hotfix = node;
                break;
            }
        }

        if (!cache->m_deleted) {
            if (hotfix) {
                hotfix->m_confirmed = true;
            }

            uint8_t** recordsById = this->m_recordsById;

            if (!recordsById || id < this->m_minID || id > this->m_maxID) {
                int32_t oldMinID = recordsById ? this->m_minID : 0;
                int32_t oldCount = recordsById ? this->m_maxID - this->m_minID + 1 : 0;

                if (recordsById && id >= this->m_minID) {
                    this->m_maxID = id;
                } else {
                    if (!recordsById) {
                        this->m_maxID = id;
                    }

                    this->m_minID = id;
                }

                int32_t count = this->m_maxID - this->m_minID + 1;

                if (recordsById) {
                    this->m_recordsById = static_cast<uint8_t**>(SMemReAlloc(recordsById, count * sizeof(uint8_t*), __FILE__, __LINE__, 0));
                } else {
                    this->m_recordsById = static_cast<uint8_t**>(SMemAlloc(count * sizeof(uint8_t*), __FILE__, __LINE__, 0));
                }

                if (this->m_meta->m_sparse && !oldCount) {
                    this->m_sparseOffsets = static_cast<uint32_t*>(SMemAlloc(count * sizeof(uint32_t), __FILE__, __LINE__, 8));
                    this->m_sparseSizes = static_cast<uint16_t*>(SMemAlloc(count * sizeof(uint16_t), __FILE__, __LINE__, 8));
                } else {
                    if (this->m_meta->m_sparse) {
                        this->m_sparseOffsets = static_cast<uint32_t*>(SMemReAlloc(this->m_sparseOffsets, count * sizeof(uint32_t), __FILE__, __LINE__, 0));
                        this->m_sparseSizes = static_cast<uint16_t*>(SMemReAlloc(this->m_sparseSizes, count * sizeof(uint16_t), __FILE__, __LINE__, 0));
                    }

                    if (oldCount) {
                        int32_t added = count - oldCount;

                        if (id >= oldMinID) {
                            memset(&this->m_recordsById[oldCount], 0, added * sizeof(uint8_t*));

                            if (this->m_meta->m_sparse) {
                                memset(&this->m_sparseOffsets[oldCount], 0, added * sizeof(uint32_t));
                                memset(&this->m_sparseSizes[oldCount], 0, added * sizeof(uint16_t));
                            }
                        } else {
                            memmove(&this->m_recordsById[added], this->m_recordsById, oldCount * sizeof(uint8_t*));
                            memset(this->m_recordsById, 0, added * sizeof(uint8_t*));

                            if (this->m_meta->m_sparse) {
                                memmove(&this->m_sparseOffsets[added], this->m_sparseOffsets, oldCount * sizeof(uint32_t));
                                memset(this->m_sparseOffsets, 0, added * sizeof(uint32_t));
                                memmove(&this->m_sparseSizes[added], this->m_sparseSizes, oldCount * sizeof(uint16_t));
                                memset(this->m_sparseSizes, 0, added * sizeof(uint16_t));
                            }
                        }
                    }
                }
            }

            this->m_recordsById[id - this->m_minID] = cache->m_record;
        }

        DBCALLBACK* node = cache->m_callbacks.Head();

        while (!cache->m_callbacks.IsEnd(node)) {
            DBCALLBACK* nextNode = cache->m_callbacks.RawNext(node);
            node->m_callback(id, node->m_guid, node->m_param, !cache->m_deleted);
            node = nextNode;
        }

        if (this->m_recordUpdatedCallback) {
            this->m_recordUpdatedCallback(this->m_meta->m_tableHash, id);
        }

        cache->m_callbacks.Clear();
        cache->m_inCallback = false;

        if (cache->m_deleted) {
            delete cache;
        }

        cache = next;
    }

    if (this->m_queriesSent < this->m_maxQueriesPerPump || !this->m_nextQueryTime || static_cast<int32_t>(OsGetAsyncTimeMs() - this->m_nextQueryTime) >= 0) {
        if (this->m_queriesSent >= this->m_maxQueriesPerPump) {
            this->m_queriesSent = 0;
            this->m_nextQueryTime = OsGetAsyncTimeMs() + 30000;
        }

        UserClientDBQueryBulk msg;
        msg.m_tableHash = this->m_meta->m_tableHash;

        DBCACHE* pending = this->m_pending.Head();

        while (!this->m_pending.IsEnd(pending)) {
            DBCACHE* next = this->m_pending.RawNext(pending);
            this->m_sent.LinkToTail(pending);
            this->m_pending.UnlinkNode(pending);

            DBQuery* query = msg.m_queries.New();
            memcpy(query->m_guid, &pending->m_guid, sizeof(query->m_guid));
            query->m_id = pending->m_id;

            if (++this->m_queriesSent >= this->m_maxQueriesPerPump) {
                break;
            }

            pending = next;
        }

        if (ClientServices::Connection() && msg.m_queries.Count()) {
            ClientServices::Send(&msg);
        }
    }
}

// PROVEN: f90c549f IDENTICAL/NONE
// OFFSET: [0x004988B0<ItemRec>]
template <class T>
void WowClientDB2<T>::IterateOverCache(ITERATE_CALLBACK callback, void* param) {
    DBCACHE* cache = this->m_cachedRecordList.Head();

    while (!this->m_cachedRecordList.IsEnd(cache)) {
        DBCACHE* next = this->m_cachedRecordList.RawNext(cache);

        if (!cache->m_deleted && !callback(cache->m_record, this, param)) {
            break;
        }

        cache = next;
    }
}

// PROVEN: f823eee8 IDENTICAL/NONE
// OFFSET: [0x00498680<ItemRec>]
template <class T>
void WowClientDB2<T>::IterateOverRecords(ITERATE_CALLBACK callback, void* param) {
    for (int32_t id = this->m_minID; id < this->m_maxID; id++) {
        const uint8_t* record = this->m_recordsById[id - this->m_minID];

        if (record && !callback(record, this, param)) {
            break;
        }
    }
}
