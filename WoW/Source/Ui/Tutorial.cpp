#include "Tutorial.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstring>
#include "WoW/Source/Net/Types.h"

// STATIC: 0x00D10978
static const uint32_t s_forcedTutorials[] = { 56, 33, 3, 4, 10, 54, 65 };

// STATIC: 0x00D10994/0x140BC6890
static const uint32_t s_repeatableTutorials[] = { 56, 65 };

// STATIC: 0x00F33758/0x140E26700
static int32_t s_tutorialsCompleted[65];

// STATIC: 0x00F3385C/0x140E26808
TSBitField<TSFixedArray<uint32_t> > CGTutorial::m_tutorialShown;

// STATIC: 0x00F3386C/0x140E26820
TSBitField<TSFixedArray<uint32_t> > CGTutorial::m_tutorialFlags;

// OFFSET: 0x00901F70
int32_t CGTutorial::OnTutorialFlags(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t bytes = msg->m_size - msg->m_read;
    void* data;
    msg->GetDataInSitu(data, bytes);
    CGTutorial::m_tutorialShown.Load(data, bytes, -1);
    CGTutorial::m_tutorialFlags.Load(data, bytes, -1);

    return 1;
}

// OFFSET: 0x00401000/0x1407D7E80
void CGTutorial::InitializeGame() {
}

// OFFSET: 0x00901690
static int32_t AddCompletedTutorial(uint32_t tutorial) {
    int32_t index = 0;

    do {
        int32_t completed = s_tutorialsCompleted[index];

        if (completed == static_cast<int32_t>(tutorial + 1)) {
            break;
        }

        if (!completed) {
            s_tutorialsCompleted[index] = tutorial + 1;
            return index;
        }

        index++;
    } while (index < 65);

    return index;
}

// OFFSET: 0x00901880/0x140659D40
void CGTutorial::FlagTutorial(uint32_t tutorial) {
    uint32_t bit = 1 << (tutorial & 0x1F);
    uint32_t index = tutorial >> 5;

    if (CGTutorial::m_tutorialFlags.m_bits.m_data[index] & bit) {
        return;
    }

    for (uint32_t i = 0; ; i++) {
        if (s_repeatableTutorials[i] == tutorial) {
            return;
        }

        if (s_repeatableTutorials[i] == 65) {
            break;
        }
    }

    CGTutorial::m_tutorialShown.m_bits.m_data[index] |= bit;
    CGTutorial::m_tutorialFlags.m_bits.m_data[index] |= bit;
    AddCompletedTutorial(tutorial);

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_TUTORIAL_FLAG));
    msg.Put(tutorial);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00901960/0x140659E90
