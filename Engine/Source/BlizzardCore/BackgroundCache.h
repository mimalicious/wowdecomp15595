#ifndef ENGINE_SOURCE_BLIZZARDCORE_BACKGROUNDCACHE_H
#define ENGINE_SOURCE_BLIZZARDCORE_BACKGROUNDCACHE_H

#include <cstddef>
#include <cstdint>
#include <deque>
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Digest.h"
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Jobs.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/Jobs/WorkerPool.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/TempBuffer.h"
#include "Engine/Source/Storm/H/Types.h"

namespace Blizzard {
namespace File {
struct ProcessDirParms;
}
namespace Mopaq {
struct HSFILERecord;
}
}

namespace System_SFile {
class ARCHIVEREC;
}

namespace BackgroundCache {

class CacheJob : public Blizzard::Jobs::Job {
    public:
    CacheJob(int32_t dependencyCount);
    virtual int64_t EstimateCost() const = 0;
};

class CacheManager : public Blizzard::Jobs::WorkerPool {
    public:
    static char s_locale[5];

    static bool ResolveUpdatePath(System_SFile::ARCHIVEREC* archive, const char* filename, const char* prefix, char* buffer, uint32_t size);
    static System_SFile::ARCHIVEREC* ResolveOggTarget(System_SFile::ARCHIVEREC* archive);
    static System_SFile::ARCHIVEREC* ResolvePatchTarget(System_SFile::ARCHIVEREC* archive);
    static void OggConvertCallback(const char* name);
    static void PatchCompleteCallback(const void* data, uint64_t size, Blizzard::Mopaq::HSFILERecord* file, const Blizzard::Util::Digest<16>& md5);
    static bool STORMAPI WritePatchFile(const void* data, uint64_t size, Blizzard::Mopaq::HSFILERecord* file, const char* name, const Blizzard::Util::Digest<16>& md5);

    /* 000C */ std::deque<Blizzard::Jobs::RefPtr<Blizzard::Jobs::Job> > m_oggJobs;
    /* 002C */ std::deque<Blizzard::Jobs::RefPtr<Blizzard::Jobs::Job> > m_writeJobs;
    /* 004C */ Blizzard::Lock::Mutex m_mutex;
    /* 0064 */ bool m_oggActive;
    /* 0068 */ uint64_t m_queuedBytes;
    /* 0070 */ uint64_t m_memoryLimit;
    /* 0078 */ int32_t m_writeRuns;

    CacheManager(uint64_t memoryLimit);
    virtual ~CacheManager();
    void Pump();
    void QueueNextJob();
    void QueueOggConversion(const char* name);
    void QueuePatchWrite(const void* data, uint64_t size, Blizzard::Mopaq::HSFILERecord* file, const char* name, const Blizzard::Util::Digest<16>& md5);
};

extern CacheManager* s_cacheManager;

void InitManager(int32_t memoryLimit);
void Pump();
void SetLocale(const char* locale);
void ShutdownManager();

}

namespace OggToWavConversion {

typedef bool (*DECOMPRESSOGGCALLBACK)(const void* data, uint32_t size, Blizzard::Memory::TempBuffer<uint8_t, 0x400000>* buffer);
typedef void (*CONVERTOGGCALLBACK)(const char* name);

class OggConverterJob : public BackgroundCache::CacheJob {
    public:
    /* 002C */ char m_name[260];
    /* 0130 */ char m_wavName[260];
    /* 0234 */ System_SFile::ARCHIVEREC* m_archive;

    OggConverterJob(const char* name);
    bool Convert();
    virtual bool Execute();
    virtual uint32_t GetClassId() const;
    virtual int64_t EstimateCost() const;
    bool WriteWav(const void* data, uint32_t size);
};

extern DECOMPRESSOGGCALLBACK s_DecompressOggCallback;
extern CONVERTOGGCALLBACK s_ConvertOggCallback;

bool InitCache(DECOMPRESSOGGCALLBACK callback);
void SetCallback(CONVERTOGGCALLBACK callback);
bool ShutdownCache();

}

namespace PatchOnDemand {

class PoDWriteJob : public BackgroundCache::CacheJob {
    public:
    static bool WriteFile(System_SFile::ARCHIVEREC* archive, const char* name, const void* data, int64_t size, const Blizzard::Util::Digest<16>& md5);

    /* 002C */ uint32_t m_size;
    /* 0030 */ uint8_t* m_data;
    /* 0034 */ System_SFile::ARCHIVEREC* m_archive;
    /* 0038 */ char m_name[260];
    /* 013C */ Blizzard::Util::Digest<16> m_md5;

    PoDWriteJob(const void* data, int64_t size, System_SFile::ARCHIVEREC* archive, const char* name, const Blizzard::Util::Digest<16>& md5);
    virtual ~PoDWriteJob();
    virtual bool Execute();
    virtual uint32_t GetClassId() const;
    virtual int64_t EstimateCost() const;
};

namespace Cache {

class ArchiveList {
    public:
    struct ListItem {
        /* 0000 */ ListItem* m_prev;
        /* 0004 */ ListItem* m_next;
        /* 0008 */ int32_t m_type;
        /* 000C */ int32_t m_build;

        ~ListItem();
    };

    static bool DirCallback(const Blizzard::File::ProcessDirParms& parms);

    /* 0000 */ int32_t m_count;
    /* 0004 */ ListItem m_head;
    /* 0014 */ ListItem* m_tail;
    /* 0018 */ char* m_path;
    /* 001C */ char* m_locale;
    /* 0020 */ bool m_oldWorld;

    ~ArchiveList();
    int32_t Add(int32_t type, int32_t build);
    int32_t AddUpdate(int32_t build);
    int32_t AddUpdateBase(int32_t build);
    int32_t AddUpdateLang(int32_t build);
    int32_t EnumerateArchives(int32_t (*callback)(const char* path, void* param, bool update, bool base, bool alternate), void* param);
    ListItem* FindInsertPosition(int32_t type, int32_t build);
    void GenerateFilename(ListItem* item, char* buffer, uint32_t size);
    int32_t LoadArchives(const char* path, const char* locale, int32_t oldWorld);
};

extern const char* s_oldWorldSuffix;
extern ArchiveList s_archiveList;

int32_t EnumerateArchives(int32_t (*callback)(const char* path, void* param, bool update, bool base, bool alternate), void* param);
void Init(int32_t memoryLimit);
int32_t LoadArchives(const char* path, const char* locale, int32_t oldWorld);
void Shutdown();

}

}

#endif
