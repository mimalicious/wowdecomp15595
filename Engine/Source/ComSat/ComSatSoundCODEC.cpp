#include "Engine/Source/ComSat/ComSatSoundCODEC.h"
#include "Engine/Source/ComSat/ComSatAllocator.h"
#include "Engine/Source/ComSat/ComSatSoundLevelCalculator.h"
#include "Engine/Source/ComSat/ComSatSpirit.h"
#include <cstring>
#include <new>

// STATIC: 0x00C27680/0x140A1DA60
const uint8_t ComSat::Sound::CODECIPMR6000::s_invalidPlayer = 0xFF;

const int32_t ComSat::Sound::CODECIPMR6000::s_maxFramesPerPacket = 20;

// STATIC: 0x00C27688/0x140A1DA68
const int32_t ComSat::Sound::CODECIPMR6000::s_sampleRate = 8000;

// STATIC: 0x00C2768C
const int32_t ComSat::Sound::CODECIPMR6000::s_bitsPerSample = 16;

// STATIC: 0x00C27690
const int32_t ComSat::Sound::CODECIPMR6000::s_samplesPerFrame = 800;

// STATIC: 0x00C27694/0x140A1DA78
const int32_t ComSat::Sound::CODECIPMR6000::s_minEncodedFrameLength = 360;

// STATIC: 0x00C27698/0x140A1DA80
const int32_t ComSat::Sound::CODECIPMR6000::s_maxEncodedFrameLength = 960;

const uint32_t ComSat::Sound::CODECIPMR6000::s_bytesPerFrame = 1600;

// OFFSET: 0x00A3A000/0x1407E6510
ComSat::Sound::CODECIPMR6000::LocalPlayer::LocalPlayer(ComSat::Allocator& allocator, int32_t channelCount, uint8_t* tables) {
    this->m_threshold = 0.5f;
    this->m_tables = tables;
    this->m_allocator = &allocator;
    this->m_channelCount = channelCount;
    this->m_hangover = 0;
    this->m_decoders = static_cast<uint8_t**>(allocator.Alloc(sizeof(uint8_t*) * channelCount));
    if (this->m_decoders) {
        for (int32_t i = 0; i < channelCount; i++) {
            this->m_decoders[i] = static_cast<uint8_t*>(this->m_allocator->Alloc(sizeof(this->m_encoder)));
            if (this->m_decoders[i] && SPIRIT_IP_MR_V3_1_Init(this->m_decoders[i], this->m_tables)) {
                this->m_allocator->Free(this->m_decoders[i]);
                this->m_decoders[i] = NULL;
            }
        }
    }
    SPIRIT_IP_MR_V3_1_Init(this->m_encoder, this->m_tables);
}

// OFFSET: [0x00A3A0E0, 0x00A3A3C0]
ComSat::Sound::CODECIPMR6000::LocalPlayer::~LocalPlayer() {
    if (this->m_decoders) {
        for (int32_t i = 0; i < this->m_channelCount; i++) {
            this->m_allocator->Free(this->m_decoders[i]);
            this->m_decoders[i] = NULL;
        }
        this->m_allocator->Free(this->m_decoders);
        this->m_decoders = NULL;
    }
}

// OFFSET: 0x00A3A150/0x1407E66A0
bool ComSat::Sound::CODECIPMR6000::LocalPlayer::CompressRawData(const uint8_t* pcm, uint32_t size, uint8_t* data, uint32_t* bits, bool* voice) {
    uint8_t* encoder = this->m_encoder;
    if (!encoder || size != 1600 || *bits < 960) {
        return false;
    }
    if (this->m_hangover > 0) {
        this->m_hangover--;
    }
    float level;
    float decibels;
    ComSat::Sound::LevelCalculator16Bit::CalculateLevelForFrame(pcm, 1600, level, decibels);
    if (this->m_threshold <= decibels) {
        this->m_hangover = 20;
    }
    if (this->m_hangover <= 0) {
        *voice = false;
        return true;
    }
    SPIRIT_IPMR_STREAM stream;
    stream.m_data = data;
    stream.m_bits = 0;
    stream.m_size = *bits >> 3;
    SPIRIT_IPMR_INFO info;
    memset(&info, 0, sizeof(info));
    SPIRIT_IPMR_PARAMS params;
    params.unk_0000 = 0;
    params.unk_0004 = 0;
    params.unk_0006 = 1;
    int32_t result = 0;
    for (int32_t i = 0; i < 5; i++) {
        result = SPIRIT_IP_MR_V3_1_Encode(encoder, pcm, &params, &stream, &info);
        pcm += 320;
        if (result) {
            break;
        }
    }
    *bits = stream.m_bits;
    *voice = true;
    return result == 0;
}

