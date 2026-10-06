#include "Engine/Source/ComSat/ComSatNetwork.h"
#include <new>

// OFFSET: %0x0097BA10
ComSat::Network::Socket::~Socket() {
}

// OFFSET: %0x0097BC10
ComSat::Network::Socket* ComSat::Network::Socket::Create(ComSat::Allocator& allocator) {
    return new (allocator.Alloc(sizeof(ComSat::Network::SocketPosix))) ComSat::Network::SocketPosix();
}
