#include "WoW/Source/DB/Rec/TransportPhysicsRec.h"

// STATIC: 0x00C1FF38/0x140A14638
const DBCMeta TransportPhysicsRec::s_meta = { "DBFilesClient\\TransportPhysics.dbc", 11, 44, TransportPhysicsRec::FixupStrings, 0 };

// PROVEN: 0416542b IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void TransportPhysicsRec::FixupStrings(void* record, const char* strings) {
}