// OFFSET: 0x00A3A270
bool ComSat::Sound::CODECIPMR6000::LocalPlayer::DecompressEncodedData(int32_t channel, const uint8_t* data, uint32_t bits, uint8_t* pcm, uint32_t* size) {
    if (bits > 960 || *size < 1600) {
        return false;
    }
    if (channel < 0 || channel >= this->m_channelCount || !this->m_decoders || !this->m_decoders[channel]) {
        return false;
    }
    uint32_t bytes = bits >> 3;
    if (bits & 7) {
        bytes++;
    }
    SPIRIT_IPMR_PARAMS params;
    params.unk_0000 = 0;
    params.unk_0004 = 0;
    SPIRIT_IPMR_STREAM stream;
    stream.m_bits = 0;
    stream.m_size = bytes;
    params.unk_0006 = 1;
    stream.m_data = data;
    int32_t result = 0;
    while (bits - stream.m_bits >= 72) {
        result = SPIRIT_IP_MR_V3_1_Decode(this->m_decoders[channel], pcm, &params, &stream, 0);
        pcm += 320;
        if (result) {
            break;
        }
    }
    *size = 1600;
    return result == 0;
}

// OFFSET: [0x00A3B6F0, 0x00A3A350]
ComSat::Sound::CODEC::~CODEC() {
}

// INLINED
ComSat::Sound::CODEC::CODEC(ComSat::Allocator& allocator) : m_allocator(&allocator) {
}

// PROVEN: c10502f8 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool ComSat::Sound::CODEC::Init() {
    return true;
}

// PROVEN: be321a10 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool ComSat::Sound::CODEC::Reset() {
    return true;
}

// PROVEN: c46dc152 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ComSat::Sound::CODEC::Shutdown() {
}

// PROVEN: e94cd971 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool ComSat::Sound::CODEC::CanBitpackFrames() {
    return true;
}

// PROVEN: 54a73a9e IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool ComSat::Sound::CODEC::IsEncodedBitrateVariable() {
    return true;
}

// PROVEN: 3d548fe8 IDENTICAL/IDENTICAL
// OFFSET: 0x00A39FF0/0x14090ED00
uint32_t ComSat::Sound::CODEC::MinimumSafeFramesBetweenResets() {
    return 32;
}

// PROVEN: a496d725 IDENTICAL/DIFFERENT
// OFFSET: 0x00A3A360/0x1407E6780
bool ComSat::Sound::CODECIPMR6000::Init() {
    if (this->m_initialized) {
        this->Shutdown();
    }
    if (SPIRIT_IP_MR_V3_1_GenerateTables(this->m_tables)) {
        return false;
    }
    this->m_initialized = true;
    return true;
}

// PROVEN: 3100d447 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3A390/0x1404C2CC0
bool ComSat::Sound::CODECIPMR6000::Reset() {
    return this->Init();
}

// PROVEN: 61551b6b IDENTICAL/DIFFERENT
// OFFSET: 0x00A3A3A0/0x1407E67A0
void ComSat::Sound::CODECIPMR6000::GenerateSilence(uint8_t* pcm, uint32_t size) {
    memset(pcm, 0, size);
}

// OFFSET: 0x00A3A3D0/0x1407E6960
ComSat::Sound::CODEC* ComSat::Sound::CODEC::Create(const ComSat::Initialization& init, ComSat::Allocator& allocator) {
    ComSat::Sound::CODEC* codec = new (allocator.Alloc(sizeof(ComSat::Sound::CODECIPMR6000))) ComSat::Sound::CODECIPMR6000(init, allocator);
    codec->Init();
    return codec;
}

// INLINED
ComSat::Sound::CODECIPMR6000::CODECIPMR6000(const ComSat::Initialization& init, ComSat::Allocator& allocator) : CODEC(allocator), m_players(allocator) {
    this->m_maxChannels = init.m_maxChannels;
    this->m_initialized = false;
}

// OFFSET: 0x00A3A440/0x1407E69F0
void ComSat::Sound::CODECIPMR6000::ResetPlayerChannelDecoder(uint8_t player, int32_t channel) {
    if (this->m_initialized) {
        LocalPlayer* localPlayer;
        if (this->m_players.Get(ComSat::MapKey<uint8_t>(player), localPlayer) && localPlayer->m_decoders && channel >= 0 && channel < localPlayer->m_channelCount && localPlayer->m_decoders[channel]) {
            SPIRIT_IP_MR_V3_1_Init(localPlayer->m_decoders[channel], localPlayer->m_tables);
        }
    }
}

