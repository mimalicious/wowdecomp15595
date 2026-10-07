#include "WoW/Source/WorldClient/WorldOcclusion.h"
#include <cstddef>
#include <cmath>
#include <malloc.h>
#include <new>
#include "Engine/Source/Base/SIMD.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "WoW/Source/WorldClient/CWorldMath.h"

// STATIC: 0x00D02F20
static TSExplicitList<CWorldOccluder, offsetof(CWorldOccluder, m_link)> s_freeOccluders;

// STATIC: 0x00D02F2C
static TSExplicitList<CWorldAntiOccluder, offsetof(CWorldAntiOccluder, m_link)> s_freeAntiOccluders;

static uint32_t FloatBits(float value) {
    return *reinterpret_cast<uint32_t*>(&value);
}

// PROVEN: f52cc1f6 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CWorldOccluder::Initialize() {
}

// PROVEN: 7665bf77 IDENTICAL/NONE
// OFFSET: 0x007553F0/0x140436830
CWorldOcclusion::~CWorldOcclusion() {
}

// OFFSET: 0x00756FD0/0x140438930
CWorldOcclusion::CWorldOcclusion() {
}

// OFFSET: 0x00780F20/0x14046E2F0
void CWorldOccluder::Free() {
    this->m_link.Unlink();
    s_freeOccluders.LinkToTail(this);
}

// OFFSET: 0x00780F70/0x14046E3C0
CWorldOccluder* CWorldOccluder::Alloc() {
    CWorldOccluder* occluder = s_freeOccluders.Head();

    if (!occluder) {
        void* mem = SMemAlloc(sizeof(CWorldOccluder), __FILE__, __LINE__, 0x8);
        occluder = mem ? new (mem) CWorldOccluder() : NULL;
        s_freeOccluders.LinkToTail(occluder);
    }

    occluder->m_link.Unlink();
    return occluder;
}

// OFFSET: 0x00781150
void CWorldOccluder::Destroy() {
    CWorldOccluder* occluder;

    while ((occluder = s_freeOccluders.Head()) != NULL) {
        occluder->m_link.Unlink();
        SMemFree(occluder, __FILE__, __LINE__, 0);
    }

    CWorldAntiOccluder* antiOccluder;

    while ((antiOccluder = s_freeAntiOccluders.Head()) != NULL) {
        antiOccluder->m_link.Unlink();
        SMemFree(antiOccluder, __FILE__, __LINE__, 0);
    }
}

// OFFSET: 0x0079AA50/0x14048F0D0
int32_t CWorldOcclusion::QueryBuffer(const CAaBox& box, uint32_t flags) {
    if (!this->m_horizon) {
        return 0;
    }

    float minX;
    float maxX;
    float maxY;

    if (!SIMD::s_functions.ProjectBoxTop(&box, &this->m_viewProj, 50.0f, &minX, &maxX, &maxY)) {
        return 0;
    }

    int32_t lo = 128 - static_cast<int32_t>(minX * -64.0f);
    int32_t hi = 128 - static_cast<int32_t>(maxX * -64.0f);

    if (lo >= 256 || hi < 0) {
        return 0;
    }

    if (lo < 0) {
        lo = 0;
    }

    if (hi >= 256) {
        hi = 255;
    }

    for (int32_t i = lo; i <= hi; i++) {
        if (!(maxY < this->m_buffer[0][i])) {
            return 0;
        }
    }

    return 1;
}

// PROVEN: bf769bea IDENTICAL/NONE
// OFFSET: 0x0079AB10
void CWorldOcclusion::Update(const C44Matrix& viewProj, const C44Matrix& proj, const C3Vector& camDir, int32_t horizon) {
    this->m_viewProj = viewProj;
    this->m_proj = proj;
    this->m_camDir = camDir;
    this->m_horizon = horizon;

    if (this->m_camDir.z < -0.89999998f || this->m_camDir.z > 0.89999998f) {
        this->m_horizon = 0;
    }
}

