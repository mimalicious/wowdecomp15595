#include <cstddef>
#include <cstdint>
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/SoundInterface/SI3.h"

// STATIC: 0x0100A6CC/0x140EE21D8
static uint32_t s_glueAmbience;

// STATIC: 0x0100A6D0/0x140EE21DC
static uint32_t s_glueMusic;

// STATIC: 0x0100A6D4/0x140EE21E0
static uint32_t s_creditsMusic;

// STATIC: 0x0100A6D8/0x140EE21E4
static int32_t s_creditsMusicKitID;

// STATIC: 0x0100A6DC/0x140EE21E8
static int32_t s_glueMusicKitID;

// STATIC: 0x0100A6E0/0x140EE21EC
static int32_t s_glueAmbienceKitID;

// OFFSET: 0x00B352C0/0x140900490
void SI3::StopGlueAmbience(int32_t fadeOutTime) {
    SE3::StopSound(s_glueAmbience, fadeOutTime, __FILE__, __LINE__, "SI3::StopGlueAmbience");
}

// OFFSET: 0x00B352F0/0x1409004C0
void SI3::StartGlueAmbience(const char* name, int32_t fadeInTime) {
    int32_t soundKitID = SE3::GetSoundKitID(name);

    if (SE3::IsPlaying(s_glueAmbience)) {
        if (soundKitID == SE3::GetSoundKitID(s_glueAmbience)) {
            return;
        }

        SI3::StopGlueAmbience(3000);
    }

    if (soundKitID <= 0) {
        return;
    }

    s_glueAmbienceKitID = soundKitID;

    SE3SoundKitProperties properties;
    properties.m_bit10 = 1;
    properties.m_bus = SE3BUS_AMBIENCE;
    properties.m_usage = 1;
    properties.m_fadeInTime = fadeInTime;
    SE3::PlaySoundKit(soundKitID, NULL, &properties, &s_glueAmbience, __FILE__, __LINE__, "SI3::StartGlueAmbience", 1, NULL);
}

// OFFSET: 0x00B353D0/0x1409005E0
int32_t SI3::GlueMusicUpdate(const void* data, void* param) {
    if (!SE3::IsPlaying(s_glueMusic)) {
        SE3SoundKitProperties properties;
        properties.m_bit10 = 1;
        properties.m_bus = SE3BUS_MUSIC;
        SE3::PlaySoundKit(s_glueMusicKitID, NULL, &properties, &s_glueMusic, __FILE__, __LINE__, "SI3::GlueMusicUpdate", 1, NULL);
    }

    return 1;
}

// OFFSET: 0x00B35460/0x1409006A0
void SI3::StopGlueMusic(int32_t fadeOutTime) {
    EventUnregister(EVENT_ID_POLL, SI3::GlueMusicUpdate);
    SE3::StopSound(s_glueMusic, fadeOutTime, __FILE__, __LINE__, "SI3::StopGlueMusic");
}

// OFFSET: 0x00B354A0/0x1409006F0
int32_t SI3::CreditsMusicUpdate(const void* data, void* param) {
    if (!SE3::IsPlaying(s_creditsMusic)) {
        SE3SoundKitProperties properties;
        properties.m_bit10 = 1;
        properties.m_bus = SE3BUS_MUSIC;
        properties.m_selectMode = 0;
        SE3::PlaySoundKit(s_creditsMusicKitID, NULL, &properties, &s_creditsMusic, __FILE__, __LINE__, "SI3::CreditsMusicUpdate", 1, NULL);
    }

    return 1;
}

// OFFSET: 0x00B35540/0x1409007B0
void SI3::StopCreditsMusic(int32_t fadeOutTime) {
    EventUnregister(EVENT_ID_POLL, SI3::CreditsMusicUpdate);
    SE3::StopSound(s_creditsMusic, fadeOutTime, __FILE__, __LINE__, "SI3::StopCreditsMusic");
}

// OFFSET: 0x00B35580/0x140900800
void SI3::StartGlueMusic(const char* name, int32_t fadeInTime) {
    int32_t soundKitID = SE3::GetSoundKitID(name);

    if (SE3::IsPlaying(s_glueMusic)) {
        if (soundKitID == SE3::GetSoundKitID(s_glueMusic)) {
            return;
        }

        SI3::StopGlueMusic(500);
    }

    SI3::StopCreditsMusic(500);

    if (soundKitID <= 0) {
        return;
    }

    s_glueMusicKitID = soundKitID;

    SE3SoundKitProperties properties;
    properties.m_bit10 = 1;
    properties.m_bus = SE3BUS_MUSIC;
    properties.m_usage = 1;
    properties.m_fadeInTime = fadeInTime;
    SE3::PlaySoundKit(soundKitID, NULL, &properties, &s_glueMusic, __FILE__, __LINE__, "SI3::StartGlueMusic", 1, NULL);
    EventRegister(EVENT_ID_POLL, SI3::GlueMusicUpdate);
}

// OFFSET: 0x00B356B0/0x140900970
void SI3::StartCreditsMusic(const char* name) {
    SI3::StopCreditsMusic(500);
    SI3::StopGlueMusic(3000);

    if (!name) {
        return;
    }

    s_creditsMusicKitID = SE3::GetSoundKitID(name);
    SE3::ResetSoundKitOrder(s_creditsMusicKitID);

    SE3SoundKitProperties properties;
    properties.m_bit10 = 1;
    properties.m_bus = SE3BUS_MUSIC;
    properties.m_selectMode = 0;
    SE3::PlaySoundKit(s_creditsMusicKitID, NULL, &properties, &s_creditsMusic, __FILE__, __LINE__, "SI3::StartCreditsMusic", 1, NULL);
    EventRegister(EVENT_ID_POLL, SI3::CreditsMusicUpdate);
}
