#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_FILE_FILESTACK_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_FILE_FILESTACK_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/System/File/Types.h"

namespace System_File {
namespace Stacked {
struct FileParms {
    /* 0000 */ offset_in_Blizzard::File::Functions_to_System_File::Stacked::FileFunc m_verb;
    /* 0004 */ char* m_path;
    /* 0008 */ char* m_newpath;
    /* 000C */ Blizzard::File::Stream m_stream;
    /* 0010 */ Blizzard::File::FileInfo* m_info;
    /* 0014 */ uint32_t m_extra;
    /* 0018 */ Blizzard::File::FileInfo m_info2;
    /* 0058 */ Blizzard::File::FileInfoMask m_has;
    /* 005C */ Blizzard::File::FileInfoMask m_want;
    /* 0060 */ int32_t m_mode;
    /* 0064 */ void* m_buffer;
    /* 0068 */ int32_t m_amount;
    /* 006C */ uint8_t unk_006C[4];
    /* 0070 */ int64_t m_pos;
    /* 0078 */ int64_t m_length;
    /* 0080 */ int32_t m_posMode;
    /* 0084 */ char* m_cwdbuf;
    /* 0088 */ int32_t m_cwdbuflen;
    /* 008C */ bool m_recursive;
    /* 008D */ bool m_makeCanonical;
    /* 008E */ uint8_t unk_008E[2];
    /* 0090 */ void* m_cookie;
    /* 0094 */ Blizzard::File::ProcessDirCallback m_dircallback;
    /* 0098 */ bool m_sorted;
    /* 0099 */ bool m_overwrite;
    /* 009A */ bool m_grantEveryone;
    /* 009B */ bool m_zeroFillAll;
    /* 009C */ bool m_supportsSparse;
    /* 009D */ uint8_t unk_009D[3];
    /* 00A0 */ int32_t m_streamFlags;
};
}
}

#endif
