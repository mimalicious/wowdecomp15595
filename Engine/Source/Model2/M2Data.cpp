#include "Engine/Source/Model2/M2Data.h"

// INLINED
ubyte4::operator uint32_t() const {
    return *reinterpret_cast<const uint32_t*>(this->b);
}

// INLINED
uint8_t ubyte4::operator[](uint32_t index) const {
    return this->b[index];
}

// OFFSET: 0x00830AD0/0x140551E20
M2Data::M2Data() {
}

// OFFSET: 0x00830C40/0x140551F80
M2Data::~M2Data() {
}

// OFFSET: 0x008300D0/0x140550F40
M2CompBone::~M2CompBone() {
}

// OFFSET: [0x00830150/0x140551010, 0x008304D0/0x1405515E0]
M2Color::~M2Color() {
}

// OFFSET: 0x008301B0/0x1405510A0
M2TextureTransform::~M2TextureTransform() {
}

// OFFSET: [0x00830230/0x140551170, 0x00830850/0x140551C40]
M2Light::~M2Light() {
}

// OFFSET: [0x00830350/0x140551360, 0x008308F0/0x140551CE0]
M2Ribbon::~M2Ribbon() {
}

// OFFSET: 0x0082FC20/0x140550780
M2Camera::~M2Camera() {
}

// OFFSET: [0x0082FCC0/0x140550890, 0x00830990/0x140551D80]
M2Particle::~M2Particle() {
}

// OFFSET: [0x00830630/0x1405517E0, 0x00830630/0x1405517E0]
M2TextureWeight::~M2TextureWeight() {
}

// OFFSET: [0x00830740/0x140551990, 0x00830740/0x140551990]
M2Attachment::~M2Attachment() {
}

// OFFSET: [0x00830010/0x140550E10, 0x00830010/0x140550E10]
M2Event::~M2Event() {
}

// OFFSET: [0x0082CB40/0x14054CAD0, 0x0082CB40/0x14054CAD0]
M2Texture::~M2Texture() {
}
