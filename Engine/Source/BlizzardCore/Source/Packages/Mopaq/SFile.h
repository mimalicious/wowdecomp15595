#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILE_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILE_H

#include <cstdint>
#include <set>
#include <vector>
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Digest.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/ThreadLocalStorage.h"

namespace System_SFile {

class ARCHIVEREC;

struct RawReadInfo {
    /* 0000 */ bool m_retryNative;

    RawReadInfo();
};

struct ListFileBlock {
    /* 0000 */ char* m_data;
    /* 0004 */ uint32_t m_size;
    /* 0008 */ uint32_t m_free;
    /* 000C */ ListFileBlock* m_next;

    ListFileBlock(uint32_t size);
};

class ListFile {
    public:
    static void FreeBlocks(ListFileBlock* blocks);

    /* 0000 */ std::vector<const char*> m_names;
    /* 0018 */ ListFileBlock* m_blocks;

    ListFile();
    ~ListFile();
    const char* Add(const char* name);
    bool Load(ARCHIVEREC* archive);
    void Parse(char* data, int64_t size);
    bool Set(const char* data, int64_t size);
};

struct ListFileParser {
    static const char* k_specialFiles[3];

    /* 0000 */ std::set<const char*> m_specials;
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ const char* m_token;
    /* 0014 */ bool m_isSpecial;
    /* 0015 */ uint8_t unk_0015[3];
    /* 0018 */ ListFile* m_listfile;
    /* 001C */ std::vector<const char*>::iterator m_current;

    ListFileParser(ListFile* listfile);
    bool Next();
};

extern Blizzard::Thread::LocalObject<RawReadInfo> s_rawReadInfo;

}

namespace Blizzard {
namespace Mopaq {

struct HSFILERecord;

bool SFileCloseFile(HSFILERecord* file);
bool SFileCreateFile(System_SFile::ARCHIVEREC* handle, const char* name, uint32_t size, uint32_t flags, HSFILERecord** result);
bool SFileGetArchiveFullName(System_SFile::ARCHIVEREC* handle, char* name, uint32_t size);
bool SFileGetArchiveName(System_SFile::ARCHIVEREC* handle, char* name, uint32_t size);
int32_t SFileFileIsLocal(HSFILERecord* file, bool ignorePatch);
uint32_t SFileGetFileCompressedSize(HSFILERecord* file, uint32_t* sizeHigh);
int32_t SFileGetFilePointer(HSFILERecord* file);
bool SFileIsFileCompressed(HSFILERecord* file);
bool SFileIsPatchFile(HSFILERecord* file);
bool SFileGetFileName(HSFILERecord* file, char* name, uint32_t size);
bool SFileGetFileMD5(HSFILERecord* file, Blizzard::Util::Digest<16>* md5);
bool SFileGetLogicalArchive(HSFILERecord* file, System_SFile::ARCHIVEREC** archive);
bool SFileGetDeltaDigests(HSFILERecord* file, Blizzard::Util::Digest<16>& source, Blizzard::Util::Digest<16>& target);
int64_t SFileGetFileSize(HSFILERecord* file, uint32_t* sizeHigh);
bool SFileLoadFile(System_SFile::ARCHIVEREC* handle, const char* name, void* buffer, uint32_t* size, uint32_t extra, uint32_t flags);
bool SFileLoadFile(System_SFile::ARCHIVEREC* handle, const char* name, void** data, uint32_t* size, uint32_t extra, uint32_t flags);
int32_t SFileOpenFileEx(System_SFile::ARCHIVEREC* handle, const char* name, const Blizzard::Util::Digest<16>& md5, uint32_t flags, HSFILERecord** result);
int32_t SFileOpenFileEx(System_SFile::ARCHIVEREC* handle, const char* name, uint32_t flags, HSFILERecord** result);
bool SFileReadFile(HSFILERecord* file, void* buffer, uint32_t count, uint32_t* bytesRead);
int64_t SFileSetFilePointer(HSFILERecord* file, int32_t distance, int32_t* distanceHigh, uint32_t method);
bool SFileUnloadFile(void* data);
bool SFileWriteFile(HSFILERecord* file, const void* data, uint32_t size);

}
}

#endif
