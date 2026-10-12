#include "Engine/Source/ComSat/ComSatManager.h"
#include "Engine/Source/ComSat/ComSatAllocator.h"
#include "Engine/Source/ComSat/ComSatBitPacker.h"
#include <cstring>
#include <new>

// OFFSET: 0x00A32ED0
ComSat::Interface::~Interface() {
}

// INLINED
ComSat::Manager::LocalTalker::LocalTalker(ComSat::Allocator& allocator) : m_guid(0), m_session(NULL), m_sessions(allocator) {
}

// PROVEN: e137e543 IDENTICAL/DIFFERENT
// OFFSET: 0x00A32F30/0x1407DD430
void ComSat::Manager::Work(uint32_t elapsed) {
    this->m_networkManager->Work(elapsed);
    this->m_soundManager->Work(elapsed);
}

// PROVEN: f7caa0a8 IDENTICAL/DIFFERENT
// OFFSET: 0x00A32F70/0x1407DF030
void ComSat::Manager::CallbackClear() {
    this->m_callback = NULL;
}

// PROVEN: e47eb668 IDENTICAL/DIFFERENT
// OFFSET: 0x00A32F60/0x14090EB30
void ComSat::Manager::CallbackSet(ComSat::Interface::Callback& callback) {
    this->m_callback = &callback;
}

// PROVEN: 042fb9e6 IDENTICAL/DIFFERENT
// OFFSET: 0x00A32F80/0x1407DD530
bool ComSat::Manager::LocalTalkerChangeOutputDevice(uint8_t localTalker, int32_t device) {
    return this->m_soundManager->LocalTalkerChangeOutputDevice(localTalker, device);
}

// PROVEN: d7455a58 IDENTICAL/DIFFERENT
// OFFSET: 0x00A32F90/0x1407DD480
bool ComSat::Manager::LocalTalkerChangeCaptureDevice(uint8_t localTalker, int32_t device) {
    return this->m_soundManager->LocalTalkerChangeCaptureDevice(localTalker, device);
}

// PROVEN: b07f4f6b IDENTICAL/DIFFERENT
// OFFSET: 0x00A32FA0/0x1407DD5C0
int32_t ComSat::Manager::LocalTalkerGetCurrentOutputDeviceIndex(uint8_t localTalker) {
    return this->m_soundManager->LocalTalkerGetCurrentOutputDeviceIndex(localTalker);
}

// PROVEN: fbfcb002 IDENTICAL/DIFFERENT
// OFFSET: 0x00A32FB0/0x1407DD4A0
int32_t ComSat::Manager::LocalTalkerGetCurrentCaptureDeviceIndex(uint8_t localTalker) {
    return this->m_soundManager->LocalTalkerGetCurrentCaptureDeviceIndex(localTalker);
}

// PROVEN: 90f7fd0d IDENTICAL/DIFFERENT
// OFFSET: 0x00A32FC0/0x1407DD4B0
bool ComSat::Manager::LocalTalkerEnable(uint8_t localTalker, bool enable) {
    return this->m_soundManager->LocalTalkerEnable(localTalker, enable);
}

// PROVEN: c5e2a601 IDENTICAL/DIFFERENT
// OFFSET: 0x00A32FD0/0x1407DD470
bool ComSat::Manager::LocalTalkerThreshold(uint8_t localTalker, float threshold) {
    if (threshold < 0.0f || threshold > 1.0f) {
        return false;
    }
    return this->m_soundManager->LocalTalkerThreshold(localTalker, threshold);
}

// PROVEN: d83eb312 IDENTICAL/DIFFERENT
// OFFSET: 0x00A33010/0x1407DD480
bool ComSat::Manager::LocalTalkerEnableVoiceLevelCallback(uint8_t localTalker, bool enable) {
    if (localTalker >= this->m_maxLocalPlayers) {
        return false;
    }
    this->m_soundManager->LocalTalkerEnableVoiceLevelCallback(localTalker, enable);
    return true;
}

// PROVEN: 77892de6 IDENTICAL/DIFFERENT
// OFFSET: 0x00A33040/0x1407DD4B0
bool ComSat::Manager::LocalTalkerSetRemoteTalkerVolume(uint8_t localTalker, uint64_t talker, float volume) {
    return this->m_soundManager->LocalTalkerSetRemoteTalkerVolume(localTalker, talker, volume);
}

