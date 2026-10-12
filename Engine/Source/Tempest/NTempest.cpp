#include <cfloat>
#include <cmath>
#include <cstdint>
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Ray.h"
#include "Engine/Source/Tempest/C3Segment.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/NTempest.h"

// STATIC: 0x00D27A68
static uint32_t s_planeAxes[3][2] = { { 1, 2 }, { 2, 0 }, { 0, 1 } };

// INLINED
static float Noise2D(int32_t x, int32_t y) {
    uint32_t n = static_cast<uint32_t>(x + y * 57);
    n = (n << 13) ^ n;

    return 1.0f - static_cast<int32_t>((n * (n * n * 15731 + 789221) - 771171069) & 0x7FFFFFFF) * 9.3132257e-10f;
}

// OFFSET: 0x00B085A0
static float SmoothNoise2D(int32_t x, int32_t y) {
    float sides = (Noise2D(x, y + 1) + Noise2D(x, y - 1) + Noise2D(x + 1, y) + Noise2D(x - 1, y)) * 0.125f;
    float corners = (Noise2D(x + 1, y + 1) + Noise2D(x - 1, y + 1) + Noise2D(x + 1, y - 1) + Noise2D(x - 1, y - 1)) * 0.0625f;
    float center = Noise2D(x, y) * 0.25f;

    return sides + corners + center;
}

// INLINED
static float InterpolatedNoise2D(float x, float y) {
    float floorX = floor(x);
    float fractionX = x - floorX;
    float floorY = floor(y);

    int32_t ix = static_cast<int32_t>(floorX);
    int32_t iy = static_cast<int32_t>(floorY);

    float v00 = SmoothNoise2D(ix, iy);
    float v01 = SmoothNoise2D(ix, iy + 1);
    float v10 = SmoothNoise2D(ix + 1, iy);
    float bottom = v00 + (v10 - v00) * fractionX;
    float v11 = SmoothNoise2D(ix + 1, iy + 1);
    float top = v01 + (v11 - v01) * fractionX;

    return (top - bottom) * (y - floorY) + bottom;
}

// OFFSET: 0x00777050
float NTempest::Distance(const CAaBox& box, const C3Vector& point) {
    float x = box.b.x <= point.x ? (box.t.x >= point.x ? point.x : box.t.x) : box.b.x;
    float y = box.b.y <= point.y ? (box.t.y >= point.y ? point.y : box.t.y) : box.b.y;
    float z = box.b.z <= point.z ? (box.t.z >= point.z ? point.z : box.t.z) : box.b.z;
    float dy = y - point.y;
    return sqrt(dy * dy + (x - point.x) * (x - point.x) + (z - point.z) * (z - point.z));
}

// OFFSET: 0x00B08BA0/0x1408CC200
bool NTempest::Intersect2dSegments(const C2Vector& a0, const C2Vector& a1, const C2Vector& b0, const C2Vector& b1) {
    float ax = a1.x - a0.x;
    float ay = a1.y - a0.y;
    float bx = b0.x - b1.x;
    float by = b0.y - b1.y;
    float cx = a0.x - b0.x;
    float cy = a0.y - b0.y;

    float denom = bx * ay - by * ax;
    float num1 = cx * by - cy * bx;

    if (!((denom > 0.0f && num1 >= 0.0f && denom >= num1) || (denom < 0.0f && num1 <= 0.0f && num1 >= denom))) {
        return false;
    }

    float num2 = cy * ax - cx * ay;

    if (denom > 0.0f) {
        if (num2 >= 0.0f && denom >= num2) {
            return true;
        }
    } else if (num2 <= 0.0f && num2 >= denom) {
        return true;
    }

    return false;
}

