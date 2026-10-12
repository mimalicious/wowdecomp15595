#include <cmath>
#include <cstdint>
#include "Engine/Source/Base/SIMD.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/CAaBox.h"

// INLINED
static void BlendBones_C(C44Matrix& matrix, uint32_t weights, uint32_t indices, const C44Matrix* bones) {
    const C44Matrix* bone = &bones[indices & 0xFF];
    float weight = static_cast<float>(weights & 0xFF) * 0.0039215689f;
    matrix.a0 = bone->a0 * weight;
    matrix.a1 = bone->a1 * weight;
    matrix.a2 = bone->a2 * weight;
    matrix.b0 = bone->b0 * weight;
    matrix.b1 = bone->b1 * weight;
    matrix.b2 = bone->b2 * weight;
    matrix.c0 = bone->c0 * weight;
    matrix.c1 = bone->c1 * weight;
    matrix.c2 = bone->c2 * weight;
    matrix.d0 = weight * bone->d0;
    matrix.d1 = bone->d1 * weight;
    matrix.d2 = bone->d2 * weight;

    for (int32_t i = 1; i < 4; i++) {
        weights >>= 8;
        indices >>= 8;

        if (!(weights & 0xFF)) {
            break;
        }

        bone = &bones[indices & 0xFF];
        weight = static_cast<float>(weights & 0xFF) * 0.0039215689f;
        matrix.a0 = bone->a0 * weight + matrix.a0;
        matrix.a1 = bone->a1 * weight + matrix.a1;
        matrix.a2 = bone->a2 * weight + matrix.a2;
        matrix.b0 = bone->b0 * weight + matrix.b0;
        matrix.b1 = bone->b1 * weight + matrix.b1;
        matrix.b2 = bone->b2 * weight + matrix.b2;
        matrix.c0 = bone->c0 * weight + matrix.c0;
        matrix.c1 = bone->c1 * weight + matrix.c1;
        matrix.c2 = bone->c2 * weight + matrix.c2;
        matrix.d0 = weight * bone->d0 + matrix.d0;
        matrix.d1 = bone->d1 * weight + matrix.d1;
        matrix.d2 = bone->d2 * weight + matrix.d2;
    }
}

// INLINED
static uint32_t SignBit(float value) {
    return *reinterpret_cast<uint32_t*>(&value) >> 31;
}

// INLINED
static uint8_t ClampColor(float value) {
    int32_t color = static_cast<int32_t>(value * 255.0f + 0.5f);

    if (color < 0) {
        return 0;
    }

    if (color > 255) {
        return 255;
    }

    return static_cast<uint8_t>(color);
}

// OFFSET: 0x00492920/0x1400B7980
static bool ProjectBoxTop_C(const CAaBox* box, const C44Matrix* matrix, float nearW, float* minX, float* maxX, float* maxY) {
    float x0 = box->b.x;
    float y0 = box->b.y;
    float x1 = box->t.x;
    float y1 = box->t.y;
    float z = box->t.z;

    float w0 = matrix->a2 * x0 + matrix->b2 * y0 + matrix->c2 * z + matrix->d2;
    float w1 = matrix->a2 * x1 + matrix->b2 * y0 + matrix->c2 * z + matrix->d2;
    float w2 = matrix->a2 * x0 + matrix->b2 * y1 + matrix->c2 * z + matrix->d2;
    float w3 = matrix->a2 * x1 + matrix->b2 * y1 + matrix->c2 * z + matrix->d2;

    if (nearW > w0 || nearW > w1 || nearW > w2 || nearW > w3) {
        return false;
    }

    float px0 = (1.0f / w0) * (matrix->a0 * x0 + matrix->b0 * y0 + matrix->c0 * z + matrix->d0);
    float px1 = (1.0f / w1) * (matrix->a0 * x1 + matrix->b0 * y0 + matrix->c0 * z + matrix->d0);
    float px2 = (1.0f / w2) * (matrix->a0 * x0 + matrix->b0 * y1 + matrix->c0 * z + matrix->d0);
    float px3 = (1.0f / w3) * (matrix->a0 * x1 + matrix->b0 * y1 + matrix->c0 * z + matrix->d0);
    float py0 = (1.0f / w0) * (matrix->a1 * x0 + matrix->b1 * y0 + matrix->c1 * z + matrix->d1);
    float py1 = (1.0f / w1) * (matrix->a1 * x1 + matrix->b1 * y0 + matrix->c1 * z + matrix->d1);
    float py2 = (1.0f / w2) * (matrix->a1 * x0 + matrix->b1 * y1 + matrix->c1 * z + matrix->d1);
    float py3 = (1.0f / w3) * (matrix->a1 * x1 + matrix->b1 * y1 + matrix->c1 * z + matrix->d1);

    float lowX = px0;

    if (px1 <= lowX) {
        lowX = px1;
    }

    if (px2 <= lowX) {
        lowX = px2;
    }

    if (px3 <= lowX) {
        lowX = px3;
    }

    float highX = px0;

    if (highX <= px1) {
        highX = px1;
    }

    if (highX <= px2) {
        highX = px2;
    }

    if (highX <= px3) {
        highX = px3;
    }

    float highY = py0;

    if (highY <= py1) {
        highY = py1;
    }

    if (highY <= py2) {
        highY = py2;
    }

    if (highY <= py3) {
        highY = py3;
    }

    *minX = lowX;
    *maxX = highX;
    *maxY = highY;
    return true;
}

