#include <cmath>
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"

// OFFSET: %0x004CEF40
void C2Vector::NormalizeSafe() {
    float length = this->y * this->y + this->x * this->x;

    if (length > 0.00000023841858f) {
        float scale = 1.0f / sqrt(length);
        this->x = this->x * scale;
        this->y = scale * this->y;
    }
}

// OFFSET: %0x004D1210
C2Vector::C2Vector(const C3Vector& v) {
    this->x = v.x;
    this->y = v.y;
}

// OFFSET: %0x004D1210
C2Vector::C2Vector(const C4Vector& v) {
    this->x = v.x;
    this->y = v.y;
}

// OFFSET: %0x005938B0
void C2Vector::Normalize() {
    float scale = 1.0f / sqrt(this->x * this->x + this->y * this->y);
    this->x = this->x * scale;
    this->y = scale * this->y;
}
