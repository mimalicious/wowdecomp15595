#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILE_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILE_H

#include <cstdint>
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Digest.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/ThreadLocalStorage.h"

namespace System_SFile {

class ARCHIVEREC;

struct RawReadInfo {
    /* 0000 */ bool m_retryNative;

    RawReadInfo();
};

struct ListfileParser {
    /* 0000 */ char* m_data;
    /* 0004 */ uint32_t m_pos;
    /* 0008 */ uint32_t m_size;
    /* 000C */ uint32_t m_special;
    /* 0010 */ char unk_0010[260];
    /* 0114 */ const char* m_token;
    /* 0118 */ bool m_isSpecial;
    /* 0119 */ uint8_t unk_0119[3];

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
int32_t SFileFileIsLocal(HSFILERecord* file, int32_t option);
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
