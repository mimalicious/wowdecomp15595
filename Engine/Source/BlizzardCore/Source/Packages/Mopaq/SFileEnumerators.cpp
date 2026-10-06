#include <algorithm>
#include <cstdint>
#include <cstring>
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileArchives.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Mopaq/SFileEnumerators.h"
#include "Engine/Source/BlizzardCore/Source/System/File/File.h"
#include "Engine/Source/BlizzardCore/Source/System/String/Path.h"
#include "Engine/Source/BlizzardCore/Source/System/String/String.h"

// INLINED
static void CopyBackslashName(char* name, uint32_t size, const char* source) {
    if (!name || !size) {
        return;
    }

    if (source) {
        uint32_t remaining = size;

        while (true) {
            char c = *source++;
            remaining--;

            if (!c) {
                break;
            }

            *name++ = c == '/' ? '\\' : c;

            if (!remaining) {
                *(name - 1) = '\0';
                return;
            }
        }
    }

    *name = '\0';
}

// OFFSET: 0x007F1500/0x1404FE280
Blizzard::Mopaq::Enumerator::Enumerator(System_SFile::ARCHIVEREC* handle, const char* path, uint32_t flags) {
    this->m_flags = flags;
    this->m_archive = System_SFile::GetArchivePtr(handle, true);

    char buffer[1024];
    Blizzard::String::MakeBackslashPath(path, buffer, sizeof(buffer));

    if (buffer[0]) {
        Blizzard::String::ForceTrailingSeparator(buffer, sizeof(buffer), '\\');
    }

    this->m_path.assign(buffer, strlen(buffer));
}

// OFFSET: 0x007F1630
Blizzard::Mopaq::Enumerator::~Enumerator() {
    if (this->m_archive) {
        this->m_archive->RemoveInternalArchiveRef();
    }
}

// OFFSET: 0x007D3750
bool Blizzard::Mopaq::OnDiskEnumerator::DoGlobCallback(const Blizzard::File::ProcessDirParms& parms) {
    OnDiskEnumerator* enumerator = *static_cast<OnDiskEnumerator**>(parms.m_cookie);
    std::deque<std::string>& list = parms.m_isDir ? enumerator->m_directories : enumerator->m_files;
    std::string path(parms.m_pathRoot);
    list.push_back(path + parms.m_filename);

    return true;
}

// OFFSET: 0x007F16B0/0x1404FE420
Blizzard::Mopaq::OnDiskEnumerator::OnDiskEnumerator(System_SFile::ARCHIVEREC* handle, const char* path, uint32_t flags) : Enumerator(handle, path, flags) {
}

// OFFSET: 0x007F29E0
Blizzard::Mopaq::OnDiskEnumerator::~OnDiskEnumerator() {
}

// OFFSET: 0x007D38C0
bool Blizzard::Mopaq::OnDiskEnumerator::Initialize() {
    if (Blizzard::File::Exists(System_File::Stacked::s_manager, this->m_path.c_str()) != 2) {
        return false;
    }

    OnDiskEnumerator* self = this;

    return Blizzard::File::ProcessDirFast(this->m_path.c_str(), &self, DoGlobCallback, true);
}

// OFFSET: 0x007D3830
bool Blizzard::Mopaq::OnDiskEnumerator::GlobDirectory(const std::string& path, bool* grew) {
    size_t count = this->m_directories.size();
    OnDiskEnumerator* self = this;
    bool result = Blizzard::File::ProcessDirFast(path.c_str(), &self, DoGlobCallback, true);

    if (grew && count && this->m_directories.size() > count) {
        *grew = true;
    }

    return result;
}

// OFFSET: 0x007E2550
int32_t Blizzard::Mopaq::OnDiskEnumerator::Enumerate(char* name, uint32_t size) {
    while (true) {
        while (!this->m_files.empty()) {
            std::string& file = this->m_files.front();

            if (this->m_flags & 2) {
                if (file.size() - this->m_path.size() >= size) {
                    this->m_files.clear();
                    this->m_directories.clear();
                    return 6;
                }

                CopyBackslashName(name, size, file.c_str() + this->m_path.size());
            }

            this->m_files.pop_front();

            if (this->m_flags & 2) {
                return 2;
            }
        }

        if (this->m_directories.empty()) {
            return 0;
        }

        while (!this->m_directories.empty()) {
            std::string& directory = this->m_directories.front();
            bool grew = false;

            if (this->m_flags & 1) {
                directory.insert(directory.end(), '\\');
                this->GlobDirectory(directory, &grew);
                directory.erase(directory.size() - 1);
            }

            if (this->m_flags & 4) {
                if (directory.size() - this->m_path.size() >= size) {
                    this->m_files.clear();
                    this->m_directories.clear();
                    return 6;
                }

                CopyBackslashName(name, size, directory.c_str() + this->m_path.size());
            }

            this->m_directories.pop_front();

            if (grew) {
                std::sort(this->m_directories.begin(), this->m_directories.end());
            }

            if (this->m_flags & 4) {
                return 3;
            }

            if (!this->m_files.empty()) {
                break;
            }
        }
    }
}

