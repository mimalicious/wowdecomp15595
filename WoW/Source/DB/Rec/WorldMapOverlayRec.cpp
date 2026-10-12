#include "WoW/Source/DB/Rec/WorldMapOverlayRec.h"

// STATIC: 0x00C1F220/0x140A13720
static const int32_t s_fieldOffsets[12] = { 0, 4, 8, 24, 28, 32, 36, 40, 44, 48, 52, 56 };

// STATIC: 0x00C1F250/0x140A13750
static const int32_t s_fieldSizes[12] = { 4, 4, 16, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1F280/0x140A13780
static const int32_t s_fieldTypes[12] = { 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C1F2B0/0x140A137B0
const DBCMeta WorldMapOverlayRec::s_meta = { "DBFilesClient\\WorldMapOverlay.dbc", 12, 60, WorldMapOverlayRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: ef96aec1 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2E920/0x1407D7F90
void WorldMapOverlayRec::FixupStrings(void* record, const char* strings) {
    WorldMapOverlayRec* rec = static_cast<WorldMapOverlayRec*>(record);
    DBCFixupString(rec->m_textureName, strings);
}
