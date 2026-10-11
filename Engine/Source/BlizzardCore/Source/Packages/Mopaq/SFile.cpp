#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQEncrypt.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/MPQGlobals.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFile.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/StackedMopaq.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "Engine/Source/Storm/H/storm.h"

Blizzard::Thread::LocalObject<System_SFile::RawReadInfo> System_SFile::s_rawReadInfo;

const char* System_SFile::ListFileParser::k_specialFiles[3] = { "(signature)", "(listfile)", "(attributes)" };

// INLINED
System_SFile::RawReadInfo::RawReadInfo() {
    this->m_retryNative = false;
}

void STORMAPI SFile::CreateOverlapped(SOVERLAPPED* overlapped) {
}

void STORMAPI SFile::DestroyOverlapped(SOVERLAPPED* overlapped) {
}

// OFFSET: 0x007F23C0/0x1404FF5A0
bool Blizzard::Mopaq::SFileGetArchiveName(System_SFile::ARCHIVEREC* handle, char* name, uint32_t size) {
    *name = '\0';

    System_SFile::ARCHIVEREC* archive = System_SFile::GetArchivePtr(handle, true);

    if (!archive) {
        return false;
    }

    Blizzard::String::Copy(name, archive->m_name, size);
    archive->RemoveInternalArchiveRef();

    return true;
}

// OFFSET: 0x007F2440/0x1404FF620
bool Blizzard::Mopaq::SFileGetArchiveFullName(System_SFile::ARCHIVEREC* handle, char* name, uint32_t size) {
    *name = '\0';

    System_SFile::ARCHIVEREC* archive = System_SFile::GetArchivePtr(handle, true);

    if (!archive) {
        return false;
    }

    if (archive->m_directPath.size()) {
        Blizzard::String::Format(name, size, "%s#%s", archive->m_name, archive->m_directPath.c_str());
    } else {
        Blizzard::String::Copy(name, archive->m_name, size);
    }

    archive->RemoveInternalArchiveRef();

    return true;
}

// PROVEN: d9c6f75e IDENTICAL/DIFFERENT
// OFFSET: 0x007B0E40/0x1404A5C60
bool Blizzard::Mopaq::SFileUnloadFile(void* data) {
    SMemFree(data, __FILE__, __LINE__, 0);
    return true;
}

// OFFSET: 0x007F7C00/0x140506DF0
bool Blizzard::Mopaq::SFileLoadFile(System_SFile::ARCHIVEREC* handle, const char* name, void** data, uint32_t* size, uint32_t extra, uint32_t flags) {
    uint8_t* buffer = NULL;
    *data = NULL;

    if (size) {
        *size = 0;
    }

    Blizzard::Util::Digest<16> md5 = System_SFile::s_emptyMd5;
    HSFILERecord* file = NULL;

    if (SFileOpenFileEx(handle, name, md5, flags, &file)) {
        uint32_t bytes = static_cast<uint32_t>(file->GetFileSize());
        buffer = static_cast<uint8_t*>(SMemAlloc(bytes + extra, __FILE__, __LINE__, 0));
        uint32_t read;

        if (SFileReadFile(file, buffer, bytes, &read)) {
            if (extra) {
                memset(buffer + bytes, 0, extra);
            }

            *data = buffer;
            buffer = NULL;

            if (size) {
                *size = bytes;
            }
        }
    }

    SMemFree(buffer, __FILE__, __LINE__, 0);
    SFileCloseFile(file);

    return *data != NULL;
}

// OFFSET: 0x007F7D20/0x140506F60
bool Blizzard::Mopaq::SFileLoadFile(System_SFile::ARCHIVEREC* handle, const char* name, void* buffer, uint32_t* size, uint32_t extra, uint32_t flags) {
    bool result = false;

    if (size) {
        *size = 0;
    }

    Blizzard::Util::Digest<16> md5 = System_SFile::s_emptyMd5;
    HSFILERecord* file = NULL;

    if (SFileOpenFileEx(handle, name, md5, flags, &file)) {
        uint32_t bytes = static_cast<uint32_t>(file->GetFileSize());
        uint32_t read;

        if (SFileReadFile(file, buffer, bytes, &read)) {
            if (extra) {
                memset(static_cast<uint8_t*>(buffer) + bytes, 0, extra);
            }

            if (size) {
                *size = bytes;
            }

            result = true;
        }
    }

    SFileCloseFile(file);

    return result;
}

// INLINED
System_SFile::ListFileBlock::ListFileBlock(uint32_t size) {
    this->m_data = new char[size];
    this->m_size = size;
    this->m_free = size;
    this->m_next = NULL;
    memset(this->m_data, 0, size);
}

// OFFSET: 0x007B1C90
void System_SFile::ListFile::FreeBlocks(ListFileBlock* blocks) {
    while (blocks) {
        ListFileBlock* block = blocks;
        blocks = blocks->m_next;
        delete[] block->m_data;
        delete block;
    }
}

// INLINED
System_SFile::ListFile::ListFile() {
    this->m_blocks = NULL;
}

// INLINED
System_SFile::ListFile::~ListFile() {
    FreeBlocks(this->m_blocks);
}

