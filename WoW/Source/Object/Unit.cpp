#include "Unit.h"
#include "WoW/Source/Object/CMoveSpline.h"
#include "WoW/Source/Object/Movement.h"
#include "WoW/Source/Object/Object.h"

// STATIC: 0x00D18C58
const uint32_t CGUnit::s_mirrorFieldIndices[120] = {
    0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xC, 0xD, 0xE, 0xF, 0x10, 0x11,
    0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21,
    0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31,
    0x32, 0x35, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x45,
    0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6D, 0x6E, 0x6F, 0x70, 0x71,
    0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x81,
    0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x8A
};

// OFFSET: 0x005B64B0
int32_t CGUnit::IsFlyingOrHovering() const {
    CMovement_C* movement = this->m_movement;

    if (!movement->IsOnFallingSpline()) {
        if (movement->IsOnFlyingSpline() || (movement->m_flags & (MOVEMENTFLAG_DISABLE_COLLISION | MOVEMENTFLAG_DISABLE_GRAVITY))) {
            return 1;
        }
    }

    return (movement->m_flags & MOVEMENTFLAG_HOVER) != 0;
}

// OFFSET: 0x00A27820/0x1407CEF80
uint32_t CGUnit::MirrorIndexFromFieldIndex(uint32_t fieldIndex) {
    for (uint32_t i = 0; i < sizeof(s_mirrorFieldIndices) / sizeof(s_mirrorFieldIndices[0]); i++) {
        if (s_mirrorFieldIndices[i] == fieldIndex) {
            return i;
        }
    }

    return 138;
}

// OFFSET: 0x00A27850
bool CGUnit::CanSwim() const {
    uint32_t flags = this->m_unit->m_unitFieldFlags;

    if (flags & 0x4000) {
        return false;
    }

    if (flags & 0x8) {
        return true;
    }

    if (this->m_unit->m_unitFieldFlags2 & 0x1000000) {
        return false;
    }

    if ((flags & 0x800) || (flags & 0x10)) {
        return true;
    }

    return (flags >> 15) & 1;
}

// OFFSET: 0x00A27950/0x1407CF0B0
int32_t CGUnit::CanPlayEmote(uint32_t emoteFlags, int32_t targeted, int32_t* standUp, int32_t inCombat, int32_t* stopCombat, int32_t restricted) const {
    *standUp = 0;
    *stopCombat = 0;

    if (targeted && (emoteFlags & 0x400)) {
        return 0;
    }

    if ((emoteFlags & 0x1) && this->m_unit->m_unitFieldBytes1[0]) {
        return 0;
    }

    if ((emoteFlags & 0x80) && (this->m_movement->m_flags & 0x100000)) {
        return 0;
    }

    if (emoteFlags & 0x8000) {
        CMoveSpline* spline = this->m_movement->m_spline;

        if (spline && !(spline->m_flags & 0x80) && (spline->m_flags & 0x200)) {
            return 0;
        }

        if (this->m_movement->m_flags & 0x1000000) {
            return 0;
        }
    }

    if ((emoteFlags & 0x10000) && this->m_unit->m_unitFieldMountdisplayid > 0) {
        return 0;
    }

    if ((emoteFlags & 0x4) && this->m_unit->m_unitChannelSpell) {
        return 0;
    }

    if ((emoteFlags & 0x800) && restricted) {
        return 0;
    }

    if (emoteFlags & 0x200000) {
        uint64_t transport = this->m_movement->m_transportGuid.m_guid;

        if (transport) {
            uint32_t high = static_cast<uint32_t>(transport >> 32);

            if ((high & 0xF0F00000) == 0xF0500000) {
                return 0;
            }

            if (!(high & 0xF0000000) && ((high & 0xF07FFFFF) || static_cast<uint32_t>(transport))) {
                return 0;
            }
        }
    }

    if (!(emoteFlags & 0x200)) {
        uint8_t standState = this->m_unit->m_unitFieldBytes1[0];

        if (standState == 3 || standState == 7) {
            return 0;
        }
    }

    if ((emoteFlags & 0x4000) && (this->m_movement->m_flags & 0x6008FF)) {
        *standUp = 1;
    }

    if ((emoteFlags & 0x400000) && (((this->m_unit->m_unitFieldFlags >> 11) & 1) || inCombat)) {
        *stopCombat = 1;
    }

    return 1;
}