// PROVEN: 6f63545b IDENTICAL/DIFFERENT
// OFFSET: 0x00A33070/0x1407DD4C0
bool ComSat::Manager::LocalTalkerSetSessionVolume(uint8_t localTalker, uint64_t session, float volume) {
    return this->m_soundManager->LocalTalkerSetSessionVolume(localTalker, session, volume);
}

// PROVEN: 4096c0df IDENTICAL/DIFFERENT
// OFFSET: 0x00A330A0/0x1407DD530
int32_t ComSat::Manager::SessionNumMembers(uint64_t session) {
    return this->m_soundManager->SessionNumMembers(session);
}

// PROVEN: 856e4889 IDENTICAL/DIFFERENT
// OFFSET: 0x00A330C0/0x1407DD540
bool ComSat::Manager::SessionGetTalkerByIndex(uint64_t session, int32_t index, uint64_t& talker, uint8_t& localTalker) {
    if (!this->m_soundManager->SessionGetTalkerByIndex(session, index, talker, localTalker)) {
        return false;
    }
    if (localTalker != 0xFF) {
        if (localTalker >= this->m_maxLocalPlayers) {
            return false;
        }
        uint64_t guid = this->m_localTalkers[localTalker]->m_guid;
        if (!guid) {
            return false;
        }
        talker = guid;
    }
    return true;
}

// PROVEN: e4c3758e IDENTICAL/DIFFERENT
// OFFSET: 0x00A33120/0x1407DD5C0
bool ComSat::Manager::SessionSetTalkerPriority(uint64_t session, uint64_t talker, uint8_t priority) {
    return this->m_soundManager->SetPlaybackPriorityForPlayerInSession(session, talker, priority);
}

// PROVEN: de3e26e4 IDENTICAL/DIFFERENT
// OFFSET: 0x00A33150/0x1407DD4A0
bool ComSat::Manager::LocalTalkerSetSessionPriority(uint8_t localTalker, uint64_t session, uint8_t priority) {
    return this->m_soundManager->SetPlaybackPriorityForSession(session, localTalker, priority);
}

// OFFSET: 0x00A33170/0x1407DD5F0
bool ComSat::Manager::IsTalkerMuted(uint8_t localTalker, uint64_t talker) {
    if (localTalker < this->m_maxLocalPlayers) {
        return this->m_soundManager->IsRemoteTalkerMuted(localTalker, talker);
    }
    return false;
}

// PROVEN: 73f42a30 IDENTICAL/DIFFERENT
// OFFSET: 0x00A331A0/0x1407DD4E0
bool ComSat::Manager::MuteTalker(uint8_t localTalker, uint64_t talker, ComSat::MuteSetting setting) {
    return this->m_soundManager->SetTalkerMuted(localTalker, talker, setting);
}

// OFFSET: 0x00A331C0/0x1407DD620
bool ComSat::Manager::MuteSession(uint8_t localTalker, uint64_t session, ComSat::MuteSetting setting) {
    if (localTalker < this->m_maxLocalPlayers && this->m_localTalkers[localTalker]->m_guid) {
        return this->m_soundManager->SetSessionMuted(localTalker, session, setting);
    }
    return false;
}

// PROVEN: 723c4e63 IDENTICAL/DIFFERENT
// OFFSET: 0x00A33200/0x1407DD510
bool ComSat::Manager::IsSessionMuted(uint8_t localTalker, uint64_t session) {
    return this->m_soundManager->IsSessionMuted(localTalker, session);
}

// PROVEN: cc772179 IDENTICAL/NONE
// OFFSET: 0x00A33220
uint8_t ComSat::Manager::GetLocalTalkerIndex(uint64_t talker) {
    for (uint32_t i = 0; i < this->m_maxLocalPlayers; i++) {
        if (this->m_localTalkers[i]->m_guid == talker) {
            return i;
        }
    }
    return 0xFF;
}

