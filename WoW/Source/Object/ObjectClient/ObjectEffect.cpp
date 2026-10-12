#include "ObjectEffect.h"
#include <new>
#include <typeinfo>
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/ObjectClient/Types.h"

// STATIC: 0x00DD6330/0x140CBCAF0
static ObjectEffectAnimState s_objectEffectAnimStateMap[ANIM_COUNT];

// STATIC: 0x00DD8220
static HASHKEY_NONE s_hashKeyNone;

// STATIC: 0x00DD8224/0x140B45780
static TSHashTable<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE> s_effectDefLookup;

// STATIC: 0x00DD8250
static TSHashTable<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE> s_groupDefLookup;

// STATIC: 0x00DD827C/0x140B45820
static TSHashTable<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE> s_packageDefLookup;

// STATIC: 0x00CA6634/0x140B45750
float (*CObjectEffectModifier::s_inputFuncs[2])(CObjectEffectManager* manager) = {
    &CObjectEffectModifier::InputNone,
    &CObjectEffectModifier::InputSpeed,
};

// STATIC: 0x00CA663C/0x140B45760
float (*CObjectEffectModifier::s_mapFuncs[2])(float input, const float* params) = {
    &CObjectEffectModifier::MapNone,
    &CObjectEffectModifier::MapLinear,
};

// STATIC: 0x00CA6644/0x140B45770
void (*CObjectEffectModifier::s_outputFuncs[2])(CObjectEffect* effect, float value) = {
    &CObjectEffectModifier::OutputNone,
    &CObjectEffectModifier::OutputPitch,
};

// INLINED
CObjectEffectModifier::CObjectEffectModifier() {
    this->m_inputType = 0;
    this->m_mapType = 0;
    this->m_outputType = 0;
    this->m_params = NULL;
    this->m_numParams = 0;
}

// INLINED
void CObjectEffectModifier::Init(const ObjectEffectModifierRec* rec) {
    this->m_inputType = rec->m_inputType;
    this->m_outputType = rec->m_outputType;
    this->m_mapType = rec->m_mapType;
    this->m_id = rec->m_id;
    this->m_numParams = this->m_mapType == 1 ? 4 : 0;

    if (this->m_numParams) {
        this->m_params = new (__FILE__, __LINE__) float[this->m_numParams];
    }
}

// INLINED
void CObjectEffectModifier::SetParam(int32_t index, float value) {
    this->m_params[index] = value;
}

// INLINED
CObjectEffectDefRef::CObjectEffectDefRef() {
    this->m_effectDef = NULL;
}

// INLINED
CObjectEffectGroupDefRef::CObjectEffectGroupDefRef() {
    this->m_groupDef = NULL;
}

