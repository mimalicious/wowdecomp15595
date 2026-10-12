#include <cmath>
#include <cstddef>
#include <cstdint>
#include <malloc.h>
#include "Engine/Source/Tempest/C3Spline_CatmullRom.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Tempest/C34Matrix.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Storm/H/stpl.h"

// STATIC: 0x00CA08E0/0x140B3CC10
static const C44Matrix s_catmullRomBasis(-0.5f, 1.0f, -0.5f, 0.0f, 1.5f, -2.5f, 0.0f, 1.0f, -1.5f, 2.0f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f);

// STATIC: 0x00CA0920/0x140B3CC50
static const C34Matrix s_catmullRomDer1(-1.5f, 2.0f, -0.5f, 4.5f, -5.0f, 0.0f, -4.5f, 4.0f, 0.5f, 1.5f, -1.0f, 0.0f);

// PROVEN: 6e188d90 IDENTICAL/NONE
// OFFSET: 0x0040BEB0
C3Spline_CatmullRom::C3Spline_CatmullRom() {
    this->m_splineMode = 1;
}

// PROVEN: cb1b7b35 IDENTICAL/IDENTICAL
// OFFSET: 0x0040BED0/0x14000EDA0
uint32_t C3Spline_CatmullRom::GetSplineMode() {
    return this->m_splineMode;
}

// OFFSET: 0x004C9C10/0x140104BB0
void C3Spline_CatmullRom::Evaluate(uint32_t seg, float t, C3Vector& pos) {
    uint32_t mode = this->m_splineMode;

    if (!mode) {
        const C3Vector& a = this->Point(seg + 1);
        const C3Vector& b = this->Point(seg + 2);
        C3Vector delta = (b - a) * t;
        pos = delta + this->Point(seg + 1);
        return;
    }

    if (mode == 1) {
        C3Spline::Evaluate(seg, t, s_catmullRomBasis, pos);
        return;
    }

    C3Spline::Evaluate(seg, t, s_catmullRomBasis, pos);

    if (this->m_splineMode == 2) {
        C3Vector a = this->Point(seg + 1);
        C3Vector b = this->Point(seg + 2);
        pos.z = (b.z - a.z) * t + a.z;
    } else if (this->m_splineMode == 3) {
        C3Vector a = this->Point(seg + 1);
        C3Vector b = this->Point(seg + 2);
        pos.y = (b.y - a.y) * t + a.y;
    }
}

// OFFSET: 0x004C9E30/0x140104E50
void C3Spline_CatmullRom::EvaluateDer1(uint32_t seg, float t, C3Vector& vel) {
    C3Spline::EvaluateDer1(seg, t, C44Matrix(s_catmullRomDer1), vel);

    if (this->m_splineMode == 2) {
        C3Vector a = this->Point(seg + 1);
        C3Vector b = this->Point(seg + 2);
        vel.z = b.z - a.z;
        vel.NormalizeSafe();
    } else if (this->m_splineMode == 3) {
        C3Vector a = this->Point(seg + 1);
        C3Vector b = this->Point(seg + 2);
        vel.y = b.y - a.y;
        vel.NormalizeSafe();
    }
}

// OFFSET: 0x004CA080/0x140105080
float C3Spline_CatmullRom::SegLength(uint32_t seg) {
    if (!this->m_splineMode) {
        const C3Vector& a = this->Point(seg + 1);
        const C3Vector& b = this->Point(seg + 2);

        float dx = b.x - a.x;
        float dy = b.y - a.y;
        float dz = b.z - a.z;

        return sqrt((dx * dx + dz * dz) + dy * dy);
    }

    return C3Spline::SegLength(seg, s_catmullRomBasis);
}

// INLINED
void C3Spline_CatmullRom::ParametricSegT(float t, uint32_t& seg, float& segT) {
    double segCount = static_cast<double>(this->m_pointCount - 3);

    seg = static_cast<uint32_t>(static_cast<int64_t>(segCount * t));
    segT = static_cast<float>(segCount * (t - 1.0 / segCount * seg));
}

// PROVEN: 38927d52 IDENTICAL/IDENTICAL
// OFFSET: 0x004CA110/0x140105120
float C3Spline_CatmullRom::ILength() {
    return C3Spline::ILength(this->m_pointCount - 3);
}

// PROVEN: ef8cc2b3 IDENTICAL/IDENTICAL
// OFFSET: 0x004CA120/0x140105130
void C3Spline_CatmullRom::IValidateCache() {
    uint32_t segCount = this->m_pointCount - 3;

    for (uint32_t i = 0; i < segCount; i++) {
        this->SetCachedSegLength(this->SegLength(i), i);
    }
}

// PROVEN: 6aab6aa4 IDENTICAL/NONE
// OFFSET: 0x004CA190
void C3Spline_CatmullRom::IPosArclength(float t, C3Vector& pos) {
    uint32_t seg;
    float segT;

    this->ArclengthSegT(t, s_catmullRomBasis, this->m_pointCount - 3, seg, segT);
    this->Evaluate(seg, segT, pos);
}

