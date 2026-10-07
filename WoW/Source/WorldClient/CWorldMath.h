#ifndef WOW_SOURCE_WORLDCLIENT_CWORLDMATH_H
#define WOW_SOURCE_WORLDCLIENT_CWORLDMATH_H

#include <cstdint>

class C3Vector;
class C44Matrix;
class CAaBox;

class CWorldMath {
    public:
    static void TransformAABox(const C44Matrix& m, const CAaBox& src, CAaBox& dst);
    static bool VectorIntersectAABox2(const CAaBox& box, const C3Vector& start, const C3Vector& end);
};

#endif
