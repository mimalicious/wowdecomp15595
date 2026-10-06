#ifndef ENGINE_SOURCE_BLIZZARDCORE_SFILE2_CORE_H
#define ENGINE_SOURCE_BLIZZARDCORE_SFILE2_CORE_H

#include <cstddef>
#include <cstdint>
#include <deque>
#include <map>
#include <string>
#include "Engine/Source/BlizzardCore/Source/System/File/Types.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/TempBuffer.h"
#include "Engine/Source/BlizzardCore/Source/System/Thread/ThreadLocalStorage.h"
#include "Engine/Source/Storm/H/Types.h"

namespace Blizzard { namespace Mopaq { struct HSFILERecord; } }
class SEvent;

namespace System_SFile {

class ARCHIVEREC;

}

struct SOVERLAPPED {
    /* 0000 */ uint32_t Offset;
    /* 0004 */ SEvent* hEvent;
};

struct SArchive {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ System_SFile::ARCHIVEREC* m_archive;
};

struct SFileLock {
    /* 0000 */ Blizzard::Lock::Mutex m_mutex;
    /* 0018 */ uint32_t unk_0018;
    /* 001C */ uint8_t unk_001C[20];
    /* 0030 */ int64_t m_position;

    SFileLock();
    ~SFileLock();
};

struct SFileStreamingStatus {
    /* 0000 */ bool m_enabled;
    /* 0001 */ bool m_initialized;
    /* 0002 */ bool m_flag2;
    /* 0003 */ bool m_flag3;
    /* 0004 */ uint32_t m_build;
};

class SFileImpl {
    public:
    /* 0000 */ int32_t m_type;
    /* 0004 */ Blizzard::Mopaq::HSFILERecord* m_file;
    /* 0008 */ Blizzard::File::StreamRecord* m_stream;
    /* 000C */ SArchive* m_archive;
    /* 0010 */ char* m_name;
    /* 0014 */ char* m_actualName;
    /* 0018 */ uint32_t m_async;
    /* 001C */ uint32_t unk_001C;
    /* 0020 */ SFileLock* m_lock;
    /* 0024 */ uint8_t m_lockData[sizeof(SFileLock)];

    SFileImpl(int32_t type);
    ~SFileImpl();
};

class SFile {
    public:
    static uint32_t s_locale;

    static char m_basePath[260];

    static int32_t STORMAPI CloseArchive(SArchive* archive);
    static void STORMAPI ConvertOggToWavInBackground(const char* name);
    static int32_t STORMAPI CreateArchive(const char* path, int32_t priority, SArchive** archive);
    static void STORMAPI CreateOverlapped(SOVERLAPPED* overlapped);
    static void Destroy();
    static void STORMAPI DestroyOverlapped(SOVERLAPPED* overlapped);
    static void DisableSFileCheckDisk();
    static int32_t STORMAPI EnableDirectAccess(uint32_t flags);
    static void STORMAPI EnableHash(bool enable);
    static int32_t STORMAPI FileExists(const char* name);
    static int32_t STORMAPI FileExistsEx(const char* name, uint32_t flags);
    static int32_t STORMAPI FileExistsInArchives(const char* name);
    static bool STORMAPI FileGetIsLocalAmount(const char* name, int64_t& local, int64_t& total);
    static void STORMAPI FileGetIsLocalAmount(SFileImpl* file, int64_t& local, int64_t& total);
    static int32_t STORMAPI FileHasHigherPriority(const char* name, const char* extension, uint32_t size);
    static int32_t STORMAPI FileIsLocal(const char* name, int32_t queue);
    static int32_t STORMAPI FileIsLocal(SFileImpl* file, int32_t queue);
    static int32_t STORMAPI FileRangeIsLocal(SFileImpl* file, int64_t offset, uint32_t size);
    static int32_t STORMAPI GetActualFileName(SFileImpl* file, char* buffer, uint32_t size);
    static int32_t STORMAPI GetBasePath(char* path, uint32_t size);
    static float GetBGStreamingDownloadedPercentage();
    static const char* GetDataPath();
    static const char* GetErrorDetails();
    static float GetStreamingBandwidth();
    static int32_t Initialize();
    static void STORMAPI InitializeRepair(const char* path);
    static int32_t STORMAPI InitializeStreaming(const char* mfilPath);
    static int32_t STORMAPI IsArchiveComplete(SArchive* archive);
    static int32_t IsBackgroundStreamingInProgress();
    static int32_t IsStreamingMode();
    static int32_t IsTrial();
    static int32_t STORMAPI List(SArchive* archive, int32_t (*callback)(const char* fileName, void* param), void* param);
    static void LogFileAccess(SFileImpl* file, int32_t mode, bool flag);
    static int32_t STORMAPI LoadFile(const char* name, void** data, uint32_t* size, uint32_t extra, SOVERLAPPED* overlapped);
    static int32_t STORMAPI MakeFileLocal(const char* name, int32_t queue, bool wait, int64_t budget, int32_t* status);
    static int32_t STORMAPI MakeFileLocal(SFileImpl* file, int32_t queue, bool wait, int64_t budget, int32_t* status);
    static int32_t STORMAPI MakeFileRangeLocal(SFileImpl* file, int32_t queue, bool wait, int64_t offset, uint32_t size);
    static int32_t STORMAPI OpenArchive(const char* path, int32_t priority, uint32_t flags, SArchive** archive);
    static int32_t STORMAPI OpenEx(SArchive* archive, const char* name, uint32_t flags, SFileImpl** result);
    static int32_t STORMAPI OpenPathAsArchive(SArchive* archive, const char* path, int32_t priority, uint32_t flags, SArchive** result);
    static void STORMAPI PauseBackgroundStreaming(bool pause);
    static int32_t STORMAPI ReadAtPosition(SFileImpl* file, void* buffer, uint32_t offset, uint32_t count, uint32_t* bytesRead);
    static int32_t STORMAPI ReadFile(SArchive* archive, const char* name, void** data, uint32_t* size, uint32_t extra, uint32_t flags, SOVERLAPPED* overlapped);
    static void RebuildHash();
    static int32_t STORMAPI SetBasePath(const char* path);
    static int32_t STORMAPI SetDataPath(const char* path);
    static int32_t STORMAPI SetDataPathAlternate(const char* path);
    static int32_t STORMAPI SetFilePointer(SFileImpl* file, int32_t distance, int32_t* distanceHigh, uint32_t method);
    static void STORMAPI SetReadFatal(bool fatal);
    static void STORMAPI SetStreamingStatus(bool enabled, bool initialized, bool flag2, bool flag3, uint32_t build);
    static void STORMAPI SetTrial(bool trial);
    static void ShutdownStreaming();
    static int32_t STORMAPI Unload(void* data);