// OFFSET: 0x004CA1E0/0x140105380
void C3Spline_CatmullRom::IPosParametric(float t, C3Vector& pos) {
    uint32_t seg;
    float segT;

    this->ParametricSegT(t, seg, segT);
    this->Evaluate(seg, segT, pos);
}

// PROVEN: 7d10670d IDENTICAL/NONE
// OFFSET: 0x004CA260
void C3Spline_CatmullRom::IVelArclength(float t, C3Vector& vel) {
    float clamped;

    if (t < 0.0f) {
        clamped = 0.0f;
    } else if (t < 1.0f) {
        clamped = t;
    } else {
        clamped = 1.0f;
    }

    uint32_t seg;
    float segT;

    this->ArclengthSegT(clamped, s_catmullRomBasis, this->m_pointCount - 3, seg, segT);
    this->EvaluateDer1(seg, segT, vel);
}

// OFFSET: 0x004CA2D0/0x1401055A0
void C3Spline_CatmullRom::IVelParametric(float t, C3Vector& vel) {
    float clamped;

    if (t < 0.0f) {
        clamped = 0.0f;
    } else if (t < 1.0f) {
        clamped = t;
    } else {
        clamped = 1.0f;
    }

    uint32_t seg;
    float segT;

    this->ParametricSegT(clamped, seg, segT);
    this->EvaluateDer1(seg, segT, vel);
}

// OFFSET: 0x004CA370/0x140105610
void C3Spline_CatmullRom::IFrameArclength(float t, C44Matrix& frame) {
    uint32_t seg;
    float segT;

    this->ArclengthSegT(t, s_catmullRomBasis, this->m_pointCount - 3, seg, segT);
    this->Evaluate(seg, segT, *reinterpret_cast<C3Vector*>(&frame.d0));

    if (this->m_splineMode) {
        C3Vector vel(0.0f, 0.0f, 0.0f);
        this->EvaluateDer1(seg, segT, vel);

        if (this->m_splineMode == 2) {
            vel.z = 0.0f;
        } else if (this->m_splineMode == 3) {
            vel.y = 0.0f;
        }

        float velSq = vel.x * vel.x + vel.y * vel.y + vel.z * vel.z;

        if (velSq > 0.000099999997f) {
            float inv = 1.0f / sqrt(velSq);
            vel.x = vel.x * inv;
            vel.y = vel.y * inv;
            vel.z = inv * vel.z;
        }

        frame.a0 = vel.x;
        frame.a1 = vel.y;
        frame.a2 = vel.z;
    } else {
        const C3Vector& a = this->Point(seg + 1);
        const C3Vector& b = this->Point(seg + 2);

        C3Vector dir(b.x - a.x, b.y - a.y, b.z - a.z);
        float dirSq = dir.z * dir.z + dir.y * dir.y + dir.x * dir.x;

        if (dirSq > 0.00000023841858f) {
            float inv = 1.0f / sqrt(dirSq);
            dir.x = dir.x * inv;
            dir.y = dir.y * inv;
            dir.z = inv * dir.z;
        }

        frame.a0 = dir.x;
        frame.a1 = dir.y;
        frame.a2 = dir.z;
    }

    frame.b0 = -frame.a1;
    frame.b1 = frame.a0;
    frame.b2 = 0.0f;

    float sideSq = frame.b1 * frame.b1 + frame.b0 * frame.b0;

    if (sideSq > 0.00000023841858f) {
        float inv = 1.0f / sqrt(sideSq);
        frame.b0 = inv * frame.b0;
        frame.b1 = inv * frame.b1;
    }

    frame.c0 = -(frame.a2 * frame.b1);
    frame.c1 = frame.a2 * frame.b0;
    frame.c2 = frame.a0 * frame.b1 - frame.a1 * frame.b0;
}

// OFFSET: 0x004CA600/0x140105A40
uint32_t C3Spline_CatmullRom::IGetPoints(float t, C3Vector* points, uint32_t count) {
    uint32_t pointCount = this->m_pointCount;
    uint32_t seg;
    float segT;

    this->ArclengthSegT(t, s_catmullRomBasis, pointCount - 3, seg, segT);

    uint32_t remaining = pointCount - seg - 3;

    if (!points) {
        return remaining;
    }

    if (remaining >= count) {
        remaining = count;
    }

    for (uint32_t i = 0; i < remaining; i++) {
        points[i] = this->Point(seg + 2 + i);
    }

    return remaining;
}