// OFFSET: 0x00A33260/0x1407DD660
bool ComSat::Manager::IsTalkerTalkingInSession(uint64_t session, uint8_t localTalker, uint64_t talker) {
    if (this->GetLocalTalkerIndex(talker) == 0xFF) {
        return this->m_soundManager->IsRemoteTalkerTalkingInSession(session, localTalker, talker);
    }
    return this->m_soundManager->IsLocalPlayerTalking(localTalker);
}

// PROVEN: 6b64af7e IDENTICAL/NONE
// OFFSET: 0x00A332B0
bool ComSat::Manager::DoesTalkerHaveHeadset(uint64_t talker) {
    return true;
}

// OFFSET: [0x00A332C0]
bool ComSat::Manager::DoesLocalPlayerHaveHeadset(uint8_t localTalker) {
    return true;
}

// PROVEN: 68a20b79 IDENTICAL/DIFFERENT
// OFFSET: 0x00A332D0/0x1407DD6C0
int32_t ComSat::Manager::EnumerateOutputDevices(ComSat::DeviceInfo* devices, int32_t maxDevices) {
    return this->m_soundManager->EnumerateOutputDevices(devices, maxDevices);
}

// PROVEN: d416adcb IDENTICAL/DIFFERENT
// OFFSET: 0x00A332E0/0x1407DD6D0
int32_t ComSat::Manager::EnumerateCaptureDevices(ComSat::DeviceInfo* devices, int32_t maxDevices) {
    return this->m_soundManager->EnumerateCaptureDevices(devices, maxDevices);
}

// OFFSET: 0x00A332F0/0x1407DD6E0
bool ComSat::Manager::UnpackStableBitrateData(uint64_t session, uint64_t talker, const uint8_t* data, uint32_t size) {
    bool reliable = false;
    uint32_t offset = 2;
    do {
        uint8_t length = data[offset];
        if (!length) {
            reliable = true;
        }
        offset += length + 1;
    } while (offset < size);
    ComSat::Sound::Manager::VoicePacket packet;
    packet.m_sequence = *reinterpret_cast<const uint16_t*>(data);
    packet.m_bits = this->m_soundManager->GetMaxEncodedFrameLength();
    uint32_t bytes = static_cast<uint32_t>(this->m_soundManager->GetMaxEncodedFrameLength()) >> 3;
    int32_t frames = 0;
    uint32_t position = 2;
    if (size > 2) {
        do {
            if (size - position < bytes) {
                break;
            }
            memcpy(packet.m_data, data + position, bytes);
            this->m_soundManager->SubmitIncomingVoicePacket(session, talker, reinterpret_cast<uint8_t*>(&packet), bytes + 4, reliable);
            packet.m_sequence++;
            frames++;
            position += bytes;
        } while (position < size);
        if (frames > 0) {
            this->m_soundManager->SubmitIncomingFrameTimeData(talker, packet.m_sequence);
        }
    }
    return true;
}

// OFFSET: 0x00A333F0/0x1407DD830
bool ComSat::Manager::UnpackVariableBitrateData(uint64_t session, uint64_t talker, const uint8_t* data, uint32_t size) {
    uint16_t sequence = data[0] | (data[1] << 8);
    uint32_t position = 3;
    uint8_t header = data[2];
    bool reliable = header >> 7;
    uint8_t count = header & 0x7F;
    if (count > 20) {
        count = 20;
    }
    ComSat::Sound::Manager::VoicePacket packet;
    uint32_t lengths[20];
    for (int32_t i = 0; i < count; i++) {
        lengths[i] = data[position];
        lengths[i] += this->m_soundManager->GetMinEncodedFrameLength();
        position++;
    }
    ComSat::BitUnpacker unpacker(&data[position], 8 * (size - position));
    for (int32_t i = 0; i < count; i++) {
        uint32_t bits = lengths[i];
        packet.m_sequence = sequence;
        packet.m_bits = bits;
        if (bits && unpacker.UnpackBits(packet.m_data, bits)) {
            uint32_t length = (bits >> 3) + 4;
            if (bits & 7) {
                length = (bits >> 3) + 5;
            }
            this->m_soundManager->SubmitIncomingVoicePacket(session, talker, reinterpret_cast<uint8_t*>(&packet), length, reliable);
        }
        sequence++;
    }
    if (count) {
        this->m_soundManager->SubmitIncomingFrameTimeData(talker, sequence - 1);
    }
    return true;
}

