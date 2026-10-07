#include <cmath>
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

// PROVEN: 11751086 IDENTICAL/IDENTICAL
// OFFSET: 0x00B145D0/0x1408DA450
void C4Vector::NormalizeSafe() {
    float length = this->w * this->w + this->z * this->z + this->y * this->y + this->x * this->x;

    if (length > 0.00000023841858f) {
        float scale = 1.0f / sqrt(length);
        this->x = this->x * scale;
        this->y = this->y * scale;
        this->z = scale * this->z;
        this->w = scale * this->w;
    }
}

// PROVEN: 24bba750 IDENTICAL/IDENTICAL
// OFFSET: 0x004CBDE0/0x140107B50
C4Vector::C4Vector(const CImVector& color) {
    this->x = color.r * 0.0039215689f;
    this->y = color.g * 0.0039215689f;
    this->z = color.b * 0.0039215689f;
    this->w = 0.0039215689f * color.a;
}

// PROVEN: ade01e28 IDENTICAL/IDENTICAL
// OFFSET: 0x004CBE40/0x140107BB0
void C4Vector::Set(const CImVector& color) {
    this->x = color.r * 0.0039215689f;
    this->y = color.g * 0.0039215689f;
    this->z = color.b * 0.0039215689f;
    this->w = color.a * 0.0039215689f;
}
