#include <cstdint>
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CFrustum.h"

// PROVEN: 8a404508 IDENTICAL/NONE
// OFFSET: 0x004CBEA0
void CFrustum::Extents(CAaBox& box) {
    box = CAaBox::Bounding(this->corners, 8);
}

// PROVEN: 194f6653 IDENTICAL/DIFFERENT
// OFFSET: 0x004CBEF0/0x140107C60
uint32_t CFrustum::Cull(const CAaBox& box) {
    const C3Vector* extremes[2] = { &box.t, &box.b };

    for (int32_t i = 0; i < 6; i++) {
        const C4Plane& plane = this->planes[i];
        const C3Vector* x = extremes[*reinterpret_cast<const uint32_t*>(&plane.n.x) >> 31];
        const C3Vector* y = extremes[*reinterpret_cast<const uint32_t*>(&plane.n.y) >> 31];
        const C3Vector* z = extremes[*reinterpret_cast<const uint32_t*>(&plane.n.z) >> 31];

        if ((z->z * plane.n.z + y->y * plane.n.y) + x->x * plane.n.x + plane.d < -0.019444443f) {
            return 0;
        }
    }

    return 3;
}

// OFFSET: 0x004CBF80
uint32_t CFrustum::Cull(const CAaBox& box, uint32_t mask) {
    const C3Vector* extremes[2] = { &box.t, &box.b };
    uint32_t bit = 1;

    for (int32_t i = 0; i < 6; i++) {
        if (mask & bit) {
            const C4Plane& plane = this->planes[i];
            const C3Vector* x = extremes[*reinterpret_cast<const uint32_t*>(&plane.n.x) >> 31];
            const C3Vector* z = extremes[*reinterpret_cast<const uint32_t*>(&plane.n.z) >> 31];
            const C3Vector* y = extremes[*reinterpret_cast<const uint32_t*>(&plane.n.y) >> 31];

            if (x->x * plane.n.x + z->z * plane.n.z + y->y * plane.n.y + plane.d < -0.019444443f) {
                return 0;
            }
        }

        bit += bit;
    }

    return 3;
}

// OFFSET: 0x004CC030
uint32_t CFrustum::Cull(const CAaBox& box, uint32_t* outside) {
    const C3Vector* extremes[2] = { &box.t, &box.b };
    *outside = 0;
    uint32_t bit = 1;

    for (int32_t i = 0; i < 6; i++) {
        const C4Plane& plane = this->planes[i];
        uint32_t sx = *reinterpret_cast<const uint32_t*>(&plane.n.x) >> 31;
        uint32_t sz = *reinterpret_cast<const uint32_t*>(&plane.n.z) >> 31;
        uint32_t sy = *reinterpret_cast<const uint32_t*>(&plane.n.y) >> 31;

        if (extremes[sx]->x * plane.n.x + extremes[sz]->z * plane.n.z + extremes[sy]->y * plane.n.y + plane.d < -0.019444443f) {
            return 0;
        }

        if (extremes[(sx - 1) & 1]->x * plane.n.x + extremes[(sz - 1) & 1]->z * plane.n.z + extremes[(sy - 1) & 1]->y * plane.n.y + plane.d > 0.019444443f) {
            *outside |= bit;
        }

        bit <<= 1;
    }

    return 3;
}

// OFFSET: 0x004CC140
uint32_t CFrustum::CullInv(const CAaBox& box) {
    const C3Vector* extremes[2] = { &box.t, &box.b };

    for (uint32_t i = 0; i < 6; i++) {
        const C4Plane& plane = this->planes[i];
        const C3Vector* x = extremes[*reinterpret_cast<const uint32_t*>(&plane.n.x) >> 31];
        const C3Vector* y = extremes[*reinterpret_cast<const uint32_t*>(&plane.n.y) >> 31];
        const C3Vector* z = extremes[*reinterpret_cast<const uint32_t*>(&plane.n.z) >> 31];

        if (z->z * plane.n.z + y->y * plane.n.y + x->x * plane.n.x + plane.d > 0.019444443f) {
            return 0;
        }
    }

    return 3;
}

// OFFSET: 0x004CC200
void CFrustum::Translate(const C3Vector& v) {
    for (uint32_t i = 0; i < 8; i++) {
        this->corners[i] += v;
    }

    for (uint32_t i = 0; i < 6; i++) {
        this->planes[i].d = this->planes[i].d - (this->planes[i].n.z * v.z + this->planes[i].n.y * v.y + v.x * this->planes[i].n.x);
    }
}

