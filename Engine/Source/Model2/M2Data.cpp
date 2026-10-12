#include "Engine/Source/Model2/M2Data.h"

// INLINED
ubyte4::operator uint32_t() const {
    return *reinterpret_cast<const uint32_t*>(this->b);
}

// INLINED
uint8_t ubyte4::operator[](uint32_t index) const {
    return this->b[index];
}

// PROVEN: f4076397 IDENTICAL/IDENTICAL
// OFFSET: 0x00830AD0/0x140551E20
M2Data::M2Data() {
}

// PROVEN: 1317d348 IDENTICAL/NONE
// OFFSET: 0x00830C40/0x140551F80
M2Data::~M2Data() {
}

// PROVEN: 127d7bdc IDENTICAL/NONE
// OFFSET: 0x008300D0
M2CompBone::~M2CompBone() {
}

// PROVEN: 01cca05f IDENTICAL/NONE
// OFFSET: [0x00830150, 0x008304D0]
M2Color::~M2Color() {
}

// PROVEN: ca894430 IDENTICAL/NONE
// OFFSET: 0x008301B0
M2TextureTransform::~M2TextureTransform() {
}

// PROVEN: d38a2ab8 IDENTICAL/NONE
// OFFSET: [0x00830230, 0x00830850]
M2Light::~M2Light() {
}

// PROVEN: 20fe2baf IDENTICAL/NONE
// OFFSET: [0x00830350, 0x008308F0]
M2Ribbon::~M2Ribbon() {
}

// PROVEN: 5fbc0039 IDENTICAL/NONE
// OFFSET: 0x0082FC20
M2Camera::~M2Camera() {
}

// OFFSET: [0x0082FCC0, 0x00830990]
M2Particle::~M2Particle() {
}

// PROVEN: 20c586b5 IDENTICAL/NONE
// OFFSET: [0x00830630, 0x00830630]
M2TextureWeight::~M2TextureWeight() {
}

// PROVEN: 8714500b IDENTICAL/NONE
// OFFSET: [0x00830740, 0x00830740]
M2Attachment::~M2Attachment() {
}

// PROVEN: 05c27210 IDENTICAL/NONE
// OFFSET: [0x00830010, 0x00830010]
M2Event::~M2Event() {
}

// PROVEN: f2860482 IDENTICAL/NONE
// OFFSET: [0x0082CB40, 0x0082CB40]
M2Texture::~M2Texture() {
}
