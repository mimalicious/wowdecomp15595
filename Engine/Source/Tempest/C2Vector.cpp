#include <cmath>
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"

// PROVEN: 1b6ababf IDENTICAL/IDENTICAL
// OFFSET: 0x004C9220/0x1401040E0
void C2Vector::NormalizeSafe() {
    float length = this->y * this->y + this->x * this->x;

    if (length > 0.00000023841858f) {
        float scale = 1.0f / sqrt(length);
        this->x = this->x * scale;
        this->y = scale * this->y;
    }
}

// PROVEN: b257d2b3 IDENTICAL/IDENTICAL
// OFFSET: 0x004CB510/0x140107180
C2Vector::C2Vector(const C3Vector& v) {
    this->x = v.x;
    this->y = v.y;
}

// PROVEN: 06dda673 IDENTICAL/IDENTICAL
// OFFSET: 0x004CB510/0x140107180
C2Vector::C2Vector(const C4Vector& v) {
    this->x = v.x;
    this->y = v.y;
}

// PROVEN: 677f5fd4 EQUIVALENT/NONE
// OFFSET: 0x00586610
void C2Vector::Normalize() {
    float scale = 1.0f / sqrt(this->x * this->x + this->y * this->y);
    this->x = this->x * scale;
    this->y = scale * this->y;
}
