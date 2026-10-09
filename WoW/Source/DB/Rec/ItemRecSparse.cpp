#include "WoW/Source/DB/Rec/ItemRecSparse.h"

// STATIC: 0x00C1C6E8
static const int32_t s_fieldOffsets[] = { 0, 4, 8, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 136, 176, 216, 256, 260, 264, 268, 272, 292, 312, 332, 352, 372, 392, 396, 400, 404, 408, 412, 416, 420, 424, 428, 432, 436, 440, 444, 448, 452, 456, 460, 464, 468, 472, 484, 496, 500, 504, 508, 512, 516, 520, 524, 528 };

// STATIC: 0x00C1C7F8
static const int32_t s_fieldSizes[] = { 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 40, 40, 40, 40, 4, 4, 4, 4, 20, 20, 20, 20, 20, 20, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 12, 12, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1C908
static const int32_t s_fieldTypes[] = { 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0 };

// STATIC: 0x00C1C6B4
const DB2Meta ItemRecSparse::s_meta = { "DBFilesClient\\Item-sparse.db2", "Item-sparse.adb", 68, 532, ItemRecSparse::FixupRecord, 0, true, true, s_fieldOffsets, s_fieldSizes, s_fieldTypes, 0x919BE54E, 0x50238EC2, 5 };

// PROVEN: 96001d76 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemRecSparse::FixupRecord(void* record) {
}
