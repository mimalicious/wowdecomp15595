#include "Engine/Source/ComSat/ComSatInterface.h"
#include "Engine/Source/ComSat/ComSatManager.h"
#include "Engine/Source/ComSat/ComSatNetwork.h"
#include "Engine/Source/ComSat/ComSatSoundManager.h"
#include <new>

// STATIC: %0x00E457FC
static ComSat::Sound::Manager* s_soundManager;

// STATIC: %0x00E45800
static ComSat::Network::Manager* s_networkManager;

// INLINED
ComSat::Allocator::~Allocator() {
}

// OFFSET: #0x007DC350
ComSat::Interface::Callback::~Callback() {
}

// OFFSET: %0x00974650
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

// OFFSET: %0x00974A40
ComSat::Interface* ComSat::Interface::Create(const ComSat::Initialization& init, ComSat::Allocator& allocator) {
    s_soundManager = new (allocator.Alloc(sizeof(ComSat::Sound::Manager))) ComSat::Sound::Manager(init, allocator);
    s_networkManager = new (allocator.Alloc(sizeof(ComSat::Network::Manager))) ComSat::Network::Manager(init, allocator);
    return new (allocator.Alloc(sizeof(ComSat::Manager))) ComSat::Manager(*s_soundManager, *s_networkManager, init, allocator);
}
