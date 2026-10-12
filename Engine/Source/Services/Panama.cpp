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

// OFFSET: 0x00A1D200/0x1407C31A0
void Crypt::Panama::Reset() {
    memset(this->m_state, 0, this->m_stateSize);
    this->m_state[17] = (OsGetProcessorFeatures() & 0x80) != 0;
}

// PROVEN: e2cde23b IDENTICAL/DIFFERENT
// OFFSET: 0x00A1D230/0x1407C31E0
Crypt::PanamaCypher::PanamaCypher() {
    this->m_leftOver = 0;
    this->m_bufferSize = 1024;
    this->m_buffer = reinterpret_cast<uint8_t*>((reinterpret_cast<uintptr_t>(this->m_bufferData) + 15) & ~static_cast<uintptr_t>(15));
}

// PROVEN: fb8b4914 IDENTICAL/IDENTICAL
// OFFSET: 0x00A1D270/0x1407C3230
Crypt::PanamaCypher::~PanamaCypher() {
    memset(this->m_bufferData, 0, sizeof(this->m_bufferData));
}

#if defined(_M_IX86)
// OFFSET: 0x00A1D2B0/0x1407C5760
static __declspec(naked) void __cdecl Panama_SSE2_Pull(uint32_t count, uint32_t* state, uint32_t* z) {
    __asm {
        push ebp
        mov ebp, esp
        push ebx
        push esi
        mov ecx, dword ptr [ebp + 8]
        mov edx, dword ptr [ebp + 0xc]
        mov esi, dword ptr [ebp + 0x10]
        shl ecx, 5
        je L_A1D60A
        mov ebx, dword ptr [edx + 0x44]
        add ecx, ebx
        push ebp
        push ecx
        movdqa xmm0, xmmword ptr [edx]
        movdqa xmm1, xmmword ptr [edx + 0x10]
        movdqa xmm2, xmmword ptr [edx + 0x20]
        movdqa xmm3, xmmword ptr [edx + 0x30]
        mov eax, dword ptr [edx + 0x40]
    L_A1D2E4:
        test ebx, 1
        jne L_A1D30C
        movdqa xmm6, xmm2
        movss xmm6, xmm3
        pshufd xmm5, xmm6, 0x39
        movd xmm6, eax
        movdqa xmm7, xmm3
        movss xmm7, xmm6
        pshufd xmm6, xmm7, 0x39
        jmp L_A1D320
    L_A1D30C:
        movdqa xmm5, xmm3
        palignr xmm5, xmm2, 4
        movd xmm6, eax
        palignr xmm6, xmm3, 4
    L_A1D320:
        movd ecx, xmm2
        not ecx
        movd ebp, xmm3
        or ecx, ebp
        xor eax, ecx
        pcmpeqb xmm7, xmm7
        pxor xmm7, xmm1
        por xmm7, xmm2
        pxor xmm7, xmm3
        movd ecx, xmm7
        rol ecx, 0xf
        mov dword ptr [edx + 0x34], ecx
        pshuflw xmm7, xmm7, 0x4e
        movd ecx, xmm7
        rol ecx, 4
        mov dword ptr [edx + 4], ecx
        punpckhqdq xmm7, xmm7
        movd ecx, xmm7
        rol ecx, 2
        mov dword ptr [edx + 0x18], ecx
        pshuflw xmm7, xmm7, 0x4e
        movd ecx, xmm7
        rol ecx, 9
        mov dword ptr [edx + 0x2c], ecx
        pcmpeqb xmm7, xmm7
        pxor xmm7, xmm0
        por xmm7, xmm1
        pxor xmm7, xmm2
        movd ecx, xmm7
        rol ecx, 0x17
        mov dword ptr [edx + 0x28], ecx
        pshuflw xmm7, xmm7, 0x4e
        movd ecx, xmm7
        rol ecx, 0x1b
        mov dword ptr [edx + 0x3c], ecx
        punpckhqdq xmm7, xmm7
        movd ecx, xmm7
        rol ecx, 8
        mov dword ptr [edx + 0xc], ecx
        pshuflw xmm7, xmm7, 0x4e
        movd ecx, xmm7
        rol ecx, 3
        mov dword ptr [edx + 0x20], ecx
        pcmpeqb xmm7, xmm7
        pxor xmm7, xmm6
        por xmm7, xmm0
        pxor xmm7, xmm1
        movd ecx, xmm7
        rol ecx, 0x18
        mov dword ptr [edx + 0x1c], ecx
        pshuflw xmm7, xmm7, 0x4e
        movd ecx, xmm7
        rol ecx, 1
        mov dword ptr [edx + 0x30], ecx
        punpckhqdq xmm7, xmm7
        movd ecx, xmm7
        rol ecx, 0xa
        mov dword ptr [edx], ecx
        pshuflw xmm7, xmm7, 0x4e
        movd ecx, xmm7
        rol ecx, 0x1c
        mov dword ptr [edx + 0x14], ecx
        pcmpeqb xmm7, xmm7
        pxor xmm7, xmm5
        por xmm7, xmm6
        pxor xmm7, xmm0
        movd ecx, xmm7
        rol ecx, 6
        mov dword ptr [edx + 0x10], ecx
        pshuflw xmm7, xmm7, 0x4e
        movd ecx, xmm7
        rol ecx, 0x15
        mov dword ptr [edx + 0x24], ecx
        punpckhqdq xmm7, xmm7
        movd ecx, xmm7
        rol ecx, 0xd
        mov dword ptr [edx + 0x38], ecx
        pshuflw xmm7, xmm7, 0x4e
        movd ecx, xmm7
        rol ecx, 0xe
        mov dword ptr [edx + 8], ecx
        movdqa xmm4, xmm3
        punpcklqdq xmm3, xmm2
        punpckhdq xmm4, xmm2
        movdqa xmm2, xmm1
        punpcklqdq xmm1, xmm0
        punpckhdq xmm2, xmm0
        test esi, esi
        je L_A1D48E
        movdqa xmm6, xmm4
        punpcklqdq xmm4, xmm2
        punpckhqdq xmm6, xmm2
        test esi, 0xf
        jne L_A1D482
        movdqa xmmword ptr [esi], xmm4
        movdqa xmmword ptr [esi + 0x10], xmm6
        add esi, 0x20
        jmp L_A1D48E
    L_A1D482:
        movdqu xmmword ptr [esi], xmm4
        movdqu xmmword ptr [esi + 0x10], xmm6
        add esi, 0x20
    L_A1D48E:
        lea ecx, [ebx + 0x20]
        and ecx, 0x3e0
        lea ebp, [ebx + 0x100]
        and ebp, 0x3e0
        movdqa xmm0, xmmword ptr [edx + ecx + 0x50]
        pxor xmm3, xmm0
        pshufd xmm0, xmm0, 0xb1
        movdqa xmmword ptr [edx + ecx + 0x50], xmm3
        pxor xmm0, xmmword ptr [edx + ebp + 0x60]
        movdqa xmmword ptr [edx + ebp + 0x60], xmm0
        movdqa xmm4, xmmword ptr [edx + ecx + 0x60]
        pxor xmm1, xmm4
        movdqa xmmword ptr [edx + ecx + 0x60], xmm1
        pxor xmm4, xmmword ptr [edx + ebp + 0x50]
        movdqa xmmword ptr [edx + ebp + 0x50], xmm4
        movdqa xmm3, xmmword ptr [edx + 0x30]
        movdqa xmm2, xmmword ptr [edx + 0x20]
        movdqa xmm1, xmmword ptr [edx + 0x10]
        movdqa xmm0, xmmword ptr [edx]
        test ebx, 1
        jne L_A1D535
        movd xmm6, eax
        movdqa xmm7, xmm3
        movss xmm7, xmm6
        movdqa xmm6, xmm2
        movss xmm6, xmm3
        movdqa xmm5, xmm1
        movss xmm5, xmm2
        movdqa xmm4, xmm0
        movss xmm4, xmm1
        pshufd xmm7, xmm7, 0x39
        pshufd xmm6, xmm6, 0x39
        pshufd xmm5, xmm5, 0x39
        pshufd xmm4, xmm4, 0x39
        jmp L_A1D55D
    L_A1D535:
        movd xmm7, eax
        palignr xmm7, xmm3, 4
        movq xmm6, xmm3
        palignr xmm6, xmm2, 4
        movq xmm5, xmm2
        palignr xmm5, xmm1, 4
        movq xmm4, xmm1
        palignr xmm4, xmm0, 4
    L_A1D55D:
        xor eax, 1
        movd ecx, xmm0
        xor eax, ecx
        movd ecx, xmm3
        xor eax, ecx
        pxor xmm3, xmm2
        pxor xmm2, xmm1
        pxor xmm1, xmm0
        pxor xmm0, xmm7
        pxor xmm3, xmm7
        pxor xmm2, xmm6
        pxor xmm1, xmm5
        pxor xmm0, xmm4
        lea ecx, [ebx + 0x380]
        and ecx, 0x3e0
        lea ebp, [ebx + 0x200]
        and ebp, 0x3e0
        movdqa xmm4, xmmword ptr [edx + ecx + 0x50]
        movdqa xmm5, xmmword ptr [edx + ebp + 0x50]
        movdqa xmm6, xmm4
        punpcklqdq xmm4, xmm5
        punpckhqdq xmm6, xmm5
        pxor xmm3, xmm4
        pxor xmm2, xmm6
        movdqa xmm4, xmmword ptr [edx + ecx + 0x60]
        movdqa xmm5, xmmword ptr [edx + ebp + 0x60]
        movdqa xmm6, xmm4
        punpcklqdq xmm4, xmm5
        punpckhqdq xmm6, xmm5
        pxor xmm1, xmm4
        pxor xmm0, xmm6
        add ebx, 0x20
        cmp ebx, dword ptr [esp]
        jne L_A1D2E4
        mov dword ptr [edx + 0x40], eax
        movdqa xmmword ptr [edx + 0x30], xmm3
        movdqa xmmword ptr [edx + 0x20], xmm2
        movdqa xmmword ptr [edx + 0x10], xmm1
        movdqa xmmword ptr [edx], xmm0
        add esp, 4
        pop ebp
    L_A1D60A:
        pop esi
        pop ebx
        pop ebp
        ret
    }
}
#endif