// OFFSET: 0x00B08C70/0x1408CC2F0
bool NTempest::HitBoundingBox(const float* origin, const float* dir, const float* minB, const float* maxB, float* t, float* coord) {
    bool inside = true;
    uint8_t quadrant[3];
    float candidatePlane[3];
    float maxT[3];

    for (int32_t i = 0; i < 3; i++) {
        if (origin[i] < minB[i]) {
            quadrant[i] = 1;
            candidatePlane[i] = minB[i];
            inside = false;
        } else if (origin[i] > maxB[i]) {
            quadrant[i] = 0;
            candidatePlane[i] = maxB[i];
            inside = false;
        } else {
            quadrant[i] = 2;
        }
    }

    if (inside) {
        *t = 0.0f;
        coord[0] = origin[0];
        coord[1] = origin[1];
        coord[2] = origin[2];
        return true;
    }

    for (int32_t i = 0; i < 3; i++) {
        if (quadrant[i] != 2 && dir[i] != 0.0f) {
            maxT[i] = (candidatePlane[i] - origin[i]) / dir[i];
        } else {
            maxT[i] = -1.0f;
        }
    }

    int32_t whichPlane = 0;

    for (int32_t i = 1; i < 3; i++) {
        if (maxT[whichPlane] < maxT[i]) {
            whichPlane = i;
        }
    }

    if (maxT[whichPlane] < 0.0f) {
        return false;
    }

    *t = maxT[whichPlane];

    for (int32_t i = 0; i < 3; i++) {
        if (whichPlane != i) {
            coord[i] = origin[i] + maxT[whichPlane] * dir[i];

            if (coord[i] < minB[i] || coord[i] > maxB[i]) {
                return false;
            }
        } else {
            coord[i] = candidatePlane[i];
        }
    }

    return true;
}

// OFFSET: 0x00B08E30/0x1408CC4D0
bool NTempest::Intersect2dBoxSphere(const float* minB, const float* maxB, const float* center, float radius, SolidIntersect mode) {
    float r2 = radius * radius;
    float dmin;
    float dmax;
    bool face;

    switch (mode) {
        case SOLID_NONE:
            dmin = 0.0f;
            dmax = 0.0f;
            face = false;

            for (uint32_t i = 0; i < 2; i++) {
                float a = (center[i] - minB[i]) * (center[i] - minB[i]);
                float b = (center[i] - maxB[i]) * (center[i] - maxB[i]);
                dmax += a > b ? a : b;

                if (center[i] < minB[i]) {
                    face = true;
                    dmin += a;
                } else if (center[i] > maxB[i]) {
                    face = true;
                    dmin += b;
                } else if ((a < b ? a : b) <= r2) {
                    face = true;
                }
            }

            return face && dmin <= r2 && r2 <= dmax;

        case SOLID_SPHERE:
            dmin = 0.0f;
            face = false;

            for (uint32_t i = 0; i < 2; i++) {
                if (center[i] < minB[i]) {
                    face = true;
                    dmin += (center[i] - minB[i]) * (center[i] - minB[i]);
                } else if (center[i] > maxB[i]) {
                    face = true;
                    dmin += (center[i] - maxB[i]) * (center[i] - maxB[i]);
                } else if (center[i] - minB[i] <= radius) {
                    face = true;
                } else if (maxB[i] - center[i] <= radius) {
                    face = true;
                }
            }

            return face && dmin <= r2;

        case SOLID_BOX:
            dmin = 0.0f;
            dmax = 0.0f;

            for (uint32_t i = 0; i < 2; i++) {
                float a = (center[i] - minB[i]) * (center[i] - minB[i]);
                float b = (center[i] - maxB[i]) * (center[i] - maxB[i]);
                dmax += a > b ? a : b;

                if (center[i] < minB[i]) {
                    dmin += a;
                } else if (center[i] > maxB[i]) {
                    dmin += b;
                }
            }

            return dmin <= r2 && r2 <= dmax;

        case SOLID_BOTH:
            dmin = 0.0f;

            for (uint32_t i = 0; i < 2; i++) {
                if (center[i] < minB[i]) {
                    dmin += (center[i] - minB[i]) * (center[i] - minB[i]);
                } else if (center[i] > maxB[i]) {
                    dmin += (center[i] - maxB[i]) * (center[i] - maxB[i]);
                }
            }

            return dmin <= r2;
    }

    return false;
}