// OFFSET: 0x00492CA0/0x1400B7D40
static uint32_t PlaneMask_C(int32_t count, const C4Plane* planes, const CAaBox* box) {
    const C3Vector* corners[2] = { &box->t, &box->b };
    uint32_t mask = 0;

    for (int32_t i = 0; i < count; i++) {
        const C4Plane& plane = planes[i];
        float dist = corners[SignBit(plane.n.x)]->x * plane.n.x + corners[SignBit(plane.n.z)]->z * plane.n.z + corners[SignBit(plane.n.y)]->y * plane.n.y + plane.d;
        mask = (dist >= 0.0f) | (mask << 1);
    }

    return mask;
}

// OFFSET: 0x00492ED0/0x1400B7FA0
static void CompressDXT1_C(const uint8_t* src, int32_t pitch, uint8_t* dst) {
    float colors[16][3];
    float* color = colors[0];

    for (int32_t y = 0; y < 4; y++) {
        const uint8_t* pixel = src + y * pitch;

        for (int32_t x = 0; x < 4; x++) {
            color[0] = static_cast<float>(pixel[0]) * 0.0039215689f;
            color[1] = static_cast<float>(pixel[1]) * 0.0039215689f;
            color[2] = static_cast<float>(pixel[2]) * 0.0039215689f;
            color += 3;
            pixel += 4;
        }
    }

    float mean[3] = { 0.0f, 0.0f, 0.0f };

    for (int32_t i = 0; i < 16; i++) {
        mean[0] = colors[i][0] * 0.0625f + mean[0];
        mean[1] = colors[i][1] * 0.0625f + mean[1];
        mean[2] = colors[i][2] * 0.0625f + mean[2];
    }

    float c00 = 0.0f;
    float c01 = 0.0f;
    float c02 = 0.0f;
    float c11 = 0.0f;
    float c12 = 0.0f;
    float c22 = 0.0f;

    for (int32_t i = 0; i < 16; i++) {
        float dx = colors[i][0] - mean[0];
        float dz = colors[i][2] - mean[2];
        float dy = colors[i][1] - mean[1];
        c01 = dy * dx + c01;
        c11 = dy * dy + c11;
        c12 = dz * dy + c12;
        c00 = dx * dx + c00;
        c02 = dz * dx + c02;
        c22 = dz * dz + c22;
    }

    float axis[3] = { 1.0f, 1.0f, 1.0f };

    for (int32_t i = 0; i < 8; i++) {
        float x = axis[2] * c02 + axis[1] * c01 + axis[0] * c00;
        float y = axis[2] * c12 + axis[1] * c11 + axis[0] * c01;
        float z = axis[2] * c22 + axis[1] * c12 + axis[0] * c02;
        float largest = x <= y ? y : x;

        if (largest <= z) {
            largest = z;
        }

        float scale = largest != 0.0f ? 1.0f / largest : 0.0f;
        axis[0] = scale * x;
        axis[1] = scale * y;
        axis[2] = scale * z;
    }

    float lengthSq = axis[2] * axis[2] + axis[1] * axis[1] + axis[0] * axis[0];

    if (lengthSq != 0.0f) {
        float scale = static_cast<float>(1.0 / sqrt(lengthSq));
        axis[0] *= scale;
        axis[1] *= scale;
        axis[2] = scale * axis[2];
    }

    float low = 1.0f;
    float high = -1.0f;

    for (int32_t i = 0; i < 16; i++) {
        float t = (colors[i][0] - mean[0]) * axis[0] + (colors[i][1] - mean[1]) * axis[1] + (colors[i][2] - mean[2]) * axis[2];

        if (low > t) {
            low = t;
        }

        if (t > high) {
            high = t;
        }
    }

    uint8_t lowColor[3];
    uint8_t highColor[3];

    for (int32_t i = 0; i < 3; i++) {
        lowColor[i] = ClampColor(axis[i] * low + mean[i]);
        highColor[i] = ClampColor(axis[i] * high + mean[i]);
    }

    uint32_t color0 = (highColor[0] >> 3) | (8 * ((highColor[1] & 0xFC) | (32 * (highColor[2] & 0xF8))));
    uint32_t color1 = (lowColor[0] >> 3) | (8 * ((lowColor[1] & 0xFC) | (32 * (lowColor[2] & 0xF8))));
    uint32_t remap[4] = { 1, 3, 2, 0 };

    if (color0 < color1) {
        uint32_t swap = color0;
        color0 = color1;
        color1 = swap;
        remap[0] = 0;
        remap[1] = 2;
        remap[2] = 3;
        remap[3] = 1;
    }

    *reinterpret_cast<uint16_t*>(&dst[0]) = static_cast<uint16_t>(color0);
    *reinterpret_cast<uint16_t*>(&dst[2]) = static_cast<uint16_t>(color1);

    if (color0 == color1) {
        dst[7] = 0;
        dst[6] = 0;
        dst[4] = 0;
        dst[5] = 0;
        return;
    }

    uint32_t bits = 0;

    for (int32_t i = 0; i < 16; i++) {
        float t = (colors[i][0] - mean[0]) * axis[0] + (colors[i][1] - mean[1]) * axis[1] + (colors[i][2] - mean[2]) * axis[2];
        int32_t index = static_cast<int32_t>((t - low) * (3.0f / (high - low)) + 0.5f);
        bits |= remap[index] << (2 * i);
    }

    *reinterpret_cast<uint32_t*>(&dst[4]) = bits;
}

