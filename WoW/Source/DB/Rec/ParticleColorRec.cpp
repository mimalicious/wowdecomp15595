#include "WoW/Source/DB/Rec/ParticleColorRec.h"

// STATIC: 0x00C1CD88/0x140A10B08
static const int32_t s_fieldOffsets[4] = { 0, 4, 16, 28 };

// STATIC: 0x00C1CD98/0x140A10B18
static const int32_t s_fieldSizes[4] = { 4, 12, 12, 12 };

// STATIC: 0x00C1CDA8/0x140A10B28
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C1CDB8/0x140A10B38
const DBCMeta ParticleColorRec::s_meta = { "DBFilesClient\\ParticleColor.dbc", 4, 40, ParticleColorRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void ParticleColorRec::FixupStrings(void* record, const char* strings) {
}
