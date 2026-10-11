#include <cstdint>
#include <cstring>
#include <list>
#include <new>
#include "Engine/Source/Services/PreloadList.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/System/Memory/AllocSTL.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/LoadingScreen.h"

// STATIC: 0x00CA4CCC
static int32_t s_allFilesLocal = 1;

// STATIC: 0x00DD4314
static float s_progressBase;

// STATIC: 0x00DD4318
static float s_progressScale;

// STATIC: 0x00DD431C
static uint32_t s_totalSize;

// STATIC: 0x00DD4320
static int32_t s_cancelled;

// STATIC: 0x00DD4324
static std::list<PreloadList*, Blizzard::Memory::AllocSTL_raw<PreloadList*> > s_lists;

// OFFSET: 0x00546870/0x14019E0B0
static void IngameProgressCallback(float progress, int32_t a2) {
    LoadingScreenUpdateProgress(s_progressScale * progress + s_progressBase, 0);
}

// OFFSET: 0x005468A0
static void ArchiveProgressCallback(float progress, int32_t a2) {
    if (s_allFilesLocal) {
        return;
    }

    ArchiveProgressSetPosition(s_progressScale * progress + s_progressBase);

    if (!ArchiveProgressPump()) {
        s_cancelled = 1;
    }
}

// OFFSET: 0x005468F0/0x14019E120
void PreloadList::ListFinishedCallback(CAsyncObject* object) {
    PreloadList* list = static_cast<PreloadList*>(object->m_userArg);
    list->m_objects[object->m_userFlags] = NULL;
    AsyncFileReadDestroyObject(object);
}

// OFFSET: 0x00546920/0x14019E150
void PreloadList::GetListName(char* name, int32_t raceID, int32_t classID) {
    SStrCopy(name, "TrialLists\\Start", 260);

    if (classID == 6) {
        SStrPack(name, s_classFileNames[6], 260);
    } else {
        SStrPack(name, s_raceFileNames[raceID], 260);
    }

    SStrPack(name, ".lst", 260);
}

// OFFSET: 0x00546A20
int32_t PreloadList::ListNameChanged(int32_t raceID, int32_t classID) {
    char name[260];
    PreloadList::GetListName(name, raceID, classID);
    return _strnicmp(name, this->m_name, 260) != 0;
}

