#include <cstdint>
#include <cstdio>
#include <cstring>
#include "Engine/Source/BlizzardCore/BackgroundCache.h"
#include "Engine/Source/BlizzardCore/OggDecompress.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/Mpqstack_Delta.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFile.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/File/Types.h"
#include "Engine/Source/BlizzardCore/Source/System/Log/Log.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/MemoryStorm.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"

// STATIC: 0x00EB3C60/0x140DAB9A8
char BackgroundCache::CacheManager::s_locale[5];

// STATIC: 0x00EB3C5C/0x140DAB9A0
BackgroundCache::CacheManager* BackgroundCache::s_cacheManager;

// STATIC: 0x00EB3C58
OggToWavConversion::DECOMPRESSOGGCALLBACK OggToWavConversion::s_DecompressOggCallback;

// STATIC: 0x00EB3D7C/0x140D9FF28
OggToWavConversion::CONVERTOGGCALLBACK OggToWavConversion::s_ConvertOggCallback;

// STATIC: 0x00D0589C/0x140BB7F80
PatchOnDemand::Cache::ArchiveList PatchOnDemand::Cache::s_archiveList = { 0, { NULL, NULL, 13, 0 }, &PatchOnDemand::Cache::s_archiveList.m_head, NULL, NULL };

// OFFSET: 0x007A2DB0/0x14050B8D0
void BackgroundCache::SetLocale(const char* locale) {
    Blizzard::String::Copy(CacheManager::s_locale, locale, sizeof(CacheManager::s_locale));
}

// OFFSET: 0x007A2DD0/0x14050B8F0
void PatchOnDemand::Cache::ArchiveList::GenerateFilename(ListItem* item, char* buffer, uint32_t size) {
    *buffer = '\0';

    if (item->m_type != 5) {
        Blizzard::String::Format(buffer, size, "%s/Cache/%s", this->m_path, this->m_locale);

        if (!Blizzard::File::Exists(buffer)) {
            Blizzard::File::CreateDirectory(buffer, true);
        }
    }

    *buffer = '\0';

    switch (item->m_type) {
        case 0: {
            Blizzard::String::Format(buffer, size, "%s/Cache/%s/patch-alternate-%d.MPQ", this->m_path, this->m_locale, item->m_build);
            break;
        }

        case 1: {
            Blizzard::String::Format(buffer, size, "%s/Cache/%s/SoundCache-patch-%s-%d.MPQ", this->m_path, this->m_locale, this->m_locale, item->m_build);
            break;
        }

        case 2: {
            Blizzard::String::Format(buffer, size, "%s/Cache/SoundCache-patch-%d.MPQ", this->m_path, item->m_build);
            break;
        }

        case 3: {
            Blizzard::String::Format(buffer, size, "%s/Cache/%s/patch-%s-%d.MPQ", this->m_path, this->m_locale, this->m_locale, item->m_build);
            break;
        }

        case 4: {
            Blizzard::String::Format(buffer, size, "%s/Cache/patch-base-%d.MPQ", this->m_path, item->m_build);
            break;
        }

        case 5: {
            Blizzard::String::Format(buffer, size, "%s/wow-update-%d.MPQ", this->m_path, item->m_build);
            break;
        }

        case 6: {
            Blizzard::String::Format(buffer, size, "%s/%s/wow-update-%s-%d.MPQ", this->m_path, this->m_locale, this->m_locale, item->m_build);
            break;
        }

        case 7: {
            Blizzard::String::Format(buffer, size, "%s/wow-update-base-%d.MPQ", this->m_path, item->m_build);
            break;
        }

        case 8: {
            Blizzard::String::Format(buffer, size, "%s/Cache/%s/SoundCache-%s.MPQ", this->m_path, this->m_locale, this->m_locale);
            break;
        }

        case 9: {
            Blizzard::String::Format(buffer, size, "%s/Cache/SoundCache-0.MPQ", this->m_path);
            break;
        }

        case 10: {
            Blizzard::String::Format(buffer, size, "%s/Cache/SoundCache-3.MPQ", this->m_path);
            break;
        }

        case 11: {
            Blizzard::String::Format(buffer, size, "%s/Cache/SoundCache-2.MPQ", this->m_path);
            break;
        }

        case 12: {
            Blizzard::String::Format(buffer, size, "%s/Cache/SoundCache-1.MPQ", this->m_path);
            break;
        }
    }
}