// OFFSET: 0x00B09130/0x1408CC7C0
bool NTempest::Intersect(const C3Ray& ray, const CAaBox& box, float* t, C3Vector* hit) {
    float tStorage;
    C3Vector hitStorage;

    if (!t) {
        t = &tStorage;
    }

    if (!hit) {
        hit = &hitStorage;
    }

    return HitBoundingBox(&ray.origin.x, &ray.dir.x, &box.b.x, &box.t.x, t, &hit->x);
}

// OFFSET: 0x00B09180/0x1408CC820
bool NTempest::Intersect2d(const CAaBox& box, const CAaSphere& sphere, SolidIntersect mode) {
    return Intersect2dBoxSphere(&box.b.x, &box.t.x, &sphere.c.x, sphere.r, mode);
}

// OFFSET: 0x00B091B0/0x1408CC850
bool NTempest::Intersect(const C3Ray& ray, const C4Plane& plane, float* t, C3Vector* hit, float tolerance) {
    float denom = ray.dir.y * plane.n.y + ray.dir.z * plane.n.z + plane.n.x * ray.dir.x;

    if (fabs(denom) < 0.0001) {
        if (fabs(ray.origin.y * plane.n.y + ray.origin.z * plane.n.z + ray.origin.x * plane.n.x + plane.d) >= tolerance) {
            return false;
        }

        if (t) {
            *t = 0.0f;
        }

        if (hit) {
            *hit = ray.origin;
        }

        return true;
    }

    if (t || hit) {
        float dist = ray.origin.x * plane.n.x + plane.n.z * ray.origin.z + plane.n.y * ray.origin.y + plane.d;
        float param = fabs(dist) >= tolerance ? -(dist / denom) : 0.0f;

        if (t) {
            *t = param;
        }

        if (hit) {
            C3Vector point;
            point.x = param * ray.dir.x + ray.origin.x;
            point.y = ray.dir.y * param + ray.origin.y;
            point.z = param * ray.dir.z + ray.origin.z;
            *hit = point;
        }
    }

    return true;
}

// OFFSET: 0x00B09320/0x1408CC990
bool NTempest::Intersect(const C3Vector& point, const C3Vector* verts, uint32_t count, C3Vector::EAxis axis) {
    uint32_t u = s_planeAxes[axis][0];
    uint32_t v = s_planeAxes[axis][1];
    const float* pt = &point.x;
    const float* vtx0 = &verts[count - 1].x;
    bool yflag0 = pt[v] <= vtx0[v];
    bool inside = false;

    for (uint32_t j = 0; j < count; j++) {
        const float* vtx1 = &verts[j].x;
        bool yflag1 = pt[v] <= vtx1[v];

        if (yflag0 != yflag1) {
            if (((vtx1[u] - pt[u]) * (vtx0[v] - vtx1[v]) <= (vtx0[u] - vtx1[u]) * (vtx1[v] - pt[v])) == yflag1) {
                inside = !inside;
            }
        }

        yflag0 = yflag1;
        vtx0 = vtx1;
    }

    return inside;
}

