#include "WoW/Source/DB/Rec/VideoHardwareRec.h"

// STATIC: 0x00C2B2FC/0x140A22090
const DBCMeta VideoHardwareRec::s_meta = { "DBFilesClient\\VideoHardware.dbc", 23, 92, VideoHardwareRec::FixupStrings, 0 };

// PROVEN: ea49e93d IDENTICAL/NONE
// OFFSET: 0x00A3B7A0/0x1407E8800
void VideoHardwareRec::FixupStrings(void* record, const char* strings) {
    VideoHardwareRec* rec = static_cast<VideoHardwareRec*>(record);
    DBCFixupString(rec->m_oglOverrides, strings);
    DBCFixupString(rec->m_d3DOverrides, strings);
}
