#include <cmath>
#include "Engine/Source/Base/Coordinate.h"

// STATIC: 0x00D096B4/0x140BBB524
static float s_aspectRatio = 4.0f / 3.0f;

// STATIC: 0x00D096B8
static float s_ddcWidthScale = 0.8f;

// STATIC: 0x00D096BC
static float s_ddcHeightScale = 0.6f;

// STATIC: 0x00D096C0/0x140BBB530
static float s_aspectCompensation = 1.0f;

// PROVEN: 98cbafae IDENTICAL/UNPROVEN
// OFFSET: 0x00872340/0x1405A8760
void CoordinateSetAspectRatio(float aspect) {
    s_aspectRatio = aspect;
    s_aspectCompensation = aspect * 0.75f;
    float scale = 1.0f / sqrt(aspect * aspect + 1.0f);
    s_ddcHeightScale = scale;
    s_ddcWidthScale = aspect * scale;
}

// PROVEN: 429128d8 IDENTICAL/IDENTICAL
// OFFSET: 0x00872390/0x1405A87B0
float CoordinateGetAspectRatio() {
    return s_aspectRatio;
}

// PROVEN: 203f7196 IDENTICAL/IDENTICAL
// OFFSET: 0x008723A0/0x1405A87C0
float CoordinateGetAspectCompensation() {
    return s_aspectCompensation;
}

// PROVEN: d4e4ae8d IDENTICAL/DIFFERENT
// OFFSET: 0x008723B0/0x1405A87D0
void NDCToDDC(float ndcx, float ndcy, float* ddcx, float* ddcy) {
    if (ddcx) {
        *ddcx = s_ddcWidthScale * ndcx;
    }

    if (ddcy) {
        *ddcy = s_ddcHeightScale * ndcy;
    }
}

// PROVEN: d388163b IDENTICAL/UNPROVEN
// OFFSET: 0x008723F0/0x1405A8800
void DDCToNDC(float ddcx, float ddcy, float* ndcx, float* ndcy) {
    if (ndcx) {
        *ndcx = ddcx / s_ddcWidthScale;
    }

    if (ndcy) {
        *ndcy = ddcy / s_ddcHeightScale;
    }
}

// PROVEN: 40f9d7c4 IDENTICAL/UNPROVEN
// OFFSET: 0x00872430/0x1405A8840
float DDCToNDCWidth(float ddcx) {
    return ddcx / s_ddcWidthScale;
}

// PROVEN: a20c1a5f IDENTICAL/UNPROVEN
// OFFSET: 0x00872440/0x1405A8830
float DDCToNDCHeight(float ddcy) {
    return ddcy / s_ddcHeightScale;
}

// PROVEN: 0e4869d2 IDENTICAL/UNPROVEN
// OFFSET: 0x00872450/0x1405A8850
float NDCToDDCWidth(float ndcx) {
    return s_ddcWidthScale * ndcx;
}

// PROVEN: f869149e IDENTICAL/UNPROVEN
// OFFSET: 0x00872460/0x1405A8860
float NDCToDDCHeight(float ndcy) {
    return s_ddcHeightScale * ndcy;
}
