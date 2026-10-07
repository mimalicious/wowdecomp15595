#include "WoW/Source/WorldClient/CWorldMath.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"

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