// OFFSET: 0x004CABF0/0x140106630
int32_t C3Spline_CatmullRom::Smooth(float minLength, int32_t keepStart) {
    if (!this->m_splineMode) {
        return 0;
    }

    int32_t segCount = this->m_pointCount - 3;

    if (segCount <= 1) {
        return 0;
    }

    uint32_t start = keepStart != 0;
    float stepLength = this->m_segLength[start];

    if (minLength > stepLength) {
        stepLength = minLength;
    }

    for (uint32_t i = start + 1; i < this->m_pointCount - 3; i++) {
        float length = this->GetCachedSegLength(i);

        if (length > minLength && stepLength > length) {
            stepLength = length;
        }
    }

    int32_t maxPoints = static_cast<int32_t>(this->m_length / stepLength) + segCount + 4;

    if (maxPoints > 100) {
        return 0;
    }

    TSStackArray<C3Vector> points(static_cast<C3Vector*>(alloca(sizeof(C3Vector) * maxPoints)), maxPoints, 1);
    points.m_data[0] = this->m_points[0];

    float totalLength = this->m_length;
    float traveled = 0.0f;
    int32_t count = 1;
    C3Vector pos(0.0f, 0.0f, 0.0f);

    if (keepStart && maxPoints > 1) {
        C3Vector delta = points.m_data[0] - this->m_points[1];

        if (fnotequal(delta.x * delta.x + delta.z * delta.z + delta.y * delta.y, 0.0f)) {
            points.m_data[1] = this->m_points[1];
            traveled = this->m_segLength[0];
            count = 2;
        }
    }

    for (int32_t seg = start; seg < segCount; seg++) {
        if (count >= maxPoints) {
            break;
        }

        const C3Vector& point = this->Point(seg + 1);
        C3Vector delta = points.m_data[count - 1] - point;

        if (fabs(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z) >= 0.00000023841858f) {
            points.m_data[count++] = this->Point(seg + 1);
        }

        float length = this->GetCachedSegLength(seg);

        if (length > stepLength) {
            int32_t steps = static_cast<int32_t>(length / stepLength);
            float step = length / static_cast<float>(steps);

            for (int32_t i = 0; i < steps - 1; i++) {
                float t = ((static_cast<float>(i) + 1.0f) * step + traveled) * (1.0f / totalLength);
                uint32_t mode = this->m_splineMode;
                this->m_splineMode = 0;

                if (t <= 0.0f) {
                    pos = this->m_points[1];
                } else if (t >= 1.0f) {
                    if (this->m_pointCount <= 1) {
                        pos = this->m_points[0];
                    } else {
                        pos = this->Point(this->m_pointCount - 2);
                    }
                } else {
                    this->IPosArclength(t, pos);
                }

                this->m_splineMode = mode;

                if (count < maxPoints) {
                    C3Vector last = points.m_data[count - 1] - pos;

                    if (fabs(last.x * last.x + last.y * last.y + last.z * last.z) >= 0.00000023841858f) {
                        points.m_data[count++] = pos;
                    }
                }
            }
        }

        traveled = length + traveled;
    }

    if (count >= maxPoints - 2) {
        return 0;
    }

    C3Vector dir = points.m_data[0] - points.m_data[1];
    float dirSq = dir.x * dir.x + dir.z * dir.z + dir.y * dir.y;

    if (dirSq > 0.00000023841858f) {
        float inv = 1.0f / sqrt(dirSq);
        dir.x = dir.x * inv;
        dir.y = dir.y * inv;
        dir.z = inv * dir.z;
    }

    points.m_data[0].x = points.m_data[1].x + dir.x * stepLength;
    points.m_data[0].y = points.m_data[1].y + dir.y * stepLength;
    points.m_data[0].z = points.m_data[1].z + dir.z * stepLength;

    points.m_data[count++] = this->Point(segCount + 1);
    points.m_data[count++] = this->Point(segCount + 2);

    this->ISetPoints(points.m_data, count);

    if (this->m_pointCount > 3) {
        this->IValidateCache();
        this->m_length = this->ILength();
    }

    return 1;
}

// PROVEN: b8cf4231 IDENTICAL/IDENTICAL
// OFFSET: 0x004CB4A0/0x1401070F0
void C3Spline_CatmullRom::ISetPoints(const C3Vector* points, uint32_t count) {
    if (count) {
        uint32_t segCount = count - 3;
        this->m_segCount = segCount;

        if (segCount > 25) {
            this->m_extraSegLength.SetCount(segCount - 25);
        }
    } else {
        this->m_segCount = 0;
        this->m_extraSegLength.SetCount(0);
    }

    C3Spline::ISetPoints(points, count);
}

// OFFSET: [0x004879C0<uint32_t>, 0x00550DA0<uint64_t>]
template <class T>
static T ReadValue(CDataStore& msg) {
    T value = 0;
    msg.Get(value);
    return value;
}

// INLINED
CDataStore& operator>>(CDataStore& msg, C3Spline_CatmullRom& spline) {
    uint32_t count = ReadValue<uint32_t>(msg);

    void* points;
    msg.GetDataInSitu(points, sizeof(C3Vector) * count);

    uint8_t mode;
    msg.Get(mode);
    spline.m_splineMode = mode;

    if (count && msg.m_read <= msg.m_size) {
        spline.SetPoints(static_cast<const C3Vector*>(points), count);
    }

    return msg;
}