// OFFSET: 0x005E09F0/0x14025E620
void InitObjectEffectStateMap() {
    for (int32_t i = 0; i < ANIM_COUNT; i++) {
        s_objectEffectAnimStateMap[i].m_animID = ANIM_COUNT;
        s_objectEffectAnimStateMap[i].m_stateID = 0;
    }

    s_objectEffectAnimStateMap[ANIM_STAND].m_animID = ANIM_STAND;
    s_objectEffectAnimStateMap[ANIM_STAND].m_stateID = 19;
    s_objectEffectAnimStateMap[ANIM_STAND].m_altStateID = 41;
    s_objectEffectAnimStateMap[ANIM_DEATH].m_animID = ANIM_DEATH;
    s_objectEffectAnimStateMap[ANIM_DEATH].m_stateID = 15;
    s_objectEffectAnimStateMap[ANIM_DEATH].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_WALK].m_animID = ANIM_WALK;
    s_objectEffectAnimStateMap[ANIM_WALK].m_stateID = 13;
    s_objectEffectAnimStateMap[ANIM_WALK].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_RUN].m_animID = ANIM_RUN;
    s_objectEffectAnimStateMap[ANIM_RUN].m_stateID = 12;
    s_objectEffectAnimStateMap[ANIM_RUN].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_DEAD].m_animID = ANIM_DEAD;
    s_objectEffectAnimStateMap[ANIM_DEAD].m_stateID = 81;
    s_objectEffectAnimStateMap[ANIM_DEAD].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_STAND_WOUND].m_animID = ANIM_STAND_WOUND;
    s_objectEffectAnimStateMap[ANIM_STAND_WOUND].m_stateID = 17;
    s_objectEffectAnimStateMap[ANIM_STAND_WOUND].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_COMBAT_WOUND].m_animID = ANIM_COMBAT_WOUND;
    s_objectEffectAnimStateMap[ANIM_COMBAT_WOUND].m_stateID = 14;
    s_objectEffectAnimStateMap[ANIM_COMBAT_WOUND].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_COMBAT_CRITICAL].m_animID = ANIM_COMBAT_CRITICAL;
    s_objectEffectAnimStateMap[ANIM_COMBAT_CRITICAL].m_stateID = 39;
    s_objectEffectAnimStateMap[ANIM_COMBAT_CRITICAL].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SHUFFLE_LEFT].m_animID = ANIM_SHUFFLE_LEFT;
    s_objectEffectAnimStateMap[ANIM_SHUFFLE_LEFT].m_stateID = 20;
    s_objectEffectAnimStateMap[ANIM_SHUFFLE_LEFT].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SHUFFLE_RIGHT].m_animID = ANIM_SHUFFLE_RIGHT;
    s_objectEffectAnimStateMap[ANIM_SHUFFLE_RIGHT].m_stateID = 21;
    s_objectEffectAnimStateMap[ANIM_SHUFFLE_RIGHT].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_WALKBACKWARDS].m_animID = ANIM_WALKBACKWARDS;
    s_objectEffectAnimStateMap[ANIM_WALKBACKWARDS].m_stateID = 22;
    s_objectEffectAnimStateMap[ANIM_WALKBACKWARDS].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_ATTACK_UNARMED].m_animID = ANIM_ATTACK_UNARMED;
    s_objectEffectAnimStateMap[ANIM_ATTACK_UNARMED].m_stateID = 40;
    s_objectEffectAnimStateMap[ANIM_ATTACK_UNARMED].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_READY_UNARMED].m_animID = ANIM_READY_UNARMED;
    s_objectEffectAnimStateMap[ANIM_READY_UNARMED].m_stateID = 18;
    s_objectEffectAnimStateMap[ANIM_READY_UNARMED].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_JUMP_START].m_animID = ANIM_JUMP_START;
    s_objectEffectAnimStateMap[ANIM_JUMP_START].m_stateID = 23;
    s_objectEffectAnimStateMap[ANIM_JUMP_START].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_JUMP].m_animID = ANIM_JUMP;
    s_objectEffectAnimStateMap[ANIM_JUMP].m_stateID = 36;
    s_objectEffectAnimStateMap[ANIM_JUMP].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_JUMP_END].m_animID = ANIM_JUMP_END;
    s_objectEffectAnimStateMap[ANIM_JUMP_END].m_stateID = 24;
    s_objectEffectAnimStateMap[ANIM_JUMP_END].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_FALL].m_animID = ANIM_FALL;
    s_objectEffectAnimStateMap[ANIM_FALL].m_stateID = 25;
    s_objectEffectAnimStateMap[ANIM_FALL].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SWIM_IDLE].m_animID = ANIM_SWIM_IDLE;
    s_objectEffectAnimStateMap[ANIM_SWIM_IDLE].m_stateID = 26;
    s_objectEffectAnimStateMap[ANIM_SWIM_IDLE].m_altStateID = 42;
    s_objectEffectAnimStateMap[ANIM_SWIM].m_animID = ANIM_SWIM;
    s_objectEffectAnimStateMap[ANIM_SWIM].m_stateID = 27;
    s_objectEffectAnimStateMap[ANIM_SWIM].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SWIM_LEFT].m_animID = ANIM_SWIM_LEFT;
    s_objectEffectAnimStateMap[ANIM_SWIM_LEFT].m_stateID = 28;
    s_objectEffectAnimStateMap[ANIM_SWIM_LEFT].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SWIM_RIGHT].m_animID = ANIM_SWIM_RIGHT;
    s_objectEffectAnimStateMap[ANIM_SWIM_RIGHT].m_stateID = 29;
    s_objectEffectAnimStateMap[ANIM_SWIM_RIGHT].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SWIM_BACKWARDS].m_animID = ANIM_SWIM_BACKWARDS;
    s_objectEffectAnimStateMap[ANIM_SWIM_BACKWARDS].m_stateID = 30;
    s_objectEffectAnimStateMap[ANIM_SWIM_BACKWARDS].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SPELL_CAST_DIRECTED].m_animID = ANIM_SPELL_CAST_DIRECTED;
    s_objectEffectAnimStateMap[ANIM_SPELL_CAST_DIRECTED].m_stateID = 16;
    s_objectEffectAnimStateMap[ANIM_SPELL_CAST_DIRECTED].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_RUN_RIGHT].m_animID = ANIM_RUN_RIGHT;
    s_objectEffectAnimStateMap[ANIM_RUN_RIGHT].m_stateID = 32;
    s_objectEffectAnimStateMap[ANIM_RUN_RIGHT].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_RUN_LEFT].m_animID = ANIM_RUN_LEFT;
    s_objectEffectAnimStateMap[ANIM_RUN_LEFT].m_stateID = 31;
    s_objectEffectAnimStateMap[ANIM_RUN_LEFT].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND_DOWN].m_animID = ANIM_SIT_GROUND_DOWN;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND_DOWN].m_stateID = 45;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND_DOWN].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND].m_animID = ANIM_SIT_GROUND;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND].m_stateID = 43;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND_UP].m_animID = ANIM_SIT_GROUND_UP;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND_UP].m_stateID = 44;
    s_objectEffectAnimStateMap[ANIM_SIT_GROUND_UP].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_ATTACK_THROWN].m_animID = ANIM_ATTACK_THROWN;
    s_objectEffectAnimStateMap[ANIM_ATTACK_THROWN].m_stateID = 8;
    s_objectEffectAnimStateMap[ANIM_ATTACK_THROWN].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_READY_THROWN].m_animID = ANIM_READY_THROWN;
    s_objectEffectAnimStateMap[ANIM_READY_THROWN].m_stateID = 11;
    s_objectEffectAnimStateMap[ANIM_READY_THROWN].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_HOLD_THROWN].m_animID = ANIM_HOLD_THROWN;
    s_objectEffectAnimStateMap[ANIM_HOLD_THROWN].m_stateID = 9;
    s_objectEffectAnimStateMap[ANIM_HOLD_THROWN].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_LOAD_THROWN].m_animID = ANIM_LOAD_THROWN;
    s_objectEffectAnimStateMap[ANIM_LOAD_THROWN].m_stateID = 10;
    s_objectEffectAnimStateMap[ANIM_LOAD_THROWN].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_BIRTH].m_animID = ANIM_BIRTH;
    s_objectEffectAnimStateMap[ANIM_BIRTH].m_stateID = 104;
    s_objectEffectAnimStateMap[ANIM_BIRTH].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_FLY].m_animID = ANIM_FLY;
    s_objectEffectAnimStateMap[ANIM_FLY].m_stateID = 33;
    s_objectEffectAnimStateMap[ANIM_FLY].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SPRINT].m_animID = ANIM_SPRINT;
    s_objectEffectAnimStateMap[ANIM_SPRINT].m_stateID = 34;
    s_objectEffectAnimStateMap[ANIM_SPRINT].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_CLOSE].m_animID = ANIM_CLOSE;
    s_objectEffectAnimStateMap[ANIM_CLOSE].m_stateID = 73;
    s_objectEffectAnimStateMap[ANIM_CLOSE].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_CLOSED].m_animID = ANIM_CLOSED;
    s_objectEffectAnimStateMap[ANIM_CLOSED].m_stateID = 74;
    s_objectEffectAnimStateMap[ANIM_CLOSED].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_OPEN].m_animID = ANIM_OPEN;
    s_objectEffectAnimStateMap[ANIM_OPEN].m_stateID = 71;
    s_objectEffectAnimStateMap[ANIM_OPEN].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_OPENED].m_animID = ANIM_OPENED;
    s_objectEffectAnimStateMap[ANIM_OPENED].m_stateID = 72;
    s_objectEffectAnimStateMap[ANIM_OPENED].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_DESTROY].m_animID = ANIM_DESTROY;
    s_objectEffectAnimStateMap[ANIM_DESTROY].m_stateID = 75;
    s_objectEffectAnimStateMap[ANIM_DESTROY].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_DESTROYED].m_animID = ANIM_DESTROYED;
    s_objectEffectAnimStateMap[ANIM_DESTROYED].m_stateID = 76;
    s_objectEffectAnimStateMap[ANIM_DESTROYED].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_0].m_animID = ANIM_CUSTOM_0;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_0].m_stateID = 77;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_0].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_1].m_animID = ANIM_CUSTOM_1;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_1].m_stateID = 78;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_1].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_2].m_animID = ANIM_CUSTOM_2;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_2].m_stateID = 79;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_2].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_3].m_animID = ANIM_CUSTOM_3;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_3].m_stateID = 80;
    s_objectEffectAnimStateMap[ANIM_CUSTOM_3].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_DECAY].m_animID = ANIM_DECAY;
    s_objectEffectAnimStateMap[ANIM_DECAY].m_stateID = 105;
    s_objectEffectAnimStateMap[ANIM_DECAY].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SHIP_START].m_animID = ANIM_SHIP_START;
    s_objectEffectAnimStateMap[ANIM_SHIP_START].m_stateID = 68;
    s_objectEffectAnimStateMap[ANIM_SHIP_START].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SHIP_MOVING].m_animID = ANIM_SHIP_MOVING;
    s_objectEffectAnimStateMap[ANIM_SHIP_MOVING].m_stateID = 69;
    s_objectEffectAnimStateMap[ANIM_SHIP_MOVING].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_SHIP_STOP].m_animID = ANIM_SHIP_STOP;
    s_objectEffectAnimStateMap[ANIM_SHIP_STOP].m_stateID = 70;
    s_objectEffectAnimStateMap[ANIM_SHIP_STOP].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_JUMP_LAND_RUN].m_animID = ANIM_JUMP_LAND_RUN;
    s_objectEffectAnimStateMap[ANIM_JUMP_LAND_RUN].m_stateID = 35;
    s_objectEffectAnimStateMap[ANIM_JUMP_LAND_RUN].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_HOVER].m_animID = ANIM_HOVER;
    s_objectEffectAnimStateMap[ANIM_HOVER].m_stateID = 46;
    s_objectEffectAnimStateMap[ANIM_HOVER].m_altStateID = 0;
    s_objectEffectAnimStateMap[ANIM_GRAB].m_animID = ANIM_GRAB;
    s_objectEffectAnimStateMap[ANIM_GRAB].m_stateID = 67;
    s_objectEffectAnimStateMap[ANIM_GRAB].m_altStateID = 0;
    s_objectEffectAnimStateMap[532].m_animID = 532;
    s_objectEffectAnimStateMap[532].m_stateID = 96;
    s_objectEffectAnimStateMap[532].m_altStateID = 0;
    s_objectEffectAnimStateMap[534].m_animID = 534;
    s_objectEffectAnimStateMap[534].m_stateID = 97;
    s_objectEffectAnimStateMap[534].m_altStateID = 0;
    s_objectEffectAnimStateMap[536].m_animID = 536;
    s_objectEffectAnimStateMap[536].m_stateID = 98;
    s_objectEffectAnimStateMap[536].m_altStateID = 0;
    s_objectEffectAnimStateMap[538].m_animID = 538;
    s_objectEffectAnimStateMap[538].m_stateID = 99;
    s_objectEffectAnimStateMap[538].m_altStateID = 0;
    s_objectEffectAnimStateMap[540].m_animID = 540;
    s_objectEffectAnimStateMap[540].m_stateID = 100;
    s_objectEffectAnimStateMap[540].m_altStateID = 0;
    s_objectEffectAnimStateMap[542].m_animID = 542;
    s_objectEffectAnimStateMap[542].m_stateID = 101;
    s_objectEffectAnimStateMap[542].m_altStateID = 0;
    s_objectEffectAnimStateMap[544].m_animID = 544;
    s_objectEffectAnimStateMap[544].m_stateID = 102;
    s_objectEffectAnimStateMap[544].m_altStateID = 0;
    s_objectEffectAnimStateMap[546].m_animID = 546;
    s_objectEffectAnimStateMap[546].m_stateID = 103;
    s_objectEffectAnimStateMap[546].m_altStateID = 0;
    s_objectEffectAnimStateMap[548].m_animID = 548;
    s_objectEffectAnimStateMap[548].m_stateID = 82;
    s_objectEffectAnimStateMap[548].m_altStateID = 0;
    s_objectEffectAnimStateMap[550].m_animID = 550;
    s_objectEffectAnimStateMap[550].m_stateID = 86;
    s_objectEffectAnimStateMap[550].m_altStateID = 0;
    s_objectEffectAnimStateMap[552].m_animID = 552;
    s_objectEffectAnimStateMap[552].m_stateID = 84;
    s_objectEffectAnimStateMap[552].m_altStateID = 0;
    s_objectEffectAnimStateMap[554].m_animID = 554;
    s_objectEffectAnimStateMap[554].m_stateID = 85;
    s_objectEffectAnimStateMap[554].m_altStateID = 0;
    s_objectEffectAnimStateMap[556].m_animID = 556;
    s_objectEffectAnimStateMap[556].m_stateID = 87;
    s_objectEffectAnimStateMap[556].m_altStateID = 0;
    s_objectEffectAnimStateMap[558].m_animID = 558;
    s_objectEffectAnimStateMap[558].m_stateID = 83;
    s_objectEffectAnimStateMap[558].m_altStateID = 0;
    s_objectEffectAnimStateMap[560].m_animID = 560;
    s_objectEffectAnimStateMap[560].m_stateID = 88;
    s_objectEffectAnimStateMap[560].m_altStateID = 0;
    s_objectEffectAnimStateMap[562].m_animID = 562;
    s_objectEffectAnimStateMap[562].m_stateID = 89;
    s_objectEffectAnimStateMap[562].m_altStateID = 0;
    s_objectEffectAnimStateMap[564].m_animID = 564;
    s_objectEffectAnimStateMap[564].m_stateID = 90;
    s_objectEffectAnimStateMap[564].m_altStateID = 0;
    s_objectEffectAnimStateMap[566].m_animID = 566;
    s_objectEffectAnimStateMap[566].m_stateID = 93;
    s_objectEffectAnimStateMap[566].m_altStateID = 0;
    s_objectEffectAnimStateMap[568].m_animID = 568;
    s_objectEffectAnimStateMap[568].m_stateID = 94;
    s_objectEffectAnimStateMap[568].m_altStateID = 0;
    s_objectEffectAnimStateMap[570].m_animID = 570;
    s_objectEffectAnimStateMap[570].m_stateID = 95;
    s_objectEffectAnimStateMap[570].m_altStateID = 0;
    s_objectEffectAnimStateMap[572].m_animID = 572;
    s_objectEffectAnimStateMap[572].m_stateID = 91;
    s_objectEffectAnimStateMap[572].m_altStateID = 0;
    s_objectEffectAnimStateMap[574].m_animID = 574;
    s_objectEffectAnimStateMap[574].m_stateID = 92;
    s_objectEffectAnimStateMap[574].m_altStateID = 0;
}

