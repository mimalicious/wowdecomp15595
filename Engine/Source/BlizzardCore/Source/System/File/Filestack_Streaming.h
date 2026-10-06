#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_FILE_FILESTACK_STREAMING_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_FILE_FILESTACK_STREAMING_H

#include <cstdint>
#include <map>
#include <string>
#include "Engine/Source/BlizzardCore/Source/System/File/Filestack.h"
#include "Engine/Source/BlizzardCore/Source/System/File/Types.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/Storm/H/Types.h"

class FileStack_Streaming {
    public:
    /* 0000 */ Blizzard::File::Filesystem* m_fs;
    /* 0004 */ System_File::Stacked::FileParms* m_parms;
    /* 0008 */ std::string m_key;
    /* 0024 */ Blizzard::Storage::StorageUnit* m_unit;

    FileStack_Streaming(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms);
    ~FileStack_Streaming();
    Blizzard::File::StreamRecord* InitRecord(Blizzard::File::StreamRecord* record, int64_t size);
    Blizzard::File::StreamRecord* StreamingOpen();
};

class StreamDirWalk {
    public:
    static bool Callback(const Blizzard::File::ProcessDirParms& parms);

    /* 0000 */ std::map<std::string, bool> m_files;

    bool Walk(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms);
};

namespace System_File {

std::string GetInode(const std::string& path);

namespace Stacked {

class LockAndMap {
    public:
    static LockAndMap* s_instance;

    static LockAndMap* Get();
    static Blizzard::File::StreamRecord* GetRecord(const std::string& key, const char* path);
    static void Release();

    /* 0000 */ Blizzard::Lock::RMutex m_mutex;
    /* 0018 */ std::map<std::string, Blizzard::File::StreamRecord*> m_map;
};

extern Blizzard::File::Filesystem s_streamstack;
extern int32_t count_retry;
extern bool g_callerGuaranteesStreamingWriteHasProperData;
extern int64_t size_local_read;

Blizzard::File::StreamRecord* STORMAPI AllocStreamingRecord(const char* path);
void PushStreaming();
void StreamingShutdown();
bool stream_close(Blizzard::File::Filesystem* fs, FileParms* parms);
bool stream_dirwalk(Blizzard::File::Filesystem* fs, FileParms* parms);
bool stream_exists(Blizzard::File::Filesystem* fs, FileParms* parms);
bool stream_flush(Blizzard::File::Filesystem* fs, FileParms* parms);
bool stream_getfileinfo(Blizzard::File::Filesystem* fs, FileParms* parms);
bool stream_open(Blizzard::File::Filesystem* fs, FileParms* parms);
bool stream_read(Blizzard::File::Filesystem* fs, FileParms* parms);
bool stream_readp(Blizzard::File::Filesystem* fs, FileParms* parms);
bool stream_shutdown(Blizzard::File::Filesystem* fs, FileParms* parms);
bool stream_unlink(Blizzard::File::Filesystem* fs, FileParms* parms);
bool stream_writep(Blizzard::File::Filesystem* fs, FileParms* parms);

}
}

#endif
