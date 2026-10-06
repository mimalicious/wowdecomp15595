#include <cstddef>
#include <cstdint>
#include "Engine/Source/Services/ModelBlob.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00DD4388/0x140CBA628
static CModelBlob* s_modelBlob;

// STATIC: %0x00CD2E80
static TSHashTableReuse<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE> s_modelBlobHT;

// INLINED
HASHKEY_MODELBLOBFILE::HASHKEY_MODELBLOBFILE() {
    this->m_filename = NULL;
}

// INLINED
bool HASHKEY_MODELBLOBFILE::operator==(const HASHKEY_MODELBLOBFILE& key) const {
    return !SStrCmpI(this->m_filename, key.m_filename, 260);
}

// INLINED
CModelBlob::CHashEntry::CHashEntry() {
    this->m_blob = NULL;
    this->m_entry = NULL;
}

// INLINED
CModelBlob::CHashEntry::~CHashEntry() {
    this->m_blob = NULL;
    this->m_entry = NULL;
}

// INLINED
CModelBlob::CModelBlob(const char* fileName) {
    this->m_data = NULL;
    this->m_entries = NULL;
    this->m_entryCount = 0;
    this->m_stringTable = NULL;
    this->Load(fileName);
}

// OFFSET: %0x005557C0
CModelBlob::~CModelBlob() {
    if (this->m_data) {
        SMemFree(this->m_data, __FILE__, __LINE__, 0);
    }

    this->m_entries = NULL;
    this->m_entryCount = 0;
    this->m_stringTable = NULL;
    s_modelBlobHT.Clear();
}

// OFFSET: %0x00555A30
void ModelBlobDestroy() {
    if (s_modelBlob) {
        delete s_modelBlob;
        s_modelBlob = NULL;
    }
}

// OFFSET: %0x00555B70
int32_t CModelBlob::Query(const char* modelName, C3Vector& min, C3Vector& max) {
    uint32_t hashval = SStrHashHT(modelName);
    HASHKEY_MODELBLOBFILE key;
    key.m_filename = modelName;
    CHashEntry* entry = s_modelBlobHT.Ptr(hashval, key);

    if (!entry) {
        return 0;
    }

    min = entry->m_entry->m_boundingBox.b;
    max = entry->m_entry->m_boundingBox.t;

    return 1;
}

// PROVEN: 5498a7ac IDENTICAL/IDENTICAL
// OFFSET: 0x005483B0/0x1401A0640
int32_t ModelBlobQuery(const char* modelName, C3Vector& min, C3Vector& max) {
    if (!s_modelBlob) {
        return 0;
    }

    return s_modelBlob->Query(modelName, min, max);
}

// OFFSET: %0x00556270
void CModelBlob::HashBlobEntry(SEntry* entry) {
    const char* name = this->m_stringTable + entry->m_filenameOffset;
    uint32_t hashval = SStrHashHT(name);
    HASHKEY_MODELBLOBFILE key;
    key.m_filename = name;

    if (s_modelBlobHT.Ptr(hashval, key)) {
        return;
    }

    CHashEntry* hashEntry = s_modelBlobHT.New(hashval, key, 0, 0);
    hashEntry->m_blob = this;
    hashEntry->m_entry = entry;
}

// OFFSET: %0x00556330
void CModelBlob::Load(const char* fileName) {
    SFileImpl* file = NULL;
    SErrSetLastError(0);

    if (!SFile::OpenEx(NULL, fileName, 0, &file)) {
        if (!SErrGetLastError()) {
            SErrSetLastError(STORM_ERROR_FILE_NOT_FOUND);
        }

        return;
    }

    SStrCopy(this->m_filename, fileName, sizeof(this->m_filename));
    uint32_t size = SFileGetFileSize(file, NULL);
    this->m_data = static_cast<uint8_t*>(SMemAlloc(size, __FILE__, __LINE__, 0));
    SFile::Read(file, this->m_data, size, NULL);
    SFileCloseFile(file);

    uint8_t* chunk = this->m_data + *reinterpret_cast<uint32_t*>(this->m_data + 4) + 8;
    uint32_t chunkSize = *reinterpret_cast<uint32_t*>(chunk + 4);
    this->m_entries = reinterpret_cast<SEntry*>(chunk + 8);
    this->m_entryCount = chunkSize / sizeof(SEntry);
    this->m_stringTable = reinterpret_cast<const char*>(this->m_entries) + chunkSize + 8;

    SEntry* entry = this->m_entries;

    for (uint32_t i = 0; i < this->m_entryCount; i++, entry++) {
        this->HashBlobEntry(entry);
    }
}

// OFFSET: %0x00556440
int32_t ModelBlobLoad(const char* fileName) {
    if (s_modelBlob) {
        delete s_modelBlob;
        s_modelBlob = NULL;
    }

    s_modelBlob = new CModelBlob(fileName);

    if (s_modelBlob->m_data) {
        return 1;
    }

    delete s_modelBlob;
    s_modelBlob = NULL;

    return 0;
}
