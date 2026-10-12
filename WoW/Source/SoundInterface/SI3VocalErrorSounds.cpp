#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"

// STATIC: 0x00D2810C/0x140BE6BDC
static int32_t s_vocalErrorRace = -1;

// STATIC: 0x0100A6E8/0x140EE21F0
static int32_t s_vocalErrorSounds[68];

// STATIC: 0x0100A7F8/0x140EE2300
static uint32_t s_vocalErrorSound;

// STATIC: 0x0100A7FC/0x140EE2304
static uint32_t s_vocalErrorSex;

// OFFSET: 0x00B357C0/0x140900AC0
void SI3::InitVocalErrorSounds(int32_t race, uint32_t sex) {
    memset(s_vocalErrorSounds, 0, sizeof(s_vocalErrorSounds));

    if (sex > 1) {
        return;
    }

    for (int32_t i = 0; i < g_vocalUISoundsDB.m_numRecords; i++) {
        VocalUISoundsRec* rec = g_vocalUISoundsDB.GetRecordByIndex(i);

        if (rec && rec->m_vocalUienum < 68 && rec->m_raceID == race) {
            s_vocalErrorSounds[rec->m_vocalUienum] = rec->m_normalSoundID[sex];
        }
    }
}

// OFFSET: 0x00B35830/0x140900B60
void SI3::PlayVocalErrorSound(int32_t error) {
    static CVar* s_enableErrorSpeech = CVar::Lookup("Sound_EnableErrorSpeech");

    if (!s_enableErrorSpeech || !s_enableErrorSpeech->m_intValue || error >= static_cast<int32_t>(sizeof(s_vocalErrorSounds) / sizeof(s_vocalErrorSounds[0]))) {
        return;
    }

    if (SE3::IsPlaying(s_vocalErrorSound)) {
        return;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return;
    }

    int32_t race = player->GetCurrentClientRace();
    uint32_t sex = player->m_player->m_playerBytes3[0];

    if (race != s_vocalErrorRace || sex != s_vocalErrorSex) {
        s_vocalErrorRace = race;
        s_vocalErrorSex = sex;
        SI3::InitVocalErrorSounds(race, sex);
    }

    int32_t soundKitID = s_vocalErrorSounds[error];

    SE3SoundKitProperties properties;
    properties.m_highPriorityDownload = 1;
    properties.m_bus = SE3BUS_10;

    if (player->m_unit->m_unitFieldBytes0[1] == 6) {
        properties.m_attachCustomDSPChain = 1;
        properties.m_fpCustomDSPChainCallback = SI3::DSP_AttachCustomDspChain;
        SI3::DK_GetPlayerFilterName(player, properties.m_sCustomDSPChainName, sizeof(properties.m_sCustomDSPChainName));
    }

    SE3::PlaySoundKit(soundKitID, NULL, &properties, &s_vocalErrorSound, __FILE__, __LINE__, "SI3::PlayVocalErrorSound");
}
