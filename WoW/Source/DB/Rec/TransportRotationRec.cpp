#include "WoW/Source/DB/Rec/TransportRotationRec.h"

// STATIC: 0x00C1FE40/0x140A14518
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C1FE50/0x140A14528
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 16 };

// STATIC: 0x00C1FE60/0x140A14538
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 3 };

// STATIC: 0x00C1FE70/0x140A14548
const DBCMeta TransportRotationRec::s_meta = { "DBFilesClient\\TransportRotation.dbc", 4, 28, TransportRotationRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void TransportRotationRec::FixupStrings(void* record, const char* strings) {
}