// OFFSET: 0x00A33520/0x1407DDA00
bool ComSat::Manager::UnpackBitpackedVariableBitrateData(uint64_t session, uint64_t talker, const uint8_t* data, uint32_t size) {
    ComSat::Sound::Manager::VoicePacket packet;
    packet.m_sequence = *reinterpret_cast<const uint16_t*>(data);
    ComSat::BitUnpacker unpacker(data + 2, 8 * size - 16);
    packet.m_bits = 8 * size - 16;
    if (!unpacker.UnpackBits(packet.m_data, packet.m_bits)) {
        return false;
    }
    this->m_soundManager->SubmitIncomingVoicePacket(session, talker, reinterpret_cast<uint8_t*>(&packet), 132, false);
    this->m_soundManager->SubmitIncomingFrameTimeData(talker, packet.m_sequence);
    return true;
}

// OFFSET: 0x00A335E0/0x1407DDAF0
void ComSat::Manager::IncomingVoiceData(uint64_t session, uint64_t talker, const uint8_t* data, uint32_t size) {
    if (this->m_soundManager->IsUsingVariableBitrateCodec()) {
        if (this->m_soundManager->CanBitpackEncodedData()) {
            this->UnpackBitpackedVariableBitrateData(session, talker, data, size);
        } else {
            this->UnpackVariableBitrateData(session, talker, data, size);
        }
    } else {
        this->UnpackStableBitrateData(session, talker, data, size);
    }
}

// OFFSET: 0x00A33660/0x1407DDB80
bool ComSat::Manager::PackBitpackedVariableBitrateData(uint8_t player, uint8_t* buffer, int32_t& size) {
    size = 0;
    if (player >= this->m_maxLocalPlayers) {
        return false;
    }
    LocalTalker* localTalker = this->m_localTalkers[player];
    if (!localTalker || !localTalker->m_frameCount) {
        return false;
    }
    *reinterpret_cast<uint16_t*>(buffer) = localTalker->m_frames[0].m_sequence;
    size += 2;
    ComSat::BitPacker packer(buffer + size, 2560 - size);
    for (int32_t i = 0; i < static_cast<int32_t>(localTalker->m_frameCount); i++) {
        packer.PackBits(localTalker->m_frames[i].m_data, localTalker->m_frames[i].m_bits);
    }
    size += packer.Finalize();
    return true;
}

// OFFSET: 0x00A33710/0x1407DDC70
bool ComSat::Manager::PackVariableBitrateData(uint8_t player, uint8_t* buffer, int32_t& size) {
    size = 0;
    if (player >= this->m_maxLocalPlayers) {
        return false;
    }
    LocalTalker* localTalker = this->m_localTalkers[player];
    if (!localTalker || !localTalker->m_frameCount) {
        return false;
    }
    int32_t minimum = this->m_soundManager->GetMinEncodedFrameLength();
    *reinterpret_cast<uint16_t*>(buffer + size) = localTalker->m_frames[0].m_sequence;
    size += 2;
    buffer[size] = localTalker->m_frameCount;
    if (localTalker->m_time < 100) {
        buffer[size] |= 0x80;
    }
    size++;
    for (int32_t i = 0; i < static_cast<int32_t>(localTalker->m_frameCount); i++) {
        int32_t length = localTalker->m_frames[i].m_bits - minimum;
        if (length < 0) {
            length = 0;
        } else if (length > 255) {
            length = 255;
        }
        buffer[size] = length;
        size++;
    }
    ComSat::BitPacker packer(buffer + size, 2560 - size);
    for (int32_t i = 0; i < static_cast<int32_t>(localTalker->m_frameCount); i++) {
        packer.PackBits(localTalker->m_frames[i].m_data, localTalker->m_frames[i].m_bits);
    }
    size += packer.Finalize();
    return true;
}

