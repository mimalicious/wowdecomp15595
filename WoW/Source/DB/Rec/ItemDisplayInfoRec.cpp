#include "WoW/Source/DB/Rec/ItemDisplayInfoRec.h"

// STATIC: 0x00C1D204/0x140A11098
static const int32_t s_fieldOffsets[12] = { 0, 4, 12, 20, 28, 40, 44, 48, 52, 60, 92, 96 };

// STATIC: 0x00C1D234/0x140A110C8
static const int32_t s_fieldSizes[12] = { 4, 8, 8, 8, 12, 4, 4, 4, 8, 32, 4, 4 };

// STATIC: 0x00C1D264/0x140A110F8
static const int32_t s_fieldTypes[12] = { 0, 2, 2, 2, 0, 0, 0, 0, 0, 2, 0, 0 };

// STATIC: 0x00C1D294/0x140A11128
const DBCMeta ItemDisplayInfoRec::s_meta = { "DBFilesClient\\ItemDisplayInfo.dbc", 12, 100, NULL, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };
