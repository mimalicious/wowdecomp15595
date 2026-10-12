#include "WoW/Source/DB/Rec/PhaseXPhaseGroupRec.h"

// STATIC: 0x00C22FB0/0x140A180C8
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C22FBC/0x140A180D8
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C22FC8/0x140A180E8
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C22FD4/0x140A180F8
const DBCMeta PhaseXPhaseGroupRec::s_meta = { "DBFilesClient\\PhaseXPhaseGroup.dbc", 3, 12, PhaseXPhaseGroupRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void PhaseXPhaseGroupRec::FixupStrings(void* record, const char* strings) {
}