// OFFSET: 0x005E2000/0x140260020
void ObjectEffectSystem_ShutDown() {
    s_effectDefLookup.Clear();
    s_groupDefLookup.Clear();
    s_packageDefLookup.Clear();
}

// OFFSET: 0x005E5A70/0x1402657A0
void ObjectEffectSystem_Initialize() {
    InitObjectEffectStateMap();

    int32_t packageCount = g_objectEffectPackageDB.m_numRecords;

    for (int32_t i = 0; i < packageCount; i++) {
        ObjectEffectPackageRec* rec = g_objectEffectPackageDB.GetRecordByIndex(i);
        OBJ_EFFECT_PACKAGE_DEF_LOOKUP* packageLookup = s_packageDefLookup.New(rec->m_id, s_hashKeyNone, 0, 0);
        packageLookup->m_objectEffectPackageDef.m_id = rec->m_id;
    }

    int32_t groupCount = g_objectEffectGroupDB.m_numRecords;

    for (int32_t i = 0; i < groupCount; i++) {
        ObjectEffectGroupRec* rec = g_objectEffectGroupDB.GetRecordByIndex(i);
        OBJ_EFFECT_GROUP_DEF_LOOKUP* groupLookup = s_groupDefLookup.New(rec->m_id, s_hashKeyNone, 0, 0);
        groupLookup->m_objectEffectGroupDef.m_id = rec->m_id;
    }

    int32_t elemCount = g_objectEffectPackageElemDB.m_numRecords;

    for (int32_t i = 0; i < elemCount; i++) {
        ObjectEffectPackageElemRec* rec = g_objectEffectPackageElemDB.GetRecordByIndex(i);
        OBJ_EFFECT_PACKAGE_DEF_LOOKUP* packageLookup = s_packageDefLookup.Ptr(rec->m_objectEffectPackageID, s_hashKeyNone);

        if (!packageLookup) {
            continue;
        }

        OBJ_EFFECT_GROUP_DEF_LOOKUP* groupLookup = s_groupDefLookup.Ptr(rec->m_objectEffectGroupID, s_hashKeyNone);
        OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP* refList = packageLookup->m_objectEffectPackageDef.m_stateGroupRefs.Ptr(rec->m_stateType, s_hashKeyNone);

        if (!refList) {
            refList = packageLookup->m_objectEffectPackageDef.m_stateGroupRefs.New(rec->m_stateType, s_hashKeyNone, 0, 0);
        }

        CObjectEffectGroupDefRef* groupRef = new (SMemAlloc(sizeof(CObjectEffectGroupDefRef), __FILE__, __LINE__, 0)) CObjectEffectGroupDefRef();
        groupRef->m_groupDef = &groupLookup->m_objectEffectGroupDef;
        refList->m_groupDefRefs.LinkToTail(groupRef);
    }

    int32_t effectCount = g_objectEffectDB.m_numRecords;

    for (int32_t i = 0; i < effectCount; i++) {
        ObjectEffectRec* rec = g_objectEffectDB.GetRecordByIndex(i);
        OBJ_EFFECT_DEF_LOOKUP* effectLookup = s_effectDefLookup.New(rec->m_id, s_hashKeyNone, 0, 0);
        effectLookup->m_objectEffectDef.m_id = rec->m_id;
        effectLookup->m_objectEffectDef.m_effectRecType = rec->m_effectRecType;
        effectLookup->m_objectEffectDef.m_effectRecID = rec->m_effectRecID;
        effectLookup->m_objectEffectDef.m_attachment = rec->m_attachment;
        effectLookup->m_objectEffectDef.m_offset.x = rec->m_offset[0];
        effectLookup->m_objectEffectDef.m_offset.y = rec->m_offset[1];
        effectLookup->m_objectEffectDef.m_offset.z = rec->m_offset[2];

        ObjectEffectModifierRec* modifierRec = rec->m_objectEffectModifierID ? g_objectEffectModifierDB.GetRecord(rec->m_objectEffectModifierID) : NULL;

        if (rec->m_objectEffectModifierID && modifierRec->m_inputType > 0 && modifierRec->m_mapType > 0 && modifierRec->m_outputType > 0) {
            effectLookup->m_objectEffectDef.m_modifier = new (SMemAlloc(sizeof(CObjectEffectModifier), __FILE__, __LINE__, 0)) CObjectEffectModifier();
            effectLookup->m_objectEffectDef.m_modifier->Init(modifierRec);

            for (int32_t param = 0; param < effectLookup->m_objectEffectDef.m_modifier->m_numParams; param++) {
                effectLookup->m_objectEffectDef.m_modifier->SetParam(param, modifierRec->m_param[param]);
            }
        } else {
            effectLookup->m_objectEffectDef.m_modifier = NULL;
        }

        OBJ_EFFECT_GROUP_DEF_LOOKUP* groupLookup = s_groupDefLookup.Ptr(rec->m_objectEffectGroupID, s_hashKeyNone);
        CObjectEffectDefRef* effectRef = new (SMemAlloc(sizeof(CObjectEffectDefRef), __FILE__, __LINE__, 0)) CObjectEffectDefRef();
        effectRef->m_effectDef = &effectLookup->m_objectEffectDef;

        switch (rec->m_triggerType) {
            case 1:
                groupLookup->m_objectEffectGroupDef.m_enterEffects.LinkToTail(effectRef);
                break;

            case 2:
                groupLookup->m_objectEffectGroupDef.m_loopEffects.LinkToTail(effectRef);
                break;

            case 3:
                groupLookup->m_objectEffectGroupDef.m_exitEffects.LinkToTail(effectRef);
                break;

            case 4:
                groupLookup->m_objectEffectGroupDef.m_eventEffects.LinkToTail(effectRef);
                break;
        }
    }

    for (OBJ_EFFECT_PACKAGE_DEF_LOOKUP* packageLookup = s_packageDefLookup.m_fulllist.Head(); packageLookup; packageLookup = s_packageDefLookup.Next(packageLookup)) {
        CObjectEffectPackageDef& packageDef = packageLookup->m_objectEffectPackageDef;

        for (OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP* stateRefList = packageDef.m_stateGroupRefs.m_fulllist.Head(); stateRefList; stateRefList = packageDef.m_eventGroupRefs.Next(stateRefList)) {
            for (CObjectEffectGroupDefRef* groupRef = stateRefList->m_groupDefRefs.Head(); groupRef; groupRef = stateRefList->m_groupDefRefs.Next(groupRef)) {
                for (CObjectEffectDefRef* effectRef = groupRef->m_groupDef->m_eventEffects.Head(); effectRef; effectRef = groupRef->m_groupDef->m_eventEffects.Next(effectRef)) {
                    ObjectEffectRec* effectRec = g_objectEffectDB.GetRecord(effectRef->m_effectDef->m_id);
                    OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP* eventRefList = packageDef.m_eventGroupRefs.Ptr(effectRec->m_eventType, s_hashKeyNone);
                    bool found = false;

                    if (eventRefList) {
                        for (CObjectEffectGroupDefRef* eventRef = eventRefList->m_groupDefRefs.Head(); eventRef; eventRef = eventRefList->m_groupDefRefs.Next(eventRef)) {
                            if (eventRef->m_groupDef == groupRef->m_groupDef) {
                                found = true;
                                break;
                            }
                        }
                    }

                    if (found) {
                        continue;
                    }

                    eventRefList = packageDef.m_eventGroupRefs.New(effectRec->m_eventType, s_hashKeyNone, 0, 0);

                    CObjectEffectGroupDefRef* eventRef = new (SMemAlloc(sizeof(CObjectEffectGroupDefRef), __FILE__, __LINE__, 0)) CObjectEffectGroupDefRef();
                    eventRef->m_groupDef = groupRef->m_groupDef;
                    eventRefList->m_groupDefRefs.LinkToTail(eventRef);
                }
            }
        }
    }
}