// OFFSET: 0x00A3A4D0/0x1407E69F0
void ComSat::Sound::CODECIPMR6000::ResetPlayerChannelEncoder(uint8_t player) {
    if (this->m_initialized) {
        LocalPlayer* localPlayer;
        if (this->m_players.Get(ComSat::MapKey<uint8_t>(player), localPlayer)) {
            SPIRIT_IP_MR_V3_1_Init(localPlayer->m_encoder, localPlayer->m_tables);
        }
    }
}

// OFFSET: 0x00A3A540/0x1407E6A80
bool ComSat::Sound::CODECIPMR6000::CompressRawData(uint8_t player, const uint8_t* pcm, uint32_t size, uint8_t* data, uint32_t* bits, bool* voice) {
    LocalPlayer* localPlayer;
    return this->m_initialized && this->m_players.Get(ComSat::MapKey<uint8_t>(player), localPlayer) && localPlayer->CompressRawData(pcm, size, data, bits, voice);
}

// OFFSET: 0x00A3A5C0/0x1407E6B70
bool ComSat::Sound::CODECIPMR6000::DecompressEncodedData(uint8_t player, int32_t channel, const uint8_t* data, uint32_t bits, uint8_t* pcm, uint32_t* size) {
    LocalPlayer* localPlayer;
    return this->m_initialized && this->m_players.Get(ComSat::MapKey<uint8_t>(player), localPlayer) && localPlayer->DecompressEncodedData(channel, data, bits, pcm, size);
}

// OFFSET: 0x00A3A640/0x1407E6CA0
void ComSat::Sound::CODECIPMR6000::SetThreshold(uint8_t player, float threshold) {
    LocalPlayer* localPlayer;
    if (this->m_players.Get(ComSat::MapKey<uint8_t>(player), localPlayer)) {
        localPlayer->m_threshold = ComSat::Sound::LevelCalculator16Bit::GetMaxDecibelLevel() * threshold;
    }
}

// OFFSET: 0x00A3A780/0x1407E7030
void ComSat::Sound::CODECIPMR6000::Shutdown() {
    LocalPlayer* localPlayer = NULL;
    ComSat::MapKey<uint8_t> key(s_invalidPlayer);
    uint32_t iterator = 0;
    while (this->m_players.Next(iterator, key, localPlayer)) {
        LocalPlayer* removed;
        this->m_players.Remove(key, removed);
        ComSat::Delete(*this->m_allocator, localPlayer);
    }
    this->m_initialized = false;
}

// OFFSET: 0x00A3A830/0x1407E7100
bool ComSat::Sound::CODECIPMR6000::InitLocalPlayer(uint8_t player, float threshold) {
    if (!this->m_initialized) {
        return false;
    }
    uint32_t index;
    if (this->m_players.Find(ComSat::MapKey<uint8_t>(player), index)) {
        return false;
    }
    LocalPlayer* localPlayer = new (this->m_allocator->Alloc(sizeof(LocalPlayer))) LocalPlayer(*this->m_allocator, this->m_maxChannels, this->m_tables);
    localPlayer->m_threshold = ComSat::Sound::LevelCalculator16Bit::GetMaxDecibelLevel() * threshold;
    this->m_players.Insert(ComSat::MapKey<uint8_t>(player), localPlayer);
    return true;
}

// PROVEN: b2f9b90d UNPROVEN/IDENTICAL
// OFFSET: 0x00A3A910/0x1407E7250
bool ComSat::Sound::CODECIPMR6000::RemoveLocalPlayer(uint8_t player) {
    if (!this->m_initialized) {
        return false;
    }
    LocalPlayer* localPlayer = NULL;
    if (this->m_players.Remove(ComSat::MapKey<uint8_t>(player), localPlayer)) {
        ComSat::Delete(*this->m_allocator, localPlayer);
    }
    return true;
}

// OFFSET: 0x00A3A990
ComSat::Sound::CODECIPMR6000::~CODECIPMR6000() {
    this->Shutdown();
}

// PROVEN: 9adda6e6 IDENTICAL/DIFFERENT
// OFFSET: 0x00627990/0x140693B10
void ComSat::Sound::CODECIPMR6000::ApplyVoiceEffects(uint8_t player, uint8_t* pcm, uint32_t* size) {
}
