#ifndef WOW_SOURCE_WORLDCLIENT_CWORLDMATH_H
#define WOW_SOURCE_WORLDCLIENT_CWORLDMATH_H

#include <cstdint>

class C3Vector;
class C44Matrix;
class C4Plane;
class CAaBox;

class CWorldMath {
    public:
    static void ClipVertexListToPlane(const C3Vector* verts, uint32_t count, C3Vector* out, uint32_t& outCount, const C4Plane& plane, float epsilon, int32_t keepCoplanar);
    static void TransformAABox(const C44Matrix& m, const CAaBox& src, CAaBox& dst);
    static bool VectorIntersectAABox2(const CAaBox& box, const C3Vector& start, const C3Vector& end);
};

#endif
