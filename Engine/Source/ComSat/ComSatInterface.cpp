#include "Engine/Source/ComSat/ComSatInterface.h"
#include "Engine/Source/ComSat/ComSatManager.h"
#include "Engine/Source/ComSat/ComSatNetwork.h"
#include "Engine/Source/ComSat/ComSatSoundManager.h"
#include <new>

// STATIC: 0x00F80448/0x140E782E0
static ComSat::Sound::Manager* s_soundManager;

// STATIC: 0x00F8044C/0x140E782E8
static ComSat::Network::Manager* s_networkManager;

// INLINED
ComSat::Allocator::~Allocator() {
}

// INLINED
ComSat::Interface::Callback::~Callback() {
}

// PROVEN: 2e8733bd IDENTICAL/IDENTICAL
// OFFSET: 0x00A340C0/0x1407DE6B0
void ComSat::Interface::Destroy(ComSat::Interface* comsat, ComSat::Allocator& allocator) {
    if (comsat) {
        comsat->~Interface();
        allocator.Free(comsat);
    }

    if (s_soundManager) {
        ComSat::Sound::Manager* soundManager = s_soundManager;
        soundManager->~Manager();
        allocator.Free(soundManager);
        s_soundManager = NULL;
    }

    if (s_networkManager) {
        ComSat::Network::Manager* networkManager = s_networkManager;
        networkManager->~Manager();
        allocator.Free(networkManager);
        s_networkManager = NULL;
    }
}

// PROVEN: a6adbfbd IDENTICAL/IDENTICAL
// OFFSET: 0x00A344B0/0x1407DEB80
ComSat::Interface* ComSat::Interface::Create(const ComSat::Initialization& init, ComSat::Allocator& allocator) {
    s_soundManager = new (allocator.Alloc(sizeof(ComSat::Sound::Manager))) ComSat::Sound::Manager(init, allocator);
    s_networkManager = new (allocator.Alloc(sizeof(ComSat::Network::Manager))) ComSat::Network::Manager(init, allocator);
    return new (allocator.Alloc(sizeof(ComSat::Manager))) ComSat::Manager(*s_soundManager, *s_networkManager, init, allocator);
}
