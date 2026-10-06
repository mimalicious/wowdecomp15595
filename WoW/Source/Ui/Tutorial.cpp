#include "Tutorial.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstring>
#include "WoW/Source/Net/Types.h"

// STATIC: #0x00BD19F0
static int32_t s_tutorialsCompleted[60];

// STATIC: #0x00BD1AE0
TSBitField<TSFixedArray<uint32_t> > CGTutorial::m_tutorialShown;

// STATIC: #0x00BD1AF0
TSBitField<TSFixedArray<uint32_t> > CGTutorial::m_tutorialFlags;

// OFFSET: #0x00530920
int32_t CGTutorial::OnTutorialFlags(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t bytes = msg->m_size - msg->m_read;
    void* data;
    msg->GetDataInSitu(data, bytes);
    CGTutorial::m_tutorialShown.Load(data, bytes, -1);
    CGTutorial::m_tutorialFlags.Load(data, bytes, -1);

    return 1;
}

// PROVEN: fc48c140 IDENTICAL
// OFFSET: 0x00401000
void CGTutorial::InitializeGame() {
}

// OFFSET: #0x00530100
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
    } while (index < 60);

    return index;
}

// OFFSET: #0x00530450
void CGTutorial::FlagTutorial(uint32_t tutorial) {
    uint32_t bit = 1 << (tutorial & 0x1F);
    uint32_t index = tutorial >> 5;

    if (CGTutorial::m_tutorialFlags.m_bits.m_data[index] & bit) {
        return;
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

// OFFSET: #0x00530510
void CGTutorial::ClearTutorials() {
    if (CGTutorial::m_tutorialShown.m_count) {
        memset(CGTutorial::m_tutorialShown.m_bits.m_data, 0xFF, sizeof(uint32_t) * CGTutorial::m_tutorialShown.m_bits.m_count);
    }

    if (CGTutorial::m_tutorialFlags.m_count) {
        memset(CGTutorial::m_tutorialFlags.m_bits.m_data, 0xFF, sizeof(uint32_t) * CGTutorial::m_tutorialFlags.m_bits.m_count);
    }

    for (int32_t i = 0; i < 60; i++) {
        if (CGTutorial::m_tutorialFlags.m_count && (CGTutorial::m_tutorialFlags.m_bits.m_data[i >> 5] & (1 << (i & 0x1F)))) {
            continue;
        }

        for (int32_t j = 0; j < 60; j++) {
            if (s_tutorialsCompleted[j] == i + 1) {
                break;
            }

            if (!s_tutorialsCompleted[j]) {
                s_tutorialsCompleted[j] = i + 1;
                break;
            }
        }
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_TUTORIAL_CLEAR));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: #0x00530630
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

// OFFSET: #0x00530840
void CGTutorial::TriggerTutorial(uint32_t tutorial) {
    if (!CGTutorial::m_tutorialShown.m_count) {
        return;
    }

    uint32_t bit = 1 << (tutorial & 0x1F);
    uint32_t index = tutorial >> 5;

    if (CGTutorial::m_tutorialShown.m_bits.m_data[index] & bit) {
        return;
    }

    if (CVar::Lookup("showTutorials")->m_intValue) {
        SI2::PlaySoundKit("TutorialPopup", NULL, NULL, NULL);
        FrameScript_SignalEvent(SCRIPT_EVENT_TUTORIAL_TRIGGER, "%d", tutorial + 1);
        CGTutorial::m_tutorialShown.m_bits.m_data[index] |= bit;
    } else {
        CGTutorial::m_tutorialShown.m_bits.m_data[index] |= bit;
        CGTutorial::FlagTutorial(tutorial);
    }
}

// OFFSET: #0x005303A0
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
        char text[256];
        text[0] = 'v';
        text[1] = 1;
        text[list.m_pos] = '\0';
        list.m_cvar->Set(text, true, false, false, true);
    }
}

// PROVEN: 01796b88 IDENTICAL
// OFFSET: 0x00401000
void CGTutorial::ShutdownGame() {
}

// OFFSET: #0x00530140
int32_t CGTutorial::GetNextCompletedTutorial(int32_t tutorial) {
    bool found = false;

    for (int32_t i = 0; i < 60; i++) {
        if (found && s_tutorialsCompleted[i]) {
            return s_tutorialsCompleted[i];
        }

        if (s_tutorialsCompleted[i] == tutorial + 1) {
            found = true;
        }
    }

    return 60;
}

// OFFSET: #0x00530190
int32_t CGTutorial::GetPrevCompletedTutorial(int32_t tutorial) {
    bool found = false;

    for (int32_t i = 60; i >= 0; i--) {
        if (found && s_tutorialsCompleted[i]) {
            return s_tutorialsCompleted[i];
        }

        if (s_tutorialsCompleted[i] == tutorial + 1) {
            found = true;
        }
    }

    return 0;
}

// OFFSET: #0x00530300
void CGTutorial::SaveFlaggedTutorials() {
    CVarIntListWrapper list;
    list.m_cvar = CVar::Lookup("flaggedTutorials");
    list.m_pos = 2;
    list.m_state = 0;

    for (uint32_t i = 0; i < 60; i++) {
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
