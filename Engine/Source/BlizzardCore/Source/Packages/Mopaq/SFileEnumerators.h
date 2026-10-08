#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILEENUMERATORS_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_MOPAQ_SFILEENUMERATORS_H

#include <cstdint>
#include <deque>
#include <set>
#include <string>
#include <vector>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/System/File/Types.h"

namespace Blizzard {
namespace Mopaq {

class Enumerator {
    public:
    /* 0004 */ System_SFile::ARCHIVEREC* m_archive;
    /* 0008 */ uint32_t m_flags;
    /* 000C */ std::string m_path;

    Enumerator(System_SFile::ARCHIVEREC* handle, const char* path, uint32_t flags);
    virtual ~Enumerator();
    virtual bool Initialize() = 0;
    virtual int32_t Enumerate(char* name, uint32_t size) = 0;
};

class OnDiskEnumerator : public Enumerator {
    public:
    static bool DoGlobCallback(const Blizzard::File::ProcessDirParms& parms);

    /* 0028 */ std::deque<std::string> m_files;
    /* 0048 */ std::deque<std::string> m_directories;

    OnDiskEnumerator(System_SFile::ARCHIVEREC* handle, const char* path, uint32_t flags);
    virtual ~OnDiskEnumerator();
    virtual bool Initialize();
    virtual int32_t Enumerate(char* name, uint32_t size);
    bool GlobDirectory(const std::string& path, bool* grew);
};

class MPQEnumerator : public Enumerator {
    public:
    /* 0028 */ bool m_complete;
    /* 0029 */ uint8_t unk_0029[3];
    /* 002C */ std::vector<System_SFile::ListFileEntry>::iterator m_current;
    /* 0034 */ std::set<std::string> m_directories;

    MPQEnumerator(System_SFile::ARCHIVEREC* handle, const char* path, uint32_t flags);
    virtual ~MPQEnumerator();
    virtual bool Initialize();
    virtual int32_t Enumerate(char* name, uint32_t size);
    std::string SynthesizeDirectories(const char* path);
};

class MPQEnumeratorBlockOrder : public MPQEnumerator {
    public:
    struct BlockToList {
        /* 0000 */ uint32_t m_blockIndex;
        /* 0004 */ const char* m_name;

        bool operator<(const BlockToList& other) const;
    };

    /* 007C */ std::vector<BlockToList> m_blocks;
    /* 0094 */ std::vector<BlockToList>::iterator m_block;

    MPQEnumeratorBlockOrder(System_SFile::ARCHIVEREC* handle, const char* path, uint32_t flags);
    virtual ~MPQEnumeratorBlockOrder();
    virtual bool Initialize();
    virtual int32_t Enumerate(char* name, uint32_t size);
};

bool SFileCloseArchiveEnumerator(Enumerator* enumerator);
int32_t SFileEnumerateArchive(Enumerator* enumerator, char* name, uint32_t size);
bool SFileFileIsDeleted(System_SFile::ARCHIVEREC* handle, const char* name, uint32_t flags);
bool SFileOpenArchiveEnumerator(System_SFile::ARCHIVEREC* handle, const char* path, uint32_t flags, Enumerator** result);

}
}

#endif