// OFFSET: 0x00A33850/0x1407DDDD0
bool ComSat::Manager::PackStableBitrateData(uint8_t player, uint8_t* buffer, int32_t& size) {
    size = 0;
    if (player >= this->m_maxLocalPlayers) {
        return false;
    }
    LocalTalker* localTalker = this->m_localTalkers[player];
    if (!localTalker || !localTalker->m_frameCount) {
        return false;
    }
    *reinterpret_cast<uint16_t*>(buffer) = localTalker->m_frames[0].m_sequence;
    size += 2;
    uint32_t bytes = static_cast<uint32_t>(this->m_soundManager->GetMaxEncodedFrameLength()) >> 3;
    if (this->m_soundManager->GetMaxEncodedFrameLength() & 7) {
        bytes++;
    }
    for (uint32_t i = 0; i < localTalker->m_frameCount; i++) {
        memcpy(buffer + size, localTalker->m_frames[i].m_data, bytes);
        size += bytes;
    }
    if (localTalker->m_time < 100) {
        buffer[size++] = 0;
    }
    return true;
}

// PROVEN: 005e1cb8 IDENTICAL/DIFFERENT
// OFFSET: 0x00A33920/0x1407DDEC0
void ComSat::Manager::SendVoiceDataToCurrentSession(uint8_t player) {
    if (player >= this->m_maxLocalPlayers) {
        return;
    }
    LocalTalker* localTalker = this->m_localTalkers[player];
    if (!localTalker || !localTalker->m_frameCount) {
        return;
    }
    if (localTalker->m_session) {
        uint8_t buffer[2560];
        int32_t size = 0;
        bool packed;
        if (this->m_soundManager->IsUsingVariableBitrateCodec()) {
            if (this->m_soundManager->CanBitpackEncodedData()) {
                packed = this->PackBitpackedVariableBitrateData(player, buffer, size);
            } else {
                packed = this->PackVariableBitrateData(player, buffer, size);
            }
        } else {
            packed = this->PackStableBitrateData(player, buffer, size);
        }
        if (!packed) {
            return;
        }
        localTalker->m_session->Send(localTalker->m_guid, buffer, size);
    }
    localTalker->m_frameCount = 0;
    localTalker->m_time = 0;
}

// OFFSET: 0x00A339F0/0x1407DDFC0
void ComSat::Manager::LocalChatDataReady(uint8_t player, uint8_t* data, uint32_t size, uint32_t time) {
    if (player >= this->m_maxLocalPlayers) {
        return;
    }
    LocalTalker* localTalker = this->m_localTalkers[player];
    if (!localTalker->m_guid) {
        return;
    }
    if (!size) {
        if (localTalker->m_talking) {
            localTalker->m_talking = false;
            if (this->m_callback) {
                this->m_callback->NotifyLocalTalkerStop(player);
            }
        }
        this->SendVoiceDataToCurrentSession(player);
        return;
    }
    if (!localTalker->m_talking) {
        localTalker->m_talking = true;
        if (this->m_callback) {
            this->m_callback->NotifyLocalTalkerStart(player);
        }
    }
    if (size <= 128) {
        LocalTalker* target = this->m_localTalkers[player];
        target->m_frames[target->m_frameCount].m_size = size;
        memcpy(&target->m_frames[target->m_frameCount], data, size);
        target->m_frameCount++;
        target->m_time += time;
        if (target->m_time >= 100) {
            this->SendVoiceDataToCurrentSession(player);
        }
    }
}

// PROVEN: e306a7c8 IDENTICAL/DIFFERENT
// OFFSET: 0x00A33AF0/0x1407DE0F0
void ComSat::Manager::RemotePlayerTalkNotifyStart(uint8_t player, uint64_t talker, uint64_t session) {
    if (this->m_callback) {
        this->m_callback->NotifyTalkerStart(player, session, talker);
    }
}

// PROVEN: 0cc4dbcc IDENTICAL/DIFFERENT
// OFFSET: 0x00A33B20/0x1407DE120
void ComSat::Manager::RemotePlayerTalkNotifyStop(uint8_t player, uint64_t talker, uint64_t session) {
    if (this->m_callback) {
        this->m_callback->NotifyTalkerStop(player, session, talker);
    }
}

// PROVEN: 3b2928bc IDENTICAL/DIFFERENT
// OFFSET: 0x00A33B50/0x1407DE150
void ComSat::Manager::LocalTalkerVoiceLevel(uint8_t player, float level, bool talking) {
    if (this->m_callback) {
        this->m_callback->NotifyLocalTalkerVoiceLevel(player, level, talking);
    }
}