// PROVEN: 0497d0ca IDENTICAL/NONE
// OFFSET: 0x00A278A0/0x1407CF000
void CGUnit::GetResistanceAndBuffs(int32_t school, int32_t* baseValue, int32_t* total, int32_t* effective, int32_t* positive, int32_t* negative) const {
    int32_t resistance = this->m_unit->m_unitFieldResistances[school];
    *effective = resistance;
    *total = resistance;
    *positive = this->m_unit->m_unitFieldResistancebuffmodspositive[school];
    *negative = this->m_unit->m_unitFieldResistancebuffmodsnegative[school];
    *baseValue = *total - *positive - *negative;

    if (*total < 0) {
        *effective = 0;
        *total = 0;
    }
}

// PROVEN: 92d0d591 IDENTICAL/NONE
// OFFSET: 0x0044E350
const WOWGUID* CGUnit::GetCharmerOrCreatorGuid() const {
    CGUnitData* unit = this->m_unit;

    if (unit->m_unitFieldCharmedby) {
        return &unit->m_unitFieldCharmedby;
    }

    return &unit->m_unitFieldCreatedby;
}

// OFFSET: 0x00554C30/0x1401AF360
float CGUnit::GetMeleeRange(const CGUnit* target) const {
    float range = target->m_unit->m_unitFieldCombatreach + this->m_unit->m_unitFieldCombatreach + 1.3333334f;

    if (range < 5.0f) {
        return 5.0f;
    }

    return range;
}

// PROVEN: ac7dd20c IDENTICAL/NONE
// OFFSET: 0x0044E370
int32_t CGUnit::GetEffectiveStat(int32_t stat) const {
    return this->m_unit->m_unitFieldStat[stat] < 0 ? 0 : this->m_unit->m_unitFieldStat[stat];
}

// PROVEN: 450a766e IDENTICAL/NONE
// OFFSET: 0x00554C90
int32_t CGUnit::HasAuraState(int32_t state) const {
    return (this->m_unit->m_unitFieldAurastate & (1 << (state - 1))) != 0;
}

// INLINED
int32_t CGUnit::IsOnFlyingSpline() const {
    CMoveSpline* spline = this->m_movement->m_spline;

    if (spline && !(spline->m_flags & 0x400) && (spline->m_flags & 0x2000)) {
        return 1;
    }

    return 0;
}

// OFFSET: 0x0043B820
int32_t CGUnit::IsOnSpline() const {
    CMoveSpline* spline = this->m_movement->m_spline;
    return spline && !(spline->m_flags & 0x80);
}

// OFFSET: 0x0043B840
int32_t CGUnit::IsOnFallingSpline() const {
    CMoveSpline* spline = this->m_movement->m_spline;
    return spline && !(spline->m_flags & 0x80) && (spline->m_flags & 0x200);
}

// OFFSET: 0x0043B7F0
int32_t CGUnit::IsJumping() const {
    return (this->m_movement->m_flags & 0x800) && 0.0f != this->m_movement->m_fallVelocity;
}

// OFFSET: 0x00448D50/0x140055870
int32_t CGUnit::CanMove() const {
    return this->m_unit->m_unitFieldHealth > 0 && !(this->m_movement->m_flags & 0x100A00) && this->m_unit->m_unitFieldBytes1[0] != 7;
}

// OFFSET: 0x0048D960/0x1400B17A0
uint32_t CGUnit::DescriptorToMirrorOffset(uint32_t offset, uint32_t a2, uint32_t a3, uint32_t base) {
    int32_t objectFields = CGObject::TotalFieldsSaved();
    return (offset & 3) + 4 * (objectFields + MirrorIndexFromFieldIndex((offset - base) >> 2));
}

// OFFSET: 0x00A27810/0x1407CEF60
int32_t CGUnit::TotalFields() {
    return CGObject::TotalFieldsSaved() + 120;
}

// PROVEN: 4fb188ef IDENTICAL/NONE
// OFFSET: 0x00447B30/0x14026EC00
float CGUnit::GetFacing(float facing) const {
    return this->m_movement->GetFacing(facing);
}

// OFFSET: 0x004E8BE0
bool CGUnit::IsPossessedBy(const WOWGUID& guid) const {
    return (this->m_unit->m_unitFieldFlags & 0x1000000) && guid == this->m_unit->m_unitFieldCharmedby;
}

