#include "WoW/Source/DB/WowClientDB2.h"
#include <cstring>
#include <malloc.h>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/WowServices/WDataStore.h"

struct DB2SAVECOUNT {
    /* 0000 */ int32_t m_count;
    /* 0004 */ uint32_t m_stringSize;
};

struct DB2SAVEWRITE {
    /* 0000 */ char* m_strings;
    /* 0004 */ uint32_t m_stringOffset;
    /* 0008 */ HOSFILE m_file;
    /* 000C */ uint8_t* m_buffer;
};

// STATIC: 0x00D73608
TSHashTable<WowClientDB2_Base::DBTABLE, HASHKEY_NONE> WowClientDB2_Base::m_tableByHash;

// PROVEN: 07e2e6d3 IDENTICAL/NONE
// OFFSET: 0x0048B6D0/0x1400AEA90
static bool SaveCountCallback(const uint8_t* record, WowClientDB2_Base* db, void* param) {
    DB2SAVECOUNT* count = static_cast<DB2SAVECOUNT*>(param);
    count->m_stringSize += db->GetStringsSize(record);
    count->m_count++;

    return true;
}

// PROVEN: b97d87d0 IDENTICAL/NONE
// OFFSET: 0x0048B6F0/0x1400AEAC0
static bool SaveWriteCallback(const uint8_t* record, WowClientDB2_Base* db, void* param) {
    DB2SAVEWRITE* save = static_cast<DB2SAVEWRITE*>(param);
    memcpy(save->m_buffer, record, db->m_meta->m_recordSize);
    save->m_stringOffset = db->Export(save->m_buffer, record, &save->m_strings[save->m_stringOffset], save->m_stringOffset);

    uint32_t bytesWritten = 0;
    OsWriteFile(save->m_file, save->m_buffer, db->m_meta->m_recordSize, &bytesWritten);

    return true;
}

// OFFSET: 0x0048BDD0
bool WowClientDB2_Base::DBReplyMultiplex(ClientDBReply* reply) {
    WowClientDB2_Base* table = WowClientDB2_Base::GetTableByHash(reply->m_tableHash);

    if (!table) {
        return false;
    }

    return table->DBReplyHandler(reply);
}

// PROVEN: 08b305b3 IDENTICAL/NONE
// OFFSET: 0x0048B2A0/0x1400AE4D0
void WowClientDB2_Base::GetCacheDir(char* buffer, const char* fileName, bool create) {
    strcpy(buffer, "Cache\\ADB");

    if (create) {
        strcat(buffer, "\\");
        strcat(buffer, s_localeCode);
    }

    OsCreateDirectory(buffer, 1);

    if (fileName) {
        strcat(buffer, "\\");
        strcat(buffer, fileName);
    }
}

// OFFSET: 0x0048BDA0/0x1400AF490
WowClientDB2_Base* WowClientDB2_Base::GetTableByHash(uint32_t tableHash) {
    DBTABLE* table = WowClientDB2_Base::m_tableByHash.Ptr(tableHash, HASHKEY_NONE());

    return table ? table->m_table : NULL;
}

// OFFSET: 0x0048C0F0
bool WowClientDB2_Base::HotfixNotifyBlob(ClientHotfixNotifyBlob* notify) {
    WowClientDB2_Base* table = WowClientDB2_Base::GetTableByHash(notify->m_tableHash);

    if (!table) {
        return false;
    }

    return table->HotfixNotifyHandler(notify->m_recordID, notify->m_timestamp);
}

// OFFSET: 0x0048C130
bool WowClientDB2_Base::HotfixNotifyMultiplex(ClientHotfixNotify* notify) {
    for (uint32_t i = 0; i < notify->m_hotfixes.Count(); i++) {
        HotfixNotify& hotfix = notify->m_hotfixes[i];
        WowClientDB2_Base* table = WowClientDB2_Base::GetTableByHash(hotfix.m_tableHash);

        if (table && !table->HotfixNotifyHandler(hotfix.m_recordID, hotfix.m_timestamp)) {
            return false;
        }
    }

    return true;
}

