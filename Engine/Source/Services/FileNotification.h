#ifndef ENGINE_SOURCE_SERVICES_FILENOTIFICATION_H
#define ENGINE_SOURCE_SERVICES_FILENOTIFICATION_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

namespace FileNotification {
    typedef void (*Callback)(const char* fileName);

    class CallbackInfo {
        public:
        /* 0000 */ char m_extension[16];
        /* 0010 */ Callback m_callback;
        /* 0014 */ int32_t m_isManifest;
        /* 0018 */ TSLink<FileNotification::CallbackInfo> m_link;
    };

    class DeferredFile : public TSHashObject<FileNotification::DeferredFile, HASHKEY_NONE> {
        public:
        /* 0018 */ char m_filename[260];
    };

    class ManifestEntry {
        public:
        /* 0000 */ uint32_t unk_0000;
        /* 0004 */ TSLink<FileNotification::ManifestEntry> m_link;
    };

    class Manifest : public TSHashObject<FileNotification::Manifest, HASHKEY_NONE> {
        public:
        /* 0018 */ uint8_t unk_0018[0x108];
        /* 0120 */ TSExplicitList<FileNotification::ManifestEntry, offsetof(FileNotification::ManifestEntry, m_link)> m_entries;
        /* 012C */ TSExplicitList<FileNotification::ManifestEntry, offsetof(FileNotification::ManifestEntry, m_link)> m_entries2;
        /* 0138 */ TSLink<FileNotification::Manifest> m_link;
    };

    void Destroy();
    void RegisterDeferredFile(const char* extension);
    void RegisterManifestFile(const char* extension, Callback callback);
    void RegisterSingleFile(const char* extension, Callback callback);
}

#endif
