#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Include/Blizzard/Util/MD5.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/Patch/Patch.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"
#include "ThirdParty/zlib/zlib.h"

// STATIC: %0x00C0C74C
static Blizzard::Patch::PatchReader s_memoryReader;

// INLINED
Blizzard::Patch::PatchReader::~PatchReader() {
}

// INLINED
void Blizzard::Patch::PatchReader::Reset() {
}

// PROVEN: 1193df1f IDENTICAL/NONE
// OFFSET: 0x008757D0
bool Blizzard::Patch::PatchReader::Load(uint32_t offset, uint8_t* dest, uint32_t size) {
    return false;
}

// PROVEN: d95819bd IDENTICAL/NONE
// OFFSET: 0x007B0930
uint8_t* Blizzard::Patch::PatchBuffer::Get(uint32_t offset, uint32_t size) {
    if (offset + size > this->m_loaded) {
        this->m_reader->Load(this->m_loaded, this->m_data + this->m_loaded, offset + size - this->m_loaded);
        this->m_loaded = offset + size;
    }

    return this->m_data + offset;
}

// PROVEN: a20f5937 IDENTICAL/NONE
// OFFSET: 0x007B0970
uint8_t* Blizzard::Patch::PatchBuffer::GetAll() {
    uint32_t size = this->m_size;

    if (size > this->m_loaded) {
        this->m_reader->Load(this->m_loaded, this->m_data + this->m_loaded, size - this->m_loaded);
        this->m_loaded = size;
    }

    return this->m_data;
}

// OFFSET: %0x00712E40
bool Blizzard::Patch::PatchBuffer::Read(uint32_t offset, void* buffer, uint32_t size) {
    if (offset + size > this->m_size) {
        return false;
    }

    if (offset + size > this->m_loaded) {
        this->m_reader->Load(this->m_loaded, this->m_data + this->m_loaded, offset + size - this->m_loaded);
        this->m_loaded = offset + size;
    }

    memmove(buffer, this->m_data + offset, size);

    return true;
}

// INLINED
static void DeleteBuffer(Blizzard::Patch::PatchBuffer* buffer) {
    if (buffer) {
        if (buffer->m_owned) {
            delete buffer->m_data;
        }

        delete buffer;
    }
}

// INLINED
Blizzard::Patch::Patch::~Patch() {
}

// OFFSET: %0x00712F40
bool Blizzard::Patch::Patch::CopyTransformData(uint8_t* dest) {
    uint32_t size = 0;
    const uint8_t* data = this->GetTransformData(&size);
    memmove(dest, data, size);

    return true;
}

// OFFSET: %0x00719A70
Blizzard::Patch::LegacyPatch::~LegacyPatch() {
    DeleteBuffer(this->m_buffer);
}

// PROVEN: 1564bfdc IDENTICAL/IDENTICAL
// OFFSET: 0x007B0A80/0x1404C1350
uint32_t Blizzard::Patch::LegacyPatch::GetSourceSize() {
    return this->m_sourceSize;
}

// PROVEN: 6a015af8 IDENTICAL/DIFFERENT
// OFFSET: 0x00B41A40/0x14090FD00
uint32_t Blizzard::Patch::LegacyPatch::GetTargetSize() {
    return this->m_targetSize;
}

// PROVEN: 9f3d4607 IDENTICAL/NONE
// OFFSET: 0x007B0A90
uint32_t Blizzard::Patch::LegacyPatch::GetType() {
    return this->m_type;
}

// PROVEN: a694ceb8 IDENTICAL/IDENTICAL
// OFFSET: 0x007B0A70/0x1404A5800
const char* Blizzard::Patch::LegacyPatch::GetTypeName() {
    return "legacy";
}

// OFFSET: %0x00719AC0
const uint8_t* Blizzard::Patch::LegacyPatch::GetTransformData(uint32_t* size) {
    *size = this->m_buffer->m_size - sizeof(LegacyHeader);
    return this->m_buffer->Get(0, *size + sizeof(LegacyHeader)) + sizeof(LegacyHeader);
}