    /* 0000 */ uint8_t unk_0000[1];
};

namespace System_SFile2 {

struct SFileFatalInfo {
    /* 0000 */ bool m_fatal;

    SFileFatalInfo();
};

struct FileHashEntry {
    /* 0000 */ std::string m_path;
    /* 001C */ int32_t m_type;
};

struct FileHashCompare {
    size_t operator()(const std::string& key) const;
    bool operator()(const std::string& key1, const std::string& key2) const;
};

typedef std::map<std::string, FileHashEntry, FileHashCompare> FileHash;

struct ScanDirCookie {
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ const char* m_root;
    /* 0008 */ uint32_t m_rootLength;
    /* 000C */ const char* m_prefix;
};

extern bool s_trial;
extern char s_dataPath[260];
extern char s_dataPath2[260];
extern bool s_checkDisk;
extern uint32_t s_directAccess;
extern uint32_t s_useOldFindFile;
extern char s_hashedBasePath[260];
extern bool s_fileHashBuilt;
extern Blizzard::Lock::RMutex s_fileHashLock;
extern FileHash s_fileHash;
extern Blizzard::Thread::LocalObject<SFileFatalInfo> s_fatalInfo;

void BuildFileSystemHash();
int32_t FindFile(const char* name, char* path, uint32_t size, uint32_t flags, int32_t* type, System_SFile::ARCHIVEREC** archive);
int32_t OldFindFile(const char* name, char* path, uint32_t size, uint32_t flags, int32_t* type);
bool ScanDirForFileSystemHash(const char* directory);

}

int32_t STORMAPI SFileCloseFile(SFileImpl* file);
uint32_t STORMAPI SFileGetFileSize(SFileImpl* file, uint32_t* sizeHigh);
int32_t STORMAPI SFileAuthenticateArchiveEx(SArchive* archive, uint32_t* result, const uint8_t* modulus, uint32_t modulusSize, const uint8_t* exponent, uint32_t exponentSize);
int32_t STORMAPI SFileCloseArchive(SArchive* archive);
int32_t STORMAPI SFileLoadFileEx(SArchive* archive, const char* name, void** data, uint32_t* size, uint32_t extra, uint32_t flags, SOVERLAPPED* overlapped);
int32_t STORMAPI SFileOpenArchive(const char* path, int32_t priority, uint32_t flags, SArchive** archive);
int32_t STORMAPI SFileUnloadFile(void* data);
int32_t STORMAPI SFileOpenFile(const char* name, SFileImpl** result);
int32_t STORMAPI SFileReadFile(SFileImpl* file, void* buffer, uint32_t count, uint32_t* bytesRead, SOVERLAPPED* overlapped, uint32_t unused);

class MD5;

int32_t SFileClearBlockTableMD5s();
int32_t STORMAPI SFileSetFileBlockEntryMD5(const char* name, const MD5& md5);
void STORMAPI SFileSetSignatureCallbacks(int32_t enable);

#endif