// OFFSET: 0x007F1760/0x1404FE4D0
Blizzard::Mopaq::MPQEnumerator::MPQEnumerator(System_SFile::ARCHIVEREC* handle, const char* path, uint32_t flags) : Enumerator(handle, path, flags) {
    this->m_complete = false;

    std::string base(this->m_path);

    if (base.size()) {
        base.erase(base.size() - 1);
    }

    this->m_directories.insert(base);
}

// OFFSET: %0x0074A0F0
Blizzard::Mopaq::MPQEnumerator::~MPQEnumerator() {
    System_SFile::ARCHIVEREC* archive = this->m_archive;
    Blizzard::Lock::MutexEnter(archive->m_mutex.m_mutex);
    archive->ReleaseListfile();
    Blizzard::Lock::MutexLeave(archive->m_mutex.m_mutex);
}

// OFFSET: %0x007577E0
bool Blizzard::Mopaq::MPQEnumerator::Initialize() {
    System_SFile::ARCHIVEREC* archive = this->m_archive;
    Blizzard::Lock::MutexEnter(archive->m_mutex.m_mutex);

    bool complete = archive->AcquireListfile();
    this->m_complete = complete;
    this->m_current = archive->m_listfile.begin();

    Blizzard::Lock::MutexLeave(archive->m_mutex.m_mutex);

    return complete;
}

// OFFSET: 0x007E0ED0/0x1404E6F40
std::string Blizzard::Mopaq::MPQEnumerator::SynthesizeDirectories(const char* path) {
    const char* separator = path ? strrchr(path, '\\') : NULL;

    if (!separator) {
        return std::string();
    }

    std::string current;
    std::string previous;

    while (true) {
        current = std::string(path, separator - path);

        if (this->m_directories.find(current) != this->m_directories.end()) {
            break;
        }

        previous.swap(current);

        if (separator > path) {
            do {
                separator--;
            } while (*separator != '\\' && separator > path);
        }
    }

    this->m_directories.insert(previous);

    return previous;
}

// OFFSET: %0x0075D030
int32_t Blizzard::Mopaq::MPQEnumerator::Enumerate(char* name, uint32_t size) {
    System_SFile::ARCHIVEREC::Listfile& listfile = this->m_archive->m_listfile;

    for (; this->m_current != listfile.end(); ++this->m_current) {
        if (this->m_current->second == -1) {
            continue;
        }

        const std::string& entry = this->m_current->first;
        size_t prefix = this->m_path.size();

        if (prefix && _strnicmp(entry.c_str(), this->m_path.c_str(), prefix)) {
            continue;
        }

        if (this->m_flags & 4) {
            std::string directory = this->SynthesizeDirectories(entry.c_str());

            if (directory.size()) {
                if (directory.size() - prefix >= size) {
                    this->m_current = listfile.end();
                    return 6;
                }

                if (!(this->m_flags & 1) && strchr(directory.c_str() + prefix, '\\')) {
                    continue;
                }

                Blizzard::String::Copy(name, directory.c_str() + prefix, size);

                return 3;
            }
        }

        if (!(this->m_flags & 0xA)) {
            continue;
        }

        if (!(this->m_flags & 1) && strchr(entry.c_str() + prefix, '\\')) {
            continue;
        }

        int32_t result = 2;

        if (SFileFileIsDeleted(this->m_archive, entry.c_str(), 16)) {
            if (!(this->m_flags & 8)) {
                continue;
            }

            result = 4;
        }

        Blizzard::String::Copy(name, entry.c_str() + prefix, size);

        if (entry.size() - prefix < size) {
            ++this->m_current;
            return result;
        }

        this->m_current = listfile.end();

        return 6;
    }

    return !this->m_complete;
}

// INLINED
bool Blizzard::Mopaq::MPQEnumeratorBlockOrder::BlockToList::operator<(const BlockToList& other) const {
    return this->m_blockIndex < other.m_blockIndex;
}

// OFFSET: 0x007F18B0/0x1404FE6D0
Blizzard::Mopaq::MPQEnumeratorBlockOrder::MPQEnumeratorBlockOrder(System_SFile::ARCHIVEREC* handle, const char* path, uint32_t flags) : MPQEnumerator(handle, path, flags) {
}

// OFFSET: %0x0074D490
Blizzard::Mopaq::MPQEnumeratorBlockOrder::~MPQEnumeratorBlockOrder() {
}