// PROVEN: 065e8797 IDENTICAL/IDENTICAL
// OFFSET: 0x007B0AA0/0x1402A6B60
uint32_t Blizzard::Patch::LegacyPatch::GetHeaderSize() {
    return sizeof(LegacyHeader);
}

// OFFSET: %0x00719B10
bool Blizzard::Patch::LegacyPatch::Apply(const uint8_t* source, uint32_t sourceSize, uint8_t* dest, uint32_t destSize) {
    if (destSize < this->m_targetSize || this->m_sourceSize != sourceSize) {
        return false;
    }

    if (this->m_sourceCrc != crc32(0, source, sourceSize)) {
        return false;
    }

    switch (this->m_type) {
    case TRANSFORM_BSDP:
    case TRANSFORM_BSD0: {
        PatchBuffer* buffer = this->m_buffer;
        uint8_t* data = buffer->GetAll();
        return BSDiff::PTCApply_BSDiff(data, buffer->m_size, dest, destSize, source, sourceSize);
    }
    case TRANSFORM_CPOG:
    case TRANSFORM_COPY:
        return this->CopyTransformData(dest);
    case TRANSFORM_COPU:
        this->m_buffer->GetAll();
        return false;
    default:
        return false;
    }
}

// OFFSET: %0x0072CAB0
bool Blizzard::Patch::LegacyPatch::VerifySource(const uint8_t* source, uint32_t size, Error& error) {
    uint32_t crc = crc32(0, source, size);
    bool result = this->m_sourceCrc == crc;

    if (!result) {
        Blizzard::String::QuickFormat<1024> message("(CRC mismatch: expected 0x%08x, actual 0x%08x.)", this->m_sourceCrc, crc);
        error.assign(message.m_buffer, strlen(message.m_buffer));
    }

    return result;
}

// OFFSET: %0x00719A70
Blizzard::Patch::V2Patch::~V2Patch() {
    DeleteBuffer(this->m_buffer);
}

// PROVEN: 89d1dab2 IDENTICAL/IDENTICAL
// OFFSET: 0x007B0B20/0x1404A58A0
uint32_t Blizzard::Patch::V2Patch::GetSourceSize() {
    return this->m_patch->m_sourceSize;
}

// PROVEN: 55cb2394 IDENTICAL/IDENTICAL
// OFFSET: 0x007B0B30/0x1404A58B0
uint32_t Blizzard::Patch::V2Patch::GetTargetSize() {
    return this->m_patch->m_targetSize;
}

// PROVEN: 41ffcbc1 IDENTICAL/IDENTICAL
// OFFSET: 0x007B0B40/0x1404A58C0
uint32_t Blizzard::Patch::V2Patch::GetType() {
    return this->m_transform->m_type;
}

// PROVEN: ac039768 IDENTICAL/IDENTICAL
// OFFSET: 0x007B0B10/0x1404A5890
const char* Blizzard::Patch::V2Patch::GetTypeName() {
    return "v2patch";
}

// PROVEN: 76f7711c IDENTICAL/DIFFERENT
// OFFSET: 0x007AA840/0x1404A0C10
const uint8_t* Blizzard::Patch::V2Patch::GetTransformData(uint32_t* size) {
    *size = this->m_transform->m_size - sizeof(TransformChunk);
    return reinterpret_cast<const uint8_t*>(this->m_transform + 1);
}

// PROVEN: 147979d9 IDENTICAL/DIFFERENT
// OFFSET: 0x00B41A40/0x14090FD00
uint32_t Blizzard::Patch::V2Patch::GetHeaderSize() {
    return this->m_headerSize;
}

// PROVEN: c58f7eb2 IDENTICAL/UNPROVEN
// OFFSET: 0x007E6400/0x1404EFE00
bool Blizzard::Patch::V2Patch::Apply(const uint8_t* source, uint32_t sourceSize, uint8_t* dest, uint32_t destSize) {
    return ApplyTransform(this->m_transform, dest, destSize, source, sourceSize);
}

