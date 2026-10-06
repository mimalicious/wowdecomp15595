#include <cmath>
#include <cstdint>
#include <xmmintrin.h>
#include "Engine/Source/Base/SIMD.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"

// INLINED
static void BlendBones_SSE(__m128& row0, __m128& row1, __m128& row2, __m128& row3, uint32_t weights, uint32_t indices, const C44Matrix* bones) {
    const __m128* bone = reinterpret_cast<const __m128*>(&bones[indices & 0xFF]);
    __m128 weight = _mm_set1_ps(static_cast<float>(weights & 0xFF) * 0.0039215689f);
    row0 = _mm_mul_ps(bone[0], weight);
    row1 = _mm_mul_ps(bone[1], weight);
    row2 = _mm_mul_ps(bone[2], weight);
    row3 = _mm_mul_ps(bone[3], weight);

    for (int32_t i = 1; i < 4; i++) {
        weights >>= 8;
        indices >>= 8;

        if (!(weights & 0xFF)) {
            break;
        }

        bone = reinterpret_cast<const __m128*>(&bones[indices & 0xFF]);
        weight = _mm_set1_ps(static_cast<float>(weights & 0xFF) * 0.0039215689f);
        row0 = _mm_add_ps(_mm_mul_ps(bone[0], weight), row0);
        row1 = _mm_add_ps(_mm_mul_ps(bone[1], weight), row1);
        row2 = _mm_add_ps(_mm_mul_ps(bone[2], weight), row2);
        row3 = _mm_add_ps(_mm_mul_ps(bone[3], weight), row3);
    }
}

// INLINED
static void ProjectToPlane_SSE(C3Vector* dst, const M2Vertex* vertex, const __m128& row0, const __m128& row1, const __m128& row2, const __m128& row3, int32_t normals, const __m128& plane) {
    __m128 p = _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(vertex->m_position.y), row1), _mm_mul_ps(_mm_set1_ps(vertex->m_position.x), row0)), _mm_add_ps(_mm_mul_ps(_mm_set1_ps(vertex->m_position.z), row2), row3));

    if (normals) {
        p = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(vertex->m_normal.y), row1), _mm_mul_ps(_mm_set1_ps(vertex->m_normal.x), row0)), _mm_mul_ps(_mm_set1_ps(vertex->m_normal.z), row2)), p);
    }

    __m128 dot = _mm_mul_ps(plane, p);
    dot = _mm_add_ps(_mm_shuffle_ps(dot, dot, 0xB1), dot);
    dot = _mm_add_ps(_mm_shuffle_ps(dot, dot, 0x4E), dot);

    __m128 projected = _mm_sub_ps(p, _mm_mul_ps(dot, plane));
    dst->x = _mm_cvtss_f32(projected);
    dst->z = _mm_cvtss_f32(dot);
    dst->y = _mm_cvtss_f32(_mm_shuffle_ps(projected, projected, 0x55));
}

// INLINED
static __m128 PlaneVector_SSE(const C3Vector* dir, float d) {
    return _mm_unpacklo_ps(_mm_unpacklo_ps(_mm_set_ss(dir->x), _mm_set_ss(dir->z)), _mm_unpacklo_ps(_mm_set_ss(dir->y), _mm_set_ss(-d)));
}

