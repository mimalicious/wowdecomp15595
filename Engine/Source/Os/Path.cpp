#include <cstdint>
#include <cstring>
#include "Engine/Source/Os/Path.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: %0x007DBDB0
uint32_t OsPathGetRootChars(const char* path) {
    uint32_t length = static_cast<uint32_t>(strlen(path));

    if (length < 2) {
        return 0;
    }

    if (path[1] == ':') {
        return (path[2] == '\\') + 2;
    }

    if (path[0] != '\\' || path[1] != '\\') {
        return 0;
    }

    const char* p = path + 2;

    for (int32_t i = 2; i; i--) {
        if (p) {
            p = strchr(p, '\\') + 1;
        }
    }

    if (p) {
        return static_cast<uint32_t>(p - path);
    }

    return length;
}

// OFFSET: %0x007DBE30
uint32_t GetExecutablePathUTF8(char* path, uint32_t size) {
    return GetModuleFileNameUTF8(NULL, path, size);
}

// OFFSET: %0x007DBEB0
char* IOsPathFindExtension(char* path, int32_t offset) {
    char* extension = NULL;

    while (true) {
        switch (*path) {
            case '\0':
                return extension ? extension : path;

            case '.':
                extension = path + offset;
                path++;
                break;

            case '/':
            case '\\':
                extension = NULL;
                path++;
                break;

            default:
                path++;
                break;
        }
    }
}

// OFFSET: #0x0086BD60
char* OsPathFindExtensionWithoutDot(char* path) {
    return IOsPathFindExtension(path, 1);
}

// OFFSET: %0x007DBE50
const char* OsPathFindFilename(const char* path) {
    const char* filename = path;
    int32_t c;

    while ((c = *path++) != 0) {
        if (c == '/' || c == '\\') {
            filename = path;
        }
    }

    return filename;
}

// OFFSET: %0x007DBE80
char* OsPathFindFilename(char* path) {
    char* filename = path;
    int32_t c;

    while ((c = *path++) != 0) {
        if (c == '/' || c == '\\') {
            filename = path;
        }
    }

    return filename;
}

// OFFSET: %0x007DBF50
const char* OsPathFindExtensionWithDot(const char* path) {
    return IOsPathFindExtension(const_cast<char*>(path), 0);
}

// OFFSET: %0x007DBF50
char* OsPathFindExtensionWithDot(char* path) {
    return IOsPathFindExtension(path, 0);
}

// OFFSET: %0x007DBF70
bool OsPathCheckExtension(const char* path, const char* extension) {
    return _stricmp(IOsPathFindExtension(const_cast<char*>(path), 1), extension) == 0;
}

// OFFSET: %0x007DBFA0
void OsPathReplaceExtension(char* dest, uint32_t size, const char* path, const char* extension) {
    char* pathExtension = IOsPathFindExtension(const_cast<char*>(path), 0);
    uint32_t length = static_cast<uint32_t>(pathExtension - path);

    if (path == dest) {
        SStrPrintf(pathExtension, size - length, "%s%s", *extension ? "." : "", extension);
    } else {
        SStrPrintf(dest, size, "%.*s%s%s", length, path, *extension ? "." : "", extension);
    }
}

// OFFSET: %0x007DC020
void OsBuildFontFilePath(const char* fontName, char* path, uint32_t size) {
    SStrPrintf(path, size, "%s\\%s", "Fonts", fontName);
}

// OFFSET: %0x007DC0F0
void OsPathStripFilename(char* path) {
    char* filename = path;

    for (char* p = path; *p; p++) {
        if (*p == '/' || *p == '\\') {
            filename = p + 1;
        }
    }

    if (filename != path) {
        char* root = path + OsPathGetRootChars(path);

        if (filename >= root) {
            *filename = '\0';
        } else {
            *root = '\0';
        }
    }
}

// OFFSET: %0x007DC140
void OsGetExePath(char* path, uint32_t size) {
    GetModuleFileNameUTF8(NULL, path, size);
    OsPathStripFilename(path);
}