// OFFSET: 0x007A2FF0/0x14050BB40
int32_t PatchOnDemand::Cache::ArchiveList::EnumerateArchives(ENUMARCHIVESCALLBACK callback, void* param) {
    int32_t result = 0;

    for (ListItem* item = this->m_head.m_next; item; item = item->m_next) {
        char path[260];
        this->GenerateFilename(item, path, sizeof(path));
        int32_t type = item->m_type;
        uint8_t flags = 0;

        if (type == 5) {
            flags = 1;
        } else if (type == 7) {
            flags = 2;
        } else if (type == 6) {
            flags = 4;
        } else if (type >= 0 && type <= 4) {
            flags = 8;
        }

        if (type == 0) {
            flags |= 0x10;
        }

        result = callback(path, param, flags);

        if (!result) {
            break;
        }
    }

    return result;
}

// OFFSET: 0x007A3090
PatchOnDemand::Cache::ArchiveList::ListItem* PatchOnDemand::Cache::ArchiveList::FindInsertPosition(int32_t type, int32_t build) {
    if (build > 15595) {
        return NULL;
    }

    ListItem* prev = &this->m_head;

    for (ListItem* item = this->m_head.m_next; item; item = item->m_next) {
        if (item->m_type == type && item->m_build == build) {
            return NULL;
        }

        if (type > 0) {
            if (item->m_type <= 0) {
                return prev;
            }

            if (item->m_build >= build && (item->m_build != build || item->m_type <= type)) {
                return prev;
            }
        } else if (item->m_type <= type && item->m_build >= build) {
            return prev;
        }

        prev = item;
    }

    return prev;
}

// OFFSET: 0x007A3100
int32_t PatchOnDemand::Cache::ArchiveList::Add(int32_t type, int32_t build) {
    ListItem* prev = this->FindInsertPosition(type, build);

    if (!prev || type == 13) {
        return this->m_count;
    }

    ListItem* item = new ListItem;
    item->m_prev = prev;
    item->m_next = prev->m_next;
    item->m_type = type;
    item->m_build = build;
    prev->m_next = item;

    if (item->m_next) {
        item->m_next->m_prev = item;
    }

    return ++this->m_count;
}

// OFFSET: 0x007A3180/0x14050BC10
int32_t PatchOnDemand::Cache::ArchiveList::AddUpdate(int32_t build) {
    if (Blizzard::String::EqualI(this->m_locale, "zhCN") || Blizzard::String::EqualI(this->m_locale, "enCN")) {
        this->Add(0, build);
    }

    this->Add(4, build);
    this->Add(3, build);
    this->Add(2, build);
    this->Add(1, build);

    return this->Add(5, build);
}

// OFFSET: 0x007A3200/0x14050BF70
int32_t PatchOnDemand::Cache::ArchiveList::AddUpdateBase(int32_t build) {
    this->Add(4, build);
    this->Add(2, build);

    return this->Add(7, build);
}

// OFFSET: 0x007A3230/0x14050C120
int32_t PatchOnDemand::Cache::ArchiveList::AddUpdateLang(int32_t build) {
    if (Blizzard::String::EqualI(this->m_locale, "zhCN") || Blizzard::String::EqualI(this->m_locale, "enCN")) {
        this->Add(0, build);
    }

    this->Add(3, build);
    this->Add(1, build);

    return this->Add(6, build);
}

// OFFSET: 0x007A32A0/0x14050C370
int32_t PatchOnDemand::Cache::EnumerateArchives(ENUMARCHIVESCALLBACK callback, void* param) {
    return s_archiveList.EnumerateArchives(callback, param);
}

// OFFSET: 0x007A32C0/0x14050C390
void PatchOnDemand::Cache::Shutdown() {
    Blizzard::Mopaq::SetPatchCompleteCallback(NULL);
}

// INLINED
BackgroundCache::CacheJob::CacheJob(int32_t dependencyCount) : Blizzard::Jobs::Job(dependencyCount) {
}