// OFFSET: 0x005E1150/0x14025EDE0
C3Vector CObjectEffect::GetWorldPosition() {
    CGObject_C* object = this->m_manager->m_owner;
    CM2Model* model = object->GetObjectModel();
    C44Matrix transform;

    if (model && model->IsLoaded() && (model->m_fFlags & 0x4000)) {
        if (model->HasAttachment(this->m_def->m_attachment)) {
            transform = model->GetAttachmentWorldTransform(this->m_def->m_attachment);
        } else {
            transform = model->GetAnimatedWorldTransform();
        }
    } else {
        transform = object->GetMatrix();
    }

    C3Vector position = this->m_def->m_offset;
    position = position * transform;
    return position;
}

// INLINED
CObjectEffect::~CObjectEffect() {
    if (this->m_def->m_effectRecType == 1) {
        SE3::StopSound(this->m_soundKitObject, -1, __FILE__, __LINE__, "CObjectEffect::~CObjectEffect");
    }
}

// OFFSET: 0x00599E20/0x14025E5A0
float CObjectEffectModifier::InputNone(CObjectEffectManager* manager) {
    return -1.0f;
}

// OFFSET: 0x005E1130/0x14025EDD0
float CObjectEffectModifier::InputSpeed(CObjectEffectManager* manager) {
    return manager->m_owner->GetSpeed();
}

