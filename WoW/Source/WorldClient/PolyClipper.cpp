#include "PolyClipper.h"
#include <cmath>
#include "Engine/Source/Tempest/C4Plane.h"

// PROVEN: d86786f4 IDENTICAL/NONE
// OFFSET: 0x00401000
PolyClipper::~PolyClipper() {
}

// PROVEN: 63910cce IDENTICAL/NONE
// OFFSET: 0x00401000
PCResultsIterator::~PCResultsIterator() {
}

// OFFSET: #0x007D91F0
void* PolyClipper::PCDataProxy::GetData(void* (*lerp)(const void*, const void*, float)) const {
    if (!this->m_data) {
        this->m_data = lerp(this->m_a->GetData(lerp), this->m_b->GetData(lerp), this->m_t);
    }

    return this->m_data;
}

// PROVEN: 20939842 IDENTICAL/NONE
// OFFSET: 0x0079E730/0x140493920
void PolyClipper::AddVertex(const C3Vector& pos, const void* data) {
    PCVertex& vertex = this->m_verts[this->m_vertCount];
    vertex.m_pos = pos;
    vertex.m_proxy.m_data = const_cast<void*>(data);
    this->m_vertCount++;
}

// OFFSET: #0x007D9270
void PolyClipper::Begin() {
    for (int32_t i = 0; i < this->m_vertCount; i++) {
        PCVertex& vertex = this->m_verts[i];
        vertex.m_edges[1] = i;
        vertex.m_stamp = -1;
        vertex.m_edges[0] = i - 1;

        PCEdge& edge = this->m_edges[this->m_edgeCount];
        edge.m_verts[0] = i;
        edge.m_verts[1] = i + 1;
        edge.m_clipped = false;
        this->m_edgeCount++;
    }

    if (this->m_edgeCount) {
        PCEdge& last = this->m_edges[this->m_edgeCount - 1];
        this->m_verts[0].m_edges[0] = this->m_edgeCount - 1;
        last.m_verts[1] %= this->m_vertCount;
    }
}

// PROVEN: 181d354b IDENTICAL/NONE
// OFFSET: 0x0079E7F0/0x1404939F0
void PolyClipper::End() {
    for (this->m_firstEdge = 0; this->m_firstEdge < this->m_edgeCount; this->m_firstEdge++) {
        if (!this->m_edges[this->m_firstEdge].m_clipped) {
            break;
        }
    }
}

// OFFSET: #0x007D9330
PCResultsIterator::PCResultsIterator(const PolyClipper* clipper, void* (*lerp)(const void*, const void*, float)) {
    this->m_lerp = lerp;
    this->m_clipper = clipper;
    this->m_front = -1;
    this->m_back = -1;
    this->m_step = 0;
    this->m_finished = true;

    if (clipper->m_firstEdge < clipper->m_edgeCount) {
        this->m_front = clipper->m_edges[clipper->m_firstEdge].m_verts[0];
        this->m_back = clipper->m_edges[clipper->m_firstEdge].m_verts[1];
        this->m_finished = false;
    }
}

// OFFSET: #0x007D9390
void PCResultsIterator::GetVertex(C3Vector& pos, const void*& data) const {
    int32_t index = (this->m_step & 1) ? this->m_back : this->m_front;
    const PolyClipper::PCVertex& vertex = this->m_clipper->m_verts[index];
    pos = vertex.m_pos;

    if (vertex.m_proxy.m_data) {
        data = vertex.m_proxy.m_data;
    } else {
        void* result = this->m_lerp(vertex.m_proxy.m_a->GetData(this->m_lerp), vertex.m_proxy.m_b->GetData(this->m_lerp), vertex.m_proxy.m_t);
        vertex.m_proxy.m_data = result;
        data = result;
    }
}

// OFFSET: #0x007D9400
void PCResultsIterator::Next() {
    if (this->m_back == this->m_front) {
        this->m_finished = true;
    } else if (this->m_step & 1) {
        this->m_back = this->m_clipper->m_edges[this->m_clipper->m_verts[this->m_back].m_edges[1]].m_verts[1];
        this->m_step++;
    } else {
        this->m_front = this->m_clipper->m_edges[this->m_clipper->m_verts[this->m_front].m_edges[0]].m_verts[0];
        this->m_step++;
    }
}