// OFFSET: 0x007D3960
void System_SFile::ListFile::Parse(char* data, int64_t size) {
    char* start = data;

    for (int64_t pos = 0; pos < size; pos++) {
        char c = data[pos];

        if (c == '\r' || c == '\n') {
            data[pos] = '\0';
            pos++;

            if (pos < size && data[pos] == '\n') {
                data[pos] = '\0';
                pos++;
            }

            this->m_names.push_back(start);
            start = data + pos;
        }
    }
}

// OFFSET: 0x007D3A50
bool System_SFile::ListFile::Set(const char* data, int64_t size) {
    this->m_names.clear();

    if (this->m_blocks) {
        FreeBlocks(this->m_blocks);
    }

    this->m_blocks = new ListFileBlock(static_cast<uint32_t>(size));
    memcpy(this->m_blocks->m_data, data, static_cast<uint32_t>(size));
    this->m_blocks->m_free = 0;
    this->Parse(this->m_blocks->m_data, size);

    return true;
}

// OFFSET: 0x007D3B60
const char* System_SFile::ListFile::Add(const char* name) {
    if (!this->m_blocks) {
        this->m_blocks = new ListFileBlock(260000);
    }

    ListFileBlock* block = this->m_blocks;
    char* copy;

    do {
        uint32_t length = Blizzard::String::Length(name);
        copy = NULL;

        if (length + 1 < block->m_free) {
            copy = block->m_data + block->m_size - block->m_free;
            memcpy(copy, name, length);
            block->m_free -= length + 1;
        }

        if (!block->m_next) {
            block->m_next = new ListFileBlock(260000);
        }

        block = block->m_next;
    } while (!copy);

    this->m_names.push_back(copy);

    return copy;
}

// OFFSET: 0x007F4A80
bool System_SFile::ListFile::Load(ARCHIVEREC* archive) {
    Blizzard::Mopaq::HSFILERecord* file = NULL;
    bool result = false;

    if (Blizzard::Mopaq::SFileOpenFileEx(archive, "(listfile)", s_emptyMd5, 16, &file) != 1) {
        return false;
    }

    uint32_t size = static_cast<uint32_t>(file->GetFileSize());
    this->m_blocks = new ListFileBlock(size);

    if (Blizzard::Mopaq::SFileReadFile(file, this->m_blocks->m_data, size, NULL)) {
        this->m_blocks->m_free = 0;
        this->Parse(this->m_blocks->m_data, size);
        result = true;
    }

    if (file) {
        if (file->m_handle && !file->m_handle->Release()) {
            file->m_handle = NULL;
        }

        delete file;
    }

    return result;
}

// OFFSET: 0x007D36B0
System_SFile::ListFileParser::ListFileParser(ListFile* listfile) {
    this->m_listfile = listfile;
    this->unk_000C = 0;
    this->m_token = NULL;
    this->m_isSpecial = false;
    this->m_current = listfile->m_names.begin();

    for (int32_t i = 0; i < 3; i++) {
        this->m_specials.insert(k_specialFiles[i]);
    }
}

// OFFSET: 0x007D01F0
bool System_SFile::ListFileParser::Next() {
    if (this->m_current == this->m_listfile->m_names.end()) {
        if (this->m_specials.empty()) {
            return true;
        }

        this->m_token = *this->m_specials.begin();
        this->m_specials.erase(this->m_specials.begin());
        this->m_isSpecial = true;

        return false;
    }

    this->m_token = *this->m_current;
    this->m_isSpecial = false;
    ++this->m_current;

    for (int32_t i = 0; i < 3 && !this->m_isSpecial; i++) {
        if (k_specialFiles[i] && this->m_token && !strcmp(k_specialFiles[i], this->m_token)) {
            this->m_isSpecial = true;
            this->m_specials.erase(k_specialFiles[i]);
        }
    }

    return false;
}

// OFFSET: 0x007F5340/0x140503350
bool System_SFile::ARCHIVEREC::FillNewHashTable(int32_t hashBits, char* listfile, int32_t size) {
    if (!this->m_listfile) {
        this->m_listfile = new ListFile();
    }

    if (listfile) {
        this->m_listfile->Set(listfile, size);
    } else if (!this->m_listfile->Load(this)) {
        return Blizzard::Mopaq::SFileFileExists(this, "(listfile)", 16, NULL) == 0;
    }

    ListFileParser parser(this->m_listfile);

    while (!parser.Next()) {
        const char* name = parser.m_token;
        uint32_t hashIndex = SearchHashTable(this, name, true);

        if (hashIndex == -1) {
            if (!parser.m_isSpecial) {
                return false;
            }

            continue;
        }

        if (!this->m_hashEntryTable->AddEntry(name, this->m_hashTable[hashIndex].m_blockIndex, this->m_blockEntryTable)) {
            if (!this->IncreaseHashBits(&hashBits)) {
                return false;
            }

            parser.unk_000C = 0;
            parser.m_current = parser.m_listfile->m_names.begin();
            parser.m_token = NULL;
            parser.m_isSpecial = false;
        }
    }

    return true;
}
