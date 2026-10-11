#ifndef WOW_SOURCE_OBJECT_UNIT_H
#define WOW_SOURCE_OBJECT_UNIT_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Tempest/C3Vector.h"

class CGUnitData;
class CMovement_C;

class CGUnit {
    public:
    int32_t GetQuestGreenRange() const;
    static const uint32_t s_mirrorFieldIndices[120];

    static uint32_t MirrorIndexFromFieldIndex(uint32_t fieldIndex);
    static uint32_t DescriptorToMirrorOffset(uint32_t offset, uint32_t a2, uint32_t a3, uint32_t base);
    static int32_t TotalFields();
    int32_t IsFlyingOrHovering() const;

    /* 0000 */ CGUnitData* m_unit;
    /* 0004 */ void* m_unitMirror;
    /* 0008 */ CMovement_C* m_movement;

    C3Vector GetVelocity() const;

    bool CanSwim() const;
    int32_t CanPlayEmote(uint32_t emoteFlags, int32_t targeted, int32_t* standUp, int32_t inCombat, int32_t* stopCombat, int32_t restricted) const;
    void GetResistanceAndBuffs(int32_t school, int32_t* baseValue, int32_t* total, int32_t* effective, int32_t* positive, int32_t* negative) const;
    const WOWGUID* GetCharmerOrCreatorGuid() const;
    bool HasCreator() const;
    bool HasSummoner() const;
    bool IsPossessedBy(const WOWGUID& guid) const;
    bool IsCharmed() const;
    bool IsGuildMaster() const;
    int32_t GetSpellCostModifier(int32_t schoolMask) const;
    float GetSpellCostMultiplier(int32_t schoolMask) const;
    float GetMeleeRange(const CGUnit* target) const;
    int32_t GetEffectiveStat(int32_t stat) const;
    int32_t HasAuraState(int32_t state) const;
    int32_t IsOnFlyingSpline() const;
    int32_t IsOnSpline() const;
    int32_t IsJumping() const;
    int32_t IsOnFallingSpline() const;
    int32_t CanMove() const;
    float GetFacing(float facing) const;
    float GetMaxSplineSpeed(uint32_t splineFlags) const;
    void CalcMissileTravel(const C3Vector& target, float speed, bool speedAsTime, float* outSpeed, int32_t* outTime) const;
};

class CGUnitData {
    public:
    /* 0000 */ WOWGUID m_unitFieldCharm;
    /* 0008 */ WOWGUID m_unitFieldSummon;
    /* 0010 */ WOWGUID m_unitFieldCritter;
    /* 0018 */ WOWGUID m_unitFieldCharmedby;
    /* 0020 */ WOWGUID m_unitFieldSummonedby;
    /* 0028 */ WOWGUID m_unitFieldCreatedby;
    /* 0030 */ WOWGUID m_unitFieldTarget;
    /* 0038 */ WOWGUID m_unitFieldChannelObject;
    /* 0040 */ uint32_t m_unitChannelSpell;
    /* 0044 */ uint8_t m_unitFieldBytes0 [4];
    /* 0048 */ int32_t m_unitFieldHealth;
    /* 004C */ uint32_t m_unitFieldPower [5];
    /* 0060 */ uint32_t m_unitFieldMaxhealth;
    /* 0064 */ uint32_t m_unitFieldMaxpower [5];
    /* 0078 */ float m_unitFieldPowerRegenFlatModifier [5];
    /* 008C */ float m_unitFieldPowerRegenInterruptedFlatModifier [5];
    /* 00A0 */ uint32_t m_unitFieldLevel;
    /* 00A4 */ uint32_t m_unitFieldFactiontemplate;
    /* 00A8 */ uint32_t m_unitVirtualItemSlotId [3];
    /* 00B4 */ uint32_t m_unitFieldFlags;
    /* 00B8 */ uint32_t m_unitFieldFlags2;
    /* 00BC */ uint32_t m_unitFieldAurastate;
    /* 00C0 */ uint32_t m_unitFieldBaseattacktime [2];
    /* 00C8 */ uint32_t m_unitFieldRangedattacktime;
    /* 00CC */ float m_unitFieldBoundingradius;
    /* 00D0 */ float m_unitFieldCombatreach;
    /* 00D4 */ uint32_t m_unitFieldDisplayid;
    /* 00D8 */ uint32_t m_unitFieldNativedisplayid;
    /* 00DC */ uint32_t m_unitFieldMountdisplayid;
    /* 00E0 */ float m_unitFieldMindamage;
    /* 00E4 */ float m_unitFieldMaxdamage;
    /* 00E8 */ float m_unitFieldMinoffhanddamage;
    /* 00EC */ float m_unitFieldMaxoffhanddamage;
    /* 00F0 */ uint8_t m_unitFieldBytes1 [4];
    /* 00F4 */ uint32_t m_unitFieldPetnumber;
    /* 00F8 */ uint32_t m_unitFieldPetNameTimestamp;
    /* 00FC */ uint32_t m_unitFieldPetexperience;
    /* 0100 */ uint32_t m_unitFieldPetnextlevelexp;
    /* 0104 */ uint32_t m_unitDynamicFlags;
    /* 0108 */ float m_unitModCastSpeed;
    /* 010C */ float m_unitModCastHaste;
    /* 0110 */ uint32_t m_unitCreatedBySpell;
    /* 0114 */ uint32_t m_unitNpcFlags;
    /* 0118 */ uint32_t m_unitNpcEmotestate;
    /* 011C */ int32_t m_unitFieldStat [5];
    /* 0130 */ int32_t m_unitFieldPosstat [5];
    /* 0144 */ int32_t m_unitFieldNegstat [5];
    /* 0158 */ int32_t m_unitFieldResistances [7];
    /* 0174 */ int32_t m_unitFieldResistancebuffmodspositive [7];
    /* 0190 */ int32_t m_unitFieldResistancebuffmodsnegative [7];
    /* 01AC */ uint32_t m_unitFieldBaseMana;
    /* 01B0 */ uint32_t m_unitFieldBaseHealth;
    /* 01B4 */ uint8_t m_unitFieldBytes2 [4];
    /* 01B8 */ int32_t m_unitFieldAttackPower;
    /* 01BC */ uint32_t m_unitFieldAttackPowerModPos;
    /* 01C0 */ uint32_t m_unitFieldAttackPowerModNeg;
    /* 01C4 */ float m_unitFieldAttackPowerMultiplier;
    /* 01C8 */ int32_t m_unitFieldRangedAttackPower;
    /* 01CC */ uint32_t m_unitFieldRangedAttackPowerModPos;
    /* 01D0 */ uint32_t m_unitFieldRangedAttackPowerModNeg;
    /* 01D4 */ float m_unitFieldRangedAttackPowerMultiplier;
    /* 01D8 */ float m_unitFieldMinrangeddamage;
    /* 01DC */ float m_unitFieldMaxrangeddamage;
    /* 01E0 */ int32_t m_unitFieldPowerCostModifier [7];
    /* 01FC */ float m_unitFieldPowerCostMultiplier [7];
    /* 0218 */ float m_unitFieldMaxhealthmodifier;
    /* 021C */ float m_unitFieldHoverheight;
    /* 0220 */ uint32_t m_unitFieldMaxitemlevel;
    /* 0224 */ uint32_t m_unitFieldPadding;
};

#endif