// PROVEN: 655b0f3e IDENTICAL/NONE
// OFFSET: 0x0048B650/0x1400AE9E0
void WowClientDB2_Base::Free() {
    this->m_records = NULL;
    this->m_cachedRecords = NULL;

    if (this->m_data) {
        SMemFree(this->m_data, __FILE__, __LINE__, 0);
    }

    if (this->m_sparseOffsets) {
        SMemFree(this->m_sparseOffsets, __FILE__, __LINE__, 0);
    }

    if (this->m_sparseSizes) {
        SMemFree(this->m_sparseSizes, __FILE__, __LINE__, 0);
    }

    if (this->m_recordsById) {
        SMemFree(this->m_recordsById, __FILE__, __LINE__, 0);
    }

    this->m_data = NULL;
    this->m_sparseOffsets = NULL;
    this->m_sparseSizes = NULL;
    this->m_recordsById = NULL;
}

// OFFSET: 0x004982C0/0x1400C2360
WowClientDB2_Base::~WowClientDB2_Base() {
}

// OFFSET: 0x0048B3D0/0x1400AE640
uint32_t WowClientDB2_Base::Export(uint8_t* record, const uint8_t* source, char* strings, uint32_t offset) {
    const DB2Meta* meta = this->m_meta;
    int32_t index = 0;

    for (int32_t field = 0; index < meta->m_numStringFields; field++) {
        if (meta->m_fieldTypes[field] == 2) {
            for (uint32_t i = 0; i < static_cast<uint32_t>(meta->m_fieldSizes[field]) / sizeof(uint32_t); i++) {
                int32_t fieldOffset = meta->m_fieldOffsets[field] + i * sizeof(const char*);
                const char* string = *reinterpret_cast<const DBCString*>(&source[fieldOffset]);
                uint32_t length = strlen(string) + 1;

                if (length == 1) {
                    *reinterpret_cast<uint32_t*>(&record[fieldOffset]) = 0;
                } else {
                    memcpy(strings, string, length);
                    *reinterpret_cast<uint32_t*>(&record[fieldOffset]) = offset;
                    strings += length;
                    offset += length;
                }

                meta = this->m_meta;
                index++;
            }
        }

        meta = this->m_meta;
    }

    this->m_meta->m_fixupRecord(record);

    return offset;
}

// OFFSET: 0x0048B550/0x1400AE880
uint32_t WowClientDB2_Base::GetFirstStringOffset(uint8_t* record) {
    const DB2Meta* meta = this->m_meta;
    int32_t index = 0;

    for (int32_t field = 0; index < meta->m_numStringFields; field++) {
        if (meta->m_fieldTypes[field] == 2) {
            uint32_t count = static_cast<uint32_t>(meta->m_fieldSizes[field]) / sizeof(uint32_t);
            const uint32_t* strings = reinterpret_cast<const uint32_t*>(&record[meta->m_fieldOffsets[field]]);

            for (uint32_t i = 0; i < count; i++) {
                if (strings[i]) {
                    return strings[i];
                }

                index++;
            }
        }
    }

    return 0;
}

// OFFSET: 0x0048B4B0/0x1400AE7C0
uint32_t WowClientDB2_Base::GetStringsSize(const uint8_t* record) {
    const DB2Meta* meta = this->m_meta;
    int32_t numStringFields = meta->m_numStringFields;
    uint32_t size = 0;
    int32_t index = 0;

    for (int32_t field = 0; index < numStringFields; field++) {
        if (meta->m_fieldTypes[field] == 2) {
            uint32_t count = static_cast<uint32_t>(meta->m_fieldSizes[field]) / sizeof(uint32_t);

            if (count) {
                const char* const* strings = reinterpret_cast<const char* const*>(&record[meta->m_fieldOffsets[field]]);

                for (uint32_t i = 0; i < count; i++) {
                    uint32_t length = strlen(*reinterpret_cast<const DBCString*>(&strings[i]));

                    if (length) {
                        size += length + 1;
                    }
                }

                index += count;
            }
        }
    }

    return size;
}

