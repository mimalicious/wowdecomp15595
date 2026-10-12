#ifndef WOW_SOURCE_BATTLENET_BSN_ADAPTER_H
#define WOW_SOURCE_BATTLENET_BSN_ADAPTER_H

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace BSN {
namespace Adapter {
namespace Array {
uint32_t GetLength(const void* data);
template <uint32_t OFFSET>
const void* GetElements(const void* data);
template <uint32_t SIZE, uint32_t OFFSET>
void* SetLength(void* data, uint32_t length);
}

namespace AsciiString {
uint32_t GetLength(const void* data);
const void* GetElements(const void* data);
void* SetLength(void* data, uint32_t length);
}

namespace Blob {
uint32_t GetLength(const void* data);
const void* GetElements(const void* data);
void* SetLength(void* data, uint32_t length);
}

namespace FixedBlob {
template <uint32_t N>
uint32_t GetLength(const void* data);
const void* GetElements(const void* data);
void* SetLength(void* data, uint32_t length);
}

namespace BitArray {
uint32_t GetLength(const void* data);
const void* GetElements(const void* data);
void* SetLength(void* data, uint32_t length);
}

namespace FixedBitArray {
template <uint32_t N>
uint32_t GetLength(const void* data);
const void* GetElements(const void* data);
void* SetLength(void* data, uint32_t length);
}

namespace String {
uint32_t GetSize(const void* data);
const void* GetElements(const void* data);
void* SetSize(void* data, uint32_t size);
void SetLength(void* data, uint32_t length);
}

namespace FourCC {
uint32_t GetValue(const void* data);
void SetValue(void* data, uint32_t value);
}

namespace Choice {
int32_t GetTag(const void* data);
const void* GetElement(const void* data);
void* SetTag(void* data, int32_t tag);
}

namespace Optional {
bool GetExists(const void* data);
template <uint32_t OFFSET>
const void* GetElement(const void* data);
template <uint32_t OFFSET>
void* SetExists(void* data, bool exists);
}
}
}

// OFFSET: [0x00B2E510<1>]
template <uint32_t OFFSET>
const void* BSN::Adapter::Array::GetElements(const void* data) {
    return static_cast<const uint8_t*>(data) + OFFSET;
}

// OFFSET: [0x00B2E480<64, 4>, 0x00B2E4D0<20, 4>, 0x00B2E520<16, 8>]
template <uint32_t SIZE, uint32_t OFFSET>
void* BSN::Adapter::Array::SetLength(void* data, uint32_t length) {
    uint32_t* count = static_cast<uint32_t*>(data);
    uint8_t* elements = static_cast<uint8_t*>(data) + OFFSET;

    if (length > *count) {
        memset(&elements[SIZE * *count], 0, SIZE * (length - *count));
    }

    *count = length;
    return elements;
}

// PROVEN: 2e535873 IDENTICAL/IDENTICAL
// OFFSET: [0x00A39FF0/0x14090ED00<32>]
template <uint32_t N>
uint32_t BSN::Adapter::FixedBlob::GetLength(const void* data) {
    return N;
}

// INLINED
template <uint32_t N>
uint32_t BSN::Adapter::FixedBitArray::GetLength(const void* data) {
    return N;
}

// INLINED
template <uint32_t OFFSET>
const void* BSN::Adapter::Optional::GetElement(const void* data) {
    return static_cast<const uint8_t*>(data) + OFFSET;
}

// INLINED
template <uint32_t OFFSET>
void* BSN::Adapter::Optional::SetExists(void* data, bool exists) {
    *static_cast<bool*>(data) = exists;

    if (!exists) {
        return NULL;
    }

    return static_cast<uint8_t*>(data) + OFFSET;
}

#endif
