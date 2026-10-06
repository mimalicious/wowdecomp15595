#include "WoW/Source/DB/Rec/LoadingScreenTaxiSplinesRec.h"

// STATIC: 0x00C23628/0x140A18930
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 40, 72 };

// STATIC: 0x00C2363C/0x140A18948
static const int32_t s_fieldSizes[5] = { 4, 4, 32, 32, 4 };

// STATIC: 0x00C23650/0x140A18960
static const int32_t s_fieldTypes[5] = { 0, 0, 3, 3, 0 };

// STATIC: 0x00C23664/0x140A18978
const DBCMeta LoadingScreenTaxiSplinesRec::s_meta = { "DBFilesClient\\LoadingScreenTaxiSplines.dbc", 5, 76, LoadingScreenTaxiSplinesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: c72bd4ab IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void LoadingScreenTaxiSplinesRec::FixupStrings(void* record, const char* strings) {
}