// OFFSET: 0x0048C010
bool WowClientDB2_Base::HotfixNotifyHandler(int32_t id, uint32_t timestamp) {
    HOTFIX* hotfix = this->m_hotfixes.Head();

    while (!this->m_hotfixes.IsEnd(hotfix)) {
        if (hotfix->m_recordID == id) {
            goto found;
        }

        hotfix = this->m_hotfixes.RawNext(hotfix);
    }

    hotfix = this->m_hotfixes.NewNode(2, 0, 0);
    hotfix->m_timestamp = 0;
    hotfix->m_recordID = id;

found:
    if (!hotfix->m_confirmed || timestamp > hotfix->m_timestamp) {
        hotfix->m_timestamp = timestamp;
        hotfix->m_confirmed = false;
        this->AppendDBRequest(id, WOWGUID(), false);
    }

    return true;
}

// OFFSET: 0x0048CA40/0x1400B0650
void WowClientDB2_Base::Load(uint32_t hotfixTime) {
    char error[256];
    char path[260];
    uint32_t header[12];

    int32_t locale = 1 << LocaleNameToDB2Locale(s_localeCode);
    uint32_t tableHash = this->m_meta->m_tableHash;

    if (!WowClientDB2_Base::m_tableByHash.Ptr(tableHash, HASHKEY_NONE())) {
        DBTABLE* table = WowClientDB2_Base::m_tableByHash.New(tableHash, HASHKEY_NONE(), 0, 0);
        table->m_table = this;
    }

    WowClientDB2_Base::GetCacheDir(path, this->m_meta->m_cacheName, true);
    SFileOpenFile(this->m_meta->m_filename, &this->m_file);
    SFile::OpenEx(NULL, path, 1, &this->m_cacheFile);
    bool cacheValid = this->m_cacheFile != NULL;

    if (!this->m_file) {
        SErrGetErrorStr(SErrGetLastError(), error, sizeof(error));
        SErrDisplayAppFatal("Unable to open %s: ", this->m_meta->m_filename, error);
    }

    uint32_t size;

    if (this->m_meta->m_sparse) {
        if (!SFile::Read(this->m_file, header, sizeof(header), NULL)) {
            SErrGetErrorStr(SErrGetLastError(), error, sizeof(error));
            SErrDisplayAppFatal("Unable to read data for %s: %s", this->m_meta->m_filename, error);
        }

        SFile::SetFilePointer(this->m_file, 0, NULL, 0);
        size = 6 * (header[9] - header[8] + 9);
    } else {
        size = SFileGetFileSize(this->m_file, NULL);
    }

    if (cacheValid && SFileGetFileSize(this->m_cacheFile, NULL) < 49) {
        cacheValid = false;
    }

    uint32_t cacheSize = 0;

    if (cacheValid) {
        cacheSize = SFileGetFileSize(this->m_cacheFile, NULL);
    }

    this->m_data = static_cast<uint8_t*>(SMemAlloc(cacheSize + size, __FILE__, __LINE__, 0));

    if (!SFile::Read(this->m_file, this->m_data, size, NULL)) {
        SErrGetErrorStr(SErrGetLastError(), error, sizeof(error));
        SErrDisplayAppFatal("Unable to read data for %s: %s", this->m_meta->m_filename, error);
    }

    if (cacheValid && !SFile::Read(this->m_cacheFile, &this->m_data[size], cacheSize, NULL)) {
        cacheValid = false;
    }

    if (!this->m_meta->m_sparse) {
        SFileCloseFile(this->m_file);
        this->m_file = NULL;
    }

    if (this->m_cacheFile) {
        SFileCloseFile(this->m_cacheFile);
        this->m_cacheFile = NULL;
    }

    uint32_t* fileHeader = reinterpret_cast<uint32_t*>(this->m_data);
    uint32_t* cacheHeader = reinterpret_cast<uint32_t*>(&this->m_data[size]);

    if (fileHeader[0] != '2BDW') {
        SErrDisplayAppFatal("Invalid signature 0x%x from %s", fileHeader[0], this->m_meta->m_filename);
    }

    if (fileHeader[5] != this->m_meta->m_tableHash) {
        SErrDisplayAppFatalCustom(STORM_ERROR_VERSION_MISMATCH, "%s has wrong table hash (found %i, expected %i)", this->m_meta->m_filename, fileHeader[5], this->m_meta->m_tableHash);
    }

    if (fileHeader[3] != static_cast<uint32_t>(this->m_meta->m_recordSize)) {
        SErrDisplayAppFatalCustom(STORM_ERROR_VERSION_MISMATCH, "%s has wrong row size (found %i, expected %i)", this->m_meta->m_filename, fileHeader[3], this->m_meta->m_recordSize);
    }

    if ((fileHeader[10] & locale) != static_cast<uint32_t>(locale)) {
        SErrDisplayAppFatalCustom(STORM_ERROR_VERSION_MISMATCH, "%s has wrong locale (found %i, expected %i)", this->m_meta->m_filename, fileHeader[10], locale);
    }

    if (cacheValid) {
        if (cacheHeader[0] == '2HCW' && cacheHeader[5] == this->m_meta->m_tableHash && cacheHeader[6] == 15595 && cacheHeader[3] == static_cast<uint32_t>(this->m_meta->m_recordSize) && (cacheHeader[7] == hotfixTime || !hotfixTime) && (cacheHeader[10] & locale) == static_cast<uint32_t>(locale) && this->m_meta->m_recordSize * cacheHeader[1] + cacheHeader[4] + 48 == cacheSize) {
            goto cacheLoaded;
        }

        cacheValid = false;
        OsDeleteFile(path);
    }

    cacheHeader = NULL;

    if (cacheSize) {
        this->m_data = static_cast<uint8_t*>(SMemReAlloc(this->m_data, size, __FILE__, __LINE__, 0));
        fileHeader = reinterpret_cast<uint32_t*>(this->m_data);
    }

cacheLoaded:
    uint8_t* rows = &this->m_data[48];
    uint8_t* cacheRows;

    if (cacheValid) {
        cacheRows = &rows[size];

        if (!hotfixTime) {
            this->m_hotfixes.Clear();
        }
    } else {
        cacheRows = NULL;
        this->m_hotfixes.Clear();
    }

    if (!cacheValid && this->m_cacheFile) {
        SFileCloseFile(this->m_cacheFile);
        this->m_cacheFile = NULL;
    }

    if (this->m_meta->m_sparse) {
        this->m_minID = fileHeader[8];
        this->m_maxID = fileHeader[9];
    } else {
        this->m_maxID = -1;
        this->m_minID = 0xFFFFFFF;
    }

    const char* strings = this->m_meta->m_sparse ? NULL : reinterpret_cast<const char*>(&rows[fileHeader[1] * this->m_meta->m_recordSize]);
    const char* cacheStrings = NULL;

    this->m_records = NULL;
    this->m_cachedRecords = NULL;
    this->m_recordsById = NULL;
    this->m_sparseOffsets = NULL;
    this->m_numRecords = fileHeader[1];
    this->m_numCachedRecords = cacheValid ? cacheHeader[1] : 0;
    this->m_numSparseRecords = this->m_meta->m_sparse ? this->m_maxID - this->m_minID + 1 : 0;

    if (!this->m_numRecords) {
        return;
    }

    this->m_records = this->m_meta->m_sparse ? NULL : rows;
    this->m_cachedRecords = this->m_numCachedRecords ? cacheRows : NULL;

    if (!this->m_meta->m_sparse) {
        for (int32_t i = 0; i < this->m_numRecords; i++) {
            uint8_t* record = &this->m_records[i * this->m_meta->m_recordSize];
            this->LoadRecord(record, strings);

            int32_t id = this->GetID(record);
            this->m_maxID = id > this->m_maxID ? id : this->m_maxID;
            this->m_minID = id < this->m_minID ? id : this->m_minID;
        }
    }

    for (int32_t i = 0; i < this->m_numCachedRecords; i++) {
        int32_t id = this->GetID(&this->m_cachedRecords[i * this->m_meta->m_recordSize]);
        this->m_maxID = id > this->m_maxID ? id : this->m_maxID;
        this->m_minID = id < this->m_minID ? id : this->m_minID;
    }

    int32_t count = this->m_maxID - this->m_minID + 1;
    this->m_recordsById = static_cast<uint8_t**>(SMemAlloc(count * sizeof(uint8_t*), __FILE__, __LINE__, 8));

    if (this->m_meta->m_sparse) {
        this->m_sparseOffsets = static_cast<uint32_t*>(SMemAlloc(count * sizeof(uint32_t), __FILE__, __LINE__, 8));
        this->m_sparseSizes = static_cast<uint16_t*>(SMemAlloc(count * sizeof(uint16_t), __FILE__, __LINE__, 8));
        memcpy(&this->m_sparseOffsets[fileHeader[8] - this->m_minID], rows, this->m_numSparseRecords * sizeof(uint32_t));
        memcpy(&this->m_sparseSizes[fileHeader[8] - this->m_minID], &rows[this->m_numSparseRecords * sizeof(uint32_t)], this->m_numSparseRecords * sizeof(uint16_t));

        if (this->m_numCachedRecords) {
            uint32_t cacheStringSize = cacheHeader[4];
            memmove(this->m_data, this->m_cachedRecords, cacheStringSize + this->m_numCachedRecords * this->m_meta->m_recordSize);
            this->m_data = static_cast<uint8_t*>(SMemReAlloc(this->m_data, cacheStringSize + this->m_numCachedRecords * this->m_meta->m_recordSize, __FILE__, __LINE__, 0));
            this->m_cachedRecords = this->m_data;
            cacheStrings = reinterpret_cast<const char*>(&this->m_data[this->m_numCachedRecords * this->m_meta->m_recordSize]);
        } else {
            SMemFree(this->m_data, __FILE__, __LINE__, 0);
            this->m_data = NULL;
            this->m_cachedRecords = NULL;
        }
    } else {
        for (int32_t i = 0; i < this->m_numRecords; i++) {
            uint8_t* record = &this->m_records[i * this->m_meta->m_recordSize];
            this->m_recordsById[this->GetID(record) - this->m_minID] = record;
        }
    }

    for (int32_t i = 0; i < this->m_numCachedRecords; i++) {
        uint8_t* record = &this->m_cachedRecords[i * this->m_meta->m_recordSize];
        this->LoadRecord(record, cacheStrings);

        int32_t id = this->GetID(record);

        if (this->m_meta->m_sparse) {
            this->m_sparseOffsets[id - this->m_minID] = 0;
            this->m_sparseSizes[id - this->m_minID] = 0;
        }

        this->m_recordsById[id - this->m_minID] = record;
    }
}