// OFFSET: 0x004935E0/0x1400B87E0
static void BlendBoneMatrices_C(C44Matrix* result, uint32_t weights, uint32_t indices, const C44Matrix* bones) {
    BlendBones_C(*result, weights, indices, bones);
}

// OFFSET: 0x00493770/0x1400B89E0
static void SkinVertices_C(void* dst, const M2Vertex* src, uint32_t count, const C44Matrix* bones, uint32_t texCoord) {
    float* out = static_cast<float*>(dst);
    C44Matrix matrix;
    uint32_t weights = 0;
    uint32_t indices = 0;

    for (uint32_t i = 0; i < count; i++, src++, out += 8) {
        uint32_t vertexWeights = *reinterpret_cast<const uint32_t*>(&src->m_weights);
        uint32_t vertexIndices = *reinterpret_cast<const uint32_t*>(&src->m_indices);

        if (vertexWeights != weights || vertexIndices != indices) {
            weights = vertexWeights;
            indices = vertexIndices;
            BlendBones_C(matrix, vertexWeights, vertexIndices, bones);
        }

        out[6] = src->m_texcoord[texCoord].x;
        out[7] = src->m_texcoord[texCoord].y;
        out[2] = src->m_position.y * matrix.b2 + src->m_position.z * matrix.c2 + src->m_position.x * matrix.a2 + matrix.d2;
        out[0] = src->m_position.y * matrix.b0 + src->m_position.z * matrix.c0 + src->m_position.x * matrix.a0 + matrix.d0;
        out[1] = src->m_position.y * matrix.b1 + src->m_position.z * matrix.c1 + src->m_position.x * matrix.a1 + matrix.d1;
        out[3] = src->m_normal.x * matrix.a0 + src->m_normal.z * matrix.c0 + src->m_normal.y * matrix.b0;
        out[4] = src->m_normal.x * matrix.a1 + src->m_normal.z * matrix.c1 + src->m_normal.y * matrix.b1;
        out[5] = src->m_normal.x * matrix.a2 + src->m_normal.z * matrix.c2 + src->m_normal.y * matrix.b2;
    }
}