// OFFSET: 0x008A0A40/0x1407D7E80
float CObjectEffectModifier::MapNone(float input, const float* params) {
    return input;
}

// OFFSET: 0x005E08B0/0x14025E5B0
float CObjectEffectModifier::MapLinear(float input, const float* params) {
    if (input <= params[0]) {
        return params[2];
    }

    if (input >= params[1]) {
        return params[3];
    }

    return (input - params[0]) / (params[1] - params[0]) * (params[3] - params[2]) + params[2];
}

// OFFSET: 0x00401000/0x1407D7E80
void CObjectEffectModifier::OutputNone(CObjectEffect* effect, float value) {
}

// OFFSET: 0x005E0920/0x14025E5F0
void CObjectEffectModifier::OutputPitch(CObjectEffect* effect, float value) {
    SE3::SetFrequencyScale(effect->m_soundKitObject, value);
}

// OFFSET: 0x005E0940
void CObjectEffectModifier::Apply(CObjectEffectManager* manager, CObjectEffect* effect) {
    if (this->m_mapType <= 0) {
        return;
    }

    float input = CObjectEffectModifier::s_inputFuncs[this->m_inputType](manager);
    float value = CObjectEffectModifier::s_mapFuncs[this->m_mapType](input, this->m_params);
    CObjectEffectModifier::s_outputFuncs[this->m_outputType](effect, value);
}