void CGTutorial::ClearTutorials() {
    if (CGTutorial::m_tutorialShown.m_count) {
        memset(CGTutorial::m_tutorialShown.m_bits.m_data, 0xFF, sizeof(uint32_t) * CGTutorial::m_tutorialShown.m_bits.m_count);
    }

    if (CGTutorial::m_tutorialFlags.m_count) {
        memset(CGTutorial::m_tutorialFlags.m_bits.m_data, 0xFF, sizeof(uint32_t) * CGTutorial::m_tutorialFlags.m_bits.m_count);
    }

    for (uint32_t i = 0; i < 65; i++) {
        if (CGTutorial::m_tutorialFlags.m_count && (CGTutorial::m_tutorialFlags.m_bits.m_data[i >> 5] & (1 << (i & 0x1F)))) {
            continue;
        }

        bool repeatable = false;

        for (uint32_t j = 0; ; j++) {
            if (s_repeatableTutorials[j] == i) {
                repeatable = true;
                break;
            }

            if (s_repeatableTutorials[j] == 65) {
                break;
            }
        }

        if (!repeatable) {
            AddCompletedTutorial(i);
        }
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_TUTORIAL_CLEAR));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00901A90/0x140659FF0
void CGTutorial::ResetTutorials() {
    if (CGTutorial::m_tutorialShown.m_count) {
        memset(CGTutorial::m_tutorialShown.m_bits.m_data, 0, sizeof(uint32_t) * CGTutorial::m_tutorialShown.m_bits.m_count);
    }

    if (CGTutorial::m_tutorialFlags.m_count) {
        memset(CGTutorial::m_tutorialFlags.m_bits.m_data, 0, sizeof(uint32_t) * CGTutorial::m_tutorialFlags.m_bits.m_count);
    }

    memset(s_tutorialsCompleted, 0, sizeof(s_tutorialsCompleted));
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_TUTORIAL_RESET));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00901DF0/0x14065A3A0
void CGTutorial::TriggerTutorial(uint32_t tutorial) {
    if (!CGTutorial::m_tutorialShown.m_count) {
        return;
    }

    bool repeatable = false;

    for (uint32_t i = 0; ; i++) {
        if (s_repeatableTutorials[i] == tutorial) {
            repeatable = true;
            break;
        }

        if (s_repeatableTutorials[i] == 65) {
            break;
        }
    }

    uint32_t bit = 1 << (tutorial & 0x1F);
    uint32_t index = tutorial >> 5;

    if (CGTutorial::m_tutorialShown.m_bits.m_data[index] & bit) {
        return;
    }

    if (CVar::Lookup("showTutorials")->m_intValue) {
        SE3SoundKitProperties properties;
        SE3::PlaySoundKit("TutorialPopup", NULL, &properties, NULL, __FILE__, __LINE__, "CGTutorial::TriggerTutorial");
        bool forced = false;

        for (uint32_t i = 0; ; i++) {
            if (s_forcedTutorials[i] == tutorial) {
                forced = true;
                break;
            }

            if (s_forcedTutorials[i] == 65) {
                break;
            }
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_TUTORIAL_TRIGGER, "%d%b", tutorial + 1, forced);

        if (!repeatable) {
            CGTutorial::m_tutorialShown.m_bits.m_data[index] |= bit;
        }
    } else {
        CGTutorial::m_tutorialShown.m_bits.m_data[index] |= bit;
        CGTutorial::FlagTutorial(tutorial);
    }
}

// OFFSET: 0x00901C00/0x14065A170
void CGTutorial::LoadFlaggedTutorials() {
    memset(s_tutorialsCompleted, 0, sizeof(s_tutorialsCompleted));
    CVarIntListWrapper list;
    list.m_cvar = CVar::Lookup("flaggedTutorials");
    list.m_pos = 2;
    list.m_state = 0;
    int32_t* tutorial = s_tutorialsCompleted;

    for (uint32_t value = list.GetNextValue(); value; value = list.GetNextValue()) {
        *tutorial++ = value;
    }

    if (list.m_state != 1) {
        list.m_buffer[0] = 'v';
        list.m_buffer[1] = 1;
        list.m_buffer[list.m_pos] = 0;
        list.m_cvar->Set(list.m_buffer, true, false, false, true);
    }
}

// OFFSET: 0x00401000/0x1407D7E80
void CGTutorial::ShutdownGame() {
}

// OFFSET: 0x009016C0/0x140659B30
int32_t CGTutorial::GetNextCompletedTutorial(int32_t tutorial) {
    bool found = false;

    for (int32_t i = 0; i < 65; i++) {
        if (found && s_tutorialsCompleted[i]) {
            return s_tutorialsCompleted[i];
        }

        if (s_tutorialsCompleted[i] == tutorial + 1) {
            found = true;
        }
    }

    return 65;
}

// OFFSET: 0x00901710
int32_t CGTutorial::GetPrevCompletedTutorial(int32_t tutorial) {
    bool found = false;

    for (int32_t i = 65; i >= 0; i--) {
        if (found && s_tutorialsCompleted[i]) {
            return s_tutorialsCompleted[i];
        }

        if (s_tutorialsCompleted[i] == tutorial + 1) {
            found = true;
        }
    }

    return 0;
}

// OFFSET: 0x00901B60/0x14065A0D0
void CGTutorial::SaveFlaggedTutorials() {
    CVarIntListWrapper list;
    list.m_cvar = CVar::Lookup("flaggedTutorials");
    list.m_pos = 2;
    list.m_state = 0;

    for (uint32_t i = 0; i < 65; i++) {
        if (!s_tutorialsCompleted[i]) {
            break;
        }

        list.AddNextValue(s_tutorialsCompleted[i]);
    }

    if (list.m_state != 1) {
        list.m_buffer[0] = 'v';
        list.m_buffer[1] = 1;
        list.m_buffer[list.m_pos] = 0;
        list.m_cvar->Set(list.m_buffer, true, false, false, true);
    }
}
