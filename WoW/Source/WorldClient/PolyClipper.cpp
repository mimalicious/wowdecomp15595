#include "WoW/Source/WorldClient/PolyClipper.h"
#include <cmath>
#include "Engine/Source/Tempest/C4Plane.h"

// OFFSET: 0x0079E6F0/0x1404938B0
const void* PCDataProxy::GetData(PCInterpolateFunc func) {
    if (!this->m_data) {
        this->m_data = func(this->m_a->GetData(func), this->m_b->GetData(func), this->m_t);
    }

    return this->m_data;
}

// OFFSET: 0x0079E730/0x140493920
void PolyClipper::AddVertex(const C3Vector& pos, const void* data) {
    Vertex& vertex = this->m_vertices[this->m_numVertices];
    vertex.m_pos = pos;
    vertex.m_proxy.m_data = data;
    this->m_numVertices++;
}

// OFFSET: 0x0079E770/0x140493950
void PolyClipper::Begin() {
    for (int32_t i = 0; i < this->m_numVertices; i++) {
        this->m_vertices[i].m_edge[0] = i - 1;
        this->m_vertices[i].m_edge[1] = i;
        this->m_vertices[i].m_stamp = 0xFFFFFFFF;
        Edge& edge = this->m_edges[this->m_numEdges];
        edge.m_vertex[0] = i;
        edge.m_vertex[1] = i + 1;
        edge.m_clipped = 0;
        this->m_numEdges++;
    }

    if (this->m_numEdges) {
        this->m_vertices[0].m_edge[0] = this->m_numEdges - 1;
        this->m_edges[this->m_numEdges - 1].m_vertex[1] %= this->m_numVertices;
    }
}

// OFFSET: 0x0079E7F0/0x1404939F0
void PolyClipper::End() {
    this->m_firstEdge = 0;

    while (this->m_firstEdge < this->m_numEdges && this->m_edges[this->m_firstEdge].m_clipped) {
        this->m_firstEdge++;
    }
}

// OFFSET: 0x0079E830
PCResultsIterator::PCResultsIterator(const PolyClipper* clipper, PCInterpolateFunc func) : m_clipper(clipper), m_func(func) {
    this->m_vertex[0] = -1;
    this->m_vertex[1] = -1;
    this->m_step = 0;
    this->m_finished = true;

    if (clipper->m_firstEdge < clipper->m_numEdges) {
        this->m_vertex[0] = clipper->m_edges[clipper->m_firstEdge].m_vertex[0];
        this->m_vertex[1] = clipper->m_edges[clipper->m_firstEdge].m_vertex[1];
        this->m_finished = false;
    }
}

// OFFSET: 0x0079E890/0x140493A30
void PCResultsIterator::GetVertex(C3Vector& pos, const void*& data) {
    int32_t index = (this->m_step & 1) == 0 ? this->m_vertex[0] : this->m_vertex[1];
    PolyClipper::Vertex& vertex = const_cast<PolyClipper::Vertex&>(this->m_clipper->m_vertices[index]);
    pos = vertex.m_pos;
    data = vertex.m_proxy.GetData(this->m_func);
}

// OFFSET: 0x0079E900/0x140493AE0
void PCResultsIterator::Next() {
    if (this->m_vertex[1] == this->m_vertex[0]) {
        this->m_finished = true;
        return;
    }

    if ((this->m_step & 1) == 0) {
        uint32_t vertex = this->m_clipper->m_edges[this->m_clipper->m_vertices[this->m_vertex[0]].m_edge[0]].m_vertex[0];
        this->m_step++;
        this->m_vertex[0] = vertex;
    } else {
        uint32_t vertex = this->m_clipper->m_edges[this->m_clipper->m_vertices[this->m_vertex[1]].m_edge[1]].m_vertex[1];
        this->m_step++;
        this->m_vertex[1] = vertex;
    }
}

// OFFSET: 0x0079E960/0x140493B50
bool PCResultsIterator::HasFinished() const {
    return this->m_finished;
}

