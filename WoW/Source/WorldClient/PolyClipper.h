#ifndef WOW_SOURCE_WORLDCLIENT_POLYCLIPPER_H
#define WOW_SOURCE_WORLDCLIENT_POLYCLIPPER_H

#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"

class C4Plane;
class PolyClipper;

typedef void* (*PCInterpolateFunc)(const void* a, const void* b, float t);

class PCDataProxy {
    public:
    /* 0000 */ const void* m_data;
    /* 0004 */ PCDataProxy* m_a;
    /* 0008 */ PCDataProxy* m_b;
    /* 000C */ float m_t;

    const void* GetData(PCInterpolateFunc func);
};

class PCResultsIterator {
    public:
    /* 0000 */ const PolyClipper* m_clipper;
    /* 0004 */ PCInterpolateFunc m_func;
    /* 0008 */ int32_t m_vertex[2];
    /* 0010 */ uint32_t m_step;
    /* 0014 */ bool m_finished;

    PCResultsIterator(const PolyClipper* clipper, PCInterpolateFunc func);
    ~PCResultsIterator();
    void GetVertex(C3Vector& pos, const void*& data);
    bool HasFinished() const;
    void Next();
};

class PolyClipper {
    public:
    struct Vertex {
        /* 0000 */ C3Vector m_pos;
        /* 000C */ PCDataProxy m_proxy;
        /* 001C */ int32_t m_edge[2];
        /* 0024 */ uint32_t m_stamp;
        /* 0028 */ float m_dist;
    };

    struct Edge {
        /* 0000 */ uint32_t m_vertex[2];
        /* 0008 */ uint8_t m_clipped;
    };

    /* 0000 */ Vertex m_vertices[32];
    /* 0580 */ Edge m_edges[16];
    /* 0640 */ int32_t m_numVertices;
    /* 0644 */ int32_t m_numEdges;
    /* 0648 */ uint32_t m_clipStamp;
    /* 064C */ int32_t m_firstEdge;

    PolyClipper();
    ~PolyClipper();
    void AddVertex(const C3Vector& pos, const void* data);
    void Begin();
    void Clip(const C4Plane& plane, int32_t sign);
    void End();
    PCResultsIterator GetResults(PCInterpolateFunc func) const;
};

#endif
