#include "Engine/Source/Services/Panama.h"
#include <cstring>
#include "Engine/Source/Os/Processor.h"

#define PANAMA_A(i) aPtr[((i) * 13 + 16) % 17]
#define PANAMA_C(i) cPtr[((i) * 13 + 16) % 17]
#define PANAMA_B(i, j) b##i[(j) * 2 % 8 + (j) / 4]
#define PANAMA_OA(i) z[i] = PANAMA_A(i + 9)
#define PANAMA_US(i) { uint32_t t = PANAMA_B(0, i); PANAMA_B(0, i) = p[i] ^ t; PANAMA_B(25, (i + 6) % 8) ^= t; }
#define PANAMA_UL(i) { uint32_t t = PANAMA_B(0, i); PANAMA_B(0, i) = PANAMA_A(i + 1) ^ t; PANAMA_B(25, (i + 6) % 8) ^= t; }
#define PANAMA_GP(i) PANAMA_C(5 * i % 17) = RotateLeft(PANAMA_A(i) ^ (PANAMA_A((i + 1) % 17) | ~PANAMA_A((i + 2) % 17)), ((5 * i % 17) * ((5 * i % 17) + 1) / 2) % 32)
#define PANAMA_T(i, x) PANAMA_A(i) = PANAMA_C(i) ^ PANAMA_C((i + 1) % 17) ^ PANAMA_C((i + 4) % 17) ^ (x)
#define PANAMA_TS1S(i) PANAMA_T(i + 1, p[i])
#define PANAMA_TS1L(i) PANAMA_T(i + 1, PANAMA_B(4, i))
#define PANAMA_TS2(i) PANAMA_T(i + 9, PANAMA_B(16, i))

// INLINED
static inline uint32_t RotateLeft(uint32_t value, uint32_t shift) {
    return shift ? (value << shift) | (value >> (32 - shift)) : value;
}

// INLINED
Crypt::Panama::Panama() {
    this->m_stateSize = 1104;
    this->m_state = reinterpret_cast<uint32_t*>((reinterpret_cast<uintptr_t>(this->m_stateData) + 15) & ~static_cast<uintptr_t>(15));
}

// INLINED
Crypt::Panama::~Panama() {
    memset(this->m_stateData, 0, sizeof(this->m_stateData));
}

// OFFSET: %0x0054E540
void Crypt::Panama::Reset() {
    memset(this->m_state, 0, this->m_stateSize);
    this->m_state[17] = (OsGetProcessorFeatures() & 0x80) != 0;
}

// OFFSET: %0x0054E570
Crypt::PanamaCypher::PanamaCypher() {
    this->m_leftOver = 0;
    this->m_bufferSize = 1024;
    this->m_buffer = reinterpret_cast<uint8_t*>((reinterpret_cast<uintptr_t>(this->m_bufferData) + 15) & ~static_cast<uintptr_t>(15));
}

// PROVEN: fb8b4914 UNPROVEN/IDENTICAL
// OFFSET: 0x00A1D270/0x1407C3230
Crypt::PanamaCypher::~PanamaCypher() {
    memset(this->m_bufferData, 0, sizeof(this->m_bufferData));
}

// OFFSET: %0x0054E950
void Crypt::PanamaCypher::Reset() {
    Crypt::Panama::Reset();
    this->m_leftOver = 0;
}

// OFFSET: %0x0054E990
void __stdcall Crypt::PanamaCypher::xorbuf(uint8_t* buf, const uint8_t* mask, uint32_t count) {
    if (!(reinterpret_cast<uintptr_t>(buf) & 3) && !(reinterpret_cast<uintptr_t>(mask) & 3)) {
        if (!(reinterpret_cast<uintptr_t>(buf) & 7) && !(reinterpret_cast<uintptr_t>(mask) & 7)) {
            for (uint32_t i = 0; i < count / 8; i++) {
                reinterpret_cast<uint32_t*>(buf)[2 * i] ^= reinterpret_cast<const uint32_t*>(mask)[2 * i];
                reinterpret_cast<uint32_t*>(buf)[2 * i + 1] ^= reinterpret_cast<const uint32_t*>(mask)[2 * i + 1];
            }

            uint32_t done = 8 * (count / 8);
            count -= done;

            if (!count) {
                return;
            }

            buf += done;
            mask += done;
        }

        for (uint32_t i = 0; i < count / 4; i++) {
            reinterpret_cast<uint32_t*>(buf)[i] ^= reinterpret_cast<const uint32_t*>(mask)[i];
        }

        uint32_t done = 4 * (count / 4);
        count -= done;

        if (!count) {
            return;
        }

        buf += done;
        mask += done;
    }

    for (uint32_t i = 0; i < count; i++) {
        buf[i] ^= mask[i];
    }
}

