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

// STATIC: #0x00B38344
Blizzard::Thread::LocalObject<System_SFile::RawReadInfo> System_SFile::s_rawReadInfo;

// STATIC: #0x00AB9090
static const char* s_specialFiles[3] = { "(signature)", "(listfile)", "(attributes)" };

// INLINED
System_SFile::RawReadInfo::RawReadInfo() {
    this->m_retryNative = false;
}

void STORMAPI SFile::CreateOverlapped(SOVERLAPPED* overlapped) {
}

void STORMAPI SFile::DestroyOverlapped(SOVERLAPPED* overlapped) {
}

// OFFSET: %0x00751EA0
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

// OFFSET: %0x00751F20
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

// OFFSET: %0x00756480
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

// OFFSET: %0x0075A6C0
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

// OFFSET: %0x0070DA20
bool System_SFile::ListfileParser::Next() {
    this->m_token = this->m_data + this->m_pos;

    while (this->m_pos < this->m_size) {
        char c = this->m_data[this->m_pos];

        if (c == '\r' || c == '\n') {
            this->m_data[this->m_pos] = '\0';
            this->m_pos++;

            if (this->m_pos < this->m_size && this->m_data[this->m_pos] == '\n') {
                this->m_pos++;
            }

            return false;
        }

        this->m_pos++;
    }

    if (this->m_special >= 2) {
        return true;
    }

    this->m_special++;
    this->m_token = s_specialFiles[this->m_special];
    this->m_isSpecial = true;

    return false;
}

// OFFSET: %0x007565A0
bool System_SFile::ARCHIVEREC::FillNewHashTable(int32_t hashBits, char* listfile, int32_t size) {
    bool loaded = false;
    void* data = NULL;
    uint32_t loadedSize = 0;

    if (listfile) {
        data = listfile;
    } else {
        loaded = true;

        if (!Blizzard::Mopaq::SFileLoadFile(this, "(listfile)", &data, &loadedSize, 0, 16)) {
            return Blizzard::Mopaq::SFileFileExists(this, "(listfile)", 16, NULL) == 0;
        }

        size = loadedSize;
    }

    ListfileParser parser;
    parser.m_data = static_cast<char*>(data);
    parser.m_pos = 0;
    parser.m_size = size;
    parser.m_special = 0;
    parser.m_token = NULL;
    parser.m_isSpecial = false;

    while (!parser.Next()) {
        const char* name = parser.m_token;
        uint32_t hashIndex = SearchHashTable(this, name, true);

        if (hashIndex == -1) {
            if (parser.m_isSpecial) {
                continue;
            }

            if (loaded) {
                SMemFree(data, __FILE__, __LINE__, 0);
            }

            return false;
        }

        if (!this->m_hashEntryTable->AddEntry(name, this->m_hashTable[hashIndex].m_blockIndex, this->m_blockEntryTable)) {
            if (!this->IncreaseHashBits(&hashBits)) {
                if (loaded) {
                    SMemFree(data, __FILE__, __LINE__, 0);
                }

                return false;
            }

            parser.m_pos = 0;
            parser.m_special = 0;
            parser.m_isSpecial = false;
        }
    }

    if (loaded) {
        SMemFree(data, __FILE__, __LINE__, 0);
    }

    return true;
}