// PROVEN: ea19a555 IDENTICAL/DIFFERENT
// OFFSET: 0x00A33B80/0x1407DE170
void ComSat::Manager::DeviceError(uint8_t player) {
    if (this->m_callback && player < this->m_maxLocalPlayers) {
        LocalTalker* localTalker = this->m_localTalkers[player];
        if (localTalker->m_guid && localTalker->m_deviceError != true) {
            localTalker->m_deviceError = true;
            this->m_callback->NotifyDeviceError(player);
        }
    }
}

// PROVEN: de61f2c6 IDENTICAL/DIFFERENT
// OFFSET: 0x00841AE0/0x140211B90
void ComSat::Manager::MicrophoneStatusUpdate(uint8_t player, ComSat::Sound::HeadsetDeviceStatus status) {
}

// PROVEN: 55e2d48f IDENTICAL/DIFFERENT
// OFFSET: 0x00A33BD0/0x1407DD620
void ComSat::Manager::DebugSetSequenceDelay(int32_t delay) {
    this->m_soundManager->DebugSetSequenceDelay(delay);
}

// PROVEN: 314d120d IDENTICAL/DIFFERENT
// OFFSET: 0x00A33BE0/0x1407DD650
void ComSat::Manager::DebugSetSoundOutputDelay(uint8_t localTalker, int32_t delay) {
    this->m_soundManager->DebugSetSoundOutputDelay(localTalker, delay);
}

// OFFSET: 0x00A33CB0/0x1407DE260
bool ComSat::Manager::LocalTalkerCreate(uint8_t localTalker, uint64_t talker, float threshold, int32_t outputDevice, int32_t captureDevice) {
    LocalTalker* local = this->m_localTalkers[localTalker];
    if (!local) {
        return false;
    }
    local->m_guid = talker;
    local->m_session = NULL;
    local->m_frameCount = 0;
    local->m_time = 0;
    local->m_deviceError = false;
    local->m_sessions.m_used = 0;
    local->m_talking = false;
    return this->m_soundManager->InitLocalPlayer(localTalker, threshold, outputDevice, captureDevice);
}

// PROVEN: 2f3c8b0c IDENTICAL/DIFFERENT
// OFFSET: 0x00A33D20/0x1407DE2D0
bool ComSat::Manager::LocalTalkerDestroy(uint8_t localTalker) {
    LocalTalker* local = this->m_localTalkers[localTalker];
    if (!local || !local->m_guid) {
        return true;
    }
    uint32_t count = local->m_sessions.Count();
    for (uint32_t i = 0; i < count; i++) {
        this->SessionRemove(local->m_sessions[i], local->m_guid);
    }
    bool result = this->m_soundManager->DestroyLocalPlayer(localTalker);
    local->m_guid = 0;
    return result;
}

// OFFSET: 0x00A33DB0/0x1407DE380
bool ComSat::Manager::SessionDestroy(uint64_t session) {
    for (uint8_t i = 0; i < this->m_maxLocalPlayers; i++) {
        LocalTalker* local = this->m_localTalkers[i];
        if (local->m_guid) {
            ComSat::Session* current = local->m_session;
            if (current && current->m_id == session) {
                local->m_session = NULL;
                local->m_frameCount = 0;
                local->m_time = 0;
            }
        }
    }
    uint32_t count = this->m_sessions.Count();
    uint32_t index = 0;
    if (!count) {
        return false;
    }
    while (this->m_sessions[index]->m_id != session) {
        if (++index >= count) {
            return false;
        }
    }
    ComSat::Session* removed = this->m_sessions[index];
    this->m_sessions.RemoveAt(index);
    if (!removed) {
        return false;
    }
    this->m_networkManager->DestroySession(removed->GetNetworkSession());
    ComSat::Delete(*this->m_allocator, removed);
    this->m_soundManager->SessionDestroy(session);
    return true;
}