// OFFSET: 0x00B09680/0x1408CCD00
bool NTempest::Intersect(const C3Ray& ray, const C3Vector* verts, const uint16_t* indices, float* t, C2Vector* uv, float tolerance) {
    float minBound = -tolerance;
    float maxBound = tolerance + 1.0f;

    const C3Vector& v0 = verts[indices[0]];
    const C3Vector& v1 = verts[indices[1]];
    const C3Vector& v2 = verts[indices[2]];

    float e1x = v1.x - v0.x;
    float e1y = v1.y - v0.y;
    float e1z = v1.z - v0.z;
    float e2x = v2.x - v0.x;
    float e2y = v2.y - v0.y;
    float e2z = v2.z - v0.z;

    float px = ray.dir.y * e2z - ray.dir.z * e2y;
    float py = ray.dir.z * e2x - e2z * ray.dir.x;
    float pz = e2y * ray.dir.x - e2x * ray.dir.y;
    float det = e1z * pz + py * e1y + px * e1x;

    if (det > -0.000001 && det < 0.000001) {
        return false;
    }

    float inv = 1.0f / det;
    float sx = ray.origin.x - v0.x;
    float sy = ray.origin.y - v0.y;
    float sz = ray.origin.z - v0.z;

    float u = (py * sy + pz * sz + sx * px) * inv;

    if (minBound > u || u > maxBound) {
        return false;
    }

    float qx = e1z * sy - sz * e1y;
    float qy = sz * e1x - e1z * sx;
    float qz = e1y * sx - e1x * sy;

    float v = (ray.dir.z * qz + ray.dir.y * qy + ray.dir.x * qx) * inv;

    if (minBound > v || v + u > maxBound) {
        return false;
    }

    if (t) {
        *t = inv * (qz * e2z + qy * e2y + qx * e2x);
    }

    if (uv) {
        uv->x = u;
        uv->y = v;
    }

    return true;
}

// OFFSET: 0x00B098E0/0x1408CCFC0
bool NTempest::Intersect(const C3Ray& ray, const C3Vector* verts, const uint32_t* indices, float* t, C2Vector* uv, float tolerance) {
    float minBound = -tolerance;
    float maxBound = tolerance + 1.0f;

    const C3Vector& v0 = verts[indices[0]];
    const C3Vector& v1 = verts[indices[1]];
    const C3Vector& v2 = verts[indices[2]];

    float e1x = v1.x - v0.x;
    float e1y = v1.y - v0.y;
    float e1z = v1.z - v0.z;
    float e2x = v2.x - v0.x;
    float e2y = v2.y - v0.y;
    float e2z = v2.z - v0.z;

    float px = ray.dir.y * e2z - ray.dir.z * e2y;
    float py = ray.dir.z * e2x - e2z * ray.dir.x;
    float pz = e2y * ray.dir.x - e2x * ray.dir.y;
    float det = e1z * pz + py * e1y + px * e1x;

    if (det > -0.000001 && det < 0.000001) {
        return false;
    }

    float inv = 1.0f / det;
    float sx = ray.origin.x - v0.x;
    float sy = ray.origin.y - v0.y;
    float sz = ray.origin.z - v0.z;

    float u = (py * sy + pz * sz + sx * px) * inv;

    if (minBound > u || u > maxBound) {
        return false;
    }

    float qx = e1z * sy - sz * e1y;
    float qy = sz * e1x - e1z * sx;
    float qz = e1y * sx - e1x * sy;

    float v = (ray.dir.z * qz + ray.dir.y * qy + ray.dir.x * qx) * inv;

    if (minBound > v || v + u > maxBound) {
        return false;
    }

    if (t) {
        *t = inv * (qz * e2z + qy * e2y + qx * e2x);
    }

    if (uv) {
        uv->x = u;
        uv->y = v;
    }

    return true;
}

// OFFSET: 0x00B08080/0x1408CB030
float NTempest::DistanceSquared(const C3Segment& segment, const C3Vector& point, float* t) {
    float dx = point.x - segment.b.x;
    float dy = point.y - segment.b.y;
    float dz = point.z - segment.b.z;
    float ex = segment.t.x - segment.b.x;
    float ey = segment.t.y - segment.b.y;
    float ez = segment.t.z - segment.b.z;

    float dot = ex * dx + ez * dz + ey * dy;
    float param;

    if (dot > 0.0f) {
        float length = ex * ex + ez * ez + ey * ey;

        if (dot <= length) {
            param = dot / length;
            ex = ex * param;
            ey = ey * param;
            ez = ez * param;
        } else {
            param = 1.0f;
        }

        dx = dx - ex;
        dy = dy - ey;
        dz = dz - ez;
    } else {
        param = 0.0f;
    }

    if (t) {
        *t = param;
    }

    return dz * dz + dy * dy + dx * dx;
}

