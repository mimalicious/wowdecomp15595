#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_UTIL_PATCH_PATCH_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_UTIL_PATCH_PATCH_H

#include <cstdint>
#include <string>
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/Digest.h"

namespace Blizzard {
namespace Patch {

enum {
    CHUNK_PTCH = 0x48435450,
    CHUNK_MD5 = 0x5F35444D,
    CHUNK_XFRM = 0x4D524658,
};

enum {
    TRANSFORM_COPY = 0x59504F43,
    TRANSFORM_BSDP = 0x50445342,
    TRANSFORM_BSD0 = 0x30445342,
    TRANSFORM_CPOG = 0x474F5043,
    TRANSFORM_COPU = 0x50554F43,
};

typedef std::string Error;

struct ChunkHeader {
    /* 0000 */ uint32_t m_tag;
    /* 0004 */ uint32_t m_size;
};

template <uint32_t TAG>
struct Chunk : public ChunkHeader {
};

struct MD5Chunk : public ChunkHeader {
    /* 0008 */ Blizzard::Util::Digest<16> m_before;
    /* 0018 */ Blizzard::Util::Digest<16> m_after;
};

struct PatchChunk : public ChunkHeader {
    /* 0008 */ uint32_t m_sourceSize;
    /* 000C */ uint32_t m_targetSize;
};

struct TransformChunk : public ChunkHeader {
    /* 0008 */ uint32_t m_type;
};

struct LegacyHeader {
    /* 0000 */ uint16_t m_headerSize;
    /* 0002 */ uint8_t m_version;
    /* 0003 */ uint8_t m_transform;
    /* 0004 */ uint32_t m_sourceCrc;
    /* 0008 */ uint32_t m_sourceSize;
    /* 000C */ uint32_t m_targetSize;
    /* 0010 */ uint32_t unk_0010;
    /* 0014 */ uint32_t unk_0014;
};

class MemoryStream {
    public:
    /* 0000 */ const uint8_t* m_data;
    /* 0004 */ uint32_t m_size;

    bool Read(uint32_t offset, void* buffer, uint32_t size);
};

class PatchReader {
    public:
    virtual ~PatchReader();
    virtual void Reset();
    virtual bool Load(uint32_t offset, uint8_t* dest, uint32_t size);
};

class PatchBuffer {
    public:
    /* 0000 */ uint32_t m_loaded;
    /* 0004 */ uint8_t* m_data;
    /* 0008 */ uint32_t m_size;
    /* 000C */ bool m_owned;
    /* 000D */ uint8_t unk_000D[3];
    /* 0010 */ PatchReader* m_reader;

    uint8_t* Get(uint32_t offset, uint32_t size);
    uint8_t* GetAll();
    bool Read(uint32_t offset, void* buffer, uint32_t size);
};

template <class S>
class PatchStream {
    public:
    /* 0000 */ S* m_stream;

    bool FindChunk(uint32_t tag, uint32_t& offset, ChunkHeader& header);
    template <class C>
    bool Search(uint32_t tag, uint32_t& offset, C& chunk);
};

class Patch {
    public:
    static Patch* CreatePatch(PatchBuffer* buffer, Error& error);
    static Patch* GetPatch(const uint8_t* data, uint32_t size);

    virtual ~Patch();
    virtual uint32_t GetSourceSize() = 0;
    virtual uint32_t GetTargetSize() = 0;
    virtual uint32_t GetType() = 0;
    virtual const char* GetTypeName() = 0;
    virtual const uint8_t* GetTransformData(uint32_t* size) = 0;
    virtual uint32_t GetHeaderSize() = 0;
    virtual bool Apply(const uint8_t* source, uint32_t sourceSize, uint8_t* dest, uint32_t destSize) = 0;
    virtual bool VerifySource(const uint8_t* source, uint32_t size, Error& error) = 0;
    bool CopyTransformData(uint8_t* dest);
};

class LegacyPatch : public Patch {
    public:
    /* 0004 */ uint32_t m_type;
    /* 0008 */ uint32_t m_sourceCrc;
    /* 000C */ uint32_t m_sourceSize;
    /* 0010 */ uint32_t m_targetSize;
    /* 0014 */ PatchBuffer* m_buffer;

    virtual ~LegacyPatch();
    virtual uint32_t GetSourceSize();
    virtual uint32_t GetTargetSize();
    virtual uint32_t GetType();
    virtual const char* GetTypeName();
    virtual const uint8_t* GetTransformData(uint32_t* size);
    virtual uint32_t GetHeaderSize();
    virtual bool Apply(const uint8_t* source, uint32_t sourceSize, uint8_t* dest, uint32_t destSize);
    virtual bool VerifySource(const uint8_t* source, uint32_t size, Error& error);
};

class V2Patch : public Patch {
    public:
    /* 0004 */ const PatchChunk* m_patch;
    /* 0008 */ const MD5Chunk* m_md5;
    /* 000C */ const TransformChunk* m_transform;
    /* 0010 */ uint32_t m_headerSize;
    /* 0014 */ PatchBuffer* m_buffer;

    virtual ~V2Patch();
    virtual uint32_t GetSourceSize();
    virtual uint32_t GetTargetSize();
    virtual uint32_t GetType();
    virtual const char* GetTypeName();
    virtual const uint8_t* GetTransformData(uint32_t* size);
    virtual uint32_t GetHeaderSize();
    virtual bool Apply(const uint8_t* source, uint32_t sourceSize, uint8_t* dest, uint32_t destSize);
    virtual bool VerifySource(const uint8_t* source, uint32_t size, Error& error);
};

bool Apply(const uint8_t* patch, uint32_t patchSize, uint8_t* dest, uint32_t destSize, uint8_t* source, uint32_t sourceSize, uint32_t flags);
bool ApplyBSDiffPatch(const uint8_t* patch, uint32_t patchSize, uint8_t* dest, uint32_t destSize, const uint8_t* source, uint32_t sourceSize);
bool ApplyRLEBSDiffPatch(const uint8_t* patch, uint32_t patchSize, uint8_t* dest, uint32_t destSize, const uint8_t* source, uint32_t sourceSize);
bool ApplyTransform(const TransformChunk* transform, uint8_t* dest, uint32_t destSize, const uint8_t* source, uint32_t sourceSize);

}
}

namespace BSDiff {

uint8_t* DecodeRLE(const uint8_t* data, uint32_t* size, uint32_t skip);
bool PTCApply_BSDiff(const uint8_t* patch, uint32_t patchSize, uint8_t* dest, uint32_t destSize, const uint8_t* source, uint32_t sourceSize);

}

#include "Engine/Source/BlizzardCore/Source/Packages/Util/Patch/Patch.inl"

#endif
