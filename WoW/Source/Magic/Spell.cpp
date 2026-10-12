#include "Spell.h"
#include <cmath>
#include "Engine/Source/Tempest/C3Vector.h"

// OFFSET: 0x00663530/0x140309680
float SpellUtils_ComputeMissileTrajectoryTravelTime(float pitch, float speed, const C3Vector& delta, float gravity) {
    bool hasGravity = fabs(gravity) > 0.000099999997f;
    float horizontal = cos(pitch) * speed;

    if (horizontal > 0.000099999997f) {
        return sqrt(delta.y * delta.y + delta.x * delta.x) / horizontal;
    }

    if (speed <= 0.000099999997f) {
        if (!hasGravity) {
            return -1.0f;
        }
    } else if (!hasGravity) {
        return fabs(delta.z) / speed;
    }

    float vertical = speed * sin(pitch);

    if (vertical * vertical - delta.z * gravity * 2.0f < 0.0f) {
        return -1.0f;
    }

    float root = sqrt(vertical * vertical - delta.z * gravity * 2.0f);
    float first = (root - vertical) * (-1.0f / gravity);
    float second = (-vertical - root) * (-1.0f / gravity);

    if (first < 0.0f) {
        return second;
    }

    if (second < 0.0f) {
        return first;
    }

    if (second <= first) {
        return second;
    }

    return first;
}

// OFFSET: 0x00663660/0x1403097C0
int32_t SpellUtils_ComputeMissileTrajectory(float pitch, const C3Vector& delta, float gravity, float& speed, float& time, C3Vector* velocity) {
    float c = cos(pitch);
    float s = sin(pitch);
    float horizontal = sqrt(delta.y * delta.y + delta.x * delta.x);

    if (fabs(c) < 0.000099999997f || fabs(horizontal) < 0.000099999997f || fabs(gravity) < 0.000099999997f) {
        return 1;
    }

    float squared = (1.0f / c * horizontal * s - delta.z) / gravity * 2.0f;

    if (squared <= 0.0099999998f) {
        return 0;
    }

    float t = sqrt(squared);
    time = t;
    float v = horizontal / t * (1.0f / c);
    speed = v;

    if (velocity) {
        velocity->x = delta.x * v * (1.0f / horizontal) * c;
        velocity->y = speed * (1.0f / horizontal) * c * delta.y;
        velocity->z = speed * s;
    }

    return 2;
}

// OFFSET: 0x006637A0/0x140309910
int32_t GetSkinningDifficulty(int32_t level) {
    if (level <= 10) {
        return 1;
    }

    if (level < 20) {
        return 2 * (5 * level - 50);
    }

    if (level <= 73) {
        return 5 * level;
    }

    if (level <= 80) {
        return 10 * level - 365;
    }

    if (level > 83) {
        return 20 * level - 1210;
    }

    return 5 * level + 35;
}

// OFFSET: 0x00663800
bool SpellIsAuraEffect(int32_t effect) {
    switch (effect) {
        case 6:
        case 27:
        case 35:
        case 65:
        case 119:
        case 128:
        case 129:
        case 143:
        case 174:
            return true;

        default:
            return false;
    }
}

// OFFSET: 0x006638E0/0x140309A50
bool SpellIsApplyAuraEffect(int32_t effect) {
    switch (effect) {
        case 6:
        case 35:
        case 65:
        case 119:
        case 128:
        case 129:
        case 143:
        case 174:
            return true;

        default:
            return false;
    }
}

// OFFSET: 0x006639C0/0x140309B40
bool SpellIsAreaAuraEffect(int32_t effect) {
    switch (effect) {
        case 27:
        case 35:
        case 65:
        case 119:
        case 128:
        case 129:
        case 143:
        case 174:
            return true;

        default:
            return false;
    }
}

// OFFSET: 0x00663A90
bool SpellIsDamageEffect(int32_t effect, int32_t aura) {
    switch (effect) {
        case 2:
        case 9:
        case 17:
        case 31:
        case 58:
        case 62:
        case 121:
            return true;

        case 6:
        case 27:
        case 35:
        case 65:
        case 119:
        case 128:
        case 129:
        case 143:
        case 174:
            switch (aura) {
                case 3:
                case 15:
                case 43:
                case 53:
                    return true;

                default:
                    return false;
            }

        default:
            return false;
    }
}
