#include "WoW/Source/DB/Rec/PhaseShiftZoneSoundsRec.h"

// STATIC: 0x00C1ECC8/0x140A130C0
static const int32_t s_fieldOffsets[14] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52 };

// STATIC: 0x00C1ED00/0x140A130F8
static const int32_t s_fieldSizes[14] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1ED38/0x140A13130
static const int32_t s_fieldTypes[14] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C1ED70/0x140A13168
const DBCMeta PhaseShiftZoneSoundsRec::s_meta = { "DBFilesClient\\PhaseShiftZoneSounds.dbc", 14, 56, PhaseShiftZoneSoundsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 1fa5b74e IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void PhaseShiftZoneSoundsRec::FixupStrings(void* record, const char* strings) {
}