// OFFSET: 0x00493AD0/0x1400B8EA0
static void SkinVerticesOneBone_C(void* dst, const M2Vertex* src, uint32_t count, const C44Matrix* bones, uint32_t texCoord) {
    float* out = static_cast<float*>(dst);

    for (uint32_t i = 0; i < count; i++, src++, out += 8) {
        const C44Matrix& bone = bones[src->m_indices.b[0]];
        out[2] = src->m_position.y * bone.b2 + src->m_position.z * bone.c2 + src->m_position.x * bone.a2 + bone.d2;
        out[0] = src->m_position.x * bone.a0 + src->m_position.y * bone.b0 + src->m_position.z * bone.c0 + bone.d0;
        out[1] = src->m_position.y * bone.b1 + src->m_position.z * bone.c1 + src->m_position.x * bone.a1 + bone.d1;
        out[3] = src->m_normal.x * bone.a0 + src->m_normal.z * bone.c0 + src->m_normal.y * bone.b0;
        out[6] = src->m_texcoord[texCoord].x;
        out[4] = src->m_normal.x * bone.a1 + src->m_normal.z * bone.c1 + src->m_normal.y * bone.b1;
        out[7] = src->m_texcoord[texCoord].y;
        out[5] = src->m_normal.x * bone.a2 + src->m_normal.z * bone.c2 + src->m_normal.y * bone.b2;
    }
}

// INLINED
static void ProjectToPlane_C(C3Vector* dst, const M2Vertex* vertex, const C44Matrix& matrix, int32_t normals, const C3Vector* dir, float d) {
    float x = vertex->m_position.x * matrix.a0 + vertex->m_position.y * matrix.b0 + vertex->m_position.z * matrix.c0 + matrix.d0;
    float z = vertex->m_position.x * matrix.a2 + vertex->m_position.y * matrix.b2 + vertex->m_position.z * matrix.c2 + matrix.d2;
    float y = vertex->m_position.x * matrix.a1 + vertex->m_position.y * matrix.b1 + vertex->m_position.z * matrix.c1 + matrix.d1;

    if (normals) {
        x = vertex->m_normal.x * matrix.a0 + vertex->m_normal.y * matrix.b0 + vertex->m_normal.z * matrix.c0 + x;
        y = vertex->m_normal.x * matrix.a1 + vertex->m_normal.y * matrix.b1 + vertex->m_normal.z * matrix.c1 + y;
        z = vertex->m_normal.x * matrix.a2 + vertex->m_normal.y * matrix.b2 + vertex->m_normal.z * matrix.c2 + z;
    }

    float dist = dir->x * x + dir->y * y + dir->z * z - d;
    dst->x = x - dir->x * dist;
    dst->y = y - dir->y * dist;
    dst->z = dist;
}

// OFFSET: 0x00493C90/0x1400B90B0
static void ProjectVertices_C(C3Vector* dst, const M2Vertex* vertices, uint32_t start, uint32_t count, const uint16_t* indices, const C44Matrix* bones, int32_t normals, const C3Vector* dir, float d) {
    C44Matrix matrix;
    uint32_t weights = 0;
    uint32_t boneIndices = 0;
    const uint16_t* index = &indices[start];

    for (uint32_t i = 0; i < count; i++, index++, dst++) {
        const M2Vertex* vertex = &vertices[*index];
        uint32_t vertexWeights = *reinterpret_cast<const uint32_t*>(&vertex->m_weights);
        uint32_t vertexIndices = *reinterpret_cast<const uint32_t*>(&vertex->m_indices);

        if (vertexWeights != weights || vertexIndices != boneIndices) {
            BlendBones_C(matrix, vertexWeights, vertexIndices, bones);
            weights = vertexWeights;
            boneIndices = vertexIndices;
        }

        ProjectToPlane_C(dst, vertex, matrix, normals, dir, d);
    }
}

// OFFSET: 0x00494020/0x1400B95D0
static void ProjectVerticesOneBone_C(C3Vector* dst, const M2Vertex* vertices, uint32_t start, uint32_t count, const uint16_t* indices, const C44Matrix* bones, int32_t normals, const C3Vector* dir, float d) {
    const uint16_t* index = &indices[start];

    for (uint32_t i = 0; i < count; i++, index++, dst++) {
        const M2Vertex* vertex = &vertices[*index];
        ProjectToPlane_C(dst, vertex, bones[vertex->m_indices.b[0]], normals, dir, d);
    }
}

