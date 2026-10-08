#include "WoW/Source/Battlenet/BSN/Adapter.h"

// PROVEN: 5090b30f IDENTICAL/IDENTICAL
// OFFSET: 0x00B2DD50/0x1408F69E0
uint32_t BSN::Adapter::Array::GetLength(const void* data) {
    return *static_cast<const uint32_t*>(data);
}

// PROVEN: 8d0c407a IDENTICAL/IDENTICAL
// OFFSET: 0x00B2DD50/0x1408F69E0
uint32_t BSN::Adapter::AsciiString::GetLength(const void* data) {
    return *static_cast<const uint32_t*>(data);
}

// OFFSET: #0x008BED40
const void* BSN::Adapter::AsciiString::GetElements(const void* data) {
    return static_cast<const uint8_t*>(data) + 4;
}

// OFFSET: #0x008BF320
void* BSN::Adapter::AsciiString::SetLength(void* data, uint32_t length) {
    char* elements = static_cast<char*>(data) + 4;
    memset(elements, 0, length);
    elements[length] = '\0';
    *static_cast<uint32_t*>(data) = length;
    return elements;
}

// PROVEN: 9b70c32f IDENTICAL/IDENTICAL
// OFFSET: 0x00B2DD50/0x1408F69E0
uint32_t BSN::Adapter::Blob::GetLength(const void* data) {
    return *static_cast<const uint32_t*>(data);
}

// OFFSET: #0x008BED40
const void* BSN::Adapter::Blob::GetElements(const void* data) {
    return static_cast<const uint8_t*>(data) + 4;
}

// OFFSET: #0x008BFA20
void* BSN::Adapter::Blob::SetLength(void* data, uint32_t length) {
    *static_cast<uint32_t*>(data) = length;
    return static_cast<uint8_t*>(data) + 4;
}

// PROVEN: 374c1c9e IDENTICAL/IDENTICAL
// OFFSET: [0x00B2DD50/0x1408F69E0]
uint32_t BSN::Adapter::BitArray::GetLength(const void* data) {
    return *static_cast<const uint32_t*>(data);
}

// PROVEN: 940552cc IDENTICAL/NONE
// OFFSET: [0x00B2E3F0/0x1408F5FA0]
const void* BSN::Adapter::BitArray::GetElements(const void* data) {
    return static_cast<const uint8_t*>(data) + 4;
}

// OFFSET: 0x00B2CFD0
void* BSN::Adapter::BitArray::SetLength(void* data, uint32_t length) {
    uint32_t* count = static_cast<uint32_t*>(data);
    uint8_t* bits = static_cast<uint8_t*>(data) + 4;
    uint32_t current = *count;

    if (length > current) {
        uint32_t used = (current + 8) >> 3;
        bits[used - 1] &= (1 << (current & 7)) - 1;
        uint32_t needed = (length + 7) >> 3;

        if (needed > used) {
            memset(bits + used, 0, needed - used);
        }
    }

    *count = length;

    return bits;
}

// OFFSET: #0x008BFC30
const void* BSN::Adapter::FixedBlob::GetElements(const void* data) {
    return data;
}

// OFFSET: #0x008BFC30
void* BSN::Adapter::FixedBlob::SetLength(void* data, uint32_t length) {
    return data;
}

// OFFSET: #0x008BFC30
const void* BSN::Adapter::FixedBitArray::GetElements(const void* data) {
    return data;
}

// OFFSET: #0x008BFC30
void* BSN::Adapter::FixedBitArray::SetLength(void* data, uint32_t length) {
    return data;
}

// OFFSET: #0x008BEFF0
uint32_t BSN::Adapter::String::GetSize(const void* data) {
    return static_cast<const uint32_t*>(data)[1];
}

// OFFSET: #0x008BF770
const void* BSN::Adapter::String::GetElements(const void* data) {
    return static_cast<const uint8_t*>(data) + 8;
}

// OFFSET: #0x008BEAF0
void* BSN::Adapter::String::SetSize(void* data, uint32_t size) {
    char* elements = static_cast<char*>(data) + 8;
    memset(elements, 0, size);
    elements[size] = '\0';
    static_cast<uint32_t*>(data)[1] = size;
    static_cast<uint32_t*>(data)[0] = size;
    return elements;
}

// OFFSET: #0x008BEAA0
void BSN::Adapter::String::SetLength(void* data, uint32_t length) {
    *static_cast<uint32_t*>(data) = length;
}

// PROVEN: 41763355 IDENTICAL/IDENTICAL
// OFFSET: 0x00B2DD50/0x1408F69E0
uint32_t BSN::Adapter::FourCC::GetValue(const void* data) {
    return *static_cast<const uint32_t*>(data);
}

// OFFSET: #0x008BEAA0
void BSN::Adapter::FourCC::SetValue(void* data, uint32_t value) {
    *static_cast<uint32_t*>(data) = value;
}

// PROVEN: e2330518 IDENTICAL/IDENTICAL
// OFFSET: 0x00B2DD50/0x1408F69E0
int32_t BSN::Adapter::Choice::GetTag(const void* data) {
    return *static_cast<const int32_t*>(data);
}

// OFFSET: #0x008BED40
const void* BSN::Adapter::Choice::GetElement(const void* data) {
    return static_cast<const uint8_t*>(data) + 4;
}

// OFFSET: #0x008BFA20
void* BSN::Adapter::Choice::SetTag(void* data, int32_t tag) {
    *static_cast<int32_t*>(data) = tag;
    return static_cast<uint8_t*>(data) + 4;
}

// OFFSET: #0x008BECB0
bool BSN::Adapter::Optional::GetExists(const void* data) {
    return *static_cast<const uint8_t*>(data) != 0;
}