// OFFSET: 0x007A32D0/0x14050C3A0
OggToWavConversion::OggConverterJob::OggConverterJob(const char* name) : BackgroundCache::CacheJob(0) {
    this->m_archive = NULL;
    Blizzard::String::Copy(this->m_name, name, sizeof(this->m_name));
    Blizzard::String::Copy(this->m_wavName, name, sizeof(this->m_wavName));
    Blizzard::String::Copy(const_cast<char*>(Blizzard::String::FindExtension(this->m_wavName)), "wav", 4);
}

// OFFSET: 0x007A3360/0x14050C440
uint32_t OggToWavConversion::OggConverterJob::GetClassId() const {
    return 'oggw';
}

// OFFSET: 0x007A3370/0x14050C450
int64_t OggToWavConversion::OggConverterJob::EstimateCost() const {
    return 520;
}

// OFFSET: 0x007A3380/0x14050C460
bool OggToWavConversion::ShutdownCache() {
    SetCallback(NULL);
    s_DecompressOggCallback = NULL;
    return true;
}

// OFFSET: 0x007A3540/0x14050C480
PatchOnDemand::Cache::ArchiveList::ListItem::~ListItem() {
    if (this->m_next) {
        delete this->m_next;
    }
}

// OFFSET: 0x007A3560
PatchOnDemand::Cache::ArchiveList::~ArchiveList() {
    Blizzard::Memory::Free(this->m_path);
    Blizzard::Memory::Free(this->m_locale);
}

// OFFSET: 0x007A35D0/0x14050C4B0
bool PatchOnDemand::Cache::ArchiveList::DirCallback(const Blizzard::File::ProcessDirParms& parms) {
    ArchiveList* list = static_cast<ArchiveList*>(parms.m_cookie);

    if (parms.m_isDir && (Blizzard::String::EqualI(parms.m_filename, list->m_locale) || Blizzard::String::EqualI(parms.m_filename, "Cache"))) {
        char path[260];
        Blizzard::String::Format(path, sizeof(path), *parms.m_pathRoot ? "%s/%s/" : "%s%s/", parms.m_pathRoot, parms.m_filename);
        Blizzard::File::ProcessDirFast(path, list, DirCallback, false);
        return true;
    }

    int32_t build = 0;

    if (Blizzard::File::SimpleGlob(parms.m_filename, "*.mpq.part")) {
        return true;
    }

    if (sscanf(parms.m_filename, "wow-update-%d.MPQ", &build)) {
        list->AddUpdate(build);
        return true;
    }

    if (sscanf(parms.m_filename, "wow-update-base-%d.MPQ", &build)) {
        list->AddUpdateBase(build);
        return true;
    }

    if (sscanf(parms.m_filename, Blizzard::String::QuickFormat<1024>("wow-update-%s-%%d.MPQ", list->m_locale).m_buffer, &build)) {
        list->AddUpdateLang(build);
    }

    return true;
}

// OFFSET: 0x007A3710/0x14050C640
bool PatchOnDemand::PoDWriteJob::WriteFile(System_SFile::ARCHIVEREC* archive, const char* name, const void* data, int64_t size, const Blizzard::Util::Digest<16>& md5) {
    Blizzard::Util::Digest<16> written;
    Blizzard::Mopaq::HSFILERecord* file = NULL;
    bool result = Blizzard::Mopaq::SFileCreateFile(archive, name, static_cast<uint32_t>(size), 512, &file);

    if (result) {
        result = Blizzard::Mopaq::SFileWriteFile(file, data, static_cast<uint32_t>(size));

        if (result) {
            result = Blizzard::Mopaq::SFileGetFileMD5(file, &written);
        }
    }

    Blizzard::Mopaq::SFileCloseFile(file);

    if (result && md5 != written) {
        result = false;
        Blizzard::Mopaq::SFileDeleteFile(archive, name, 0);
    }

    return result;
}

// OFFSET: 0x007A34E0
static void GetMetadataFromFilename(const char* filename, Blizzard::Util::Buffer<4096, char>& metadata) {
    const char* start = Blizzard::String::Find(Blizzard::String::Find(filename, '-', 0x7FFFFFFF) + 1, '-', 0x7FFFFFFF) + 1;
    const char* end = Blizzard::String::Find(start, '-', 0x7FFFFFFF);

    if (end) {
        Blizzard::String::Copy(metadata.m_data, start, static_cast<uint32_t>(end - start + 1));
    } else {
        Blizzard::String::Copy(metadata.m_data, "base", metadata.m_count);
    }
}

