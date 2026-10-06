#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_ARCHIVECHECKPOINT_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_ARCHIVECHECKPOINT_H

#include <cstdint>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"

namespace Blizzard {
namespace Mopaq {
struct HSFILERecord;
}
}

class ArchiveCheckpoint {
    public:
    static bool ApplyRestorePoint(Blizzard::Mopaq::HSFILERecord* file);
    static bool CreateRestorePoint(const System_SFile::ARCHIVEREC* archive);
    static void DeleteRestorePoint(const char* path);
};

#endif
