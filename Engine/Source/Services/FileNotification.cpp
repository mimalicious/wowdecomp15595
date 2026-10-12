#include "Engine/Source/Services/FileNotification.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00CA4E18/0x140B432C0
static TSExplicitList<FileNotification::CallbackInfo, offsetof(FileNotification::CallbackInfo, m_link)> s_callbacks;

// STATIC: 0x00DD4330/0x140B432E0
static TSHashTable<FileNotification::DeferredFile, HASHKEY_NONE> s_deferredFiles;

// STATIC: 0x00DD435C/0x140B43330
static TSHashTable<FileNotification::Manifest, HASHKEY_NONE> s_manifests;

// OFFSET: 0x00547160/0x14019EC60
void FileNotification::RegisterSingleFile(const char* extension, Callback callback) {
    FileNotification::CallbackInfo* info = s_callbacks.NewNode(2, 0, 0);
    SStrCopy(info->m_extension, extension, sizeof(info->m_extension));
    info->m_isManifest = 0;
    info->m_callback = callback;
}

// OFFSET: 0x00547240/0x14019ED70
void FileNotification::RegisterManifestFile(const char* extension, Callback callback) {
    FileNotification::CallbackInfo* info = s_callbacks.NewNode(2, 0, 0);
    SStrCopy(info->m_extension, extension, sizeof(info->m_extension));
    info->m_callback = callback;
    info->m_isManifest = 1;
}

// OFFSET: 0x005476A0/0x14019F280
void FileNotification::Destroy() {
    FileNotification::Manifest* manifest = s_manifests.Head();

    while (manifest) {
        FileNotification::Manifest* next = s_manifests.Next(manifest);
        manifest->m_entries.Clear();
        manifest->m_entries2.Clear();
        manifest = next;
    }

    s_callbacks.Clear();
    s_deferredFiles.Clear();
    s_manifests.Clear();
}

// OFFSET: 0x00548130/0x1401A0170
static void AddDeferredFile(const char* fileName) {
    uint32_t hashval = SStrHash(fileName, 0, 0);

    if (s_deferredFiles.Ptr(hashval, HASHKEY_NONE())) {
        return;
    }

    FileNotification::DeferredFile* file = s_deferredFiles.New(hashval, HASHKEY_NONE(), 0, 0);
    SStrCopy(file->m_filename, fileName, sizeof(file->m_filename));
}

// OFFSET: 0x00548200/0x1401A0390
void FileNotification::RegisterDeferredFile(const char* extension) {
    FileNotification::CallbackInfo* info = s_callbacks.NewNode(2, 0, 0);
    SStrCopy(info->m_extension, extension, sizeof(info->m_extension));
    info->m_isManifest = 0;
    info->m_callback = AddDeferredFile;
}