// OFFSET: 0x004C6C90/0x140101F60
void NTempest::SmoothDamp(C3Vector& value, C3Vector& velocity, const C3Vector& target, float omega, float deltaTime, float decay) {
    float end = target.x;
    float delta = value.x - end;

    if (fabs(delta) > 0.00000011920929f) {
        float temp = (delta * omega + velocity.x) * deltaTime;
        value.x = (temp + delta) * decay + end;
        velocity.x = (velocity.x - temp * omega) * decay;
    } else {
        value.x = end;
        velocity.x = 0.0f;
    }

    end = target.y;
    delta = value.y - end;

    if (fabs(delta) > 0.00000011920929f) {
        float temp = (delta * omega + velocity.y) * deltaTime;
        value.y = (delta + temp) * decay + end;
        velocity.y = (velocity.y - temp * omega) * decay;
    } else {
        value.y = end;
        velocity.y = 0.0f;
    }

    end = target.z;
    delta = value.z - end;

    if (fabs(delta) > 0.00000011920929f) {
        float temp = (delta * omega + velocity.z) * deltaTime;
        value.z = (temp + delta) * decay + end;
        velocity.z = (velocity.z - temp * omega) * decay;
    } else {
        value.z = end;
        velocity.z = 0.0f;
    }
}

// OFFSET: 0x004C6DE0
void NTempest::SmoothDamp(C4Vector& value, C4Vector& velocity, const C4Vector& target, float omega, float deltaTime, float decay) {
    float* current = &value.x;
    float* speed = &velocity.x;
    const float* goal = &target.x;

    for (uint32_t i = 0; i < 4; i++) {
        float end = goal[i];
        float delta = current[i] - end;

        if (fabs(delta) > 0.00000011920929f) {
            float temp = (delta * omega + speed[i]) * deltaTime;
            current[i] = (temp + delta) * decay + end;
            speed[i] = (speed[i] - temp * omega) * decay;
        } else {
            current[i] = end;
            speed[i] = 0.0f;
        }
    }
}

// OFFSET: 0x00B081A0/0x1408CB140
float NTempest::DistanceSquaredFromEdge(const C2Vector& a, const C2Vector& b, const C2Vector& point) {
    float ex = b.x - a.x;
    float ey = b.y - a.y;
    float dx = point.x - a.x;
    float dy = point.y - a.y;
    float dot = ey * dy + ex * dx;
    float length = ey * ey + ex * ex;

    if (fabs(length) < 0.00000023841858f) {
        float ny = a.y - point.y;
        return (a.x - point.x) * (a.x - point.x) + ny * ny;
    }

    float t = dot / length;

    if (t < 0.0f) {
        return dy * dy + dx * dx;
    }

    if (t > 1.0f) {
        return (point.x - b.x) * (point.x - b.x) + (point.y - b.y) * (point.y - b.y);
    }

    float py = point.y - (t * ey + a.y);
    float px = point.x - (t * ex + a.x);
    return py * py + px * px;
}

// OFFSET: 0x00B082D0
float NTempest::DistanceFromPolygonEdge(const C3Vector& point, const C3Vector* verts, uint32_t count) {
    float best = FLT_MAX;
    uint32_t prev = count - 1;

    for (uint32_t i = 0; i < count; i++) {
        C3Segment segment;
        segment.b = verts[prev];
        segment.t = verts[i];

        float distance = DistanceSquared(segment, point, NULL);

        if (best > distance) {
            best = distance;
        }

        prev = i;
    }

    return sqrt(best);
}

