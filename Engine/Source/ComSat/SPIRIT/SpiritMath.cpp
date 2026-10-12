#include <cstdint>
#include <cstring>
#include "Engine/Source/ComSat/SPIRIT/ipmr.h"

extern "C" {

// OFFSET: 0x00B4CB60
void SPIRIT_IP_MR_V3_NAME_191(float* dst, const float* src) {
    for (int32_t i = 0; i < 40; i++) {
        dst[i] = src[i];
    }
}

// OFFSET: 0x00B4CB90
void SPIRIT_IP_MR_V3_NAME_190(float* dst, const float* src) {
    for (int32_t i = 0; i < 10; i++) {
        dst[i] = src[i];
    }
}

// OFFSET: 0x00B4CBB0
void SPIRIT_IP_MR_V3_NAME_189(float* dst, const float* src, int32_t count) {
    while (count) {
        count--;
        *dst++ = *src++;
    }
}

// OFFSET: 0x00B4CBE0
void SPIRIT_IP_MR_V3_NAME_333(float* dst) {
    for (int32_t i = 0; i < 40; i++) {
        dst[i] = 0.0f;
    }
}

// OFFSET: 0x00B4CC10
void SPIRIT_IP_MR_V3_NAME_314(float* dst) {
    for (int32_t i = 0; i < 10; i++) {
        dst[i] = 0.0f;
    }
}

// OFFSET: 0x00B4CC30
void SPIRIT_IP_MR_V3_NAME_332(float* dst, int32_t count) {
    if (count) {
        memset(dst, 0, count * sizeof(float));
    }
}

// OFFSET: 0x00B4CC50
void SPIRIT_IP_MR_V3_NAME_334(void* dst, uint32_t size) {
    if (size) {
        memset(dst, 0, size);
    }
}

// OFFSET: 0x00B4CC70
void SPIRIT_IP_MR_V3_NAME_179(float* dst, const float* src, int32_t count) {
    while (count) {
        count--;
        *dst = *dst + *src;
        dst++;
        src++;
    }
}

// OFFSET: 0x00B4CCA0
void SPIRIT_IP_MR_V3_AddSubframe(float* dst, const float* src) {
    for (int32_t i = 0; i < 40; i++) {
        dst[i] = dst[i] + src[i];
    }
}

// OFFSET: 0x00B4CCF0
void SPIRIT_IP_MR_V3_NAME_386(float* dst, float value) {
    for (int32_t i = 0; i < 10; i++) {
        dst[i] = dst[i] + value;
    }
}

// OFFSET: 0x00B4CD30
void SPIRIT_IP_MR_V3_NAME_322(float* dst, const float* src) {
    for (int32_t i = 0; i < 40; i++) {
        dst[i] = dst[i] - src[i];
    }
}

// OFFSET: 0x00B4CD80
void SPIRIT_IP_MR_V3_NAME_321(float* dst, const float* src, int32_t count) {
    while (count) {
        count--;
        *dst = *dst - *src;
        dst++;
        src++;
    }
}

// OFFSET: 0x00B4CDB0
void SPIRIT_IP_MR_V3_NAME_248(float* dst, float value) {
    for (int32_t i = 0; i < 40; i++) {
        dst[i] = dst[i] * value;
    }
}

// OFFSET: 0x00B4D710
float SPIRIT_IP_MR_V3_NAME_42(const float* a, const float* b, int32_t count) {
    float sum = 0.0f;

    for (int32_t i = 0; i < count; i++) {
        sum += a[i] * b[i];
    }

    return sum;
}

// OFFSET: 0x00B4DA50
int32_t SPIRIT_IP_MR_V3_NAME_66(int32_t index) {
    switch (index) {
        case 0:
            return 8000;

        case 1:
            return 16000;

        case 2:
            return 32000;
    }

    return 0;
}

// OFFSET: 0x00B4DA80
int32_t SPIRIT_IP_MR_V3_NAME_65(int32_t rate) {
    switch (rate) {
        case 8000:
            return 0;

        case 16000:
            return 1;

        case 32000:
            return 2;
    }

    return 0;
}

}