// PROVEN: f843bb9a IDENTICAL/DIFFERENT
// OFFSET: 0x00A33EB0/0x1407DE1E0
ComSat::Session* ComSat::Manager::FindSession(uint64_t id) {
    for (uint32_t i = 0; i < this->m_sessions.Count(); i++) {
        if (this->m_sessions[i]->m_id == id) {
            return this->m_sessions[i];
        }
    }
    return NULL;
}

// PROVEN: 975bd9d1 IDENTICAL/DIFFERENT
// OFFSET: 0x00A33F10/0x1407DE4A0
bool ComSat::Manager::SessionAddRemote(uint64_t session, uint64_t talker, uint8_t networkId, const uint8_t (&key)[16], const char* address) {
    ComSat::Session* current = this->FindSession(session);
    if (!current) {
        return false;
    }
    for (uint32_t i = 0; i < this->m_maxLocalPlayers; i++) {
        if (this->m_localTalkers[i]->m_guid == talker) {
            return false;
        }
    }
    ComSat::Network::Address remote;
    remote.SetAddress(address);
    if (!remote.GetPort()) {
        remote.SetPort(this->m_defaultPort);
    }
    current->Add(talker, networkId, key, remote);
    return this->m_soundManager->RegisterRemoteTalkerForSession(session, talker);
}

// PROVEN: aa607609 IDENTICAL/DIFFERENT
// OFFSET: 0x00A33FE0/0x1407DE5B0
bool ComSat::Manager::SessionSetTalkerNetworkIdLocal(uint64_t session, uint64_t talker, uint8_t networkId) {
    ComSat::Session* current = this->FindSession(session);
    if (!current) {
        return false;
    }
    ComSat::Network::Session* networkSession = current->GetNetworkSession();
    if (!networkSession) {
        return false;
    }
    networkSession->SetTalkerNetworkIdLocal(talker, networkId);
    return true;
}

// PROVEN: 082f304b IDENTICAL/DIFFERENT
// OFFSET: 0x00A34020/0x1407DE630
bool ComSat::Manager::SessionSetTalkerNetworkIdRemote(uint64_t session, uint64_t talker, uint8_t networkId) {
    ComSat::Session* current = this->FindSession(session);
    if (!current) {
        return false;
    }
    ComSat::Network::Session* networkSession = current->GetNetworkSession();
    if (!networkSession) {
        return false;
    }
    networkSession->SetTalkerNetworkIdRemote(talker, networkId);
    return true;
}

// OFFSET: 0x00A34140/0x1407DE740
ComSat::Manager::Manager(ComSat::Sound::Manager& soundManager, ComSat::Network::Manager& networkManager, const ComSat::Initialization& init, ComSat::Allocator& allocator) : m_sessions(allocator) {
    this->m_maxLocalPlayers = init.m_maxLocalPlayers;
    this->unk_0010 = init.unk_0004;
    this->m_soundManager = &soundManager;
    this->m_networkManager = &networkManager;
    this->m_allocator = &allocator;
    this->m_defaultPort = init.m_port;
    this->m_localTalkers = NULL;
    this->m_callback = NULL;
    this->m_localTalkers = static_cast<LocalTalker**>(this->m_allocator->Alloc(sizeof(LocalTalker*) * this->m_maxLocalPlayers));
    for (uint32_t i = 0; i < this->m_maxLocalPlayers; i++) {
        this->m_localTalkers[i] = new (this->m_allocator->Alloc(sizeof(LocalTalker))) LocalTalker(*this->m_allocator);
    }
    soundManager.SetCallback(this);
}

// OFFSET: [0x00A346D0/0x1407DEEF0, 0x00A347D0]
ComSat::Manager::~Manager() {
    for (uint32_t i = 0; i < this->m_maxLocalPlayers; i++) {
        if (this->m_localTalkers[i]) {
            ComSat::Delete(*this->m_allocator, this->m_localTalkers[i]);
            this->m_localTalkers[i] = NULL;
        }
    }
    if (this->m_localTalkers) {
        this->m_allocator->Free(this->m_localTalkers);
    }
    for (uint32_t i = 0; i < this->m_sessions.Count(); i++) {
        ComSat::Delete(*this->m_allocator, this->m_sessions[i]);
    }
}