// PROVEN: b993f5e7 IDENTICAL/NONE
// OFFSET: 0x0079E960/0x140493B50
bool PCResultsIterator::HasFinished() const {
    return this->m_finished;
}

// OFFSET: #0x007D9470
int32_t PolyClipper::Clip(const C4Plane& plane, int32_t side) {
    this->m_stamp++;

    int32_t crossings = 0;
    int32_t crossEdge[2];
    int32_t crossSide[2];
    int32_t crossVert[2];
    int32_t newEdge[2];

    for (int32_t e = 0; e < this->m_edgeCount; e++) {
        PCEdge& edge = this->m_edges[e];

        if (edge.m_clipped) {
            continue;
        }

        PCVertex& a = this->m_verts[edge.m_verts[0]];
        PCVertex& b = this->m_verts[edge.m_verts[1]];

        if (a.m_stamp != this->m_stamp) {
            a.m_dist = (a.m_pos.z * plane.n.z + a.m_pos.y * plane.n.y + plane.n.x * a.m_pos.x + plane.d) * side;
            a.m_stamp = this->m_stamp;
        }

        if (b.m_stamp != this->m_stamp) {
            b.m_dist = (b.m_pos.z * plane.n.z + b.m_pos.y * plane.n.y + b.m_pos.x * plane.n.x + plane.d) * side;
            b.m_stamp = this->m_stamp;
        }

        if (!(0.0f > a.m_dist || 0.0f > b.m_dist)) {
            continue;
        }

        if (!(0.0f <= a.m_dist || 0.0f <= b.m_dist)) {
            edge.m_clipped = true;
            continue;
        }

        float distA = fabs(a.m_dist);
        float sum = fabs(b.m_dist) + distA;

        if (sum <= 1.1920929e-7f) {
            continue;
        }

        if (++crossings > 2) {
            break;
        }

        float t = distA / sum;
        int32_t keep = 0.0f <= a.m_dist;
        int32_t k = crossings - 1;
        crossEdge[k] = e;
        crossSide[k] = keep;
        crossVert[k] = this->m_vertCount;
        newEdge[1 - keep] = this->m_vertCount;

        PCVertex& vertex = this->m_verts[this->m_vertCount];
        vertex.m_pos.x = (b.m_pos.x - a.m_pos.x) * t + a.m_pos.x;
        vertex.m_pos.y = (b.m_pos.y - a.m_pos.y) * t + a.m_pos.y;
        vertex.m_pos.z = (b.m_pos.z - a.m_pos.z) * t + a.m_pos.z;
        vertex.m_proxy.m_b = &b.m_proxy;
        vertex.m_proxy.m_a = &a.m_proxy;
        vertex.m_proxy.m_t = t;
        vertex.m_proxy.m_data = NULL;
        vertex.m_edges[1 - keep] = e;
        vertex.m_edges[keep] = this->m_edgeCount;
        vertex.m_stamp = this->m_stamp;
        vertex.m_dist = 0.0f;
        this->m_vertCount++;
    }

    if (crossings != 2) {
        this->m_vertCount -= crossings;
        return crossings;
    }

    this->m_edges[crossEdge[0]].m_verts[crossSide[0]] = crossVert[0];
    this->m_edges[crossEdge[1]].m_verts[crossSide[1]] = crossVert[1];
    this->m_edges[this->m_edgeCount].m_verts[0] = newEdge[0];
    this->m_edges[this->m_edgeCount].m_verts[1] = newEdge[1];
    this->m_edges[this->m_edgeCount].m_clipped = false;
    this->m_edgeCount++;

    return 2;
}

// PROVEN: 834d2bc2 IDENTICAL/NONE
// OFFSET: 0x0079EC60
PCResultsIterator PolyClipper::GetResults(void* (*lerp)(const void*, const void*, float)) const {
    return PCResultsIterator(this, lerp);
}

// PROVEN: 3d567ceb IDENTICAL/NONE
// OFFSET: 0x0079EC80/0x140493F60
PolyClipper::PolyClipper() {
    this->m_vertCount = 0;
    this->m_edgeCount = 0;
    this->m_stamp = 0;
    this->m_firstEdge = 0;
}
