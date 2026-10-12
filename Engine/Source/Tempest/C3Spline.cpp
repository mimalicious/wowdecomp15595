#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/Tempest/C3Spline.h"
#include "Engine/Source/Tempest/C44Matrix.h"

// OFFSET: 0x0040BE30/0x140267B20
C3Spline::C3Spline() {
    this->m_pointCount = 0;
    this->m_segCount = 0;
}

// OFFSET: 0x004C92A0/0x140104160
float C3Spline::LengthBeforePoint(uint32_t index) {
    float length = 0.0f;

    if (index) {
        for (uint32_t i = 0; i < index - 1; i++) {
            length += this->GetCachedSegLength(i);
        }
    }

    return length;
}

// OFFSET: 0x004C9390/0x140104270
void C3Spline::SetPoints(const C3Vector* points, uint32_t count) {
    this->ISetPoints(points, count);

    if (this->m_pointCount > 3) {
        this->IValidateCache();
        this->m_length = this->ILength();
    }
}

// OFFSET: 0x004C93D0/0x1401042B0
void C3Spline::Pos(float t, C3Vector& pos, EvalType type) {
    if (t <= 0.0f) {
        pos = this->Point(1);
        return;
    }

    if (t >= 1.0f) {
        uint32_t count = this->m_pointCount;

        if (count <= 1) {
            pos = this->Point(0);
        } else if (count - 1 <= 25) {
            pos = this->m_points[count - 2];
        } else {
            pos = this->m_extraPoints[count - 27];
        }

        return;
    }

    switch (type) {
        case EVAL_PARAMETRIC:
            this->IPosParametric(t, pos);
            break;
        case EVAL_ARCLENGTH:
            this->IPosArclength(t, pos);
            break;
    }
}

// OFFSET: 0x004C94A0/0x140104390
void C3Spline::Vel(float t, C3Vector& vel, EvalType type) {
    t = t < 0.0f ? 0.0f : (t < 1.0f ? t : 1.0f);

    switch (type) {
        case EVAL_PARAMETRIC:
            this->IVelParametric(t, vel);
            break;
        case EVAL_ARCLENGTH:
            this->IVelArclength(t, vel);
            break;
    }
}

// OFFSET: 0x004C9500/0x1401043D0
void C3Spline::Frame(float t, C44Matrix& frame, EvalType type) {
    t = t < 0.0f ? 0.0f : (t < 1.0f ? t : 1.0f);

    switch (type) {
        case EVAL_ARCLENGTH:
            this->IFrameArclength(t, frame);
            break;
    }
}

// OFFSET: 0x004C9540/0x140104400
void C3Spline::Evaluate(uint32_t seg, float t, const C44Matrix& basis, C3Vector& pos) {
    const float* row = &basis.a0;

    pos.x = 0.0f;
    pos.y = 0.0f;
    pos.z = 0.0f;

    uint32_t i = 0;

    do {
        float weight = ((row[0] * t + row[1]) * t + row[2]) * t + row[3];
        pos += this->Point(seg + i) * weight;
        row += 4;
        i++;
    } while (i < 4);
}

// OFFSET: 0x004C9620/0x1401044D0
void C3Spline::EvaluateDer1(uint32_t seg, float t, const C44Matrix& basis, C3Vector& vel) {
    const float* row = &basis.a0;

    vel.x = 0.0f;
    vel.y = 0.0f;
    vel.z = 0.0f;

    uint32_t i = 0;

    do {
        float weight = (row[0] * t + row[1]) * t + row[2];
        vel += this->Point(seg + i) * weight;
        row += 4;
        i++;
    } while (i < 4);
}

// OFFSET: 0x004C96F0/0x1401045A0
float C3Spline::SegLength(uint32_t seg, const C44Matrix& basis) {
    float step = 0.05f;
    float length = 0.0f;
    C3Vector prev;
    C3Vector cur;

    this->Evaluate(seg, 0.0f, basis, prev);

    for (int32_t i = 20; i; i--) {
        this->Evaluate(seg, step, basis, cur);

        float dx = cur.x - prev.x;
        float dy = cur.y - prev.y;
        float dz = cur.z - prev.z;
        prev = cur;

        length = sqrt(dx * dx + dy * dy + dz * dz) + length;
        step = step + 0.05f;
    }

    return length;
}

// OFFSET: 0x004C97C0/0x140104400
void C3Spline::ArclengthSegT(float t, const C44Matrix& basis, uint32_t segCount, uint32_t& seg, float& segT) {
    if (segCount <= 1) {
        seg = 0;
        segT = t;
        return;
    }

    float target = this->m_length * t;
    float before = 0.0f;
    for (seg = 0; seg < segCount - 1; seg++) {
        float after = this->GetCachedSegLength(seg) + before;

        if (after > target) {
            break;
        }

        before = after;
    }

    segT = (target - before) / this->GetCachedSegLength(seg);
}

// OFFSET: 0x004C9880/0x140104740
float C3Spline::ILength(uint32_t segCount) {
    float length = 0.0f;

    for (uint32_t i = 0; i < segCount; i++) {
        length += this->GetCachedSegLength(i);
    }

    return length;
}

// OFFSET: 0x004C9970/0x140104840
void C3Spline::GetPoints(C3Vector* points, uint32_t count) {
    uint32_t n = count >= this->m_pointCount ? this->m_pointCount : count;

    for (uint32_t i = 0; i < n; i++) {
        if (i >= 25) {
            break;
        }

        points[i] = this->m_points[i];
    }

    if (n > 25) {
        memcpy(&points[25], this->m_extraPoints.m_data, sizeof(C3Vector) * (n - 25));
    }
}

// OFFSET: 0x004CB3D0/0x140107000
void C3Spline::ISetPoints(const C3Vector* points, uint32_t count) {
    this->m_pointCount = count;

    for (uint32_t i = 0; i < count; i++) {
        if (i >= 25) {
            break;
        }

        this->m_points[i] = points[i];
    }

    if (count > 25) {
        this->m_extraPoints.Set(count - 25, &points[25]);
    } else {
        this->m_extraPoints.SetCount(0);
    }
}

// OFFSET: 0x00B286B0/0x1408F0DE0
C3Spline& C3Spline::operator=(const C3Spline& source) {
    this->m_length = source.m_length;

    for (uint32_t i = 0; i < 25; i++) {
        this->m_points[i] = source.m_points[i];
    }

    this->m_extraPoints = source.m_extraPoints;
    this->m_pointCount = source.m_pointCount;

    for (uint32_t i = 0; i < 25; i++) {
        this->m_segLength[i] = source.m_segLength[i];
    }

    this->m_extraSegLength = source.m_extraSegLength;
    this->m_segCount = source.m_segCount;

    return *this;
}

// OFFSET: 0x00842700/0x1407FDD80
uint32_t C3Spline::GetSplineMode() {
    return 0;
}

// OFFSET: 0x00AF4CF0/0x1407FDD80
uint32_t C3Spline::IGetPoints(float t, C3Vector* points, uint32_t count) {
    return 0;
}