// OFFSET: 0x007A37D0/0x14050C720
bool BackgroundCache::CacheManager::ResolveUpdatePath(System_SFile::ARCHIVEREC* archive, const char* filename, const char* prefix, char* buffer, uint32_t size) {
    Blizzard::Util::Buffer<4096, char> metadata;

    if (!Blizzard::Mopaq::SFileGetPatchMetadata(archive, metadata)) {
        GetMetadataFromFilename(filename, metadata);
    }

    char suffix[10] = { 0 };

    if (Blizzard::String::Copy(suffix, Blizzard::String::FindReverse(filename, "-", 0x7FFFFFFF), sizeof(suffix))) {
        char* extension = const_cast<char*>(Blizzard::String::FindExtensionWithLeadingDot(suffix));

        if (extension) {
            *extension = '\0';
        }
    }

    if (Blizzard::String::EqualI(metadata.m_data, "alternate") || Blizzard::String::EqualI(metadata.m_data, "base")) {
        Blizzard::String::Format(buffer, size, "Data/Cache/%s-%s%s.mpq", prefix, metadata.m_data, suffix);
    } else {
        char* alternate = const_cast<char*>(Blizzard::String::FindWordI(metadata.m_data, "-alternate", 0x7FFFFFFF));

        if (alternate) {
            *alternate = '\0';
            Blizzard::String::Format(buffer, size, "Data/Cache/%s/%s-alternate%s.mpq", metadata.m_data, prefix, suffix);
        } else {
            Blizzard::String::Format(buffer, size, "Data/Cache/%s/%s-%s%s.mpq", metadata.m_data, prefix, metadata.m_data, suffix);
        }
    }

    return true;
}

// OFFSET: 0x007A3960/0x14050C940
System_SFile::ARCHIVEREC* BackgroundCache::CacheManager::ResolveOggTarget(System_SFile::ARCHIVEREC* archive) {
    char path[260];

    if (!Blizzard::Mopaq::SFileGetArchiveName(archive, path, sizeof(path))) {
        return NULL;
    }

    const char* filename = Blizzard::String::FindFilename(path);

    if (Blizzard::String::EqualI(filename, "wow-update-", 11)) {
        if (!ResolveUpdatePath(archive, filename, "SoundCache-patch", path, sizeof(path))) {
            return NULL;
        }

        return Blizzard::Mopaq::SFileGetArchiveHandle(path);
    }

    if (Blizzard::String::Find(filename, s_locale, 0x7FFFFFFF)) {
        Blizzard::String::Format(path, sizeof(path), "Data/Cache/%s/SoundCache-%s.mpq", s_locale, s_locale);
        return Blizzard::Mopaq::SFileGetArchiveHandle(path);
    }

    if (Blizzard::String::EqualI(filename, "expansion1.MPQ")) {
        Blizzard::String::Format(path, sizeof(path), "Data/Cache/SoundCache-1.mpq");
    } else if (Blizzard::String::EqualI(filename, "expansion2.MPQ")) {
        Blizzard::String::Format(path, sizeof(path), "Data/Cache/SoundCache-2.mpq");
    } else if (Blizzard::String::EqualI(filename, "expansion3.MPQ")) {
        Blizzard::String::Format(path, sizeof(path), "Data/Cache/SoundCache-3.mpq");
    } else if (Blizzard::String::EqualI(filename, "sound.MPQ")) {
        Blizzard::String::Format(path, sizeof(path), "Data/Cache/SoundCache-0.mpq");
    }

    return Blizzard::Mopaq::SFileGetArchiveHandle(path);
}

// OFFSET: 0x007A3AC0/0x14050CAB0
System_SFile::ARCHIVEREC* BackgroundCache::CacheManager::ResolvePatchTarget(System_SFile::ARCHIVEREC* archive) {
    char path[260];

    if (!Blizzard::Mopaq::SFileGetArchiveName(archive, path, sizeof(path))) {
        return NULL;
    }

    const char* filename = Blizzard::String::FindFilename(path);

    if (Blizzard::String::EqualI(filename, "wow-update-", 11) && ResolveUpdatePath(archive, filename, "patch", path, sizeof(path))) {
        return Blizzard::Mopaq::SFileGetArchiveHandle(path);
    }

    return NULL;
}