// OFFSET: 0x005E09A0/0x14025E600
int32_t FindObjectEffectStateFromAnimID(int32_t animId, int32_t alternate) {
    if (alternate == 1) {
        return s_objectEffectAnimStateMap[animId].m_altStateID;
    }

    return s_objectEffectAnimStateMap[animId].m_stateID;
}

// INLINED
CObjectEffect::CObjectEffect() {
    this->m_def = NULL;
    this->m_manager = NULL;
    this->m_playCount = 0;
    this->m_refCount = 0;
}

// INLINED
CObjectEffectRef::CObjectEffectRef() {
    this->m_effect = NULL;
}

// INLINED
void CObjectEffect::StopLoopingEffect() {
    if (this->m_def->m_effectRecType == 1) {
        SE3::StopSound(this->m_soundKitObject, -1, __FILE__, __LINE__, "CObjectEffect::StopLoopingEffect");
    }
}

// OFFSET: 0x005E12B0/0x14025F040
void CObjectEffect::PlayOneShot() {
    C3Vector position = this->GetWorldPosition();

    if (this->m_def->m_effectRecType == 1) {
        SE3SoundKitProperties properties;
        properties.ResetToDefaults();
        properties.m_usage = 2;

        if (!SE3::IsPlaying(this->m_soundKitObject)) {
            SE3::PlaySoundKit(this->m_def->m_effectRecID, &position, &properties, &this->m_soundKitObject, __FILE__, __LINE__, "CObjectEffect::TriggerOneShotEffect", 0, NULL);
        }
    }

    this->m_playCount++;
}

// OFFSET: 0x005E1350/0x14025F120
void CObjectEffect::TriggerLoopingEffect() {
    if (this->m_playCount > 0) {
        return;
    }

    C3Vector position = this->GetWorldPosition();

    if (this->m_def->m_effectRecType == 1) {
        if (!SE3::IsPlaying(this->m_soundKitObject)) {
            int32_t usage = SE3::GetSoundKitUsage(this->m_def->m_effectRecID);

            SE3SoundKitProperties properties;
            properties.ResetToDefaults();
            properties.m_checkRange = 0;
            properties.m_fadeInTime = 700;
            properties.m_fadeOutTime = 700;
            properties.m_usage = usage != 1;
            properties.m_maxAudibleBehavior = 1;
            properties.m_priority = 128;

            SE3::PlaySoundKit(this->m_def->m_effectRecID, &position, &properties, &this->m_soundKitObject, __FILE__, __LINE__, "CObjectEffect::TriggerLoopingEffect()", 0, NULL);
            this->m_playCount++;
            return;
        }

        SE3::FadeInNow(this->m_soundKitObject);
    }

    this->m_playCount++;
}

// OFFSET: 0x005E1460
void CObjectEffect::UpdateLoopingEffect(CObjectEffectManager* manager) {
    if (this->m_def->m_effectRecType != 1 && this->m_def->m_effectRecType != 2) {
        return;
    }

    C3Vector position = this->GetWorldPosition();

    if (SE3::IsPlaying(this->m_soundKitObject)) {
        SE3::FadeInNow(this->m_soundKitObject);
    } else {
        this->TriggerLoopingEffect();
    }

    if (this->m_def->m_modifier) {
        this->m_def->m_modifier->Apply(manager, this);
    }

    SE3::Set3DPosition(this->m_soundKitObject, position);
}