// OFFSET: 0x00546A70/0x14019E300
int32_t PreloadList::IsComplete() {
    for (uint32_t i = 0; i < this->m_objects.Count(); i++) {
        if (this->m_objects[i]) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x00546B00
void PreloadList::PreloadFile(const char* name) {
    CAsyncObject* object = NULL;

    if (AsyncFileReadCreateObject(name, 5, 0, 0, &object, NULL, this, PreloadList::ListFinishedCallback, PreloadList::ListFinishedCallback, this->m_objects.Count(), 0, 0) && object) {
        this->m_objects.New(object);
    }
}

// OFFSET: 0x00546B60
void PreloadList::ParseAndLoad(const char* name) {
    char* data;
    uint32_t size;

    if (!SFile::LoadFile(name, reinterpret_cast<void**>(&data), &size, 1)) {
        return;
    }

    data[size] = 0;
    uint32_t end = size + 1;
    size++;
    uint32_t start = 0;
    uint32_t pos = 0;

    if (end) {
        do {
            bool found = true;

            for (char c = data[pos]; c != '\r'; pos++) {
                if (c == '\n' || !c) {
                    break;
                }

                if (pos >= end) {
                    found = false;
                    break;
                }

                c = data[pos + 1];
            }

            if (found && pos < end && pos > start && data[start] != '#') {
                data[pos] = 0;
                this->PreloadFile(&data[start]);
                end = size;
            }

            for (start = pos + 1;; start++) {
                char c = data[start];

                if (c != '\r' && c != '\n' && c) {
                    break;
                }

                if (start >= end) {
                    break;
                }
            }

            pos = start + 1;
        } while (start + 1 < end);
    }

    SFile::Unload(data);
}

// OFFSET: 0x00546C20/0x14019E4D0
PreloadList::PreloadList(const char* name, int32_t waitAll) {
    this->m_waitAll = waitAll;
    SStrCopy(this->m_name, name, sizeof(this->m_name));
    this->ParseAndLoad(name);
}

// INLINED
PreloadList::PreloadList(int32_t raceID, int32_t classID, int32_t waitAll) {
    this->m_waitAll = waitAll;
    PreloadList::GetListName(this->m_name, raceID, classID);
    this->ParseAndLoad(this->m_name);
}

// OFFSET: 0x00546C90/0x14019E540
void PreloadList::Cancel() {
    s_lists.remove(this);

    for (uint32_t i = 0; i < this->m_objects.Count(); i++) {
        if (this->m_objects[i]) {
            AsyncFileReadCancel(this->m_objects[i], AsyncFileReadDestroyObject);
        }
    }

    this->m_objects.SetCount(0);
}

// INLINED
PreloadList::~PreloadList() {
    this->Cancel();
}

// OFFSET: 0x00546CF0
PreloadList* PreloadList::StartDownload(int32_t raceID, int32_t classID, int32_t waitAll) {
    PreloadList* list = new (SMemAlloc(sizeof(PreloadList), __FILE__, __LINE__, 0)) PreloadList(raceID, classID, waitAll);
    s_lists.push_back(list);
    return list;
}

// OFFSET: 0x00546D90/0x14019E6D0
PreloadList* PreloadList::StartDownload(const char* name, int32_t waitAll) {
    PreloadList* list = new (SMemAlloc(sizeof(PreloadList), __FILE__, __LINE__, 0)) PreloadList(name, waitAll);
    s_lists.push_back(list);
    return list;
}

// OFFSET: 0x00546E00/0x14019E760
int32_t PreloadList::Wait() {
    s_allFilesLocal = 1;

    for (uint32_t i = 0; i < this->m_objects.Count() && !s_cancelled; i++) {
        if (!this->m_objects[i]) {
            continue;
        }

        AsyncFileReadEnterQueueLock();

        CAsyncObject* object = this->m_objects[i];

        if (object->m_file && SFile::FileIsLocal(object->m_file, 0, 0) != 1) {
            s_allFilesLocal = 0;
        }

        AsyncFileReadLeaveQueueLock();

        if (s_totalSize) {
            s_progressScale = static_cast<double>(this->m_objects[i]->m_size) / static_cast<double>(s_totalSize);
        } else {
            s_progressScale = 0.0f;
        }

        AsyncFileReadWait(this->m_objects[i]);
        s_progressBase += s_progressScale;
    }

    if (s_cancelled) {
        this->Cancel();
    }

    this->m_objects.SetCount(0);
    s_allFilesLocal = 1;

    return s_cancelled == 0;
}

// OFFSET: 0x00546F10
int32_t PreloadList::DownloadAndWait(const char* name, int32_t showProgress, void (*progressCallback)(float, int32_t)) {
    s_cancelled = 0;
    PreloadList list(name, 0);

    if (showProgress) {
        uint32_t total = 0;
        s_totalSize = 0;
        s_progressBase = 0.0f;

        for (uint32_t i = 0; i < list.m_objects.Count(); i++) {
            if (list.m_objects[i]) {
                total += list.m_objects[i]->m_size;
                s_totalSize = total;
            }
        }

        AsyncFileReadIngameProgressCallback(NULL, progressCallback ? progressCallback : ArchiveProgressCallback);
    }

    int32_t result = list.Wait();
    AsyncFileReadIngameProgressCallback(NULL, NULL);

    return result;
}

// OFFSET: 0x00546FE0/0x14019EA80
void PreloadList::StopDownload(PreloadList* list) {
    if (list) {
        list->~PreloadList();
        SMemFree(list, "delete", SLOG_FUNCTION, 0);
    }
}

// OFFSET: 0x00547030/0x14019EAE0
void PreloadList::WaitAll() {
    uint32_t total = 0;
    s_totalSize = 0;
    bool waiting = false;
    s_progressBase = 0.0f;

    for (std::list<PreloadList*, Blizzard::Memory::AllocSTL_raw<PreloadList*> >::iterator it = s_lists.begin(); it != s_lists.end(); ++it) {
        PreloadList* list = *it;

        if (!list->m_waitAll) {
            continue;
        }

        for (uint32_t i = 0; i < list->m_objects.Count(); i++) {
            if (list->m_objects[i]) {
                total += list->m_objects[i]->m_size;
                waiting = true;
                s_totalSize = total;
            }
        }
    }

    if (waiting) {
        AsyncFileReadIngameProgressCallback(NULL, IngameProgressCallback);
    }

    for (std::list<PreloadList*, Blizzard::Memory::AllocSTL_raw<PreloadList*> >::iterator it = s_lists.begin(); it != s_lists.end(); ++it) {
        (*it)->Wait();
    }

    if (waiting) {
        AsyncFileReadIngameProgressCallback(NULL, NULL);
        LoadingScreenUpdateProgress(1.0f, 0);
    }
}

// OFFSET: 0x00547100
void PreloadList::Shutdown() {
    for (std::list<PreloadList*, Blizzard::Memory::AllocSTL_raw<PreloadList*> >::iterator it = s_lists.begin(); it != s_lists.end();) {
        PreloadList* list = *it;
        ++it;
        PreloadList::StopDownload(list);
    }
}
