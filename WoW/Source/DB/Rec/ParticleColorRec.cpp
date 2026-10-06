#include "WoW/Source/DB/Rec/ParticleColorRec.h"

// STATIC: 0x00C1CDB8/0x140A10B38
const DBCMeta ParticleColorRec::s_meta = { "DBFilesClient\\ParticleColor.dbc", 4, 40, ParticleColorRec::FixupStrings, 0 };

// PROVEN: ab3598b8 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ParticleColorRec::FixupStrings(void* record, const char* strings) {
}