// PROVEN: 0ce1f4fb IDENTICAL/NONE
// OFFSET: 0x005AC900
bool CGUnit::HasCreator() const {
    return this->m_unit->m_unitFieldCreatedby.m_guid != 0;
}

// PROVEN: 57e9bf7a IDENTICAL/NONE
// OFFSET: 0x005AC8E0
bool CGUnit::HasSummoner() const {
    return this->m_unit->m_unitFieldSummonedby.m_guid != 0;
}

// OFFSET: 0x00554CC0
bool CGUnit::IsGuildMaster() const {
    uint32_t flags = this->m_unit->m_unitNpcFlags;
    return (flags & 0x40000) && (flags & 0x80000);
}

// OFFSET: 0x0044E330
bool CGUnit::IsCharmed() const {
    return this->m_unit->m_unitFieldCharmedby != WOWGUID();
}

// OFFSET: 0x008A4690
int32_t CGUnit::GetSpellCostModifier(int32_t schoolMask) const {
    int32_t result = 0;
    bool found = false;

    for (uint32_t i = 0; i < sizeof(this->m_unit->m_unitFieldPowerCostModifier) / sizeof(this->m_unit->m_unitFieldPowerCostModifier[0]); i++) {
        if (((1 << i) & schoolMask) && (!found || this->m_unit->m_unitFieldPowerCostModifier[i] < result)) {
            result = this->m_unit->m_unitFieldPowerCostModifier[i];
            found = true;
        }
    }

    return result;
}

// OFFSET: 0x008A46E0
float CGUnit::GetSpellCostMultiplier(int32_t schoolMask) const {
    float result = 0.0f;
    bool found = false;

    for (uint32_t i = 0; i < sizeof(this->m_unit->m_unitFieldPowerCostMultiplier) / sizeof(this->m_unit->m_unitFieldPowerCostMultiplier[0]); i++) {
        if (((1 << i) & schoolMask) && (!found || result > this->m_unit->m_unitFieldPowerCostMultiplier[i])) {
            result = this->m_unit->m_unitFieldPowerCostMultiplier[i];
            found = true;
        }
    }

    return result + 1.0f;
}

// OFFSET: 0x00945040/0x1406B1E20
int32_t CGUnit::GetQuestGreenRange() const {
    static const int32_t s_questGreenRange[20] = { 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };
    uint32_t band = static_cast<int32_t>(this->m_unit->m_unitFieldLevel) / 5;
    return band < 20 ? s_questGreenRange[band] : 8;
}

// INLINED
C3Vector CGUnit::GetVelocity() const {
    float speed = this->m_movement->m_currentSpeed;
    return C3Vector(this->m_movement->m_moveDir.x * speed, this->m_movement->m_moveDir.y * speed, speed * this->m_movement->m_moveDir.z);
}

// OFFSET: 0x00A27AC0
void CGUnit::CalcMissileTravel(const C3Vector& target, float speed, bool speedAsTime, float* outSpeed, int32_t* outTime) const {
    if (speedAsTime) {
        if (speed <= 0.0f) {
            *outSpeed = 10.0f;
        } else {
            C3Vector position = this->m_movement->GetPosition(this->m_movement->m_position);
            float dz = target.z - position.z;
            float dy = target.y - position.y;
            float dx = target.x - position.x;
            *outSpeed = sqrt(dx * dx + (dy * dy + dz * dz)) / speed;
        }

        if (outTime) {
            *outTime = static_cast<int32_t>(speed * 1000.0f);
        }
    } else {
        *outSpeed = speed;

        if (outTime) {
            if (speed <= 0.0f) {
                *outTime = 0;
            } else {
                C3Vector position = this->m_movement->GetPosition(this->m_movement->m_position);
                float dz = target.z - position.z;
                float dy = target.y - position.y;
                float dx = target.x - position.x;
                *outTime = static_cast<int32_t>(sqrt(dx * dx + (dy * dy + dz * dz)) / speed * 1000.0f);
            }
        }
    }
}

// OFFSET: 0x00A27900
float CGUnit::GetMaxSplineSpeed(uint32_t splineFlags) const {
    if (splineFlags & 0x2000A40) {
        return 50.0f;
    }

    float speed = this->m_movement->m_runSpeed * 4.0f;

    if (speed <= 28.0f) {
        return 28.0f;
    }

    return speed;
}
