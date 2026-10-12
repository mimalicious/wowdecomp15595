#include <cstddef>
#include <cstdint>
#include "WoW/Source/SoundInterface/SI3Combat.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ResistancesRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/SoundInterface/SI3.h"

// STATIC: 0x00E28808/0x140D10458
static TSFixedArray<IMPACTSOUNDARRAY> s_impactSounds;

// STATIC: 0x00E28814
static int32_t s_swingSounds[3][2];

// PROVEN: 939e3a79 IDENTICAL/IDENTICAL
// OFFSET: 0x00704C50/0x1403D1A70
IMPACTSOUND::IMPACTSOUND() {
    this->m_soundId = 0;
    this->m_critSoundId = 0;
}

// OFFSET: 0x00631CD0/0x1402C71D0
void SI3::PlayWeaponSwooshSound(int32_t type, int32_t crit, const C3Vector& position, int32_t quiet, int32_t self) {
    if (type >= 3) {
        return;
    }

    float volume = quiet ? 0.5f : 1.0f;

    SE3SoundKitProperties properties;
    properties.m_bus = SE3BUS_13;
    properties.unk_0004 = volume;

    if (self) {
        properties.m_priority = 110;
    }

    SE3::PlaySoundKit(s_swingSounds[type][crit != 0], self ? NULL : &position, &properties, NULL, __FILE__, __LINE__, "SI3::PlayWeaponSwooshSound");
}

// PROVEN: 38532f83 IDENTICAL/DIFFERENT
// OFFSET: 0x00631DA0/0x1402C72F0
void SI3::InitWeaponSwingCombatSounds() {
    for (int32_t i = g_weaponSwingSounds2DB.m_numRecords; i;) {
        i--;

        WeaponSwingSounds2Rec* rec = g_weaponSwingSounds2DB.GetRecordByIndex(i);

        if (rec && rec->m_swingType < 3 && rec->m_crit < 2) {
            s_swingSounds[rec->m_swingType][rec->m_crit != 0] = rec->m_soundID;
        }
    }
}

// OFFSET: 0x00631E00/0x1402C7360
static void InitWeaponImpactSound(WeaponImpactSoundsRec* rec) {
    if (!s_impactSounds.m_count) {
        return;
    }

    IMPACTSOUNDARRAY* sounds = &s_impactSounds.m_data[rec->m_weaponSubClassID];

    for (int32_t i = 0; i < 10; i++) {
        sounds->m_sounds[i][rec->m_parrySoundType].m_soundId = rec->m_impactSoundID[i];
        sounds->m_sounds[i][rec->m_parrySoundType].m_critSoundId = rec->m_critImpactSoundID[i];
    }
}

// PROVEN: e771aac4 IDENTICAL/UNPROVEN
// OFFSET: 0x00631F60/0x1402C7510
void SI3::PlaySheatheSound(const CGUnitVirtualItem* item, int32_t sheathe, const C3Vector& position, int32_t self) {
    if (!item) {
        return;
    }

    MaterialRec* material = g_materialDB.GetRecord(item->m_material);

    if (!material) {
        return;
    }

    C3Vector soundPosition;
    soundPosition.x = position.x;
    soundPosition.y = position.y;
    soundPosition.z = position.z + 2.0f;

    int32_t soundKitId = sheathe ? material->m_sheatheSoundID : material->m_unsheatheSoundID;

    SE3SoundKitProperties properties;

    if (self) {
        properties.m_priority = 110;
    }

    SE3::PlaySoundKit(soundKitId, self ? NULL : &soundPosition, &properties, NULL, __FILE__, __LINE__, "SI3::PlaySheatheSound");
}

// PROVEN: 414a7bec IDENTICAL/DIFFERENT
// OFFSET: 0x00632050/0x1402C7640
void SI3::PlayMissedSound(bool oneHand, const C3Vector& position, int32_t self) {
    SE3SoundKitProperties properties;

    if (self) {
        properties.m_priority = 110;
    }

    SE3::PlaySoundKit(oneHand ? "(DONOTRENAME)Combat Miss 1H" : "(DONOTRENAME)Combat Miss 2H", self ? NULL : &position, &properties, NULL, __FILE__, __LINE__, "SI3::PlayMissedSound");
}

