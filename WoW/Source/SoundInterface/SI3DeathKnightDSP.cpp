#include <cstddef>
#include <cstdint>
#include <new>
#include "WoW/Source/SoundInterface/SI3DeathKnightDSP.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/SoundInterface/SI3.h"

// STATIC: 0x00D1F1F8
static const char* s_raceFileNames[] = { NULL, "Human", "Orc", "Dwarf", "NightElf", "Scourge", "Tauren", "Gnome", "Troll", "Goblin", "BloodElf", "Draenei", "FelOrc", "Naga_", "Broken", "Skeleton", "Vrykul", "Tuskarr", "ForestTroll", "Taunka", "NorthrendSkeleton", "IceTroll", "Worgen", "Gilnean" };

// STATIC: 0x00E2882C
static TSHashTable<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI> s_dspChains;

// OFFSET: 0x00632760
static FMOD_DSP_TYPE TranslateDspType(int32_t type) {
    switch (type) {
        case SI3_DSP_TYPE_PITCHSHIFT:
            return FMOD_DSP_TYPE_PITCHSHIFT;

        case SI3_DSP_TYPE_HIGHPASS:
            return FMOD_DSP_TYPE_HIGHPASS;

        case SI3_DSP_TYPE_LOWPASS:
            return FMOD_DSP_TYPE_LOWPASS;

        case SI3_DSP_TYPE_CHORUS:
            return FMOD_DSP_TYPE_CHORUS;

        case SI3_DSP_TYPE_ECHO:
            return FMOD_DSP_TYPE_ECHO;

        default:
            return FMOD_DSP_TYPE_UNKNOWN;
    }
}

// INLINED
SI3DspNode::SI3DspNode() {
    this->m_dsp = NULL;
    this->m_type = -1;
}

// INLINED
SI3DspChainHead::SI3DspChainHead() {
    this->m_expireTime = 0.1f;
}

// OFFSET: 0x006327B0/0x1402C7F90
void SI3::DK_GetPlayerFilterName(void* unit, char* name, int32_t size) {
    CGUnit_C* object = static_cast<CGUnit_C*>(unit);
    SStrPrintf(name, size, "Death Knight %s %s", s_raceFileNames[object->m_unit->m_unitFieldBytes0[0]], object->m_unit->m_unitFieldBytes0[2] ? "Female" : "Male");
}

// OFFSET: 0x006328F0/0x1402C7FE0
void SI3::DSP_AttachCustomDspChain(void* channel, char* name) {
    DSP_CHAIN_HEAD_LOOKUP* head = s_dspChains.Ptr(name);

    if (!head) {
        return;
    }

    SI3DspNode* next;

    for (SI3DspNode* node = head->m_chain.m_nodes.Head(); node; node = next) {
        next = head->m_chain.m_nodes.Next(node);
        SE3::EnableDSP(node->m_dsp, true);
    }

    head->m_chain.m_expireTime = 3.0f;
    SE3::AddChannelToChannelGroup(channel, head->m_chain.m_channelGroup);
}

// OFFSET: 0x00632960
void SI3::DSP_MakeDSPsExpire(float elapsed) {
    for (DSP_CHAIN_HEAD_LOOKUP* head = s_dspChains.m_fulllist.Head(); head; head = s_dspChains.m_fulllist.Next(head)) {
        if (SE3::GetChannelGroupNumChannels(head->m_chain.m_channelGroup)) {
            continue;
        }

        if (head->m_chain.m_expireTime > 0.0f && head->m_chain.m_expireTime - elapsed <= 0.0f) {
            SI3DspNode* next;

            for (SI3DspNode* node = head->m_chain.m_nodes.Head(); node; node = next) {
                next = head->m_chain.m_nodes.Next(node);
                SE3::EnableDSP(node->m_dsp, false);
            }
        }

        head->m_chain.m_expireTime -= elapsed;

        if (head->m_chain.m_expireTime < 0.0f) {
            head->m_chain.m_expireTime = 0.0f;
        }
    }
}

// OFFSET: 0x00632BF0
void SI3DspChainHead::DestroyChain() {
    SI3DspNode* next;

    for (SI3DspNode* node = this->m_nodes.Head(); node; node = next) {
        next = this->m_nodes.Next(node);
        SE3::DspDestroy(node->m_dsp);
        node->m_dsp = NULL;
    }

    this->m_nodes.Clear();
}

