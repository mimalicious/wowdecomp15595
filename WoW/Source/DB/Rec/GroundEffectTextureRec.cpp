#include "WoW/Source/DB/Rec/GroundEffectTextureRec.h"

// STATIC: 0x00C2810C/0x140A1E838
static const int32_t s_fieldOffsets[5] = { 0, 4, 20, 36, 40 };

// STATIC: 0x00C28120/0x140A1E850
static const int32_t s_fieldSizes[5] = { 4, 16, 16, 4, 4 };

// STATIC: 0x00C28134/0x140A1E868
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 0, 0 };

// STATIC: 0x00C28148/0x140A1E880
const DBCMeta GroundEffectTextureRec::s_meta = { "DBFilesClient\\GroundEffectTexture.dbc", 5, 44, GroundEffectTextureRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void GroundEffectTextureRec::FixupStrings(void* record, const char* strings) {
}
