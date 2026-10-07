#include "WoW/Source/WorldClient/CWorldMath.h"
#include <cstring>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/CAaBox.h"

// STATIC: 0x01008A30
static uint32_t s_clipSides[32];

// STATIC: 0x01008AB0
static float s_clipDists[32];

// OFFSET: 0x00B130F0/0x1408D8D90
static void TransformAABoxInner(CAaBox& dst, const CAaBox& src, const float* row1, const float* row0, const float* row2) {
    float* bmin = &dst.b.x;
    float* bmax = &dst.t.x;

    for (uint32_t i = 0; i < 3; i++) {
        float lo = row0[i] * src.b.x;
        float hi = row0[i] * src.t.x;

        if (hi <= lo) {
            bmin[i] += hi;
            bmax[i] += lo;
        } else {
            bmin[i] += lo;
            bmax[i] += hi;
        }

        lo = src.b.y * row1[i];
        hi = row1[i] * src.t.y;

        if (hi <= lo) {
            bmin[i] += hi;
            bmax[i] += lo;
        } else {
            bmin[i] += lo;
            bmax[i] += hi;
        }

        lo = row2[i] * src.b.z;
        hi = row2[i] * src.t.z;

        if (hi <= lo) {
            bmin[i] += hi;
            bmax[i] += lo;
        } else {
            bmin[i] += lo;
            bmax[i] += hi;
        }
    }
}

// OFFSET: 0x00B13210/0x1408D8E90
void CWorldMath::TransformAABox(const C44Matrix& m, const CAaBox& src, CAaBox& dst) {
    dst.b.x = m.d0;
    dst.b.y = m.d1;
    dst.b.z = m.d2;
    dst.t.x = m.d0;
    dst.t.y = m.d1;
    dst.t.z = m.d2;
    TransformAABoxInner(dst, src, &m.b0, &m.a0, &m.c0);
}

// OFFSET: 0x00B13260/0x1408D8EE0
bool CWorldMath::VectorIntersectAABox2(const CAaBox& box, const C3Vector& start, const C3Vector& end) {
    float dir[3];
    dir[0] = end.x - start.x;
    dir[1] = end.y - start.y;
    dir[2] = end.z - start.z;

    float maxT[3] = { -1.0f, -1.0f, -1.0f };
    const float* bmin = &box.b.x;
    const float* bmax = &box.t.x;
    const float* s = &start.x;
    const float* e = &end.x;
    bool inside = true;

    for (uint32_t i = 0; i < 3; i++) {
        float candidate = bmin[i];

        if (candidate <= s[i]) {
            candidate = bmax[i];

            if (s[i] <= candidate) {
                continue;
            }

            if (!(e[i] <= candidate)) {
                return false;
            }
        } else if (!(candidate <= e[i])) {
            return false;
        }

        inside = false;

        if (dir[i] != 0.0f) {
            maxT[i] = (candidate - s[i]) / dir[i];
        }
    }

    if (inside) {
        return true;
    }

    uint32_t plane = 0;

    if (maxT[1] > maxT[0]) {
        plane = 1;
    }

    if (maxT[2] > maxT[plane]) {
        plane = 2;
    }

    if (maxT[plane] < 0.0f) {
        return false;
    }

    for (uint32_t i = 0; i < 3; i++) {
        if (i == plane) {
            continue;
        }

        float coord = dir[i] * maxT[plane] + s[i];

        if (bmin[i] - 0.0000099999997f > coord || coord > bmax[i] + 0.0000099999997f) {
            return false;
        }
    }

    return true;
}

// OFFSET: 0x00B13410
void CWorldMath::ClipVertexListToPlane(const C3Vector* verts, uint32_t count, C3Vector* out, uint32_t& outCount, const C4Plane& plane, float epsilon, int32_t keepCoplanar) {
    uint32_t counts[3];
    counts[0] = 0;
    counts[1] = 0;
    counts[2] = 0;

    for (uint32_t i = 0; i < count; i++) {
        float dist = plane.n.z * verts[i].z + verts[i].x * plane.n.x + verts[i].y * plane.n.y + plane.d;
        s_clipDists[i] = dist;

        if (dist > epsilon) {
            s_clipSides[i] = 1;
        } else if (-epsilon <= dist) {
            s_clipSides[i] = 0;
        } else {
            s_clipSides[i] = 2;
        }

        counts[s_clipSides[i]]++;
    }

    s_clipSides[count] = s_clipSides[0];
    s_clipDists[count] = s_clipDists[0];

    if (!counts[1]) {
        if (!counts[2] && keepCoplanar) {
            memcpy(out, verts, sizeof(C3Vector) * count);
            outCount = count;
        } else {
            outCount = 0;
        }

        return;
    }

    if (!counts[2]) {
        memcpy(out, verts, sizeof(C3Vector) * count);
        outCount = count;
        return;
    }

    outCount = 0;

    for (uint32_t i = 0; i < count; i++) {
        uint32_t side = s_clipSides[i];

        if (side == 0) {
            out[outCount++] = verts[i];
            continue;
        }

        if (side == 1) {
            out[outCount++] = verts[i];
        }

        uint32_t next = s_clipSides[i + 1];

        if (next && next != s_clipSides[i]) {
            float t = s_clipDists[i] / (s_clipDists[i] - s_clipDists[i + 1]);
            const C3Vector& a = verts[i];
            const C3Vector& b = verts[(i + 1) % count];
            C3Vector& v = out[outCount];
            v.x = (b.x - a.x) * t + a.x;
            v.y = (b.y - a.y) * t + a.y;
            v.z = (b.z - a.z) * t + a.z;
            outCount++;
        }
    }
}
