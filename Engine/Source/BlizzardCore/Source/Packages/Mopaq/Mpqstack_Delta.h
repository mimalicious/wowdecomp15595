#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_MPQSTACK_DELTA_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_MPQSTACK_DELTA_H

#include <cstdint>
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Digest.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/StackedMopaq.h"

namespace System_Mopaq {

#pragma pack(push, 4)
struct MpqDeltaMetadata {
    /* 0000 */ uint16_t m_size;
    /* 0002 */ uint8_t m_version;
    /* 0003 */ uint8_t unk_0003;
    /* 0004 */ uint32_t m_extendedFlags;
    /* 0008 */ uint32_t m_deltaSize;
    /* 000C */ Blizzard::Util::Digest<16> m_md5;
};
#pragma pack(pop)

struct DeltaData {
    typedef bool (*ApplyPatchFunc)(const uint8_t* patch, uint32_t patchSize, uint8_t* dest, uint32_t destSize, uint8_t* source, uint32_t sourceSize, uint32_t flags);
    typedef void (*PatchCompletionFunc)(const void* data, uint64_t size, Blizzard::Mopaq::HSFILERecord* file, const Blizzard::Util::Digest<16>& md5);

    static ApplyPatchFunc ApplyPatch;
    static PatchCompletionFunc s_onPatchCompletion;

    /* 0000 */ uint32_t m_deltaSize;
    /* 0004 */ uint8_t unk_0004[4];
    /* 0008 */ uint64_t m_fileSize;
    /* 0010 */ bool m_headerProcessed;
    /* 0011 */ uint8_t unk_0011[3];
    /* 0014 */ uint8_t* m_appliedBuffer;
    /* 0018 */ uint32_t m_appliedSize;
    /* 001C */ Blizzard::Mopaq::MpqHandleRecord* m_handle;
    /* 0020 */ Blizzard::Mopaq::HSFILERecord* m_baseFile;
    /* 0024 */ uint8_t unk_0024[4];
    /* 0028 */ int64_t m_offset;
    /* 0030 */ uint32_t m_sizeBefore;
    /* 0034 */ uint32_t m_sizeAfter;
    /* 0038 */ Blizzard::Util::Digest<16> m_sourceMd5;
    /* 0048 */ Blizzard::Util::Digest<16> m_targetMd5;
    /* 0058 */ Blizzard::Util::Digest<16> m_md5;

    DeltaData(Blizzard::Mopaq::MpqHandleRecord* handle, const MpqDeltaMetadata* metadata);
    ~DeltaData();
    bool AdjustHandleBlockEntry();
    void CollectLevels(std::vector<DeltaData*>& levels);
    bool CopyFromApplied(void* buffer, int64_t offset, int32_t count);
    bool InitializeBaseMetadata();
    bool InitMd5s(Blizzard::Mopaq::Mpqsystem* system);
    bool IsLargeDelta(uint32_t sectorSize);
    bool OpenSrcFile();
    bool PatchIntoBuffer(Blizzard::Mopaq::Mpqsystem* system, uint8_t* dest);
    bool PatchLevel(Blizzard::Mopaq::Mpqsystem* system, uint8_t* dest, uint8_t* source);
    bool ProcessPatchHeader(const void* buffer, int32_t size);
    bool Read(Blizzard::Mopaq::Mpqsystem* system, MpqParams* parms);
    bool ReadSource(void* buffer, uint32_t size);
    void ReserveDeltaSpace(int64_t size);
    bool VerifyMd5(const void* buffer, uint32_t size, int32_t which);
};

bool InitializeDeltaData(Blizzard::Mopaq::MpqHandleRecord* handle, const MpqDeltaMetadata* metadata);
bool ReleaseDeltaData(Blizzard::Mopaq::MpqHandleRecord* handle, System_SFile::ArchivePtr& archive);
bool RepairFile(Blizzard::Mopaq::HSFILERecord* file);
bool CloseFileRecord(Blizzard::Mopaq::HSFILERecord* file);

}

namespace Blizzard {
namespace Mopaq {

void SetPatchCompleteCallback(System_Mopaq::DeltaData::PatchCompletionFunc callback);

}
}

#endif