// OFFSET: 0x005E14D0/0x14025F260
void CObjectEffectManager::PropertyChanged(int32_t modifierId) {
    OBJ_EFFECT_REF_LIST_LOOKUP* refList = this->m_refLists.Ptr(modifierId, s_hashKeyNone);

    if (!refList) {
        return;
    }

    for (CObjectEffectRef* ref = refList->m_refs.Head(); ref; ref = refList->m_refs.Next(ref)) {
        CObjectEffectModifier* modifier = ref->m_effect->m_def->m_modifier;

        if (modifier) {
            modifier->Apply(this, ref->m_effect);
        }
    }
}

// OFFSET: 0x005E23A0/0x140260630
void CObjectEffectManager::UpdateAllPlayingEffects() {
    for (CObjectEffect* effect = this->m_loopingEffects.Head(); effect; effect = this->m_loopingEffects.Next(effect)) {
        effect->UpdateLoopingEffect(this);
    }

    CObjectEffect* effect = this->m_oneShotEffects.Head();

    while (effect) {
        CObjectEffect* next = this->m_oneShotEffects.Next(effect);

        if (static_cast<uint32_t>(effect->m_def->m_effectRecType - 1) <= 1) {
            C3Vector position = effect->GetWorldPosition();

            if (SE3::IsPlaying(effect->m_soundKitObject)) {
                SE3::Set3DPosition(effect->m_soundKitObject, position);
                CObjectEffectModifier* modifier = effect->m_def->m_modifier;

                if (modifier && modifier->m_mapType > 0) {
                    modifier->Apply(this, effect);
                }

                effect = next;
                continue;
            }
        }

        int32_t modifierId = effect->m_def->m_modifier ? effect->m_def->m_modifier->m_id : 0;
        OBJ_EFFECT_REF_LIST_LOOKUP* refList = this->m_refLists.Ptr(modifierId, s_hashKeyNone);

        if (refList) {
            CObjectEffectRef* ref = refList->m_refs.Head();

            while (ref) {
                CObjectEffectRef* nextRef = refList->m_refs.Next(ref);

                if (ref->m_effect == effect) {
                    refList->m_refs.DeleteNode(ref);
                }

                ref = nextRef;
            }
        }

        effect->m_playCount--;
        effect->~CObjectEffect();
        SMemFree(effect, typeid(CObjectEffect).raw_name(), SLOG_OBJECT, 0);
        effect = next;
    }
}

// OFFSET: 0x005E5000/0x140264940
CObjectEffectManager::CObjectEffectManager() {
    this->m_states.SetRawData(static_cast<char*>(SMemAlloc((NUM_OBJECT_EFFECT_STATES + 7) / 8, __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY)), NUM_OBJECT_EFFECT_STATES, true);
}

// OFFSET: 0x005E5710
void CObjectEffectManager::PlayOneShotEffect(CObjectEffectDef* def) {
    CObjectEffect* effect = new (__FILE__, __LINE__) CObjectEffect();
    effect->m_manager = this;
    effect->m_def = def;
    this->m_oneShotEffects.LinkToTail(effect);

    int32_t modifierId = def->m_modifier ? def->m_modifier->m_id : 0;
    OBJ_EFFECT_REF_LIST_LOOKUP* refList = this->m_refLists.Ptr(modifierId, s_hashKeyNone);

    if (!refList) {
        refList = this->m_refLists.New(modifierId, s_hashKeyNone, 0, 0);
    }

    CObjectEffectRef* ref = new (__FILE__, __LINE__) CObjectEffectRef();
    ref->m_effect = effect;
    refList->m_refs.LinkToTail(ref);

    effect->PlayOneShot();

    if (def->m_modifier) {
        def->m_modifier->Apply(this, effect);
    }
}

// OFFSET: 0x005E6640/0x140266AC0
void CObjectEffectManager::ExitState(uint32_t state, int32_t playExitEffects) {
    if (!this->m_states.Get(state)) {
        return;
    }

    this->m_states.Set(state, false);

    OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP* stateRefs = this->m_packageDef->m_stateGroupRefs.Ptr(state, s_hashKeyNone);

    if (!stateRefs) {
        return;
    }

    for (CObjectEffectGroupDefRef* groupRef = stateRefs->m_groupDefRefs.Head(); groupRef; groupRef = stateRefs->m_groupDefRefs.Next(groupRef)) {
        if (playExitEffects) {
            for (CObjectEffectDefRef* effectRef = groupRef->m_groupDef->m_exitEffects.Head(); effectRef; effectRef = groupRef->m_groupDef->m_exitEffects.Next(effectRef)) {
                this->PlayOneShotEffect(effectRef->m_effectDef);
            }
        }

        int32_t clear = 1;
        OBJ_EFFECT_GROUP_LOOKUP* group = this->m_groups.Ptr(groupRef->m_groupDef->m_id, s_hashKeyNone);

        if (!group) {
            continue;
        }

        for (OBJ_EFFECT_REF_LOOKUP* lookup = group->m_effects.m_fulllist.Head(); lookup; lookup = group->m_effects.Next(lookup)) {
            CObjectEffect* effect = lookup->m_effect;
            effect->m_refCount--;

            if (effect->m_refCount) {
                clear = 0;
                continue;
            }

            this->m_loopingEffects.UnlinkNode(effect);
            this->m_oneShotEffects.LinkToTail(effect);

            effect->StopLoopingEffect();
        }

        if (clear) {
            group->m_effects.InternalClear(0);
        }
    }
}