// PROVEN: 6b9d4e0b IDENTICAL/DIFFERENT
// OFFSET: 0x00A34240/0x1407DD6D0
bool ComSat::Manager::SessionSetNetworkId(uint64_t session, uint16_t networkId) {
    ComSat::Session* current = this->FindSession(session);
    if (!current) {
        return false;
    }
    ComSat::Network::Session* networkSession = current->GetNetworkSession();
    if (!networkSession) {
        return false;
    }
    networkSession->SetNetworkId(networkId);
    return true;
}

// OFFSET: 0x00A34280/0x1407DE900
bool ComSat::Manager::LocalTalkerSelectSession(uint8_t localTalker, uint64_t session) {
    if (localTalker >= this->m_maxLocalPlayers) {
        return false;
    }
    LocalTalker* local = this->m_localTalkers[localTalker];
    if (local->m_session && local->m_session->m_id == session) {
        return true;
    }
    this->SendVoiceDataToCurrentSession(localTalker);
    if (!session) {
        local->m_session = NULL;
        return true;
    }
    local->m_session = this->FindSession(session);
    return local->m_session != NULL;
}

// OFFSET: 0x00A34300/0x1407DE9C0
bool ComSat::Manager::SessionRemove(uint64_t session, uint64_t talker) {
    ComSat::Session* current = this->FindSession(session);
    if (!current) {
        return false;
    }
    current->Remove(talker);
    if (!this->m_localTalkers) {
        return false;
    }
    uint8_t localTalker = 0xFF;
    for (uint32_t i = 0; i < this->m_maxLocalPlayers; i++) {
        if (this->m_localTalkers[i]->m_guid == talker) {
            localTalker = i;
        }
    }
    if (localTalker == 0xFF) {
        return this->m_soundManager->UnregisterRemoteTalkerFromSession(session, talker);
    }
    bool result = this->m_soundManager->UnregisterLocalTalkerFromSession(session, localTalker);
    bool unused = true;
    for (uint32_t i = 0; i < this->m_maxLocalPlayers; i++) {
        LocalTalker* local = this->m_localTalkers[i];
        if (local->m_guid) {
            if (local->m_guid == talker) {
                local->m_sessions.Remove(session);
            } else {
                uint32_t index = 0;
                if (local->m_sessions.Find(session, index)) {
                    unused = false;
                }
            }
        }
    }
    if (unused) {
        this->SessionDestroy(session);
    }
    return result;
}

// OFFSET: 0x00A34540/0x1407DEC50
bool ComSat::Manager::SessionCreate(uint64_t session) {
    if (this->FindSession(session)) {
        return false;
    }
    void* memory = this->m_allocator->Alloc(sizeof(ComSat::Session));
    if (!memory) {
        return false;
    }
    ComSat::Session* created = new (memory) ComSat::Session(*this, session, *this->m_allocator);
    if (!created) {
        return false;
    }
    ComSat::Network::Session* networkSession = this->m_networkManager->CreateSession(session, *created);
    if (!networkSession) {
        ComSat::Delete(*this->m_allocator, created);
        return false;
    }
    this->m_sessions.PushBack(created);
    created->SetNetworkSession(networkSession);
    this->m_soundManager->SessionCreate(session);
    return true;
}

// OFFSET: 0x00A345F0/0x1407DEDA0
bool ComSat::Manager::SessionAddLocal(uint64_t session, uint8_t localTalker, uint64_t talker) {
    ComSat::Session* current = this->FindSession(session);
    if (!current) {
        return false;
    }
    if (localTalker >= this->m_maxLocalPlayers) {
        return false;
    }
    LocalTalker* local = this->m_localTalkers[localTalker];
    if (!local->m_guid) {
        local->m_guid = talker;
    }
    ComSat::Vector<uint64_t>& sessions = this->m_localTalkers[localTalker]->m_sessions;
    uint32_t index = 0;
    if (!sessions.Find(session, index)) {
        sessions.PushBack(session);
    }
    current->LocalAdd(talker);
    bool result = this->m_soundManager->RegisterLocalTalkerForSession(session, localTalker);
    if (result && !this->m_localTalkers[localTalker]->m_session) {
        this->LocalTalkerSelectSession(localTalker, session);
    }
    return result;
}
