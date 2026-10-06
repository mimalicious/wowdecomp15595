#include "WoW/Source/DB/Rec/WorldMapOverlayRec.h"

// STATIC: 0x00C1F2B0/0x140A137B0
const DBCMeta WorldMapOverlayRec::s_meta = { "DBFilesClient\\WorldMapOverlay.dbc", 12, 60, WorldMapOverlayRec::FixupStrings, 0 };

// PROVEN: ec207016 IDENTICAL/NONE
// OFFSET: 0x00A2E920/0x1407D7F90
void WorldMapOverlayRec::FixupStrings(void* record, const char* strings) {
    WorldMapOverlayRec* rec = static_cast<WorldMapOverlayRec*>(record);
    DBCFixupString(rec->unk_0018, strings);
}