// OFFSET: 0x0079AB90/0x14048F320
void CWorldOcclusion::UpdateBuffer(const C3Vector* segment, uint32_t flags) {
    if (!this->m_horizon) {
        return;
    }

    C3Vector p0 = segment[0] * this->m_viewProj;
    C3Vector p1 = segment[1] * this->m_viewProj;
    float x0 = p0.x;
    float y0 = p0.y;
    float z0 = p0.z;
    float x1 = p1.x;
    float y1 = p1.y;
    float z1 = p1.z;

    if (z0 >= 0.000099999997f) {
        if (z1 < 0.000099999997f) {
            float t = (z0 - 0.000099999997f) / ((z0 - 0.000099999997f) - (z1 - 0.000099999997f));
            x1 = (x1 - x0) * t + x0;
            y1 = (y1 - y0) * t + y0;
            z1 = (z1 - z0) * t + z0;
        }
    } else {
        if (z1 < 0.000099999997f) {
            return;
        }

        float t = (z1 - 0.000099999997f) / ((z1 - 0.000099999997f) - (z0 - 0.000099999997f));
        y0 = (y0 - y1) * t + y1;
        x0 = (x0 - x1) * t + x1;
        z0 = (z0 - z1) * t + z1;
    }

    float sx1 = (1.0f / z1) * x1;
    uint32_t buffer = 0;
    float sy0 = (1.0f / z0) * y0;
    float fill = -99999.0f;
    float sy1 = (1.0f / z1) * y1;

    if (flags & 0x2) {
        fill = 99999.0f;
        buffer = 1;
    }

    uint8_t mark = (flags & 0x1) != 0;
    float left = ((1.0f / z0) * x0) * 64.0f + 128.0f;
    float right = sx1 * 64.0f + 128.0f;

    if (left > right) {
        float swap = left;
        left = right;
        right = swap;
    }

    if (left < 0.0f) {
        left = 0.0f;
    }

    if (right >= 256.0f) {
        right = 255.0f;
    }

    int32_t lo = static_cast<int32_t>(left);
    int32_t hi = static_cast<int32_t>(right);

    if (flags & 0x4) {
        for (int32_t i = lo; i <= hi; i++) {
            if (!(this->m_bufferFlags[i] & 0x1)) {
                this->m_buffer[buffer][i] = fill;
            }
        }
    } else if (buffer) {
        float y = sy0 <= sy1 ? sy1 : sy0;

        for (int32_t i = lo; i <= hi; i++) {
            if (this->m_buffer[1][i] > y) {
                this->m_buffer[1][i] = y;
                this->m_bufferFlags[i] |= mark;
            }
        }
    } else {
        float y = sy1 <= sy0 ? sy1 : sy0;

        for (int32_t i = lo; i <= hi; i++) {
            if (y > this->m_buffer[0][i]) {
                this->m_buffer[0][i] = y;
                this->m_bufferFlags[i] |= mark;
            }
        }
    }
}