// PROVEN: af83934d IDENTICAL/NONE
// OFFSET: 0x0048B360/0x1400AE5A0
void WowClientDB2_Base::LoadRecord(uint8_t* record, const char* strings) {
    if (!strings) {
        return;
    }

    const DB2Meta* meta = this->m_meta;
    int32_t index = 0;

    for (int32_t field = 0; index < meta->m_numStringFields; field++) {
        if (meta->m_fieldTypes[field] == 2) {
            for (uint32_t i = 0; i < static_cast<uint32_t>(meta->m_fieldSizes[field]) / sizeof(uint32_t); i++) {
                DBCFixupString(reinterpret_cast<DBCString*>(&record[meta->m_fieldOffsets[field]])[i], strings);
                meta = this->m_meta;
                index++;
            }
        }

        meta = this->m_meta;
    }
}

// OFFSET: 0x0048C3F0
void WowClientDB2_Base::PurgeUnconfirmedHotfixes() {
    HOTFIX* hotfix = this->m_hotfixes.Head();

    while (!this->m_hotfixes.IsEnd(hotfix)) {
        HOTFIX* next = this->m_hotfixes.RawNext(hotfix);

        if (!hotfix->m_confirmed) {
            this->m_hotfixes.DeleteNode(hotfix);
        }

        hotfix = next;
    }
}

