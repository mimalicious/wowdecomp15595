#ifndef WOW_SOURCE_WORLDCLIENT_AREALIST_H
#define WOW_SOURCE_WORLDCLIENT_AREALIST_H

#include <cstddef>
#include <cstdint>

class AreaTableRec;
class WMOAreaTableRec;

AreaTableRec* AreaListGetObjectArea(uintptr_t object);
int32_t AreaListGetName(uint32_t areaID, char* buffer, uint32_t size, int32_t includeParent);
void AreaListRegisterLocation(int32_t mapID, uintptr_t object);
void AreaListShutdown();
void AreaListUpdateSounds(const AreaTableRec* zone, const AreaTableRec* area, const WMOAreaTableRec* rootRec, const WMOAreaTableRec* groupRec, int32_t inside, int32_t mapID);
bool AreaListZoneHasBreathParticles(uintptr_t object);

#endif
