#ifndef WOW_SOURCE_SOUNDINTERFACE_SI3DEATHKNIGHTDSP_H
#define WOW_SOURCE_SOUNDINTERFACE_SI3DEATHKNIGHTDSP_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/SoundEngine/fmodex/src/fmod.hpp"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/SoundInterface/Types.h"

class SI3DspNode : public TSLinkedNode<SI3DspNode> {
    public:
    /* 0008 */ FMOD::DSP* m_dsp;
    /* 000C */ int32_t m_type;

    SI3DspNode();
};

class SI3DspChainHead {
    public:
    /* 0000 */ TSList<SI3DspNode, TSGetLink<SI3DspNode> > m_nodes;
    /* 000C */ FMOD::ChannelGroup* m_channelGroup;
    /* 0010 */ float m_expireTime;

    SI3DspChainHead();
    void DestroyChain();
};

struct DSP_CHAIN_HEAD_LOOKUP : public TSHashObject<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI> {
    /* 0018 */ SI3DspChainHead m_chain;
};

#endif