// OFFSET: 0x0048BF80/0x1400AF730
void WowClientDB2_Base::RestoreHotfix(uint32_t recordID, uint32_t timestamp) {
    HOTFIX* hotfix = this->m_hotfixes.NewNode(2, 0, 0);
    hotfix->m_timestamp = timestamp;
    hotfix->m_recordID = recordID;
}

// OFFSET: 0x0048B760
void WowClientDB2_Base::Save(uint32_t hotfixTime) {
    char path[260];
    uint32_t header[12];
    DB2SAVECOUNT count;
    DB2SAVEWRITE save;
    uint32_t bytesWritten;

    WowClientDB2_Base::GetCacheDir(path, this->m_meta->m_cacheName, true);

    save.m_stringOffset = 0;
    save.m_file = OS_INVALID_FILE;
    save.m_buffer = NULL;
    count.m_count = 0;
    count.m_stringSize = 0;
    save.m_strings = NULL;

    memset(header, 0, sizeof(header));
    header[0] = '2HCW';
    header[2] = this->m_meta->m_numFields;
    header[3] = this->m_meta->m_recordSize;
    header[5] = this->m_meta->m_tableHash;
    header[6] = 15595;
    header[7] = hotfixTime;
    header[10] = 1 << LocaleNameToDB2Locale(s_localeCode);

    uint8_t* cachedRecords = this->m_cachedRecords;
    save.m_file = OsCreateFile(path, 0x40000000, 1, 2, 0x80, 0x3F3F3F3F);

    if (save.m_file == OS_INVALID_FILE) {
        return;
    }

    for (int32_t i = 0; i < this->m_numCachedRecords; i++) {
        const uint8_t* record = &cachedRecords[i * this->m_meta->m_recordSize];
        const uint8_t* exported = this->ExportCachedRecord(this->GetID(record));

        if (exported) {
            record = exported;
        }

        count.m_stringSize += this->GetStringsSize(record);
        count.m_count++;
    }

    this->IterateOverCache(SaveCountCallback, &count);

    save.m_strings = static_cast<char*>(SMemAlloc(count.m_stringSize + 1, __FILE__, __LINE__, 0));
    save.m_strings[0] = '\0';
    save.m_buffer = static_cast<uint8_t*>(alloca(this->m_meta->m_recordSize));
    header[1] = count.m_count;
    header[4] = count.m_stringSize + 1;
    save.m_stringOffset = 1;
    OsWriteFile(save.m_file, header, sizeof(header), &bytesWritten);

    if (this->m_numCachedRecords) {
        uint8_t* rows = static_cast<uint8_t*>(SMemAlloc(this->m_numCachedRecords * this->m_meta->m_recordSize, __FILE__, __LINE__, 0));

        for (int32_t i = 0; i < this->m_numCachedRecords; i++) {
            const uint8_t* source = &cachedRecords[i * this->m_meta->m_recordSize];
            uint8_t* record = &rows[i * this->m_meta->m_recordSize];
            memcpy(record, source, this->m_meta->m_recordSize);

            const uint8_t* exported = this->ExportCachedRecord(this->GetID(record));

            if (exported) {
                memcpy(record, exported, this->m_meta->m_recordSize);
                source = exported;
            }

            save.m_stringOffset = this->Export(record, source, &save.m_strings[save.m_stringOffset], save.m_stringOffset);
        }

        OsWriteFile(save.m_file, rows, this->m_numCachedRecords * this->m_meta->m_recordSize, &bytesWritten);
        this->IterateOverCache(SaveWriteCallback, &save);
        SMemFree(rows, __FILE__, __LINE__, 0);
    }

    OsWriteFile(save.m_file, save.m_strings, save.m_stringOffset, &bytesWritten);

    if (save.m_strings) {
        SMemFree(save.m_strings, __FILE__, __LINE__, 0);
    }

    OsCloseFile(save.m_file);

    if (this->m_meta->m_sparse) {
        SFileCloseFile(this->m_file);
        this->m_file = NULL;

        if (this->m_sparseOffsets) {
            SMemFree(this->m_sparseOffsets, __FILE__, __LINE__, 0);
        }

        if (this->m_sparseSizes) {
            SMemFree(this->m_sparseSizes, __FILE__, __LINE__, 0);
        }

        this->m_sparseOffsets = NULL;
        this->m_sparseSizes = NULL;
    }
}

