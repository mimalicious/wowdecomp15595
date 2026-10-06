#include <cstdint>
#include "Engine/Source/Gx/CGxTex.h"

// OFFSET: 0x00844340/0x140570D80
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

// OFFSET: 0x00844580/0x140571100
CGxTex::~CGxTex() {
}

// PROVEN: 47d8cc07 IDENTICAL/NONE
// OFFSET: 0x0086B9F0
CGxTex& CGxTex::operator=(const CGxTex& source) {
    this->m_updateRect = source.m_updateRect;
    this->m_updatePlaneMin = source.m_updatePlaneMin;
    this->m_updatePlaneMax = source.m_updatePlaneMax;
    this->m_width = source.m_width;
    this->m_height = source.m_height;
    this->m_depth = source.m_depth;
    this->m_target = source.m_target;
    this->m_format = source.m_format;
    this->m_dataFormat = source.m_dataFormat;
    this->m_flags = source.m_flags;
    this->m_userArg = source.m_userArg;
    this->m_userFunc = source.m_userFunc;
    this->m_apiSpecificData = source.m_apiSpecificData;
    this->m_apiSpecificData2 = source.m_apiSpecificData2;
    this->unk_0050 = source.unk_0050;
    this->m_needsUpdate = source.m_needsUpdate;
    this->m_needsCreation = source.m_needsCreation;
    this->m_needsFlagUpdate = source.m_needsFlagUpdate;
    this->m_char5D = source.m_char5D;

    return *this;
}