// OFFSET: 0x007A3B60/0x14050CB60
int32_t PatchOnDemand::Cache::ArchiveList::LoadArchives(const char* path, const char* locale) {
    this->m_path = Blizzard::String::Duplicate(path);
    this->m_locale = Blizzard::String::Duplicate(locale);

    char buffer[260] = { 0 };
    Blizzard::String::Format(buffer, sizeof(buffer), "%s/wave.mpq", this->m_path);

    if (!Blizzard::File::Exists(buffer)) {
        this->Add(9, 0);

        Blizzard::String::Format(buffer, sizeof(buffer), "%s/expansion1.mpq", this->m_path);

        if (Blizzard::File::Exists(buffer)) {
            this->Add(12, 0);
        }

        Blizzard::String::Format(buffer, sizeof(buffer), "%s/expansion2.mpq", this->m_path);

        if (Blizzard::File::Exists(buffer)) {
            this->Add(11, 0);
        }

        Blizzard::String::Format(buffer, sizeof(buffer), "%s/expansion3.mpq", this->m_path);

        if (Blizzard::File::Exists(buffer)) {
            this->Add(10, 0);
        }

        this->Add(8, 0);
        InitOggDecompression();
    }

    Blizzard::File::ProcessDirFast(this->m_path, this, DirCallback, false);

    return this->m_count;
}

// OFFSET: 0x007A3CB0/0x14050CF00
int32_t PatchOnDemand::Cache::LoadArchives(const char* path, const char* locale) {
    return s_archiveList.LoadArchives(path, locale);
}

// OFFSET: 0x007A3CD0
PatchOnDemand::PoDWriteJob::PoDWriteJob(const void* data, int64_t size, System_SFile::ARCHIVEREC* archive, const char* name, const Blizzard::Util::Digest<16>& md5) : BackgroundCache::CacheJob(0) {
    this->m_size = static_cast<uint32_t>(size);
    this->m_data = this->m_size ? new uint8_t[this->m_size] : NULL;
    this->m_archive = archive;
    this->m_md5 = md5;
    Blizzard::String::MemCopy(this->m_data, data, this->m_size);

    if (name) {
        Blizzard::String::Copy(this->m_name, name, sizeof(this->m_name));
    }
}

// OFFSET: 0x007A3DA0/0x14050CFE0
uint32_t PatchOnDemand::PoDWriteJob::GetClassId() const {
    return 'mpqw';
}

// OFFSET: 0x007A3DB0/0x14090CAF0
int64_t PatchOnDemand::PoDWriteJob::EstimateCost() const {
    return this->m_size;
}

// OFFSET: 0x007A3DC0/0x14050CFF0
bool PatchOnDemand::PoDWriteJob::Execute() {
    System_SFile::ARCHIVEREC* target = BackgroundCache::CacheManager::ResolvePatchTarget(this->m_archive);
    bool result = false;

    if (target) {
        result = WriteFile(target, this->m_name, this->m_data, this->m_size, this->m_md5);

        if (result) {
            return result;
        }
    }

    if (Blizzard::Mopaq::SFileFileExists(target, this->m_name, 0, NULL) == 1) {
        return true;
    }

    Blizzard::Log::Printf(0, "Logs/PatchOnDemand.log", "Failed to write file '%s' to disk.\n", this->m_name);

    return result;
}

// OFFSET: 0x007A3E40/0x14050D090
bool OggToWavConversion::OggConverterJob::WriteWav(const void* data, uint32_t size) {
    Blizzard::Mopaq::HSFILERecord* file = NULL;
    bool result = Blizzard::Mopaq::SFileCreateFile(BackgroundCache::CacheManager::ResolveOggTarget(this->m_archive), this->m_wavName, size, 0, &file);

    if (result) {
        result = Blizzard::Mopaq::SFileWriteFile(file, data, size);
    }

    Blizzard::Mopaq::SFileCloseFile(file);

    return result;
}

// OFFSET: 0x007A4010
bool STORMAPI BackgroundCache::CacheManager::WritePatchFile(const void* data, uint64_t size, Blizzard::Mopaq::HSFILERecord* file, const char* name, const Blizzard::Util::Digest<16>& md5) {
    System_SFile::ARCHIVEREC* archive = NULL;

    if (!Blizzard::Mopaq::SFileGetLogicalArchive(file, &archive)) {
        return false;
    }

    return PatchOnDemand::PoDWriteJob::WriteFile(ResolvePatchTarget(archive), name, data, size, md5);
}

