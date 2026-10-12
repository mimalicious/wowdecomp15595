#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_MPQSTACK_MD5VERIFY_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_MPQSTACK_MD5VERIFY_H

#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/StackedMopaq.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/SBig.h"

namespace System_Mopaq {

struct MD5VerifyData {
    enum IoOperationType {
        IO_ALIGNED = 0,
        IO_SINGLE_BLOCK = 1,
        IO_MULTIPLE_BLOCKS = 2,
        IO_INVALID = -1,
    };

    /* 0000 */ Blizzard::Util::Array<uint8_t> m_md5s;
    /* 0008 */ Blizzard::Util::Array<uint8_t> m_workBuffer;
    /* 0010 */ uint8_t* m_buffer;
    /* 0014 */ int32_t m_size;
    /* 0018 */ int32_t m_blockSize;
    /* 001C */ uint8_t unk_001C[4];
    /* 0020 */ int64_t m_fileSize;
    /* 0028 */ int32_t m_blockCount;
    /* 002C */ uint8_t unk_002C[4];
    /* 0030 */ int64_t m_baseOffset;
    /* 0038 */ bool m_unalignedStart;
    /* 0039 */ bool m_unalignedEnd;
    /* 003A */ bool m_lastBlock;
    /* 003C */ Blizzard::Mopaq::MpqHandleRecord* m_handle;
    /* 0040 */ bool m_writing;
    /* 0041 */ uint8_t unk_0041[7];
    /* 0048 */ int64_t m_written;

    MD5VerifyData(int32_t blockSize, int64_t fileSize, bool writing);
    void ComputeMD5s(const uint8_t* buffer, int64_t offset, int32_t size);
    bool ComputeWriteMD5s(int64_t offset, IoOperationType type);
    bool EnsureMD5Table(int64_t offset, int64_t size);
    bool InitializeMD5Buffer();
    bool MD5AndWrite(int64_t offset, int64_t size, Blizzard::Mopaq::MpqHandleRecord* handle);
    bool MD5MultipleBlocks(int64_t offset);
    bool MD5SingleBlock(int64_t offset);
    IoOperationType PrepareWorkBuffer(int64_t offset);
    bool ReadAligned(int64_t offset);
    bool ReadAndValidate(int64_t offset, Blizzard::Mopaq::MpqHandleRecord* handle);
    bool ReadMultipleBlocks(int64_t offset);
    bool ReadSingleBlock(int64_t offset);
    bool ValidateAlignedRead();
    bool ValidateBlock(uint8_t* buffer, int32_t block, int32_t size, bool& repaired);
    bool ValidateBlocks(uint8_t* buffer, int64_t offset, int64_t size);
    bool ValidateExistingBlock(int64_t offset);
    bool ValidateLastBlock(uint8_t* buffer, bool& repaired);
    bool ValidateMultipleBlocks(int64_t offset);
    bool ValidateRead(IoOperationType type, int64_t offset);
    bool ValidateSingleBlock(int64_t offset);
    bool WriteMD5Table(Blizzard::Mopaq::MpqHandleRecord* handle);
};

void CreateMD5VerifyData(Blizzard::Mopaq::MpqHandleRecord* handle, int32_t blockSize, int64_t fileSize, bool writing);
void DestroyMD5VerifyData(Blizzard::Mopaq::MpqHandleRecord* handle);

}

#endif