// OFFSET: 0x004CC500
uint32_t CFrustum::Cull(const CAaSphere& sphere) {
    float limit = -sphere.r;

    for (uint32_t i = 0; i < 6; i++) {
        const C4Plane& plane = this->planes[i];

        if (plane.n.x * sphere.c.x + plane.n.z * sphere.c.z + plane.n.y * sphere.c.y + plane.d < limit) {
            return 0;
        }
    }

    return 3;
}

// PROVEN: 9f10b196 IDENTICAL/IDENTICAL
// OFFSET: 0x004CC580/0x1401082C0
void CFrustum::Cull(const C3Vector& point, uint8_t& outside) const {
    outside = 0;

    for (uint32_t i = 0; i < 6; i++) {
        const C4Plane& plane = this->planes[i];

        if (plane.n.z * point.z + plane.n.y * point.y + point.x * plane.n.x + plane.d < -0.019444443f) {
            outside |= 1 << i;
        }
    }
}

// OFFSET: 0x004CC7F0/0x140108530
void CFrustum::CalcPlanesFromCorners() {
    this->planes[0].From3Pos(this->corners[1], this->corners[5], this->corners[6]);
    this->planes[1].From3Pos(this->corners[0], this->corners[7], this->corners[4]);
    this->planes[2].From3Pos(this->corners[0], this->corners[4], this->corners[5]);
    this->planes[3].From3Pos(this->corners[3], this->corners[6], this->corners[7]);
    this->planes[4].From3Pos(this->corners[5], this->corners[4], this->corners[6]);

    C3Vector n = -this->planes[4].n;
    this->planes[5].n = n;
    this->planes[5].d = -((this->corners[2].z * n.z + this->corners[2].y * n.y) + this->corners[2].x * n.x);
}

// OFFSET: 0x004CC8F0/0x140108670
void CFrustum::Transform(const C44Matrix& m) {
    for (uint32_t i = 0; i < 8; i++) {
        this->corners[i] *= m;
    }

    this->CalcPlanesFromCorners();
}

// OFFSET: #0x00983FB0
uint32_t CFrustum::Cull(const C3Vector& center, float radius) {
    CAaSphere sphere;
    sphere.c = center;
    sphere.r = radius;
    return this->Cull(sphere);
}

// PROVEN: 4b4e948b IDENTICAL/IDENTICAL
// OFFSET: 0x00437100/0x140040D00
CFrustum::CFrustum() {
}

// OFFSET: 0x004CCD90/0x140108B00
CFrustum::CFrustum(const C3Vector* corners) {
    for (uint32_t i = 0; i < 8; i++) {
        this->corners[i] = corners[i];
    }

    this->CalcPlanesFromCorners();
}

// OFFSET: 0x004CD010/0x140108D80
void CFrustum::CalcPlanesFromCorners(const C3Vector* corners) {
    for (uint32_t i = 0; i < 8; i++) {
        this->corners[i] = corners[i];
    }

    this->CalcPlanesFromCorners();
}

// PROVEN: 299ec294 IDENTICAL/IDENTICAL
// OFFSET: 0x00435110/0x14003E850
CFrustum& CFrustum::operator=(const CFrustum& source) {
    if (this != &source) {
        for (uint32_t i = 0; i < 6; i++) {
            this->planes[i] = source.planes[i];
        }

        for (uint32_t i = 0; i < 8; i++) {
            this->corners[i] = source.corners[i];
        }
    }

    return *this;
}

// PROVEN: 2693512a IDENTICAL/IDENTICAL
// OFFSET: 0x004354F0/0x14003EC40
void CFrustum::InvertPlanes() {
    for (uint32_t i = 0; i < 6; i++) {
        this->planes[i].n = -this->planes[i].n;
        this->planes[i].d = -this->planes[i].d;
    }
}

// PROVEN: 647b9f24 IDENTICAL/IDENTICAL
// OFFSET: 0x00768BE0/0x14044FEA0
void CFrustum::SwapCorners() {
    C3Vector corner = this->corners[0];
    this->corners[0] = this->corners[3];
    this->corners[3] = corner;

    corner = this->corners[1];
    this->corners[1] = this->corners[2];
    this->corners[2] = corner;

    corner = this->corners[4];
    this->corners[4] = this->corners[7];
    this->corners[7] = corner;

    corner = this->corners[5];
    this->corners[5] = this->corners[6];
    this->corners[6] = corner;
}