// OFFSET: 0x007A4060/0x14050D220
PatchOnDemand::PoDWriteJob::~PoDWriteJob() {
    if (this->m_data) {
        delete[] this->m_data;
    }
}

// OFFSET: 0x007A3EB0/0x14050D120
bool OggToWavConversion::OggConverterJob::ConvertToOgg() {
    void* data = NULL;
    uint32_t size = 0;
    bool result;

    if (Blizzard::Mopaq::SFileLoadFile(this->m_archive, this->m_name, &data, &size, 0, 0)) {
        Blizzard::Util::Buffer<0x400000, uint8_t>* buffer = BackgroundCache::s_cacheManager->m_oggBuffer;
        result = s_DecompressOggCallback(data, size, buffer);

        if (result && !this->WriteWav(buffer->m_data, buffer->m_count)) {
            result = false;
        }
    } else {
        result = false;
    }

    if (data) {
        Blizzard::Mopaq::SFileUnloadFile(data);
    }

    return result;
}

// OFFSET: 0x007A3F40/0x14050D1D0
bool OggToWavConversion::OggConverterJob::Execute() {
    if (Blizzard::Mopaq::SFileFileExists(NULL, this->m_wavName, 0, NULL) || Blizzard::Mopaq::SFileFileExists(NULL, this->m_name, 0, &this->m_archive) != 1) {
        return true;
    }

    return this->ConvertToOgg();
}

// OFFSET: 0x007A4250/0x14050D270
void BackgroundCache::CacheManager::QueueNextJob() {
    Blizzard::Lock::MutexEnter(this->m_mutex);

    Blizzard::Jobs::RefPtr<Blizzard::Jobs::Job> job;

    if (!this->m_oggActive && (this->m_writeRuns >= 3 || !this->m_writeJobs.size()) && this->m_oggJobs.size()) {
        job = this->m_oggJobs.front();

        if (job.Get()) {
            this->m_oggActive = true;
            this->m_oggJobs.pop_front();
        }

        this->m_writeRuns = 0;
    }

    if (!job.Get() && this->m_writeJobs.size()) {
        job = this->m_writeJobs.front();

        if (job.Get()) {
            this->m_writeJobs.pop_front();
        }

        this->m_writeRuns++;
    }

    if (job.Get()) {
        this->QueueJob(job);
    }

    Blizzard::Lock::MutexLeave(this->m_mutex);
}

// OFFSET: 0x007A4380/0x14050D470
void BackgroundCache::CacheManager::Pump() {
    Blizzard::Lock::MutexEnter(this->m_mutex);

    Blizzard::Jobs::RefPtr<Blizzard::Jobs::Job> job;

    while (true) {
        job = this->GetCompletedJob();

        if (!job.Get()) {
            break;
        }

        if (job->GetClassId() == 'oggw') {
            this->m_oggActive = false;
        }

        this->m_queuedBytes -= static_cast<CacheJob*>(job.Get())->EstimateCost();
    }

    this->QueueNextJob();
    this->WorkerPool::Pump();

    Blizzard::Lock::MutexLeave(this->m_mutex);
}

// OFFSET: 0x007A44B0/0x14050D5B0
void BackgroundCache::Pump() {
    s_cacheManager->Pump();
}

// OFFSET: 0x007A4510
BackgroundCache::CacheManager::CacheManager(uint64_t memoryLimit) : Blizzard::Jobs::WorkerPool(1) {
    Blizzard::Lock::MutexCreate(this->m_mutex);
    this->m_oggActive = false;
    this->m_queuedBytes = 0;
    this->m_memoryLimit = memoryLimit;
    this->m_writeRuns = 0;
    this->m_oggBuffer = new Blizzard::Util::Buffer<0x400000, uint8_t>;
}

// OFFSET: 0x007A45F0/0x14050D6C0
BackgroundCache::CacheManager::~CacheManager() {
    delete this->m_oggBuffer;
    Blizzard::Lock::MutexDestroy(this->m_mutex);
}