// OFFSET: %0x004995C0
static bool ProjectBoxTop_SSE(const CAaBox* box, const C44Matrix* matrix, float nearW, float* minX, float* maxX, float* maxY) {
    const float* m = &matrix->a0;
    __m128 bottom = _mm_loadu_ps(&box->b.x);
    __m128 top = _mm_loadu_ps(&box->t.x);
    __m128 x = _mm_shuffle_ps(bottom, top, 0x00);
    x = _mm_shuffle_ps(x, x, 0x88);
    __m128 y = _mm_shuffle_ps(bottom, top, 0x55);
    __m128 z = _mm_shuffle_ps(top, top, 0xAA);

    __m128 w = _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(m[6]), y), _mm_mul_ps(_mm_set1_ps(m[2]), x)), _mm_add_ps(_mm_mul_ps(_mm_set1_ps(m[10]), z), _mm_set1_ps(m[14])));

    if (_mm_movemask_ps(_mm_sub_ps(w, _mm_set1_ps(nearW)))) {
        return false;
    }

    __m128 rcp = _mm_rcp_ps(w);
    __m128 px = _mm_mul_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(m[4]), y), _mm_mul_ps(_mm_set1_ps(m[0]), x)), _mm_add_ps(_mm_mul_ps(_mm_set1_ps(m[8]), z), _mm_set1_ps(m[12]))), rcp);
    __m128 py = _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(m[5]), y), _mm_mul_ps(_mm_set1_ps(m[1]), x)), _mm_add_ps(_mm_mul_ps(_mm_set1_ps(m[9]), z), _mm_set1_ps(m[13])));

    __m128 swapped = _mm_shuffle_ps(px, px, 0xB1);
    __m128 high = _mm_max_ps(px, swapped);
    __m128 low = _mm_min_ps(px, swapped);
    py = _mm_mul_ps(py, rcp);
    __m128 highY = _mm_max_ps(py, _mm_shuffle_ps(py, py, 0xB1));

    *minX = _mm_cvtss_f32(_mm_min_ss(low, _mm_shuffle_ps(low, low, 0x4E)));
    *maxX = _mm_cvtss_f32(_mm_max_ss(high, _mm_shuffle_ps(high, high, 0x4E)));
    *maxY = _mm_cvtss_f32(_mm_max_ss(highY, _mm_shuffle_ps(highY, highY, 0x4E)));
    return true;
}

// OFFSET: %0x00499730
static void BlendBoneMatrices_SSE(C44Matrix* result, uint32_t weights, uint32_t indices, const C44Matrix* bones) {
    __m128 row0;
    __m128 row1;
    __m128 row2;
    __m128 row3;
    BlendBones_SSE(row0, row1, row2, row3, weights, indices, bones);

    __m128* out = reinterpret_cast<__m128*>(result);
    out[0] = row0;
    out[1] = row1;
    out[2] = row2;
    out[3] = row3;
    result->a3 = 0.0f;
    result->b3 = 0.0f;
    result->c3 = 0.0f;
    result->d3 = 1.0f;
}

// OFFSET: %0x00499860
static void SkinVertices_SSE(void* dst, const M2Vertex* src, uint32_t count, const C44Matrix* bones, uint32_t texCoord) {
    float* out = static_cast<float*>(dst);
    uint32_t weights = 0;
    uint32_t indices = 0;
    __m128 row0;
    __m128 row1;
    __m128 row2;
    __m128 row3;

    for (uint32_t i = 0; i < count; i++, src++, out += 8) {
        uint32_t vertexWeights = *reinterpret_cast<const uint32_t*>(&src->m_weights);
        uint32_t vertexIndices = *reinterpret_cast<const uint32_t*>(&src->m_indices);

        if (vertexWeights != weights || vertexIndices != indices) {
            weights = vertexWeights;
            indices = vertexIndices;
            BlendBones_SSE(row0, row1, row2, row3, vertexWeights, vertexIndices, bones);
        }

        float u = src->m_texcoord[texCoord].x;
        _mm_storeu_ps(&out[0], _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(src->m_position.z), row2), _mm_add_ps(_mm_mul_ps(_mm_set1_ps(src->m_position.y), row1), _mm_mul_ps(_mm_set1_ps(src->m_position.x), row0))), row3));
        _mm_storeu_ps(&out[3], _mm_add_ps(_mm_add_ps(_mm_mul_ps(_mm_set1_ps(src->m_normal.x), row0), _mm_mul_ps(_mm_set1_ps(src->m_normal.y), row1)), _mm_mul_ps(_mm_set1_ps(src->m_normal.z), row2)));
        out[6] = u;
        out[7] = src->m_texcoord[texCoord].y;
    }
}