// OFFSET: %0x0072CC90
bool Blizzard::Patch::V2Patch::VerifySource(const uint8_t* source, uint32_t size, Error& error) {
    Blizzard::Util::MD5 md5;
    md5.Process(source, size);

    Blizzard::Util::Digest<16> digest;
    md5.Finish(digest.m_data);

    bool result = this->m_md5->m_before == digest;

    if (!result) {
        char expected[36];
        char actual[36];
        this->m_md5->m_before.ToHexString(expected, 33);
        digest.ToHexString(actual, 33);

        Blizzard::String::QuickFormat<1024> message("(MD5 mismatch: expected 0x%s, actual 0x%s.)", expected, actual);
        error.assign(message.m_buffer, strlen(message.m_buffer));
    }

    return result;
}

// OFFSET: %0x0070D680
static uint32_t GetTransformType(uint8_t transform) {
    switch (transform) {
    case 1:
        return Blizzard::Patch::TRANSFORM_COPY;
    case 2:
        return Blizzard::Patch::TRANSFORM_BSDP;
    case 4:
        return Blizzard::Patch::TRANSFORM_BSD0;
    case 16:
        return Blizzard::Patch::TRANSFORM_CPOG;
    case 32:
        return Blizzard::Patch::TRANSFORM_COPU;
    default:
        return 0;
    }
}

// OFFSET: %0x0072C940
static bool ValidateLegacyHeader(const Blizzard::Patch::LegacyHeader& header, Blizzard::Patch::Error& error) {
    if (header.m_headerSize != sizeof(Blizzard::Patch::LegacyHeader)) {
        error.assign("Old style patch - header size mismatch");
    }

    if (error.empty()) {
        if (header.m_version != 4) {
            error.assign("Old style patch - version mismatch");
        }

        if (error.empty() && !GetTransformType(header.m_transform)) {
            error.assign("Old style patch - Invalid flag for transform type");
        }
    }

    return error.empty();
}

// OFFSET: %0x0072C9B0
static Blizzard::Patch::Patch* CreateLegacyPatch(Blizzard::Patch::PatchBuffer* buffer, Blizzard::Patch::Error& error) {
    if (buffer->m_size < sizeof(Blizzard::Patch::LegacyHeader)) {
        error.assign("Failed to read old-style header - too short?");
        return NULL;
    }

    Blizzard::Patch::LegacyHeader header;
    memcpy(&header, buffer->Get(0, sizeof(header)), sizeof(header));

    if (!ValidateLegacyHeader(header, error)) {
        return NULL;
    }

    Blizzard::Patch::LegacyPatch* patch = new Blizzard::Patch::LegacyPatch;
    patch->m_buffer = buffer;
    patch->m_sourceSize = header.m_sourceSize;
    patch->m_targetSize = header.m_targetSize;
    patch->m_type = GetTransformType(header.m_transform);
    patch->m_sourceCrc = header.m_sourceCrc;

    return patch;
}

