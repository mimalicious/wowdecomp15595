#include "BLLLoader.h"

// INLINED
BLLLoader::~BLLLoader() {
    this->Unload();
}

// OFFSET: %0x007E1AD0
void* BLLLoader::GetExport(int32_t ordinal) {
    uint8_t* image = this->m_image;

    if (!image) {
        return NULL;
    }

    const uint32_t* header = reinterpret_cast<const uint32_t*>(image);
    uint32_t index = ordinal - header[6];

    if (index >= header[5]) {
        return NULL;
    }

    return image + *reinterpret_cast<const uint32_t*>(image + header[4] + index * 4);
}
