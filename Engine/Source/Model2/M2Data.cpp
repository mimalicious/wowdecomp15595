#include "Engine/Source/Model2/M2Data.h"

// INLINED
ubyte4::operator uint32_t() const {
    return *reinterpret_cast<const uint32_t*>(this->b);
}

// INLINED
uint8_t ubyte4::operator[](uint32_t index) const {
    return this->b[index];
}

// INLINED
fixed16::fixed16(float value) {
    if (value < 1.0f) {
        if (value > -1.0f) {
            this->m_value = static_cast<int16_t>(value * 32767.0f + 0.5f);
        } else {
            this->m_value = -32767;
        }
    } else {
        this->m_value = 0x7FFF;
    }
}

// PROVEN: f4076397 IDENTICAL/NONE
// OFFSET: 0x00830AD0
M2Data::M2Data() {
}

// OFFSET: 0x00830C40
M2Data::~M2Data() {
}

// OFFSET: 0x008300D0
M2CompBone::~M2CompBone() {
}

// OFFSET: [0x00830150, 0x008304D0]
M2Color::~M2Color() {
}

// OFFSET: 0x008301B0
M2TextureTransform::~M2TextureTransform() {
}

// OFFSET: [0x00830230, 0x00830850]
M2Light::~M2Light() {
}

// OFFSET: [0x00830350, 0x008308F0]
M2Ribbon::~M2Ribbon() {
}

// OFFSET: 0x0082FC20
M2Camera::~M2Camera() {
}

// OFFSET: [0x0082FCC0, 0x00830990]
M2Particle::~M2Particle() {
}

// OFFSET: [0x00830630, 0x00830630]
M2TextureWeight::~M2TextureWeight() {
}

// OFFSET: [0x00830740, 0x00830740]
M2Attachment::~M2Attachment() {
}

// OFFSET: [0x00830010, 0x00830010]
M2Event::~M2Event() {
}

// OFFSET: [0x0082CB40, 0x0082CB40]
M2Texture::~M2Texture() {
}