// PROVEN: 35db40fd IDENTICAL/NONE
// OFFSET: 0x0048B5B0/0x1400AE920
void WowClientDB2_Base::StringsFixup(uint8_t* record) {
    const DB2Meta* meta = this->m_meta;
    const char* strings = reinterpret_cast<const char*>(&record[meta->m_recordSize]);
    int32_t index = 0;

    for (int32_t field = 0; index < meta->m_numStringFields; field++) {
        if (meta->m_fieldTypes[field] == 2) {
            for (uint32_t i = 0; i < static_cast<uint32_t>(meta->m_fieldSizes[field]) / sizeof(uint32_t); i++) {
                DBCString& string = reinterpret_cast<DBCString*>(&record[meta->m_fieldOffsets[field]])[i];

                if (*reinterpret_cast<uint32_t*>(&string)) {
                    string = strings;
                    strings += strlen(strings) + 1;
                } else {
                    string = "";
                }

                meta = this->m_meta;
                index++;
            }
        }

        meta = this->m_meta;
    }
}

// OFFSET: 0x0048C360
void WowClientDB2_Base::Unload() {
    this->Free();
    this->m_numRecords = 0;
    this->m_maxID = -1;
    this->m_minID = 0xFFFFFFF;

    DBTABLE* table = WowClientDB2_Base::m_tableByHash.Ptr(this->m_meta->m_tableHash, HASHKEY_NONE());

    if (table) {
        WowClientDB2_Base::m_tableByHash.Delete(table);
    }

    if (this->m_file) {
        SFileCloseFile(this->m_file);
        this->m_file = NULL;
    }

    if (this->m_cacheFile) {
        SFileCloseFile(this->m_cacheFile);
        this->m_cacheFile = NULL;
    }

    this->m_hotfixes.Clear();
}

