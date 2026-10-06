#include <cmath>
#include "Engine/Source/Base/Coordinate.h"

// STATIC: #0x00AC0CB0
static float s_aspectRatio = 4.0f / 3.0f;

// STATIC: %0x00C0E9F8
static float s_ddcWidthScale = 0.8f;

// STATIC: %0x00C0E9FC
static float s_ddcHeightScale = 0.6f;

// STATIC: %0x00C0EA00
static float s_aspectCompensation = 1.0f;

// OFFSET: %0x007D6F10
void CoordinateSetAspectRatio(float aspect) {
    s_aspectRatio = aspect;
    s_aspectCompensation = aspect * 0.75f;
    float scale = 1.0f / sqrt(aspect * aspect + 1.0f);
    s_ddcHeightScale = scale;
    s_ddcWidthScale = aspect * scale;
}

// OFFSET: #0x0047BFD0
float CoordinateGetAspectRatio() {
    return s_aspectRatio;
}

// OFFSET: %0x007D6F70
float CoordinateGetAspectCompensation() {
    return s_aspectCompensation;
}

// OFFSET: %0x007D6F80
void NDCToDDC(float ndcx, float ndcy, float* ddcx, float* ddcy) {
    if (ddcx) {
        *ddcx = s_ddcWidthScale * ndcx;
    }

    if (ddcy) {
        *ddcy = s_ddcHeightScale * ndcy;
    }
}

// OFFSET: %0x007D6FC0
void DDCToNDC(float ddcx, float ddcy, float* ndcx, float* ndcy) {
    if (ndcx) {
        *ndcx = ddcx / s_ddcWidthScale;
    }

    if (ndcy) {
        *ndcy = ddcy / s_ddcHeightScale;
    }
}

// OFFSET: %0x007D7000
float DDCToNDCWidth(float ddcx) {
    return ddcx / s_ddcWidthScale;
}

// OFFSET: %0x007D7010
float DDCToNDCHeight(float ddcy) {
    return ddcy / s_ddcHeightScale;
}

// OFFSET: %0x007D7020
float NDCToDDCWidth(float ndcx) {
    return s_ddcWidthScale * ndcx;
}

// OFFSET: %0x007D7030
float NDCToDDCHeight(float ndcy) {
    return s_ddcHeightScale * ndcy;
}
