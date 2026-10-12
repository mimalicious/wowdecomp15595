#include "WoW/Source/DB/Rec/VideoHardwareRec.h"

// STATIC: 0x00C2B1E0/0x140A21F70
static const int32_t s_fieldOffsets[23] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88 };

// STATIC: 0x00C2B240/0x140A21FD0
static const int32_t s_fieldSizes[23] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C2B2A0/0x140A22030
static const int32_t s_fieldTypes[23] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 0, 0, 0 };

// STATIC: 0x00C2B2FC/0x140A22090
const DBCMeta VideoHardwareRec::s_meta = { "DBFilesClient\\VideoHardware.dbc", 23, 92, VideoHardwareRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: ea49e93d IDENTICAL/DIFFERENT
// OFFSET: 0x00A3B7A0/0x1407E8800
void VideoHardwareRec::FixupStrings(void* record, const char* strings) {
    VideoHardwareRec* rec = static_cast<VideoHardwareRec*>(record);
    DBCFixupString(rec->m_oglOverrides, strings);
    DBCFixupString(rec->m_d3DOverrides, strings);
}