// OFFSET: 0x005E57F0/0x1402652C0
void CObjectEffectManager::PlayLoopingEffect(CObjectEffectDef* def, int32_t groupId) {
    OBJ_EFFECT_GROUP_LOOKUP* group = this->m_groups.Ptr(groupId, s_hashKeyNone);

    if (!group) {
        group = this->m_groups.New(groupId, s_hashKeyNone, 0, 0);
    }

    for (CObjectEffect* effect = this->m_oneShotEffects.Head(); effect; effect = this->m_oneShotEffects.Next(effect)) {
        if (effect->m_def != def) {
            continue;
        }

        OBJ_EFFECT_REF_LOOKUP* lookup = group->m_effects.New(def->m_id, s_hashKeyNone, 0, 0);
        lookup->m_effect = effect;
        effect->m_link.Unlink();
        this->m_loopingEffects.LinkToTail(effect);

        if (effect->m_def->m_effectRecType == 1) {
            SE3::FadeInNow(effect->m_soundKitObject);
            effect->m_refCount++;
        }

        return;
    }

    OBJ_EFFECT_REF_LOOKUP* lookup = group->m_effects.Ptr(def->m_id, s_hashKeyNone);

    if (lookup) {
        lookup->m_effect->m_refCount++;
        return;
    }

    lookup = group->m_effects.New(def->m_id, s_hashKeyNone, 0, 0);
    lookup->m_effect = new (__FILE__, __LINE__) CObjectEffect();
    lookup->m_effect->m_manager = this;
    lookup->m_effect->m_def = def;
    this->m_loopingEffects.LinkToTail(lookup->m_effect);
    lookup->m_effect->m_refCount++;

    int32_t modifierId = def->m_modifier ? def->m_modifier->m_id : 0;
    OBJ_EFFECT_REF_LIST_LOOKUP* refList = this->m_refLists.Ptr(modifierId, s_hashKeyNone);

    if (!refList) {
        refList = this->m_refLists.New(modifierId, s_hashKeyNone, 0, 0);
    }

    CObjectEffectRef* ref = new (__FILE__, __LINE__) CObjectEffectRef();
    ref->m_effect = lookup->m_effect;
    refList->m_refs.LinkToTail(ref);

    lookup->m_effect->TriggerLoopingEffect();

    if (def->m_modifier) {
        def->m_modifier->Apply(this, lookup->m_effect);
    }
}

// OFFSET: 0x005E6900/0x140267090
void CObjectEffectManager::ShutDown() {
    for (int32_t state = 0; state < NUM_OBJECT_EFFECT_STATES; state++) {
        this->ExitState(state, 0);
    }

    for (OBJ_EFFECT_REF_LIST_LOOKUP* refList = this->m_refLists.m_fulllist.Head(); refList; refList = this->m_refLists.Next(refList)) {
        CObjectEffectRef* ref = refList->m_refs.Head();

        while (ref) {
            CObjectEffectRef* next = refList->m_refs.Next(ref);
            refList->m_refs.UnlinkNode(ref);
            delete ref;
            ref = next;
        }
    }

    this->m_refLists.Clear();
    this->m_groups.Clear();

    while (CObjectEffect* effect = this->m_loopingEffects.Head()) {
        effect->~CObjectEffect();
        SMemFree(effect, typeid(CObjectEffect).raw_name(), SLOG_OBJECT, 0);
    }

    while (CObjectEffect* effect = this->m_oneShotEffects.Head()) {
        effect->~CObjectEffect();
        SMemFree(effect, typeid(CObjectEffect).raw_name(), SLOG_OBJECT, 0);
    }
}

// OFFSET: 0x005E6550/0x140266940
void CObjectEffectManager::EnterState(uint32_t state, int32_t playEnterEffects, int32_t refresh) {
    if (refresh) {
        if (!this->m_states.Get(state)) {
            return;
        }
    } else {
        if (this->m_states.Get(state)) {
            return;
        }

        this->m_states.Set(state, true);
    }

    OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP* stateRefs = this->m_packageDef->m_stateGroupRefs.Ptr(state, s_hashKeyNone);

    if (!stateRefs) {
        return;
    }

    for (CObjectEffectGroupDefRef* groupRef = stateRefs->m_groupDefRefs.Head(); groupRef; groupRef = stateRefs->m_groupDefRefs.Next(groupRef)) {
        if (playEnterEffects) {
            for (CObjectEffectDefRef* effectRef = groupRef->m_groupDef->m_enterEffects.Head(); effectRef; effectRef = groupRef->m_groupDef->m_enterEffects.Next(effectRef)) {
                this->PlayOneShotEffect(effectRef->m_effectDef);
            }
        }

        for (CObjectEffectDefRef* effectRef = groupRef->m_groupDef->m_loopEffects.Head(); effectRef; effectRef = groupRef->m_groupDef->m_loopEffects.Next(effectRef)) {
            this->PlayLoopingEffect(effectRef->m_effectDef, groupRef->m_groupDef->m_id);
        }
    }
}

// OFFSET: 0x005E6A60/0x140267360
CObjectEffectManager::~CObjectEffectManager() {
    this->ShutDown();
}

// OFFSET: 0x005E68A0/0x140266DB0
int32_t CObjectEffectManager::Init(int32_t packageId, CGObject_C* owner) {
    this->m_packageId = packageId;

    OBJ_EFFECT_PACKAGE_DEF_LOOKUP* lookup = s_packageDefLookup.Ptr(packageId, s_hashKeyNone);

    if (!lookup) {
        return 0;
    }

    this->m_packageDef = &lookup->m_objectEffectPackageDef;
    this->m_owner = owner;
    this->EnterState(1, 1, 0);
    this->EnterState(38, 1, 0);
    return 1;
}