// OFFSET: %0x00499A20
static void SkinVerticesOneBone_SSE(void* dst, const M2Vertex* src, uint32_t count, const C44Matrix* bones, uint32_t texCoord) {
    float* out = static_cast<float*>(dst);

    for (uint32_t i = 0; i < count; i++, src++, out += 8) {
        float u = src->m_texcoord[texCoord].x;
        const __m128* bone = reinterpret_cast<const __m128*>(&bones[src->m_indices.b[0]]);
        __m128 row0 = bone[0];
        __m128 row1 = bone[1];
        __m128 row2 = bone[2];
        _mm_storeu_ps(&out[0], _mm_add_ps(_mm_add_ps(_mm_mul_ps(row2, _mm_set1_ps(src->m_position.z)), _mm_add_ps(_mm_mul_ps(row1, _mm_set1_ps(src->m_position.y)), _mm_mul_ps(row0, _mm_set1_ps(src->m_position.x)))), bone[3]));
        _mm_storeu_ps(&out[3], _mm_add_ps(_mm_add_ps(_mm_mul_ps(row0, _mm_set1_ps(src->m_normal.x)), _mm_mul_ps(row1, _mm_set1_ps(src->m_normal.y))), _mm_mul_ps(_mm_set1_ps(src->m_normal.z), row2)));
        out[6] = u;
        out[7] = src->m_texcoord[texCoord].y;
    }
}

// OFFSET: %0x00499B10
static void ProjectVertices_SSE(C3Vector* dst, const M2Vertex* vertices, uint32_t start, uint32_t count, const uint16_t* indices, const C44Matrix* bones, int32_t normals, const C3Vector* dir, float d) {
    uint32_t weights = 0;
    uint32_t boneIndices = 0;
    __m128 row0;
    __m128 row1;
    __m128 row2;
    __m128 row3;
    __m128 plane = PlaneVector_SSE(dir, d);
    const uint16_t* index = &indices[start];

    for (uint32_t i = 0; i < count; i++, index++, dst++) {
        const M2Vertex* vertex = &vertices[*index];
        uint32_t vertexWeights = *reinterpret_cast<const uint32_t*>(&vertex->m_weights);
        uint32_t vertexIndices = *reinterpret_cast<const uint32_t*>(&vertex->m_indices);

        if (vertexWeights != weights || vertexIndices != boneIndices) {
            weights = vertexWeights;
            boneIndices = vertexIndices;
            BlendBones_SSE(row0, row1, row2, row3, vertexWeights, vertexIndices, bones);
        }

        ProjectToPlane_SSE(dst, vertex, row0, row1, row2, row3, normals, plane);
    }
}

// OFFSET: %0x00499D40
static void ProjectVerticesOneBone_SSE(C3Vector* dst, const M2Vertex* vertices, uint32_t start, uint32_t count, const uint16_t* indices, const C44Matrix* bones, int32_t normals, const C3Vector* dir, float d) {
    __m128 plane = PlaneVector_SSE(dir, d);
    const uint16_t* index = &indices[start];

    for (uint32_t i = 0; i < count; i++, index++, dst++) {
        const M2Vertex* vertex = &vertices[*index];
        const __m128* bone = reinterpret_cast<const __m128*>(&bones[vertex->m_indices.b[0]]);
        ProjectToPlane_SSE(dst, vertex, bone[0], bone[1], bone[2], bone[3], normals, plane);
    }
}

// OFFSET: %0x00499E90
static void FFT_SSE(__m128* re, __m128* im, int32_t n, int32_t log2n) {
    int32_t j = 0;

    for (int32_t i = 0; i < n - 1; i++) {
        if (i < j) {
            __m128 tr = re[i];
            __m128 ti = im[i];
            re[i] = re[j];
            im[i] = im[j];
            re[j] = tr;
            im[j] = ti;
        }

        int32_t k = n >> 1;

        while (k <= j) {
            j -= k;
            k >>= 1;
        }

        j += k;
    }

    __m128 one = _mm_set1_ps(1.0f);
    __m128 half = _mm_set1_ps(0.5f);
    __m128 c1 = _mm_set1_ps(-1.0f);
    __m128 c2 = _mm_setzero_ps();
    int32_t l2 = 1;

    for (int32_t l = 0; l < log2n; l++) {
        int32_t l1 = l2;
        l2 <<= 1;
        __m128 u1 = one;
        __m128 u2 = _mm_setzero_ps();

        for (int32_t m = 0; m < l1; m++) {
            for (int32_t i = m; i < n; i += l2) {
                int32_t i1 = i + l1;
                __m128 t2 = _mm_add_ps(_mm_mul_ps(re[i1], u2), _mm_mul_ps(im[i1], u1));
                __m128 t1 = _mm_sub_ps(_mm_mul_ps(re[i1], u1), _mm_mul_ps(im[i1], u2));
                re[i1] = _mm_sub_ps(re[i], t1);
                im[i1] = _mm_sub_ps(im[i], t2);
                re[i] = _mm_add_ps(t1, re[i]);
                im[i] = _mm_add_ps(t2, im[i]);
            }

            __m128 z = u2;
            u2 = _mm_add_ps(_mm_mul_ps(u1, c2), _mm_mul_ps(u2, c1));
            u1 = _mm_sub_ps(_mm_mul_ps(u1, c1), _mm_mul_ps(z, c2));
        }

        c2 = _mm_sqrt_ps(_mm_mul_ps(_mm_sub_ps(one, c1), half));
        c1 = _mm_sqrt_ps(_mm_mul_ps(_mm_add_ps(c1, one), half));
    }
}