// OFFSET: 0x0048BA50
uint8_t* WowClientDB2_Base::Unpack(WDataStore* msg) {
    const DB2Meta* meta = this->m_meta;
    int32_t numStringFields = meta->m_numStringFields;
    uint32_t stringSize = 0;
    int32_t stringIndex = 0;
    uint8_t* record;
    char** strings;
    uint16_t* lengths;

    if (numStringFields) {
        record = static_cast<uint8_t*>(alloca(meta->m_recordSize));
        strings = static_cast<char**>(alloca(numStringFields * sizeof(char*)));
        lengths = static_cast<uint16_t*>(alloca(numStringFields * sizeof(char*)));
    } else {
        record = static_cast<uint8_t*>(SMemAlloc(meta->m_recordSize, __FILE__, __LINE__, 0));
        strings = NULL;
        lengths = NULL;
    }

    meta = this->m_meta;

    for (int32_t field = 0; field < meta->m_numFields; field++) {
        switch (meta->m_fieldTypes[field]) {
            case 0: {
                for (uint32_t i = 0; i < static_cast<uint32_t>(meta->m_fieldSizes[field]) / sizeof(uint32_t); i++) {
                    uint32_t* value = reinterpret_cast<uint32_t*>(&record[meta->m_fieldOffsets[field]]) + i;
                    uint32_t data;
                    msg->Get(data);
                    *value = data;
                    meta = this->m_meta;
                }

                break;
            }

            case 2: {
                for (uint32_t i = 0; i < static_cast<uint32_t>(meta->m_fieldSizes[field]) / sizeof(uint32_t); i++) {
                    uint32_t* value = reinterpret_cast<uint32_t*>(&record[meta->m_fieldOffsets[field]]) + i;
                    msg->Get(lengths[stringIndex]);

                    if (lengths[stringIndex]) {
                        uint32_t length = static_cast<int16_t>(lengths[stringIndex]) + 1;
                        strings[stringIndex] = static_cast<char*>(alloca(length));
                        msg->GetString(strings[stringIndex], length);
                        *value = 1;
                        stringSize += static_cast<int16_t>(lengths[stringIndex]) + 1;
                    } else {
                        strings[stringIndex] = NULL;
                        *value = 0;
                    }

                    meta = this->m_meta;
                    stringIndex++;
                }

                break;
            }

            case 3: {
                for (uint32_t i = 0; i < static_cast<uint32_t>(meta->m_fieldSizes[field]) / sizeof(uint32_t); i++) {
                    float* value = reinterpret_cast<float*>(&record[meta->m_fieldOffsets[field]]) + i;
                    float data;
                    msg->Get(data);
                    meta = this->m_meta;
                    *value = data;
                }

                break;
            }

            case 4: {
                for (uint32_t i = 0; i < static_cast<uint32_t>(meta->m_fieldSizes[field]); i++) {
                    uint8_t* value = &record[meta->m_fieldOffsets[field] + i];
                    uint8_t data;
                    msg->Get(data);
                    *value = data;
                    meta = this->m_meta;
                }

                break;
            }
        }

        meta = this->m_meta;
    }

    if (!this->m_meta->m_numStringFields) {
        return record;
    }

    uint8_t* result = static_cast<uint8_t*>(SMemAlloc(stringSize + this->m_meta->m_recordSize, __FILE__, __LINE__, 0));
    memcpy(result, record, this->m_meta->m_recordSize);
    char* dest = reinterpret_cast<char*>(&result[this->m_meta->m_recordSize]);

    for (int32_t i = 0; i < this->m_meta->m_numStringFields; i++) {
        int16_t length = lengths[i]++;

        if (length) {
            memcpy(dest, strings[i], static_cast<int16_t>(length + 1));
            dest += static_cast<int16_t>(lengths[i]);
        }
    }

    this->StringsFixup(result);

    return result;
}

// OFFSET: 0x0048BD30
void WowClientDB2_Base::WriteHotfixes(HOSFILE file) {
    uint32_t data[3];
    uint32_t bytesWritten;

    data[0] = this->m_meta->m_tableHash;

    for (HOTFIX* hotfix = this->m_hotfixes.Head(); !this->m_hotfixes.IsEnd(hotfix); hotfix = this->m_hotfixes.RawNext(hotfix)) {
        if (hotfix->m_confirmed) {
            data[1] = hotfix->m_recordID;
            data[2] = hotfix->m_timestamp;
            OsWriteFile(file, data, sizeof(data), &bytesWritten);
        }
    }
}