// OFFSET: 0x00B08460/0x1408CB950
float NTempest::DistanceFromPolygon(const C3Vector& point, const C3Vector* verts, uint32_t count, const C4Plane& plane) {
    C3Vector hit = point;
    float dist = point.x * plane.n.x + plane.n.y * point.y + plane.n.z * point.z + plane.d;

    if (dist > 0.001) {
        C3Vector end;
        end.x = point.x - plane.n.x;
        end.y = point.y - plane.n.y;
        end.z = point.z - plane.n.z;
        Intersect(C3Ray::FromStartEnd(point, end, false), plane, NULL, &hit, 0.01f);
    } else if (dist < -0.001) {
        C3Vector end;
        end.x = point.x + plane.n.x;
        end.y = plane.n.y + point.y;
        end.z = plane.n.z + point.z;
        Intersect(C3Ray::FromStartEnd(point, end, false), plane, NULL, &hit, 0.01f);
    }

    if (Intersect(hit, verts, count, plane.n.MajorAxis())) {
        return fabs(point.x * plane.n.x + plane.n.y * point.y + plane.n.z * point.z + plane.d);
    }

    return DistanceFromPolygonEdge(point, verts, count);
}

// OFFSET: 0x00B07DA0/0x1408CAD30
void RGBtoHSV(const C3Vector& rgb, C3Vector& hsv) {
    C3Vector::EAxis major = rgb.MajorAxis();
    C3Vector::EAxis minor = rgb.MinorAxis();
    const float* c = &rgb.x;

    hsv.z = c[major];
    hsv.y = c[major] == 0.0f ? 0.0f : (c[major] - c[minor]) / c[major];

    if (hsv.y == 0.0f) {
        hsv.x = -1.0f;
        return;
    }

    float delta = c[major] - c[minor];

    switch (major) {
        case C3Vector::AXIS_X:
            hsv.x = (rgb.y - rgb.z) / delta;
            break;

        case C3Vector::AXIS_Y:
            hsv.x = (rgb.z - rgb.x) / delta + 2.0f;
            break;

        case C3Vector::AXIS_Z:
            hsv.x = (rgb.x - rgb.y) / delta + 4.0f;
            break;
    }

    hsv.x = hsv.x * 60.0f;

    if (hsv.x < 0.0f) {
        hsv.x = hsv.x + 360.0f;
    }
}

// OFFSET: 0x00B07E90/0x1408CAE30
void HSVtoRGB(const C3Vector& hsv, C3Vector& rgb) {
    if (hsv.y == 0.0f) {
        rgb = C3Vector(hsv.z, hsv.z, hsv.z);
        return;
    }

    float hue = hsv.x;

    if (hue >= 360.0f) {
        hue = hue - 360.0f;
    }

    float sector = hue * 0.016666668f;
    int32_t index = static_cast<int32_t>(sector);

    if (index > 5) {
        index = 5;
    }

    float fraction = sector - index;
    float saturation = hsv.y >= 1.0f ? 1.0f : hsv.y;
    float q = (1.0f - saturation * fraction) * hsv.z;
    float p = (1.0f - saturation) * hsv.z;
    float t = (1.0f - saturation * (1.0f - fraction)) * hsv.z;

    switch (index) {
        case 0:
            rgb.x = hsv.z;
            rgb.y = t;
            rgb.z = p;
            break;

        case 1:
            rgb.x = q;
            rgb.y = hsv.z;
            rgb.z = p;
            break;

        case 2:
            rgb.x = p;
            rgb.y = hsv.z;
            rgb.z = t;
            break;

        case 3:
            rgb.x = p;
            rgb.y = q;
            rgb.z = hsv.z;
            break;

        case 4:
            rgb.x = t;
            rgb.y = p;
            rgb.z = hsv.z;
            break;

        case 5:
            rgb.x = hsv.z;
            rgb.y = p;
            rgb.z = q;
            break;
    }
}

// OFFSET: 0x00B08810/0x1408CBEC0
float PerlinNoise2D(float x, float y, int32_t octaves) {
    float amplitude = 1.0f;
    float total = 0.0f;

    for (int32_t i = 0; i < octaves; i++) {
        float frequency = static_cast<float>(1 << i);
        total = InterpolatedNoise2D(x * frequency, y * frequency) * amplitude + total;
        amplitude = amplitude * 0.5f;
    }

    return total;
}