// OFFSET: %0x00757860
bool Blizzard::Mopaq::MPQEnumeratorBlockOrder::Initialize() {
    System_SFile::ARCHIVEREC* archive = this->m_archive;
    Blizzard::Lock::MutexEnter(archive->m_mutex.m_mutex);

    this->m_complete = archive->AcquireListfile();
    this->m_blocks.clear();

    for (System_SFile::ARCHIVEREC::Listfile::iterator it = archive->m_listfile.begin(); it != archive->m_listfile.end(); ++it) {
        if (it->second != -1) {
            BlockToList block;
            block.m_blockIndex = it->second;
            block.m_name = it->first;
            this->m_blocks.push_back(block);
        }
    }

    std::sort(this->m_blocks.begin(), this->m_blocks.end());
    this->m_block = this->m_blocks.begin();

    Blizzard::Lock::MutexLeave(archive->m_mutex.m_mutex);

    return true;
}

// OFFSET: %0x0075D440
int32_t Blizzard::Mopaq::MPQEnumeratorBlockOrder::Enumerate(char* name, uint32_t size) {
    for (; this->m_block != this->m_blocks.end(); ++this->m_block) {
        if (this->m_block->m_blockIndex == -1) {
            continue;
        }

        const std::string& entry = this->m_block->m_name;
        size_t prefix = this->m_path.size();

        if (prefix && _strnicmp(entry.c_str(), this->m_path.c_str(), prefix)) {
            continue;
        }

        if (this->m_flags & 4) {
            std::string directory = this->SynthesizeDirectories(entry.c_str());

            if (directory.size()) {
                if (directory.size() - prefix >= size) {
                    this->m_block = this->m_blocks.end();
                    return 6;
                }

                if (!(this->m_flags & 1) && strchr(directory.c_str() + prefix, '\\')) {
                    continue;
                }

                Blizzard::String::Copy(name, directory.c_str() + prefix, size);

                return 3;
            }
        }

        if (!(this->m_flags & 0xA)) {
            continue;
        }

        if (!(this->m_flags & 1) && strchr(entry.c_str() + prefix, '\\')) {
            continue;
        }

        int32_t result = 2;

        if (SFileFileIsDeleted(this->m_archive, entry.c_str(), 0)) {
            if (!(this->m_flags & 8)) {
                continue;
            }

            result = 4;
        }

        Blizzard::String::Copy(name, entry.c_str() + prefix, size);

        if (entry.size() - prefix < size) {
            ++this->m_block;
            return result;
        }

        this->m_block = this->m_blocks.end();

        return 6;
    }

    return !this->m_complete;
}

// PROVEN: 364abc74 IDENTICAL/NONE
// OFFSET: 0x007F9880
bool Blizzard::Mopaq::SFileFileIsDeleted(System_SFile::ARCHIVEREC* handle, const char* name, uint32_t flags) {
    Blizzard::Util::Digest<16> md5 = System_SFile::s_emptyMd5;
    return GetFileBlockEntry(handle, name, md5, flags, NULL, NULL, NULL, NULL) == 4;
}

// OFFSET: 0x007F2A70/0x1404FFCA0
bool Blizzard::Mopaq::SFileOpenArchiveEnumerator(System_SFile::ARCHIVEREC* handle, const char* path, uint32_t flags, Enumerator** result) {
    *result = NULL;

    System_SFile::ArchivePtr archive(System_SFile::GetArchivePtr(handle, true));

    if (!archive.m_archive) {
        return false;
    }

    char fullPath[1024];
    Blizzard::String::JoinPath(fullPath, sizeof(fullPath), archive.m_archive->m_directPath.c_str(), path ? path : "");

    if (archive.m_archive->m_parent) {
        return SFileOpenArchiveEnumerator(archive.m_archive->m_parent, fullPath, flags, result);
    }

    if (!(flags & 6)) {
        System_SFile::s_lastError = 87;
        Blizzard::String::QuickFormat<1024> message("SFileOpenArchiveEnumerator- SFILE_ERROR_INVALID_PARAMETER");
        Blizzard::File::AddToLastErrorStack(87, message.m_buffer, 4);
        return false;
    }

    if (!archive.m_archive->m_file) {
        *result = new OnDiskEnumerator(handle, fullPath, flags);
    } else if (!(flags & 0x10)) {
        *result = new MPQEnumerator(handle, fullPath, flags);
    } else {
        *result = new MPQEnumeratorBlockOrder(handle, fullPath, flags);
    }

    archive.Reset(NULL);

    if ((*result)->Initialize()) {
        return true;
    }

    delete *result;
    *result = NULL;

    return false;
}

// PROVEN: 7b7aef82 IDENTICAL/IDENTICAL
// OFFSET: 0x007AAE40/0x1404A1160
int32_t Blizzard::Mopaq::SFileEnumerateArchive(Enumerator* enumerator, char* name, uint32_t size) {
    return enumerator->Enumerate(name, size);
}

// PROVEN: 2958584f IDENTICAL/IDENTICAL
// OFFSET: 0x007AAE60/0x1404A1170
bool Blizzard::Mopaq::SFileCloseArchiveEnumerator(Enumerator* enumerator) {
    delete enumerator;
    return true;
}
