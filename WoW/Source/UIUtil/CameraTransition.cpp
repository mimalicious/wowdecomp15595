#include "WoW/Source/UIUtil/CameraTrack.h"

// INLINED
CCameraTransition::CCameraTransition() {
    this->m_flags = 0;
    this->m_duration = 0.0f;
    this->m_delay = 0.0f;
    this->m_easeIn = 0.0f;
    this->m_easeOut = 0.0f;
    this->unk_0014 = 0;
    this->unk_0018 = 0;
}

// OFFSET: 0x00463FF0/0x1400779A0
int32_t CCameraTransition::IsComplete(float time) const {
    return time >= this->m_delay + this->m_duration;
}

// OFFSET: 0x00464020/0x1400779C0
float CCameraTransition::CalcBezier(float t, float easeIn, float easeOut) {
    float a = (easeOut * 3.0f + easeIn * 3.0f) - 2.0f;
    float c = easeIn * 3.0f;
    float b = 3.0f - (easeIn * 6.0f + easeOut * 3.0f);
    float hi = 1.0f;
    float lo = 0.0f;
    float valueLo = 0.0f;
    float valueHi = 1.0f;

    for (int32_t i = 0; i < 8; i++) {
        float mid = (hi + lo) * 0.5f;
        float value = ((mid * a + b) * mid + c) * mid;

        if (value <= t) {
            lo = mid;
            valueLo = value;
        } else {
            hi = mid;
            valueHi = value;
        }
    }

    float f = (t - valueLo) / (valueHi - valueLo);
    float s = (1.0f - f) * lo + f * hi;
    return 3.0f * (s * s) - (s * s * s + s * s * s);
}

// OFFSET: 0x004641E0/0x140077BB0
float CCameraTransition::CalcLerpFactor(float time) const {
    if (this->m_duration <= 0.00000011920929f) {
        return CCameraTransition::CalcBezier(1.0f, this->m_easeIn, this->m_easeOut);
    }

    float t = (time - this->m_delay) / this->m_duration;

    if (t < 0.0f) {
        return CCameraTransition::CalcBezier(0.0f, this->m_easeIn, this->m_easeOut);
    }

    if (t >= 1.0f) {
        return CCameraTransition::CalcBezier(1.0f, this->m_easeIn, this->m_easeOut);
    }

    return CCameraTransition::CalcBezier(t, this->m_easeIn, this->m_easeOut);
}
