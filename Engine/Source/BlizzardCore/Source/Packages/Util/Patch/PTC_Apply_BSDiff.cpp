#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/Packages/Util/Patch/Patch.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: 0x007AA7A0
static int32_t ReadOffset(const uint32_t* value) {
    int32_t result = static_cast<int32_t>(*value);

    if (result >= 0) {
        return result;
    }

    return static_cast<int32_t>(0x80000000 - *value);
}

// OFFSET: 0x007B0690/0x1404A5450
bool Blizzard::Patch::ApplyBSDiffPatch(const uint8_t* patch, uint32_t patchSize, uint8_t* dest, uint32_t destSize, const uint8_t* source, uint32_t sourceSize) {
    if (patchSize < 32 || memcmp(patch, "BSDIFF40", 8)) {
        return false;
    }

    const uint32_t* header = reinterpret_cast<const uint32_t*>(patch);
    int32_t controlSize = ReadOffset(&header[2]);
    int32_t diffSize = ReadOffset(&header[4]);
    int32_t newSize = ReadOffset(&header[6]);

    if (controlSize < 0 || diffSize < 0 || newSize != static_cast<int32_t>(destSize)) {
        return false;
    }

    const uint32_t* control = reinterpret_cast<const uint32_t*>(patch + 32);
    const uint8_t* controlEnd = patch + 32 + controlSize;
    const uint8_t* diff = controlEnd;
    const uint8_t* diffEnd = diff + diffSize;
    const uint8_t* extra = diffEnd;
    uint32_t extraSize = patchSize - diffSize - controlSize - 32;
    int32_t newPos = 0;
    int32_t oldPos = 0;

    while (newPos < newSize) {
        int32_t values[3];

        for (int32_t i = 0; i < 3; i++) {
            uint32_t value = *control++;

            if (reinterpret_cast<const uint8_t*>(control) > controlEnd) {
                return false;
            }

            values[i] = ReadOffset(&value);
        }

        int32_t addSize = values[0];

        if (addSize + newPos > newSize) {
            return false;
        }

        uint8_t* out = dest + newPos;
        memcpy(out, diff, addSize);
        diff += addSize;

        if (diff > diffEnd) {
            return false;
        }

        const uint8_t* old = source + oldPos;
        const uint8_t* oldEnd = source + sourceSize;

        for (int32_t i = addSize; i > 0; i--) {
            if (old < oldEnd) {
                *out++ += *old++;
            }
        }

        int32_t copySize = values[1];
        int32_t copyPos = addSize + newPos;

        if (copySize + copyPos > newSize) {
            return false;
        }

        memcpy(dest + copyPos, extra, copySize);
        extra += copySize;

        if (extra > diffEnd + extraSize) {
            return false;
        }

        newPos = copySize + copyPos;
        oldPos += addSize + values[2];
    }

    if (reinterpret_cast<const uint8_t*>(control) < controlEnd || diff < diffEnd) {
        return false;
    }

    return extra >= diffEnd + extraSize;
}

// OFFSET: 0x007B0860
uint8_t* BSDiff::DecodeRLE(const uint8_t* data, uint32_t* size, uint32_t skip) {
    uint32_t outSize = *reinterpret_cast<const uint32_t*>(data) - skip;
    uint8_t* result = static_cast<uint8_t*>(SMemAlloc(outSize, __FILE__, __LINE__, 0));
    uint8_t* end = result + outSize;
    const uint8_t* in = data + 4;
    const uint8_t* inEnd = data + *size;
    uint8_t* out = result;

    while (in < inEnd) {
        uint8_t c = *in++;
        int32_t count = (c & 0x7F) + 1;

        if (count > end - out) {
            count = static_cast<int32_t>(end - out);
        }

        if (c & 0x80) {
            memcpy(out, in, count);
            in += count;
        } else {
            memset(out, 0, count);
        }

        out += count;
    }

    *size = outSize;

    return result;
}

// OFFSET: 0x007B8520
bool Blizzard::Patch::ApplyRLEBSDiffPatch(const uint8_t* patch, uint32_t patchSize, uint8_t* dest, uint32_t destSize, const uint8_t* source, uint32_t sourceSize) {
    uint8_t* decoded = BSDiff::DecodeRLE(patch, &patchSize, 0);
    bool result = ApplyBSDiffPatch(decoded, patchSize, dest, destSize, source, sourceSize);
    SMemFree(decoded, __FILE__, __LINE__, 0);

    return result;
}

// OFFSET: 0x007B8570
bool BSDiff::PTCApply_BSDiff(const uint8_t* patch, uint32_t patchSize, uint8_t* dest, uint32_t destSize, const uint8_t* source, uint32_t sourceSize) {
    if (patchSize < 32) {
        return false;
    }

    uint8_t transform = patch[3];

    if (transform == 4) {
        patchSize -= 24;
        uint8_t* decoded = DecodeRLE(patch + 24, &patchSize, 24);
        bool result = Blizzard::Patch::ApplyBSDiffPatch(decoded, patchSize, dest, destSize, source, sourceSize);
        SMemFree(decoded, __FILE__, __LINE__, 0);
        return result;
    }

    return transform == 2 && Blizzard::Patch::ApplyBSDiffPatch(patch + 24, patchSize - 24, dest, destSize, source, sourceSize);
}
