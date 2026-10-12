#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_FILE_TYPES_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_FILE_TYPES_H

#include <cstddef>
#include <cstdint>
#include <string>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"
#include "Engine/Source/BlizzardCore/Source/System/Lock/Lock.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Types.h"

namespace Blizzard { namespace File { struct Filesystem; }}
namespace Blizzard { namespace File { struct ProcessDirParms; }}
namespace Blizzard { namespace File { struct StreamRecord; }}
namespace Blizzard { namespace File { struct StreamingInfo; }}
namespace System_File { namespace Stacked { struct FileParms; }}

namespace Blizzard {
namespace File {
typedef int32_t INODE;
}
}

namespace Blizzard {
namespace File {
struct FileInfo {
    /* 0000 */ char* m_path;
    /* 0008 */ int64_t m_size;
    /* 0010 */ int64_t m_compressedSize;
    /* 0018 */ int32_t m_attributes;
    /* 0020 */ Blizzard::Time::Timestamp m_createTime;
    /* 0028 */ Blizzard::Time::Timestamp m_modTime;
    /* 0030 */ Blizzard::Time::Timestamp m_accessTime;
    /* 0038 */ int32_t m_existence;
    /* 003C */ bool m_regularFile;
};
}
}

namespace Blizzard {
namespace File {
union FileInfoMask {
    /* 0000 */ uint8_t unk_0000[4];
    /* 0000 */ uint32_t m_all;
};
}
}

namespace System_File {
namespace Stacked {
typedef bool (*FileFunc)(Blizzard::File::Filesystem* fs, System_File::Stacked::FileParms* parms);
}
}

namespace Blizzard {
namespace File {
struct Functions {
    /* 0000 */ System_File::Stacked::FileFunc m_supportssparse;
    /* 0004 */ System_File::Stacked::FileFunc m_cd;
    /* 0008 */ System_File::Stacked::FileFunc m_close;
    /* 000C */ System_File::Stacked::FileFunc m_create;
    /* 0010 */ System_File::Stacked::FileFunc m_cwd;
    /* 0014 */ System_File::Stacked::FileFunc m_dirwalk;
    /* 0018 */ System_File::Stacked::FileFunc m_exists;
    /* 001C */ System_File::Stacked::FileFunc m_flush;
    /* 0020 */ System_File::Stacked::FileFunc m_getfileinfo;
    /* 0024 */ System_File::Stacked::FileFunc m_getfreespace;
    /* 0028 */ System_File::Stacked::FileFunc m_getpos;
    /* 002C */ System_File::Stacked::FileFunc m_getrootchars;
    /* 0030 */ System_File::Stacked::FileFunc m_isabspath;
    /* 0034 */ System_File::Stacked::FileFunc m_isreadonly;
    /* 0038 */ System_File::Stacked::FileFunc m_makeabspath;
    /* 003C */ System_File::Stacked::FileFunc m_mkdir;
    /* 0040 */ System_File::Stacked::FileFunc m_move;
    /* 0044 */ System_File::Stacked::FileFunc m_copy;
    /* 0048 */ System_File::Stacked::FileFunc m_open;
    /* 004C */ System_File::Stacked::FileFunc m_read;
    /* 0050 */ System_File::Stacked::FileFunc m_readp;
    /* 0054 */ System_File::Stacked::FileFunc m_rmdir;
    /* 0058 */ System_File::Stacked::FileFunc m_setcachemode;
    /* 005C */ System_File::Stacked::FileFunc m_seteof;
    /* 0060 */ System_File::Stacked::FileFunc m_setfileinfo;
    /* 0064 */ System_File::Stacked::FileFunc m_setpos;
    /* 0068 */ System_File::Stacked::FileFunc m_setsparse;
    /* 006C */ System_File::Stacked::FileFunc m_zerorange;
    /* 0070 */ System_File::Stacked::FileFunc m_unlink;
    /* 0074 */ System_File::Stacked::FileFunc m_write;
    /* 0078 */ System_File::Stacked::FileFunc m_writep;
    /* 007C */ System_File::Stacked::FileFunc m_shutdown;
};
}
}

namespace Blizzard {
namespace File {
struct Filesystem {
    /* 0000 */ Blizzard::File::Filesystem* m_original;
    /* 0004 */ Blizzard::File::Filesystem* m_next;
    /* 0008 */ Blizzard::File::Functions m_verbs;
};
}
}

namespace Blizzard {
namespace File {
typedef bool (*ProcessDirCallback_interface)(const Blizzard::File::ProcessDirParms& parms);
}
}

namespace Blizzard {
namespace File {
typedef Blizzard::File::ProcessDirCallback_interface ProcessDirCallback;
}
}

namespace Blizzard {
namespace File {
typedef Blizzard::File::StreamRecord* Stream;
}
}

namespace Blizzard {
namespace File {
struct StreamRecord {
    /* 0000 */ void* m_fileHandle;
    /* 0004 */ int32_t m_mode;
    /* 0008 */ bool m_infoValid;
    /* 0009 */ uint8_t unk_0009[3];
    /* 000C */ uint32_t unk_000C;
    /* 0010 */ Blizzard::File::FileInfo m_info;
    /* 0050 */ Blizzard::File::StreamingInfo* m_streaming;
    /* 0054 */ char* m_pathname;
};
}
}

namespace Blizzard {
namespace File {
struct ProcessDirParms {
    /* 0000 */ const char* m_pathRoot;
    /* 0004 */ const char* m_filename;
    /* 0008 */ void* m_cookie;
    /* 000C */ bool m_isDir;
    /* 000D */ uint8_t unk_000D[3];
};
}
}


namespace Blizzard {
namespace File {
struct StreamingInfo {
    /* 0000 */ Blizzard::Storage::StorageUnit* m_unit;
    /* 0004 */ std::string m_name;
    /* 0020 */ Blizzard::Lock::Mutex m_lock;
    /* 0038 */ int32_t m_refCount;
    /* 0040 */ Blizzard::File::FileInfo m_info;
};
}
}

namespace offset_in_Blizzard {
namespace File {
namespace Functions_to_System_File {
namespace Stacked {
typedef uintptr_t FileFunc;
}
}
}
}

#endif