// OFFSET: %0x0049A0B0
static void FFT2D_SSE(float* data, void* re, void* im, int32_t n, int32_t log2n) {
    __m128* real = static_cast<__m128*>(re);
    __m128* imag = static_cast<__m128*>(im);
    int32_t groups = n / 4;

    for (int32_t group = 0; group < groups; group++) {
        float* line0 = &data[2 * n * (4 * group + 0)];
        float* line1 = &data[2 * n * (4 * group + 1)];
        float* line2 = &data[2 * n * (4 * group + 2)];
        float* line3 = &data[2 * n * (4 * group + 3)];

        for (int32_t i = 0; i < n; i++) {
            real[i] = _mm_setr_ps(line0[2 * i], line1[2 * i], line2[2 * i], line3[2 * i]);
            imag[i] = _mm_setr_ps(line0[2 * i + 1], line1[2 * i + 1], line2[2 * i + 1], line3[2 * i + 1]);
        }

        FFT_SSE(real, imag, n, log2n);

        for (int32_t i = 0; i < n; i++) {
            const float* r = reinterpret_cast<const float*>(&real[i]);
            const float* m = reinterpret_cast<const float*>(&imag[i]);
            line0[2 * i] = r[0];
            line0[2 * i + 1] = m[0];
            line1[2 * i] = r[1];
            line1[2 * i + 1] = m[1];
            line2[2 * i] = r[2];
            line2[2 * i + 1] = m[2];
            line3[2 * i] = r[3];
            line3[2 * i + 1] = m[3];
        }
    }

    for (int32_t group = 0; group < groups; group++) {
        for (int32_t i = 0; i < n; i++) {
            const float* cell = &data[2 * (n * i + 4 * group)];
            real[i] = _mm_setr_ps(cell[0], cell[2], cell[4], cell[6]);
            imag[i] = _mm_setr_ps(cell[1], cell[3], cell[5], cell[7]);
        }

        FFT_SSE(real, imag, n, log2n);

        for (int32_t i = 0; i < n; i++) {
            float* cell = &data[2 * (n * i + 4 * group)];
            const float* r = reinterpret_cast<const float*>(&real[i]);
            const float* m = reinterpret_cast<const float*>(&imag[i]);
            cell[0] = r[0];
            cell[1] = m[0];
            cell[2] = r[1];
            cell[3] = m[1];
            cell[4] = r[2];
            cell[5] = m[2];
            cell[6] = r[3];
            cell[7] = m[3];
        }
    }
}

// PROVEN: de111e50 IDENTICAL/UNPROVEN
// OFFSET: 0x004928E0/0x1400B7930
void SIMD::Init_SSE(SIMD::FunctionTable* table) {
    table->ProjectBoxTop = ProjectBoxTop_SSE;
    table->BlendBoneMatrices = BlendBoneMatrices_SSE;
    table->SkinVertices = SkinVertices_SSE;
    table->SkinVerticesOneBone = SkinVerticesOneBone_SSE;
    table->ProjectVertices = ProjectVertices_SSE;
    table->ProjectVerticesOneBone = ProjectVerticesOneBone_SSE;
    table->FFT2D = FFT2D_SSE;
}