// OFFSET: 0x00A1D610/0x1407C3270
void Crypt::PanamaCypher::Reset() {
    Crypt::Panama::Reset();
    this->m_leftOver = 0;
}

// OFFSET: 0x00A1D650/0x1407C32B0
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

// OFFSET: 0x00A1D730/0x1407C33A0
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

// OFFSET: 0x00A1DB90/0x1407C3990
void Crypt::Panama::SetKey(const uint8_t* key, const uint8_t* iv) {
    this->Reset();
    this->Iterate(1, reinterpret_cast<const uint32_t*>(key), NULL);

    if (iv) {
        this->Iterate(1, reinterpret_cast<const uint32_t*>(iv), NULL);
    } else {
        this->Iterate(1, reinterpret_cast<const uint32_t*>(key), NULL);
    }

#if defined(_M_IX86)
    if (OsGetProcessorFeatures() & 0x10) {
        Panama_SSE2_Pull(32, this->m_state, NULL);
        return;
    }
#endif

    this->Iterate(32, NULL, NULL);
}

// OFFSET: 0x00A1DC00/0x1407C3A00
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

#if defined(_M_IX86)
        if (!(reinterpret_cast<uintptr_t>(this->m_buffer) & 0xF) && (OsGetProcessorFeatures() & 0x10)) {
            Panama_SSE2_Pull(this->m_bufferSize / 32, this->m_state, reinterpret_cast<uint32_t*>(this->m_buffer));
        } else
#endif
        {
            this->Iterate(this->m_bufferSize / 32, NULL, reinterpret_cast<uint32_t*>(this->m_buffer));
        }

        count = this->m_leftOver < size ? this->m_leftOver : size;
        Crypt::PanamaCypher::xorbuf(buffer, this->m_buffer, count);
        this->m_leftOver -= count;
        size -= count;
        buffer += count;
    }
}
