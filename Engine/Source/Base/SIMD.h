#ifndef ENGINE_SOURCE_BASE_SIMD_H
#define ENGINE_SOURCE_BASE_SIMD_H

#include <cstddef>
#include <cstdint>

class C3Vector;
class C44Matrix;
class C4Plane;
class CAaBox;
class M2Vertex;

namespace SIMD {
    struct FunctionTable {
        /* 0000 */ bool (*ProjectBoxTop)(const CAaBox* box, const C44Matrix* matrix, float nearW, float* minX, float* maxX, float* maxY);
        /* 0004 */ uint32_t (*PlaneMask)(int32_t count, const C4Plane* planes, const CAaBox* box);
        /* 0008 */ void (*CompressDXT1)(const uint8_t* src, int32_t pitch, uint8_t* dst);
        /* 000C */ void (*BlendBoneMatrices)(C44Matrix* result, uint32_t weights, uint32_t indices, const C44Matrix* bones);
        /* 0010 */ void (*SkinVertices)(void* dst, const M2Vertex* src, uint32_t count, const C44Matrix* bones, uint32_t texCoord);
        /* 0014 */ void (*SkinVerticesOneBone)(void* dst, const M2Vertex* src, uint32_t count, const C44Matrix* bones, uint32_t texCoord);
        /* 0018 */ void (*ProjectVertices)(C3Vector* dst, const M2Vertex* vertices, uint32_t start, uint32_t count, const uint16_t* indices, const C44Matrix* bones, int32_t normals, const C3Vector* dir, float d);
        /* 001C */ void (*ProjectVerticesOneBone)(C3Vector* dst, const M2Vertex* vertices, uint32_t start, uint32_t count, const uint16_t* indices, const C44Matrix* bones, int32_t normals, const C3Vector* dir, float d);
        /* 0020 */ void (*FFT2D)(float* data, void* re, void* im, int32_t n, int32_t log2n);
        /* 0024 */ void (*Downsample2x2)(const uint32_t* src, uint32_t width, uint32_t height, uint32_t* dst);
    };

    extern FunctionTable s_functions;

    void* AllocA16(uint32_t size);
    void BindAll(int32_t features);
    void FreeA16(void* ptr);
    void Init_AVX(FunctionTable* table);
    void Init_C(FunctionTable* table);
    void Init_SSE(FunctionTable* table);
    void Init_SSE2(FunctionTable* table);
    void Init_SSE3(FunctionTable* table);
    void Init_SSE41(FunctionTable* table);
    void Init_SSSE3(FunctionTable* table);
}

#endif