// OFFSET: %0x0072CB20
static Blizzard::Patch::Patch* CreateV2Patch(Blizzard::Patch::PatchBuffer* buffer, Blizzard::Patch::Error& error) {
    Blizzard::Patch::PatchStream<Blizzard::Patch::PatchBuffer> stream;
    stream.m_stream = buffer;

    uint32_t offset = 0;
    Blizzard::Patch::Chunk<0> chunk;
    chunk.m_tag = 0;
    chunk.m_size = 8;

    if (!stream.Search(Blizzard::Patch::CHUNK_PTCH, offset, chunk)) {
        return NULL;
    }

    const Blizzard::Patch::PatchChunk* ptch = reinterpret_cast<const Blizzard::Patch::PatchChunk*>(buffer->Get(offset, sizeof(Blizzard::Patch::PatchChunk)));

    if (!ptch) {
        return NULL;
    }

    const Blizzard::Patch::MD5Chunk* md5 = NULL;
    const Blizzard::Patch::TransformChunk* transform = NULL;
    chunk.m_tag = 0;
    chunk.m_size = 8;

    if (!stream.Search(Blizzard::Patch::CHUNK_MD5, offset, chunk) || !(md5 = reinterpret_cast<const Blizzard::Patch::MD5Chunk*>(buffer->Get(offset, sizeof(Blizzard::Patch::MD5Chunk))))) {
        error.assign("New style patch - couldn't find MD5 chunk");
    } else {
        chunk.m_tag = 0;
        chunk.m_size = 8;

        if (stream.Search(Blizzard::Patch::CHUNK_XFRM, offset, chunk)) {
            transform = reinterpret_cast<const Blizzard::Patch::TransformChunk*>(buffer->Get(offset, sizeof(Blizzard::Patch::TransformChunk)));
        }
    }

    if (error.empty() && !transform) {
        error.assign("New style patch - couldn't find transform chunk");
    }

    if (!md5 || !transform) {
        return NULL;
    }

    Blizzard::Patch::V2Patch* patch = new Blizzard::Patch::V2Patch;
    patch->m_buffer = buffer;
    patch->m_patch = ptch;
    patch->m_transform = transform;
    patch->m_md5 = md5;
    patch->m_headerSize = offset + sizeof(Blizzard::Patch::TransformChunk);

    return patch;
}

// OFFSET: %0x0072CD60
Blizzard::Patch::Patch* Blizzard::Patch::Patch::CreatePatch(PatchBuffer* buffer, Error& error) {
    Patch* patch = CreateV2Patch(buffer, error);

    if (patch) {
        return patch;
    }

    if (error.empty()) {
        patch = CreateLegacyPatch(buffer, error);

        if (patch) {
            return patch;
        }

        if (error.empty()) {
            error.assign("Unknown patch type!");
        }
    }

    DeleteBuffer(buffer);

    return NULL;
}

// OFFSET: %0x007319F0
Blizzard::Patch::Patch* Blizzard::Patch::Patch::GetPatch(const uint8_t* data, uint32_t size) {
    Error error;
    PatchBuffer* buffer = new PatchBuffer;
    buffer->m_loaded = size;
    buffer->m_data = const_cast<uint8_t*>(data);
    buffer->m_size = size;
    buffer->m_owned = false;
    buffer->m_reader = &s_memoryReader;

    return CreatePatch(buffer, error);
}

// PROVEN: c7a65302 IDENTICAL/NONE
// OFFSET: 0x007D0160
bool Blizzard::Patch::Apply(const uint8_t* patch, uint32_t patchSize, uint8_t* dest, uint32_t destSize, uint8_t* source, uint32_t sourceSize, uint32_t flags) {
    bool result = false;
    Patch* instance = Patch::GetPatch(patch, patchSize);

    if (instance) {
        result = instance->Apply(source, sourceSize, dest, destSize);
        delete instance;
    }

    return result;
}

// OFFSET: %0x0074B9B0
bool Blizzard::Patch::ApplyTransform(const TransformChunk* transform, uint8_t* dest, uint32_t destSize, const uint8_t* source, uint32_t sourceSize) {
    uint32_t type = transform->m_type;
    uint32_t size = transform->m_size - sizeof(TransformChunk);
    const uint8_t* data = reinterpret_cast<const uint8_t*>(transform + 1);

    switch (type) {
    case TRANSFORM_COPY:
        if (destSize == sourceSize) {
            Blizzard::String::MemCopy(dest, source, destSize);
            return true;
        }

        break;
    case TRANSFORM_COPU:
        return false;
    case TRANSFORM_BSD0:
        return ApplyRLEBSDiffPatch(data, size, dest, destSize, source, sourceSize);
    case TRANSFORM_BSDP:
        return ApplyBSDiffPatch(data, size, dest, destSize, source, sourceSize);
    default:
        break;
    }

    Blizzard::String::QuickFormat<1024> message("%s - apply patch error %u %u %u %u", "Blizzard::Patch::ApplyTransform", type, size, destSize, sourceSize);
    Blizzard::File::AddToLastErrorStack(1006, message.m_buffer, 4);

    return false;
}