// OFFSET: 0x00494240/0x1400B9870
static void FFT_C(float* re, float* im, int32_t n, int32_t log2n) {
    int32_t j = 0;

    for (int32_t i = 0; i < n - 1; i++) {
        if (i < j) {
            float tr = re[i];
            float ti = im[i];
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

    float c1 = -1.0f;
    float c2 = 0.0f;
    int32_t l2 = 1;

    for (int32_t l = 0; l < log2n; l++) {
        int32_t l1 = l2;
        l2 <<= 1;
        float u1 = 1.0f;
        float u2 = 0.0f;

        for (int32_t m = 0; m < l1; m++) {
            for (int32_t i = m; i < n; i += l2) {
                int32_t i1 = i + l1;
                float t2 = re[i1] * u2 + im[i1] * u1;
                float t1 = re[i1] * u1 - im[i1] * u2;
                re[i1] = re[i] - t1;
                im[i1] = im[i] - t2;
                re[i] = re[i] + t1;
                im[i] = im[i] + t2;
            }

            float z = u1 * c1 - u2 * c2;
            u2 = u1 * c2 + u2 * c1;
            u1 = z;
        }

        c2 = static_cast<float>(sqrt((1.0 - c1) * 0.5));
        c1 = static_cast<float>(sqrt((c1 + 1.0) * 0.5));
    }
}

// OFFSET: 0x00494410/0x1400B9AB0
static void FFT2D_C(float* data, void* re, void* im, int32_t n, int32_t log2n) {
    float* real = static_cast<float*>(re);
    float* imag = static_cast<float*>(im);

    for (int32_t row = 0; row < n; row++) {
        float* line = &data[2 * n * row];

        for (int32_t i = 0; i < n; i++) {
            real[i] = line[2 * i];
            imag[i] = line[2 * i + 1];
        }

        FFT_C(real, imag, n, log2n);

        for (int32_t i = 0; i < n; i++) {
            line[2 * i] = real[i];
            line[2 * i + 1] = imag[i];
        }
    }

    for (int32_t column = 0; column < n; column++) {
        for (int32_t i = 0; i < n; i++) {
            real[i] = data[2 * (n * i + column)];
            imag[i] = data[2 * (n * i + column) + 1];
        }

        FFT_C(real, imag, n, log2n);

        for (int32_t i = 0; i < n; i++) {
            data[2 * (n * i + column)] = real[i];
            data[2 * (n * i + column) + 1] = imag[i];
        }
    }
}

// OFFSET: 0x00494880/0x1400B9FB0
static void Downsample2x2_C(const uint32_t* src, uint32_t width, uint32_t height, uint32_t* dst) {
    uint32_t dstRow = 0;

    for (uint32_t y = 0; y < height; y += 2) {
        const uint32_t* row0 = src + y * width;
        const uint32_t* row1 = row0 + width;
        uint32_t index = dstRow;

        for (uint32_t x = 0; x < width; x += 2) {
            uint32_t p0 = row0[x];
            uint32_t p1 = row0[x + 1];
            uint32_t p2 = row1[x];
            uint32_t p3 = row1[x + 1];
            uint32_t b = (p0 & 0xFF) + (p1 & 0xFF) + (p2 & 0xFF) + (p3 & 0xFF);
            uint32_t g = ((p0 >> 8) & 0xFF) + ((p1 >> 8) & 0xFF) + ((p2 >> 8) & 0xFF) + ((p3 >> 8) & 0xFF);
            uint32_t r = ((p0 >> 16) & 0xFF) + ((p1 >> 16) & 0xFF) + ((p2 >> 16) & 0xFF) + ((p3 >> 16) & 0xFF);
            uint32_t a = (p0 >> 24) + (p1 >> 24) + (p2 >> 24) + (p3 >> 24);
            dst[index >> 1] = (b >> 2) + (((g & 0x3FC) + ((((a & ~3u) << 8) + (r & 0x3FC)) << 8)) << 6);
            index += 2;
        }

        dstRow += 2 * (width >> 1);
    }
}

// PROVEN: 51a426e3 IDENTICAL/IDENTICAL
// OFFSET: 0x00494A10/0x1400BA1C0
void SIMD::Init_C(SIMD::FunctionTable* table) {
    table->ProjectBoxTop = ProjectBoxTop_C;
    table->PlaneMask = PlaneMask_C;
    table->CompressDXT1 = CompressDXT1_C;
    table->BlendBoneMatrices = BlendBoneMatrices_C;
    table->SkinVertices = SkinVertices_C;
    table->SkinVerticesOneBone = SkinVerticesOneBone_C;
    table->ProjectVertices = ProjectVertices_C;
    table->ProjectVerticesOneBone = ProjectVerticesOneBone_C;
    table->FFT2D = FFT2D_C;
    table->Downsample2x2 = Downsample2x2_C;
}
