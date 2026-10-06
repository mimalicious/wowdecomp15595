#include <cstdint>
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CFrustum.h"

// OFFSET: %0x004D1E90
void CFrustum::Extents(CAaBox& box) {
    box = CAaBox::Bounding(this->corners, 8);
}

// OFFSET: %0x004D1EE0
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

// OFFSET: %0x004D2130
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

// OFFSET: #0x00983AE0
void CFrustum::Translate(const C3Vector& v) {
    for (uint32_t i = 0; i < 8; i++) {
        this->corners[i] += v;
    }

    for (uint32_t i = 0; i < 6; i++) {
        this->planes[i].d = this->planes[i].d - (this->planes[i].n.z * v.z + this->planes[i].n.y * v.y + v.x * this->planes[i].n.x);
    }

    this->lookPos += v;
    this->lookAt += v;
}

// OFFSET: %0x004D2570
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

// OFFSET: %0x004D25F0
void CFrustum::Cull(const C3Vector& point, uint8_t& outside) const {
    outside = 0;

    for (uint32_t i = 0; i < 6; i++) {
        const C4Plane& plane = this->planes[i];

        if (plane.n.z * point.z + plane.n.y * point.y + point.x * plane.n.x + plane.d < -0.019444443f) {
            outside |= 1 << i;
        }
    }
}

// OFFSET: %0x004D2860
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

// OFFSET: %0x004D2960
void CFrustum::Transform(const C44Matrix& m) {
    for (uint32_t i = 0; i < 8; i++) {
        this->corners[i] *= m;
    }

    this->CalcPlanesFromCorners();

    this->lookPos *= m;
    this->lookAt *= m;
}

// OFFSET: #0x00983FB0
uint32_t CFrustum::Cull(const C3Vector& center, float radius) {
    CAaSphere sphere;
    sphere.c = center;
    sphere.r = radius;
    return this->Cull(sphere);
}

// OFFSET: %0x004412F0
CFrustum::CFrustum() {
}

// OFFSET: %0x004D2F20
CFrustum::CFrustum(const C3Vector* corners) {
    for (uint32_t i = 0; i < 8; i++) {
        this->corners[i] = corners[i];
    }

    this->CalcPlanesFromCorners();
}

// OFFSET: %0x004D31E0
void CFrustum::CalcPlanesFromCorners(const C3Vector* corners) {
    for (uint32_t i = 0; i < 8; i++) {
        this->corners[i] = corners[i];
    }

    this->CalcPlanesFromCorners();
}

// OFFSET: %0x0043F540
CFrustum& CFrustum::operator=(const CFrustum& source) {
    if (this != &source) {
        for (uint32_t i = 0; i < 6; i++) {
            this->planes[i] = source.planes[i];
        }

        for (uint32_t i = 0; i < 8; i++) {
            this->corners[i] = source.corners[i];
        }

        this->lookPos = source.lookPos;
        this->lookAt = source.lookAt;
        this->lookUp = source.lookUp;
        this->fovy = source.fovy;
        this->aspect = source.aspect;
        this->minz = source.minz;
        this->maxz = source.maxz;
    }

    return *this;
}

// OFFSET: %0x0043F8D0
void CFrustum::InvertPlanes() {
    for (uint32_t i = 0; i < 6; i++) {
        this->planes[i].n = -this->planes[i].n;
        this->planes[i].d = -this->planes[i].d;
    }
}

// OFFSET: %0x006C9710
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