// OFFSET: %0x0054EA70
void Crypt::Panama::Iterate(uint32_t count, const uint32_t* p, uint32_t* z) {
    uint32_t* const aPtr = this->m_state;
    uint32_t bstart = aPtr[17];
    uint8_t* const bPtr = reinterpret_cast<uint8_t*>(aPtr + 20);
    uint32_t cPtr[17];

    while (count--) {
        if (z) {
            PANAMA_OA(0);
            PANAMA_OA(1);
            PANAMA_OA(2);
            PANAMA_OA(3);
            PANAMA_OA(4);
            PANAMA_OA(5);
            PANAMA_OA(6);
            PANAMA_OA(7);
            z += 8;
        }

        uint32_t* const b16 = reinterpret_cast<uint32_t*>(bPtr + ((bstart + 16 * 32) & (31 * 32)));
        uint32_t* const b4 = reinterpret_cast<uint32_t*>(bPtr + ((bstart + (32 - 4) * 32) & (31 * 32)));
        bstart += 32;
        uint32_t* const b0 = reinterpret_cast<uint32_t*>(bPtr + (bstart & (31 * 32)));
        uint32_t* const b25 = reinterpret_cast<uint32_t*>(bPtr + ((bstart + (32 - 25) * 32) & (31 * 32)));

        if (p) {
            PANAMA_US(0);
            PANAMA_US(1);
            PANAMA_US(2);
            PANAMA_US(3);
            PANAMA_US(4);
            PANAMA_US(5);
            PANAMA_US(6);
            PANAMA_US(7);
        } else {
            PANAMA_UL(0);
            PANAMA_UL(1);
            PANAMA_UL(2);
            PANAMA_UL(3);
            PANAMA_UL(4);
            PANAMA_UL(5);
            PANAMA_UL(6);
            PANAMA_UL(7);
        }

        PANAMA_GP(0);
        PANAMA_GP(1);
        PANAMA_GP(2);
        PANAMA_GP(3);
        PANAMA_GP(4);
        PANAMA_GP(5);
        PANAMA_GP(6);
        PANAMA_GP(7);
        PANAMA_GP(8);
        PANAMA_GP(9);
        PANAMA_GP(10);
        PANAMA_GP(11);
        PANAMA_GP(12);
        PANAMA_GP(13);
        PANAMA_GP(14);
        PANAMA_GP(15);
        PANAMA_GP(16);

        PANAMA_T(0, 1);

        if (p) {
            PANAMA_TS1S(0);
            PANAMA_TS1S(1);
            PANAMA_TS1S(2);
            PANAMA_TS1S(3);
            PANAMA_TS1S(4);
            PANAMA_TS1S(5);
            PANAMA_TS1S(6);
            PANAMA_TS1S(7);
            p += 8;
        } else {
            PANAMA_TS1L(0);
            PANAMA_TS1L(1);
            PANAMA_TS1L(2);
            PANAMA_TS1L(3);
            PANAMA_TS1L(4);
            PANAMA_TS1L(5);
            PANAMA_TS1L(6);
            PANAMA_TS1L(7);
        }

        PANAMA_TS2(0);
        PANAMA_TS2(1);
        PANAMA_TS2(2);
        PANAMA_TS2(3);
        PANAMA_TS2(4);
        PANAMA_TS2(5);
        PANAMA_TS2(6);
        PANAMA_TS2(7);
    }

    aPtr[17] = bstart;
}

// OFFSET: %0x0054EED0
void Crypt::Panama::SetKey(const uint8_t* key, const uint8_t* iv) {
    this->Reset();
    this->Iterate(1, reinterpret_cast<const uint32_t*>(key), NULL);

    if (iv) {
        this->Iterate(1, reinterpret_cast<const uint32_t*>(iv), NULL);
    } else {
        this->Iterate(1, reinterpret_cast<const uint32_t*>(key), NULL);
    }

    this->Iterate(32, NULL, NULL);
}

// OFFSET: %0x0054EF40
void Crypt::PanamaCypher::ProcessBuffer(uint8_t* buffer, uint32_t size) {
    uint32_t count = this->m_leftOver < size ? this->m_leftOver : size;

    if (count) {
        Crypt::PanamaCypher::xorbuf(buffer, this->m_buffer + this->m_bufferSize - this->m_leftOver, count);
        this->m_leftOver -= count;
        size -= count;
        buffer += count;
    }

    while (size) {
        this->m_leftOver = this->m_bufferSize;
        this->Iterate(this->m_bufferSize / 32, NULL, reinterpret_cast<uint32_t*>(this->m_buffer));

        count = this->m_leftOver < size ? this->m_leftOver : size;
        Crypt::PanamaCypher::xorbuf(buffer, this->m_buffer, count);
        this->m_leftOver -= count;
        size -= count;
        buffer += count;
    }
}