// OFFSET: 0x0079AF70/0x14048F760
int32_t CWorldOcclusion::QueryBuffer(const CAaSphere& sphere, uint32_t flags) {
    if (!this->m_horizon) {
        return 0;
    }

    if (fabs(sphere.r) < 0.00000023841858f) {
        return 0;
    }

    C3Vector center = sphere.c * this->m_viewProj;
    float r = sphere.r;
    float extentX = this->m_proj.b0 * r + this->m_proj.a0 * r + this->m_proj.c0 * 0.0f + this->m_proj.d0;
    float extentY = this->m_proj.b1 * r + this->m_proj.a1 * r + this->m_proj.c1 * 0.0f + this->m_proj.d1;

    if (!(flags & 0x10) && center.z < 50.0f) {
        return 0;
    }

    float inv = 1.0f / center.z;
    float ry = extentY * inv;
    float sx = center.x * inv;
    float rx = extentX * inv;
    float sy = center.y * inv;
    float top = rx + sy;
    float bottom = sy - rx;
    int32_t lo = 128 - static_cast<int32_t>((sx - ry) * -64.0f);
    int32_t hi = 128 - static_cast<int32_t>((ry + sx) * -64.0f);

    if (lo >= 256 || hi < 0) {
        return 0;
    }

    if (lo < 0) {
        lo = 0;
    }

    if (hi >= 256) {
        hi = 255;
    }

    for (int32_t i = lo; i <= hi; i++) {
        if (!(top < this->m_buffer[0][i] || this->m_buffer[1][i] < bottom)) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x0079B1E0/0x14048F9E0
int32_t CWorldOcclusion::QueryVolumes(const CAaSphere& sphere) {
    for (uint32_t v = 0; v < this->m_volumes.Count(); v++) {
        const CWorldOcclusionVolume& volume = this->m_volumes.m_data[v];
        const C4Plane* plane = &this->m_planes.m_data[volume.m_firstPlane];
        uint32_t i = 0;

        for (; i < volume.m_planeCount; i++, plane++) {
            if (plane->n.y * sphere.c.y + plane->n.z * sphere.c.z + sphere.c.x * plane->n.x + plane->d + sphere.r > 0.0f) {
                break;
            }
        }

        if (i == volume.m_planeCount) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x0079B3E0/0x14048FC00
int32_t CWorldOcclusion::QueryVolumes(const CAaBox& box) {
    const C3Vector* corners[2];
    corners[0] = &box.t;
    corners[1] = &box.b;

    for (uint32_t v = 0; v < this->m_volumes.Count(); v++) {
        const CWorldOcclusionVolume& volume = this->m_volumes.m_data[v];
        const C4Plane* plane = &this->m_planes.m_data[volume.m_firstPlane];
        uint32_t i = 0;

        for (; i < volume.m_planeCount; i++, plane++) {
            float x = corners[FloatBits(plane->n.x) >> 31]->x;
            float y = corners[FloatBits(plane->n.y) >> 31]->y;
            float z = corners[FloatBits(plane->n.z) >> 31]->z;

            if (z * plane->n.z + y * plane->n.y + x * plane->n.x + plane->d > 0.0f) {
                break;
            }
        }

        if (i == volume.m_planeCount) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x0079B640/0x14048FE90
int32_t CWorldOcclusion::QueryVolumes(const C3Vector* verts, uint32_t count) {
    for (uint32_t v = 0; v < this->m_volumes.Count(); v++) {
        const CWorldOcclusionVolume& volume = this->m_volumes.m_data[v];
        uint32_t i = 0;

        for (; i < volume.m_planeCount; i++) {
            const C4Plane& plane = this->m_planes.m_data[volume.m_firstPlane + i];
            uint32_t j = 0;

            for (; j < count; j++) {
                if (verts[j].x * plane.n.x + verts[j].z * plane.n.z + verts[j].y * plane.n.y + plane.d > 0.0f) {
                    break;
                }
            }

            if (j < count) {
                break;
            }
        }

        if (i == volume.m_planeCount) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x0079BD80/0x1404907A0
void CWorldOcclusion::ClearVolumes() {
    this->m_planes.SetCount(0);
    this->m_planes.m_chunk = 32;
    this->m_volumes.SetCount(0);
    this->m_volumes.m_chunk = 8;
}

// OFFSET: 0x0079BDC0/0x1404907E0
void CWorldOcclusion::AddVolume(const C3Vector& pos, const C3Vector* verts, int32_t count, int32_t clip, float extend) {
    static C3Vector s_unused[4];

    uint32_t firstPlane = this->m_planes.Count();
    uint32_t planeCount = 0;

    if (!clip || extend <= 0.000001f) {
        if (count <= 0) {
            return;
        }

        for (int32_t i = 0; i < count; i++) {
            const C3Vector& next = verts[(i + 1) % count];
            C4Plane* plane = this->m_planes.New();
            plane->From3Pos(verts[i], next, pos);
        }

        planeCount = count;
    } else {
        C4Plane farPlane;
        farPlane.n = this->m_camDir;
        farPlane.d = -(this->m_camDir.y * (pos.y + this->m_camDir.y * extend) + this->m_camDir.z * (pos.z + this->m_camDir.z * extend) + this->m_camDir.x * (pos.x + extend * this->m_camDir.x));
        C4Plane plane;
        uint32_t clippedCount = 0;
        C3Vector* clipped = static_cast<C3Vector*>(alloca(2 * sizeof(C3Vector) * count));
        CWorldMath::ClipVertexListToPlane(verts, count, clipped, clippedCount, farPlane, 0.000099999997f, 1);

        if (clippedCount <= 2) {
            return;
        }

        for (uint32_t i = 0; i < clippedCount; i++) {
            const C3Vector& cur = clipped[i];
            const C3Vector& next = clipped[(i + 1) % clippedCount];
            float ey = pos.y - cur.y;
            float ex = pos.x - cur.x;
            float ez = pos.z - cur.z;
            float fx = next.x - cur.x;
            float fy = next.y - cur.y;
            float fz = next.z - cur.z;
            float nx = fy * ez - fz * ey;
            float ny = fz * ex - ez * fx;
            float nz = ey * fx - fy * ex;
            plane.n.x = nx;
            plane.n.y = ny;
            plane.n.z = nz;
            float lengthSq = nz * nz + ny * ny + nx * nx;

            if (lengthSq > 0.000099999997f) {
                float inv = 1.0f / sqrt(lengthSq);
                plane.n.z = plane.n.z * inv;
                plane.n.y = plane.n.y * inv;
                plane.n.x = nx * inv;
                plane.d = -(cur.z * plane.n.z + cur.y * plane.n.y + cur.x * plane.n.x);
                this->m_planes.Add(1, &plane);
                planeCount++;
            }
        }
    }

    if (!planeCount) {
        return;
    }

    C4Plane* cap = this->m_planes.New();
    cap->From3Pos(verts[0], verts[1], verts[2]);
    planeCount++;

    if (cap->n.z * pos.z + cap->n.y * pos.y + cap->n.x * pos.x + cap->d < 0.0f) {
        for (uint32_t i = 0; i < planeCount; i++) {
            C4Plane& flip = this->m_planes.m_data[firstPlane + i];
            flip.d = -flip.d;
            flip.n.x = -flip.n.x;
            flip.n.y = -flip.n.y;
            flip.n.z = -flip.n.z;
        }
    }

    CWorldOcclusionVolume volume;
    volume.m_firstPlane = firstPlane;
    volume.m_planeCount = planeCount;
    this->m_volumes.Add(1, &volume);
}

// OFFSET: 0x0079C410
void CWorldOcclusion::Clear() {
    for (uint32_t i = 0; i < 256; i++) {
        this->m_buffer[0][i] = -99999.0f;
        this->m_buffer[1][i] = 99999.0f;
        this->m_bufferFlags[i] = 0;
    }

    this->m_planes.SetCount(0);
    this->m_planes.m_chunk = 32;
    this->m_volumes.SetCount(0);
    this->m_volumes.m_chunk = 8;
}