// OFFSET: 0x0079E970/0x140493B60
void PolyClipper::Clip(const C4Plane& plane, int32_t sign) {
    this->m_clipStamp++;
    int32_t crossings = 0;
    int32_t count = 0;
    uint32_t crossEdge[2];
    uint32_t crossSide[2];
    uint32_t crossVertex[2];
    uint32_t newEdge[2];

    for (int32_t e = 0; e < this->m_numEdges; e++) {
        if (this->m_edges[e].m_clipped) {
            continue;
        }

        Vertex& a = this->m_vertices[this->m_edges[e].m_vertex[0]];
        Vertex& b = this->m_vertices[this->m_edges[e].m_vertex[1]];

        if (a.m_stamp != this->m_clipStamp) {
            a.m_dist = (a.m_pos.z * plane.n.z + a.m_pos.y * plane.n.y + a.m_pos.x * plane.n.x + plane.d) * static_cast<float>(sign);
            a.m_stamp = this->m_clipStamp;
        }

        if (b.m_stamp != this->m_clipStamp) {
            b.m_dist = (b.m_pos.z * plane.n.z + b.m_pos.y * plane.n.y + plane.n.x * b.m_pos.x + plane.d) * static_cast<float>(sign);
            b.m_stamp = this->m_clipStamp;
        }

        float distA = a.m_dist;

        if (distA >= 0.0f && b.m_dist >= 0.0f) {
            continue;
        }

        if (distA < 0.0f && b.m_dist < 0.0f) {
            this->m_edges[e].m_clipped = 1;
            continue;
        }

        float absA = fabs(distA);
        float sum = absA + fabs(b.m_dist);

        if (sum <= 0.00000011920929f) {
            continue;
        }

        if (++crossings > 2) {
            break;
        }

        float t = absA / sum;
        uint32_t side = distA < 0.0f ? 0 : 1;
        crossEdge[count] = e;
        crossSide[count] = side;
        crossVertex[count] = this->m_numVertices;
        count++;
        newEdge[1 - side] = this->m_numVertices;
        Vertex& vertex = this->m_vertices[this->m_numVertices];
        vertex.m_pos = a.m_pos + (b.m_pos - a.m_pos) * t;
        vertex.m_proxy.m_a = &a.m_proxy;
        vertex.m_proxy.m_b = &b.m_proxy;
        vertex.m_proxy.m_data = NULL;
        vertex.m_proxy.m_t = t;
        vertex.m_edge[1 - side] = e;
        vertex.m_edge[side] = this->m_numEdges;
        vertex.m_stamp = this->m_clipStamp;
        vertex.m_dist = 0.0f;
        this->m_numVertices++;
    }

    if (crossings != 2) {
        this->m_numVertices -= crossings;
        return;
    }

    this->m_edges[crossEdge[0]].m_vertex[crossSide[0]] = crossVertex[0];
    this->m_edges[crossEdge[1]].m_vertex[crossSide[1]] = crossVertex[1];
    this->m_edges[this->m_numEdges].m_vertex[0] = newEdge[0];
    this->m_edges[this->m_numEdges].m_vertex[1] = newEdge[1];
    this->m_edges[this->m_numEdges].m_clipped = 0;
    this->m_numEdges++;
}

// OFFSET: 0x0079EC60/0x140493EF0
PCResultsIterator PolyClipper::GetResults(PCInterpolateFunc func) const {
    return PCResultsIterator(this, func);
}

// OFFSET: 0x0079EC80/0x140493F60
PolyClipper::PolyClipper() {
    this->m_numVertices = 0;
    this->m_numEdges = 0;
    this->m_clipStamp = 0;
    this->m_firstEdge = 0;
}

// OFFSET: 0x00401000/0x1407D7E80
PolyClipper::~PolyClipper() {
}

// OFFSET: 0x00401000/0x1407D7E80
PCResultsIterator::~PCResultsIterator() {
}