// OFFSET: 0x00632C40/0x1402C83B0
void SI3::DSP_CreateDsp(const char* name, int32_t type, const float* params) {
    if (!SI3::sm_pCVar_Sound_EnableDSPEffects || !SI3::sm_pCVar_Sound_EnableDSPEffects->m_intValue) {
        return;
    }

    SI3DspNode* node = new (SMemAlloc(sizeof(SI3DspNode), __FILE__, __LINE__, 0)) SI3DspNode();

    if (type != SI3_DSP_TYPE_VOLUME && SE3::CreateDSPByType(TranslateDspType(type), reinterpret_cast<void**>(&node->m_dsp))) {
        if (node) {
            delete node;
        }

        return;
    }

    DSP_CHAIN_HEAD_LOOKUP* head = s_dspChains.Ptr(name);
    head->m_chain.m_nodes.LinkToTail(node);
    node->m_type = type;

    switch (type) {
        case SI3_DSP_TYPE_PITCHSHIFT:
            SE3::SetDspParam(node->m_dsp, 0, params[1]);
            SE3::SetDspParam(node->m_dsp, 1, 512.0f);
            break;

        case SI3_DSP_TYPE_HIGHPASS:
        case SI3_DSP_TYPE_LOWPASS:
            SE3::SetDspParam(node->m_dsp, 0, params[1]);
            SE3::SetDspParam(node->m_dsp, 1, params[2]);
            break;

        case SI3_DSP_TYPE_CHORUS:
            SE3::SetDspParam(node->m_dsp, 0, params[1]);
            SE3::SetDspParam(node->m_dsp, 1, params[2]);
            SE3::SetDspParam(node->m_dsp, 2, params[3]);
            SE3::SetDspParam(node->m_dsp, 3, params[4]);
            SE3::SetDspParam(node->m_dsp, 4, params[5]);
            SE3::SetDspParam(node->m_dsp, 5, params[6]);
            SE3::SetDspParam(node->m_dsp, 6, params[7]);
            SE3::SetDspParam(node->m_dsp, 7, params[8]);
            break;

        case SI3_DSP_TYPE_ECHO:
            SE3::SetDspParam(node->m_dsp, 0, params[1]);
            SE3::SetDspParam(node->m_dsp, 1, params[2]);
            SE3::SetDspParam(node->m_dsp, 3, params[3]);
            SE3::SetDspParam(node->m_dsp, 4, params[4]);
            break;

        case SI3_DSP_TYPE_VOLUME:
            if (params[0] > 0.0f) {
                SE3::SetFMODChannelGroupVolume(head->m_chain.m_channelGroup, params[1]);
            }

            break;

        default:
            break;
    }

    if (type != SI3_DSP_TYPE_VOLUME) {
        SE3::BypassDsp(node->m_dsp, 0.0f == params[0]);
        SE3::ConnectDspToChannelGroup(node->m_dsp, head->m_chain.m_channelGroup);
    }
}

// OFFSET: 0x00632F10
void SI3::DSP_ShutDown() {
    for (DSP_CHAIN_HEAD_LOOKUP* head = s_dspChains.m_fulllist.Head(); head; head = s_dspChains.m_fulllist.Next(head)) {
        SI3DspNode* next;

        for (SI3DspNode* node = head->m_chain.m_nodes.Head(); node; node = next) {
            next = head->m_chain.m_nodes.Next(node);
            SE3::DspDestroy(node->m_dsp);
            node->m_dsp = NULL;
        }

        head->m_chain.m_nodes.Clear();

        if (head->m_chain.m_channelGroup) {
            SE3::DestroyChannelGroup(head->m_chain.m_channelGroup);
            head->m_chain.m_channelGroup = NULL;
        }
    }

    s_dspChains.Clear();
}

// OFFSET: 0x006336B0/0x1402C90B0
void SI3::DSP_Init() {
    for (int32_t i = 0; i < g_soundFilterDB.m_numRecords; i++) {
        SoundFilterRec* rec = g_soundFilterDB.GetRecordByIndex(i);

        if (s_dspChains.Ptr(rec->m_name)) {
            continue;
        }

        const char* name = rec->m_name;
        DSP_CHAIN_HEAD_LOOKUP* head = s_dspChains.New(name, 0, 0);
        head->m_chain.DestroyChain();

        if (!head->m_chain.m_channelGroup) {
            head->m_chain.m_channelGroup = static_cast<FMOD::ChannelGroup*>(SE3::CreateChannelGroup(rec->m_name));
        }
    }

    for (int32_t i = 0; i < g_soundFilterElemDB.m_numRecords; i++) {
        SoundFilterElemRec* elem = g_soundFilterElemDB.GetRecordByIndex(i);
        SoundFilterRec* filter = g_soundFilterDB.GetRecord(elem->m_soundFilterID);

        SI3::DSP_CreateDsp(filter->m_name, elem->m_filterType, elem->m_params);
    }

    for (DSP_CHAIN_HEAD_LOOKUP* head = s_dspChains.m_fulllist.Head(); head; head = s_dspChains.m_fulllist.Next(head)) {
        SI3DspNode* next;

        for (SI3DspNode* node = head->m_chain.m_nodes.Head(); node; node = next) {
            next = head->m_chain.m_nodes.Next(node);
            SE3::EnableDSP(node->m_dsp, false);
        }
    }
}
