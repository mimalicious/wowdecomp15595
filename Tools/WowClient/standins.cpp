#include <windows.h>
#include <cstdio>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/IOHandler.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/StackedMopaq.h"

static void StandIn(const char* name, uint32_t address) {
    char text[512];
    _snprintf_s(text, sizeof(text), _TRUNCATE, "STAND-IN HIT: %s (13623 0x%08X) - not ported, exiting\n", name, address);
    fputs(text, stderr);
    fflush(stderr);
    OutputDebugStringA(text);
    ExitProcess(3);
}

bool Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleLoadAndVerify(HSFILERecord* file, uint8_t* buffer, int64_t offset, uint32_t size, const Blizzard::Util::Digest<16>& md5) {
    StandIn("MpqRepairOnDemandErrorHandler::HandleLoadAndVerify", 0x0074D8F0);
    return false;
}

bool Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleMPQHeaderV3Error(HSFILERecord* file, uint8_t* buffer, int64_t offset, uint32_t size) {
    StandIn("MpqRepairOnDemandErrorHandler::HandleMPQHeaderV3Error", 0x0074D6D0);
    return false;
}

bool Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleInitSectorTableError(MpqHandleRecord* handle, uint8_t* table, int64_t offset, uint32_t size) {
    StandIn("MpqRepairOnDemandErrorHandler::HandleInitSectorTableError", 0x0075C910);
    return false;
}

bool Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleMD5BlockError(MpqHandleRecord* handle, uint8_t* buffer, int64_t fileSize, uint32_t block, int32_t blockSize, uint32_t size, uint8_t* md5) {
    StandIn("MpqRepairOnDemandErrorHandler::HandleMD5BlockError", 0x0074DB10);
    return false;
}

bool Blizzard::Mopaq::MpqRepairOnDemandErrorHandler::HandleNonStreamableFileError(MpqHandleRecord* handle, uint8_t* buffer, int64_t offset, uint32_t count) {
    StandIn("MpqRepairOnDemandErrorHandler::HandleNonStreamableFileError", 0x0075C4B0);
    return false;
}

bool Blizzard::Mopaq::HSFILERecord::Repair() {
    StandIn("HSFILERecord::Repair", 0x007531B0);
    return false;
}

bool PurgeCacheFile(Blizzard::Mopaq::HSFILERecord* file) {
    StandIn("PurgeCacheFile", 0x007530A0);
    return false;
}
