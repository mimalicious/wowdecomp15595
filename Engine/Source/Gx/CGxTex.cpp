#include <cstdint>
#include "Engine/Source/Gx/CGxTex.h"

// OFFSET: %0x007AA1C0
CGxTex::CGxTex(EGxTexTarget target, uint32_t width, uint32_t height, uint32_t depth, EGxTexFormat format, EGxTexFormat dataFormat, CGxTexFlags flags, void* userArg, GxTexUserFunc userFunc, const char* name) {
    if (!depth) {
        depth = 1;
    }

    this->m_updateRect.minY = 0;
    this->m_updateRect.minX = 0;
    this->m_updatePlaneMax = depth;
    this->m_depth = depth;
    this->m_updateRect.maxY = height;
    this->m_updateRect.maxX = width;
    this->m_updatePlaneMin = 0;
    this->m_width = width;
    this->m_format = format;
    this->m_target = target;
    this->m_dataFormat = dataFormat;
    this->m_apiSpecificData = NULL;
    this->m_apiSpecificData2 = NULL;
    this->m_userArg = userArg;
    this->m_height = height;
    this->m_needsUpdate = 1;
    this->m_needsFlagUpdate = 1;
    this->m_needsCreation = 1;
    this->m_flags = flags;
    this->m_userFunc = userFunc;
    this->m_char5D = 1;
}

// OFFSET: %0x007AAAB0
CGxTex::~CGxTex() {
}
