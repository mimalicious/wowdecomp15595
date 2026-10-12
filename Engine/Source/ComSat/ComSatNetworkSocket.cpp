#include "Engine/Source/ComSat/ComSatNetwork.h"
#include <new>

// OFFSET: 0x00A3B4D0/0x1407E8230
ComSat::Network::Socket::~Socket() {
}

// OFFSET: 0x00A3B6D0/0x1407E84C0
ComSat::Network::Socket* ComSat::Network::Socket::Create(ComSat::Allocator& allocator) {
    return new (allocator.Alloc(sizeof(ComSat::Network::SocketPosix))) ComSat::Network::SocketPosix();
}