// OFFSET: 0x007A46A0
void BackgroundCache::InitManager(int32_t memoryLimit) {
    if (s_cacheManager) {
        s_cacheManager->m_memoryLimit = memoryLimit <= 0 ? 0 : memoryLimit;
    } else {
        s_cacheManager = new CacheManager(static_cast<int64_t>(memoryLimit));
    }
}

// OFFSET: 0x007A48B0/0x14050D760
void BackgroundCache::ShutdownManager() {
    if (s_cacheManager) {
        delete s_cacheManager;
    }

    s_cacheManager = NULL;
}

// OFFSET: 0x007A4990
void BackgroundCache::CacheManager::QueuePatchWrite(const void* data, uint64_t size, Blizzard::Mopaq::HSFILERecord* file, const char* name, const Blizzard::Util::Digest<16>& md5) {
    Blizzard::Lock::MutexEnter(this->m_mutex);

    this->m_queuedBytes += size;
    System_SFile::ARCHIVEREC* archive = NULL;

    if (Blizzard::Mopaq::SFileGetLogicalArchive(file, &archive)) {
        Blizzard::Jobs::RefPtr<Blizzard::Jobs::Job> job(new PatchOnDemand::PoDWriteJob(data, size, archive, name, md5));
        this->m_writeJobs.push_back(job);
    }

    Blizzard::Lock::MutexLeave(this->m_mutex);
}

// OFFSET: 0x007A4A90/0x14050D8B0
void BackgroundCache::CacheManager::QueueOggConversion(const char* name) {
    Blizzard::Lock::MutexEnter(this->m_mutex);

    if (this->m_oggJobs.size() < 20) {
        Blizzard::Jobs::RefPtr<Blizzard::Jobs::Job> job(new OggToWavConversion::OggConverterJob(name));
        this->m_oggJobs.push_back(job);
    }

    Blizzard::Lock::MutexLeave(this->m_mutex);
}

// OFFSET: 0x007A4B60/0x14050D970
void BackgroundCache::CacheManager::PatchCompleteCallback(const void* data, uint64_t size, Blizzard::Mopaq::HSFILERecord* file, const Blizzard::Util::Digest<16>& md5) {
    if (!s_cacheManager) {
        return;
    }

    char name[260] = { 0 };

    if (Blizzard::Mopaq::SFileGetFileName(file, name, sizeof(name))) {
        if (size + s_cacheManager->m_queuedBytes < s_cacheManager->m_memoryLimit) {
            s_cacheManager->QueuePatchWrite(data, size, file, name, md5);
        } else {
            WritePatchFile(data, size, file, name, md5);
        }

        return;
    }

    Blizzard::String::Copy(name, "<unknown>", sizeof(name));
    uint64_t available = s_cacheManager->m_memoryLimit > s_cacheManager->m_queuedBytes ? s_cacheManager->m_memoryLimit - s_cacheManager->m_queuedBytes : 0;
    Blizzard::Log::Printf(0, "Logs/PatchOnDemand.log", "Failed to queue file '%s' for writing due to memory limit (%lld bytes available).\n\tFile size: %lld\n\tQueue Item Count: %d\n", name, available, size, static_cast<int32_t>(s_cacheManager->m_oggJobs.size() + s_cacheManager->m_writeJobs.size()));
}

// OFFSET: 0x007A4C80/0x14050DAE0
void BackgroundCache::CacheManager::OggConvertCallback(const char* name) {
    if (s_cacheManager) {
        s_cacheManager->QueueOggConversion(name);
    }
}

// OFFSET: 0x007A4CA0/0x14050DB00
void PatchOnDemand::Cache::Init(int32_t memoryLimit) {
    BackgroundCache::InitManager(memoryLimit);
    Blizzard::Mopaq::SetPatchCompleteCallback(BackgroundCache::CacheManager::PatchCompleteCallback);
}

// OFFSET: 0x007A4CC0/0x14050DB70
bool OggToWavConversion::InitCache(DECOMPRESSOGGCALLBACK callback) {
    BackgroundCache::InitManager(10240);
    s_DecompressOggCallback = callback;

    if (callback) {
        SetCallback(BackgroundCache::CacheManager::OggConvertCallback);
    }

    return true;
}

// OFFSET: 0x007A4EB0/0x140499B30
void OggToWavConversion::SetCallback(CONVERTOGGCALLBACK callback) {
    s_ConvertOggCallback = callback;
}
