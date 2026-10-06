#include "WoW/Source/DB/Rec/TransportRotationRec.h"

// STATIC: 0x00C1FE70/0x140A14548
const DBCMeta TransportRotationRec::s_meta = { "DBFilesClient\\TransportRotation.dbc", 4, 28, TransportRotationRec::FixupStrings, 0 };

// PROVEN: 0b1f474d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void TransportRotationRec::FixupStrings(void* record, const char* strings) {
}
