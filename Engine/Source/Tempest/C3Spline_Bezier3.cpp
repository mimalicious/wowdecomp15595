#include <cmath>
#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Spline_Bezier3.h"
#include "Engine/Source/Tempest/C34Matrix.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CMath.h"

// STATIC: %0x00BB4520
static const C44Matrix s_bezier3Basis(-1.0f, 3.0f, -3.0f, 1.0f, 3.0f, -6.0f, 3.0f, 0.0f, -3.0f, 3.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);

// INLINED
void C3Spline_Bezier3::ParametricSegT(float t, uint32_t& seg, float& segT) {
    double segCount = static_cast<double>(this->m_pointCount / 3);

    seg = static_cast<uint32_t>(static_cast<int64_t>(segCount * t));
    segT = static_cast<float>(segCount * (t - 1.0 / segCount * seg));
}

// PROVEN: 91dbe5e0 IDENTICAL/UNPROVEN
// OFFSET: 0x004C99E0/0x1401048D0
void C3Spline_Bezier3::EvaluateDer1(uint32_t seg, float t, C3Vector& vel) {
    // STATIC: #0x00B4A308
    static const C34Matrix s_bezier3Der1(-3.0f, 6.0f, -3.0f, 9.0f, -12.0f, 3.0f, -9.0f, 6.0f, 0.0f, 3.0f, 0.0f, 0.0f);

    C3Spline::EvaluateDer1(3 * seg, t, C44Matrix(s_bezier3Der1), vel);
}

// PROVEN: 669731df IDENTICAL/UNPROVEN
// OFFSET: 0x004C9B70/0x140104AF0
float C3Spline_Bezier3::ILength() {
    return C3Spline::ILength(this->m_pointCount / 3);
}

// PROVEN: b35d2fb8 IDENTICAL/IDENTICAL
// OFFSET: 0x004C9B90/0x140104B10
void C3Spline_Bezier3::IValidateCache() {
    uint32_t segCount = this->m_pointCount / 3;

    for (uint32_t i = 0; i < segCount; i++) {
        this->SetCachedSegLength(C3Spline::SegLength(3 * i, s_bezier3Basis), i);
    }
}

// OFFSET: 0x004CA8B0
void C3Spline_Bezier3::IPosParametric(float t, C3Vector& pos) {
    uint32_t seg;
    float segT;

    this->ParametricSegT(t, seg, segT);
    C3Spline::Evaluate(3 * seg, segT, s_bezier3Basis, pos);
}

// PROVEN: 15d7cb10 IDENTICAL/NONE
// OFFSET: 0x004CA930
void C3Spline_Bezier3::IPosArclength(float t, C3Vector& pos) {
    uint32_t seg;
    float segT;

    this->ArclengthSegT(t, s_bezier3Basis, this->m_pointCount / 3, seg, segT);
    C3Spline::Evaluate(3 * seg, segT, s_bezier3Basis, pos);
}

// OFFSET: 0x004CA990
void C3Spline_Bezier3::IVelParametric(float t, C3Vector& vel) {
    uint32_t seg;
    float segT;

    this->ParametricSegT(t, seg, segT);
    this->EvaluateDer1(seg, segT, vel);
}

// PROVEN: 7021593a IDENTICAL/NONE
// OFFSET: 0x004CAA10
void C3Spline_Bezier3::IVelArclength(float t, C3Vector& vel) {
    uint32_t seg;
    float segT;

    this->ArclengthSegT(t, s_bezier3Basis, this->m_pointCount / 3, seg, segT);
    this->EvaluateDer1(seg, segT, vel);
}

// OFFSET: 0x004CAA60
void C3Spline_Bezier3::IFrameArclength(float t, C44Matrix& frame) {
    uint32_t seg;
    float segT;

    this->ArclengthSegT(t, s_bezier3Basis, this->m_pointCount / 3, seg, segT);
    C3Spline::Evaluate(3 * seg, segT, s_bezier3Basis, *reinterpret_cast<C3Vector*>(&frame.d0));

    C3Vector vel(0.0f, 0.0f, 0.0f);
    this->EvaluateDer1(seg, segT, vel);

    float length = sqrt(vel.z * vel.z + vel.y * vel.y + vel.x * vel.x);

    if (!(fabs(length) < 0.0099999998f)) {
        float inv = 1.0f / length;
        vel.x = vel.x * inv;
        frame.a0 = vel.x;
        vel.y = vel.y * inv;
        frame.a1 = vel.y;
        vel.z = vel.z * inv;
        frame.a2 = vel.z;
    }

    frame.b0 = -frame.a1;
    frame.b1 = frame.a0;
    frame.b2 = 0.0f;

    float inv = 1.0f / sqrt(frame.b0 * frame.b0 + frame.b1 * frame.b1);
    frame.b0 = inv * frame.b0;
    frame.b1 = inv * frame.b1;

    frame.c0 = -(frame.a2 * frame.b1);
    frame.c1 = frame.a2 * frame.b0;
    frame.c2 = frame.b1 * frame.a0 - frame.b0 * frame.a1;
}

// OFFSET: 0x004CB450/0x140107080
void C3Spline_Bezier3::ISetPoints(const C3Vector* points, uint32_t count) {
    uint32_t segCount = count / 3;
    uint32_t pointCount = 3 * (segCount - 1) + 4 * (segCount > 0);
    this->m_segCount = segCount;

    if (segCount > 25) {
        this->m_extraSegLength.SetCount(segCount - 25);
    }

    C3Spline::ISetPoints(points, pointCount);
}