// OFFSET: 0x006320E0/0x1402C7700
void SI3::PlaySpellFizzleSound(uint32_t spellId, const CGObject_C* object, int32_t self) {
    if (!object) {
        return;
    }

    SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return;
    }

    ResistancesRec* resistance;
    int32_t school = 0;

    while (true) {
        if (((1 << school) & spell->m_schoolMask) && (resistance = g_resistancesDB.GetRecord(school))) {
            break;
        }

        if (++school >= 7) {
            return;
        }
    }

    C3Vector position = object->GetPosition();

    SE3SoundKitProperties properties;

    if (self) {
        properties.m_priority = 110;
    }

    properties.m_uniqueness = 2;
    SE3::PlaySoundKit(resistance->m_fizzleSoundID, self ? NULL : &position, &properties, NULL, __FILE__, __LINE__, "SI3::PlaySpellFizzleSound");
}

// OFFSET: 0x00632200/0x1402C7890
void SI3::PlayParrySound(const CGUnitVirtualItem* weapon, const CGUnitVirtualItem* defender, int32_t crit, const C3Vector& position, int32_t self) {
    if (!s_impactSounds.m_count) {
        return;
    }

    int32_t subclass;
    int32_t metal;

    if (weapon) {
        subclass = weapon->m_soundOverrideSubclassID;

        if (subclass == 0xFF) {
            subclass = weapon->m_subclassID;
        }

        metal = CGItem_C::IsMetal(weapon->m_material) != 0;
    } else {
        metal = 0;
        subclass = ClientDBGetUnarmedWeapon();
    }

    int32_t type = 6;

    if (defender) {
        if (defender->m_classID == 2) {
            type = 6 - (CGItem_C::IsMetal(defender->m_material) != 0);
        } else if (defender->m_classID == 4) {
            type = 4 - (CGItem_C::IsMetal(defender->m_material) != 0);
        }
    }

    IMPACTSOUND* sound = &s_impactSounds.m_data[subclass].m_sounds[type][metal];

    C3Vector soundPosition;
    soundPosition.x = position.x;
    soundPosition.y = position.y;
    soundPosition.z = position.z + 2.0f;

    SE3SoundKitProperties properties;
    properties.m_bus = SE3BUS_17;

    if (self) {
        properties.m_priority = 110;
    }

    SE3::PlaySoundKit(crit != 0 ? sound->m_critSoundId : sound->m_soundId, self ? NULL : &soundPosition, &properties, NULL, __FILE__, __LINE__, "SI3::PlayParrySound");
}

// OFFSET: 0x00632350/0x1402C7A50
void SI3::PlayHitSound(const CGUnitVirtualItem* weapon, uint32_t impactType, int32_t crit, const C3Vector& position, int32_t self) {
    if (!s_impactSounds.m_count) {
        return;
    }

    int32_t subclass;
    int32_t metal;

    if (weapon) {
        subclass = weapon->m_soundOverrideSubclassID;

        if (subclass == 0xFF) {
            subclass = weapon->m_subclassID;
        }

        metal = CGItem_C::IsMetal(weapon->m_material) != 0;
    } else {
        metal = 0;
        subclass = ClientDBGetUnarmedWeapon();
    }

    IMPACTSOUND* sound = &s_impactSounds.m_data[subclass].m_sounds[impactType][metal];

    SE3SoundKitProperties properties;
    properties.m_bus = SE3BUS_17;

    if (self) {
        properties.m_priority = 110;
    }

    SE3::PlaySoundKit(crit != 0 ? sound->m_critSoundId : sound->m_soundId, self ? NULL : &position, &properties, NULL, __FILE__, __LINE__, "SI3::PlayHitSound");
}

// PROVEN: 7e441fba IDENTICAL/DIFFERENT
// OFFSET: 0x00632710/0x1402C7F30
void SI3::InitWeaponImpactCombatSounds() {
    s_impactSounds.SetCount(ClientDBGetNumWeaponSubClasses());

    for (int32_t i = g_weaponImpactSoundsDB.m_numRecords; i;) {
        i--;

        WeaponImpactSoundsRec* rec = g_weaponImpactSoundsDB.GetRecordByIndex(i);
        InitWeaponImpactSound(rec);
    }
}
