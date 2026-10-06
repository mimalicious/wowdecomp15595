#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include <typeinfo>
#include "Engine/Source/FrameXML/FrameXML.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Frame/CLayoutFrame.h"
#include "Engine/Source/Frame/CSimpleButton.h"
#include "Engine/Source/Frame/CSimpleCheckbox.h"
#include "Engine/Source/Frame/CSimpleColorSelect.h"
#include "Engine/Source/Frame/CSimpleEditBox.h"
#include "Engine/Source/Frame/CSimpleFont.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleHTML.h"
#include "Engine/Source/Frame/CSimpleMessageFrame.h"
#include "Engine/Source/Frame/CSimpleMessageScrollFrame.h"
#include "Engine/Source/Frame/CSimpleModel.h"
#include "Engine/Source/Frame/CSimpleMovieFrame.h"
#include "Engine/Source/Frame/CSimpleScrollFrame.h"
#include "Engine/Source/Frame/CSimpleSlider.h"
#include "Engine/Source/Frame/CSimpleStatusBar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/MD5.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: %0x00BB806C
static CFrameXMLStatus s_frameXMLStatus;

// STATIC: %0x00CC9124
static uint32_t s_progressTotal;

// STATIC: %0x00CC9128
static uint32_t s_progressFiles;

// STATIC: %0x00CC912C
static void* s_progressParam;

// STATIC: %0x00CC9130
static void (*s_progressCallback)(float progress, void* param);

// STATIC: %0x00CC9134
static int32_t s_debugLevel;

// STATIC: %0x00CC9138
static TSHashTable<HashedNode, HASHKEY_STRI> s_virtualNodes;

// STATIC: %0x00CC9164
static TSHashTable<FrameFactoryNode, HASHKEY_STRI> s_factories;

// OFFSET: %0x004FC1E0
XMLTree* FrameXML_LoadXML(const char* filename, MD5_CTX* md5, CStatus* status) {
    void* data;
    uint32_t size;

    if (!SFile::ReadFile(NULL, filename, &data, &size, 0, 1, NULL)) {
        status->Add(STATUS_ERROR, "Couldn't open %s", filename);
        return NULL;
    }

    MD5Update(md5, static_cast<const uint8_t*>(data), size);

    XMLTree* tree = XMLTree_Load(static_cast<const char*>(data), size);

    if (!tree) {
        status->Add(STATUS_ERROR, "Couldn't parse XML in %s", filename);
    }

    SFile::Unload(data);

    return tree;
}

// OFFSET: %0x004FC270
void FrameXML_SetDebugLevel(int32_t level) {
    s_debugLevel = level;
}

// OFFSET: %0x004FC280
int32_t FrameXML_GetDebugLevel() {
    return s_debugLevel;
}

// OFFSET: %0x004FC290
int32_t FrameXML_GuessNumFiles(const char* toc) {
    int32_t count = 0;
    const char* current = toc;

    while (*current) {
        while (*current == '\r' || *current == '\n') {
            current++;
        }

        char c = *current;

        if (!c) {
            break;
        }

        if (c != '#') {
            count++;
        }

        while (c != '\r' && c != '\n') {
            c = *++current;

            if (!c) {
                return count;
            }
        }
    }

    return count;
}

// OFFSET: %0x004FC320
CSimpleFrame* Create_SimpleCheckButton(CSimpleFrame* parent) {
    void* data = CSimpleCheckbox::s_allocator.GetData(0, typeid(CSimpleCheckbox).raw_name(), SLOG_OBJECT);
    return data ? new (data) CSimpleCheckbox(parent) : NULL;
}

// OFFSET: %0x004FC360
CSimpleFrame* Create_SimpleEditBox(CSimpleFrame* parent) {
    void* data = CSimpleEditBox::s_allocator.GetData(0, typeid(CSimpleEditBox).raw_name(), SLOG_OBJECT);
    return data ? new (data) CSimpleEditBox(parent) : NULL;
}

// OFFSET: %0x004FC3A0
CSimpleFrame* Create_SimpleMessageFrame(CSimpleFrame* parent) {
    void* data = CSimpleMessageFrame::s_allocator.GetData(0, typeid(CSimpleMessageFrame).raw_name(), SLOG_OBJECT);
    return data ? new (data) CSimpleMessageFrame(parent) : NULL;
}

// OFFSET: %0x004FC3E0
CSimpleFrame* Create_SimpleModel(CSimpleFrame* parent) {
    void* data = CSimpleModel::s_allocator.GetData(0, typeid(CSimpleModel).raw_name(), SLOG_OBJECT);
    return data ? new (data) CSimpleModel(parent) : NULL;
}

// OFFSET: %0x004FC420
CSimpleFrame* Create_SimpleScrollFrame(CSimpleFrame* parent) {
    void* data = CSimpleScrollFrame::s_allocator.GetData(0, typeid(CSimpleScrollFrame).raw_name(), SLOG_OBJECT);
    return data ? new (data) CSimpleScrollFrame(parent) : NULL;
}

// OFFSET: %0x004FC460
CSimpleFrame* Create_SimpleScrollingMessageFrame(CSimpleFrame* parent) {
    void* data = CSimpleMessageScrollFrame::s_allocator.GetData(0, typeid(CSimpleMessageScrollFrame).raw_name(), SLOG_OBJECT);
    return data ? new (data) CSimpleMessageScrollFrame(parent, 8) : NULL;
}

// OFFSET: %0x004FC4A0
CSimpleFrame* Create_SimpleSlider(CSimpleFrame* parent) {
    void* data = CSimpleSlider::s_allocator.GetData(0, typeid(CSimpleSlider).raw_name(), SLOG_OBJECT);
    return data ? new (data) CSimpleSlider(parent) : NULL;
}

// OFFSET: %0x004FC4E0
CSimpleFrame* Create_SimpleHTML(CSimpleFrame* parent) {
    void* data = CSimpleHTML::s_allocator.GetData(0, typeid(CSimpleHTML).raw_name(), SLOG_OBJECT);
    return data ? new (data) CSimpleHTML(parent) : NULL;
}

// OFFSET: %0x004FC520
CSimpleFrame* Create_SimpleStatusBar(CSimpleFrame* parent) {
    void* data = CSimpleStatusBar::s_allocator.GetData(0, typeid(CSimpleStatusBar).raw_name(), SLOG_OBJECT);
    return data ? new (data) CSimpleStatusBar(parent) : NULL;
}

// OFFSET: %0x004FC560
CSimpleFrame* Create_SimpleColorSelect(CSimpleFrame* parent) {
    void* data = CSimpleColorSelect::s_allocator.GetData(0, typeid(CSimpleColorSelect).raw_name(), SLOG_OBJECT);
    return data ? new (data) CSimpleColorSelect(parent) : NULL;
}

// OFFSET: %0x004FC5A0
CSimpleFrame* Create_SimpleMovieFrame(CSimpleFrame* parent) {
    void* data = CSimpleMovieFrame::s_allocator.GetData(0, typeid(CSimpleMovieFrame).raw_name(), SLOG_OBJECT);
    return data ? new (data) CSimpleMovieFrame(parent) : NULL;
}

// OFFSET: %0x004FC5E0
void FrameXML_RegisterLoadProgressCallback(void (*callback)(float progress, void* param), void* param, uint32_t total) {
    s_progressCallback = callback;
    s_progressParam = param;
    s_progressFiles = 0;
    s_progressTotal = total;
}

// OFFSET: %0x004FC2E0
CSimpleFrame* Create_SimpleButton(CSimpleFrame* parent) {
    void* data = CSimpleButton::s_allocator.GetData(0, typeid(CSimpleButton).raw_name(), SLOG_OBJECT);
    return data ? new (data) CSimpleButton(parent) : NULL;
}

// OFFSET: %0x004FC690
CSimpleFrame* Create_SimpleFrame(CSimpleFrame* parent) {
    void* data = CSimpleFrame::s_allocator.GetData(0, typeid(CSimpleFrame).raw_name(), SLOG_OBJECT);
    return data ? new (data) CSimpleFrame(parent) : NULL;
}

// OFFSET: %0x004FCA80
void FrameXML_FreeHashNodes() {
    s_virtualNodes.InternalClear(0);
}

// OFFSET: %0x004FCA90
void FrameXML_ClearFactories() {
    s_factories.InternalClear(0);
    XMLTree_Cleanup();
}

// INLINED
HashedNode::~HashedNode() {
    FrameXML_FreeCompiled(this->m_node, true);
}

// INLINED
XMLHashNodeRef::XMLHashNodeRef() {
    this->m_node = NULL;
}

// OFFSET: %0x004FCE10
linkObject* XMLHashNodeRef::Acquire(const char* name) {
    this->m_locked = true;
    SStrCopy(this->m_name, name, sizeof(this->m_name));

    HashedNode* hashed = s_virtualNodes.Ptr(this->m_name);

    if (hashed) {
        this->m_locked = hashed->m_locked;
        this->m_taint = hashed->m_taint;
        hashed->m_locked = true;

        if (hashed->m_node) {
            this->m_node = hashed->m_node;
        }
    }

    return this->m_node;
}

// OFFSET: %0x004FCE80
XMLHashNodeRef::~XMLHashNodeRef() {
    if (!this->m_node) {
        return;
    }

    if (!this->m_locked) {
        FrameXML_ReleaseHashNode(this->m_name);
    }

    this->m_node = NULL;
}

// OFFSET: %0x004FCBD0
void FrameXML_ReleaseHashNode(const char* name) {
    s_virtualNodes.Ptr(name)->m_locked = false;
}

// OFFSET: %0x0087E570
void CFrameXMLStatus::Add(STATUS_TYPE severity, const char* format, ...) {
    char buffer[512];

    va_list args;
    va_start(args, format);
    SStrVPrintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    DebugLog(buffer);
}

// OFFSET: %0x004FCEB0
const char* FindParent(const Frame* frame) {
    if (frame->m_frameFlags & 0x400000) {
        return frame->m_parent.m_string;
    }

    if (!(frame->m_flags & 0x2000000)) {
        return NULL;
    }

    const char* inherits = frame->m_inherits.m_string;
    char token[1024];
    SStrTokenize(&inherits, token, sizeof(token), " ,", NULL);

    if (!token[0]) {
        return NULL;
    }

    while (true) {
        XMLHashNodeRef ref;
        ref.Acquire(token);

        if (ref.m_node && (ref.m_node->m_flags & 8)) {
            const char* parent = FindParent(static_cast<Frame*>(ref.m_node));

            if (parent) {
                return parent;
            }
        }

        SStrTokenize(&inherits, token, sizeof(token), " ,", NULL);

        if (!token[0]) {
            return NULL;
        }
    }
}

// OFFSET: %0x004FCFC0
CSimpleFrame* FrameXML_CreateFrame(linkObject* node, CSimpleFrame* parent, CStatus* status) {
    if (s_debugLevel > 0) {
        if (node->m_flags & 0x1000000) {
            status->Add(STATUS_INFO, "-- Creating %s named %s", node->m_type.m_string, node->m_name);
        } else {
            status->Add(STATUS_INFO, "-- Creating unnamed %s", node->m_type.m_string);
        }
    }

    FrameFactoryNode* factory = s_factories.Ptr(node->m_type.m_string);

    if (!factory) {
        status->Add(STATUS_WARNING, "Unknown frame type: %s", node->m_type.m_string);
        return NULL;
    }

    if (!(node->m_flags & 8)) {
        return NULL;
    }

    const char* parentName = FindParent(static_cast<Frame*>(node));

    if (parentName && *parentName) {
        parent = static_cast<CSimpleFrame*>(CScriptObject::GetScriptObjectByName(parentName, CSimpleFrame::GetObjectType()));

        if (!parent) {
            status->Add(STATUS_WARNING, "Couldn't find frame parent: %s", parentName);
        }
    }

    CSimpleFrame* frame = factory->m_factory(parent);

    if (!frame) {
        status->Add(STATUS_WARNING, "Unable to create frame type: %s", node->m_type.m_string);
        return NULL;
    }

    if (factory->m_unique) {
        s_factories.Delete(factory);
    }

    frame->PreLoadXML(node, status);
    frame->LoadXML(node, status);
    frame->PostLoadXML(node, status);

    return frame;
}

// OFFSET: %0x004FDB60
bool FrameXML_StoreHashNode(linkObject* node, const char* name, const char* taint, CStatus* status) {
    HashedNode* hashed = s_virtualNodes.Ptr(name);

    if (hashed) {
        if (!hashed->m_taint || taint) {
            status->Add(STATUS_WARNING, "Virtual object named %s already exists", name);
            return false;
        }

        FrameXML_FreeCompiled(hashed->m_node, true);
    } else {
        hashed = s_virtualNodes.New(name, 0, 0);
    }

    uint32_t size;
    hashed->m_node = node->Compile(size);
    hashed->m_node->m_flags |= 1;
    hashed->m_taint = taint;

    if (s_debugLevel > 0) {
        status->Add(STATUS_INFO, "-- Added virtual frame %s", name);
    }

    return true;
}

// OFFSET: %0x004FDCA0
int32_t FrameXML_RegisterFactory(const char* name, CSimpleFrame* (*factory)(CSimpleFrame* parent), bool unique) {
    if (s_factories.Ptr(name)) {
        return 0;
    }

    FrameFactoryNode* node = s_factories.New(name, 0, 0);
    node->m_factory = factory;
    node->m_unique = unique;

    return 1;
}

// OFFSET: %0x004FDD80
int32_t FrameXML_RegisterDefault() {
    FrameXML_RegisterFactory("Button", Create_SimpleButton, false);
    FrameXML_RegisterFactory("CheckButton", Create_SimpleCheckButton, false);
    FrameXML_RegisterFactory("EditBox", Create_SimpleEditBox, false);
    FrameXML_RegisterFactory("Frame", Create_SimpleFrame, false);
    FrameXML_RegisterFactory("MessageFrame", Create_SimpleMessageFrame, false);
    FrameXML_RegisterFactory("Model", Create_SimpleModel, false);
    FrameXML_RegisterFactory("ScrollFrame", Create_SimpleScrollFrame, false);
    FrameXML_RegisterFactory("ScrollingMessageFrame", Create_SimpleScrollingMessageFrame, false);
    FrameXML_RegisterFactory("Slider", Create_SimpleSlider, false);
    FrameXML_RegisterFactory("SimpleHTML", Create_SimpleHTML, false);
    FrameXML_RegisterFactory("StatusBar", Create_SimpleStatusBar, false);
    FrameXML_RegisterFactory("ColorSelect", Create_SimpleColorSelect, false);
    FrameXML_RegisterFactory("MovieFrame", Create_SimpleMovieFrame, false);

    return 1;
}

// INLINED
static void MakeRelativePath(char* dest, const char* filename, const char* file) {
    const char* slash = SStrChrR(const_cast<char*>(filename), '\\');

    if (slash) {
        size_t length = slash - filename + 1;

        if (length < 260) {
            SStrCopy(dest, filename, 260);
            dest[length] = '\0';
            SStrPack(dest, file, 260);
        }
    } else {
        SStrCopy(dest, file, 260);
    }
}

// OFFSET: %0x004FDE70
int32_t FrameXML_ProcessFile(const char* filename, const char* wrapper, MD5_CTX* md5, CStatus* status) {
    char cleanPath[260];

    if (SStrStr(filename, "..")) {
        SStrCopy(cleanPath, filename, sizeof(cleanPath));

        for (char* dots = SStrStr(cleanPath, ".."); dots; dots = SStrStr(cleanPath, "..")) {
            char before = dots[-1];

            if (before != '\\' && before != '/') {
                break;
            }

            char after = dots[2];

            if (after != '\\' && after != '/') {
                break;
            }

            char* dir = dots - 2;

            while (dir >= cleanPath && *dir != '\\' && *dir != '/') {
                dir--;
            }

            SStrCopy(dir + 1, dots + 3, STORM_MAX_STR);
        }

        filename = cleanPath;
    }

    const char* extension = SStrChrR(const_cast<char*>(filename), '.');

    if (extension && !SStrCmpI(extension, ".lua", STORM_MAX_STR)) {
        return FrameScript_ExecuteFile(filename, wrapper, md5, status);
    }

    CStatus fileStatus;
    MemBlock block;
    linkObject* ui;
    XMLTree* tree = FrameXML_LoadXML(filename, md5, status);

    if (!tree || (ui = FrameXML_CompileXML_Ui(XMLTree_GetRoot(tree), block), XMLTree_Free(tree), !ui)) {
        return 0;
    }

    for (linkObject** link = &ui->m_child; *link; ) {
        linkObject* child = *link;

        switch (child->m_element) {
            case XML_INCLUDE: {
                Include* include = static_cast<Include*>(child);

                if (include->m_includeFlags & 1) {
                    char includePath[260];
                    MakeRelativePath(includePath, filename, include->m_file.m_string);
                    FrameXML_ProcessFile(includePath, wrapper, md5, &fileStatus);
                } else {
                    fileStatus.Add(STATUS_ERROR, "Element 'Include' without file attribute");
                }

                link = &child->m_next;
                break;
            }

            case XML_SCRIPT: {
                Script* script = static_cast<Script*>(child);

                if (script->m_fileFlags & 1) {
                    char scriptPath[260];
                    MakeRelativePath(scriptPath, filename, script->m_file.m_string);
                    FrameScript_ExecuteFile(scriptPath, wrapper, md5, &fileStatus);
                }

                if (script->m_bodyFlags & 1) {
                    char scriptName[271];
                    SStrPrintf(scriptName, sizeof(scriptName), "%s:<Scripts>", filename);
                    FrameScript_Execute(script->m_body.m_string, scriptName, lua_tainted);
                }

                link = &child->m_next;
                break;
            }

            case XML_FONT: {
                if (child->m_flags & 0x1000000) {
                    CSimpleFont* font = CSimpleFont::GetFont(child->m_name.m_string, 1);
                    font->LoadXML(child, status);
                } else {
                    fileStatus.Add(STATUS_WARNING, "Unnamed font node at top level");
                }

                link = &child->m_next;
                break;
            }

            default: {
                uint32_t flags = child->m_flags;

                if (!(flags & 0x800000) || !(flags & 0x4000000)) {
                    FrameXML_CreateFrame(child, NULL, &fileStatus);
                    CLayoutFrame::ResizePending();
                    link = &child->m_next;
                } else if (!(flags & 0x1000000)) {
                    fileStatus.Add(STATUS_WARNING, "Unnamed virtual node at top level");
                    link = &child->m_next;
                } else if (!FrameXML_StoreHashNode(child, child->m_name.m_string, lua_tainted, &fileStatus)) {
                    link = &child->m_next;
                } else {
                    *link = child->m_next;
                }

                break;
            }
        }
    }

    FrameXML_FreeCompiled(ui, false);

    if (s_debugLevel > 0 || fileStatus.m_highestSeverity > STATUS_INFO) {
        fileStatus.Prepend(STATUS_INFO, "++ Loading file %s", filename);
    }

    status->Add(fileStatus);

    return 1;
}

// OFFSET: %0x004FE3A0
int32_t FrameXML_CreateFrames(const char* tocFile, const char* wrapper, MD5_CTX* md5, CStatus* status) {
    if (!status) {
        status = &s_frameXMLStatus;
    }

    char tocPath[260];
    memset(tocPath, 0, sizeof(tocPath));

    const char* slash = SStrChrR(const_cast<char*>(tocFile), '\\');

    if (slash) {
        size_t length = slash - tocFile + 1;

        if (length < sizeof(tocPath) - 1) {
            SStrCopy(tocPath, tocFile, length + 1);
            tocPath[length] = '\0';
        }
    }

    void* data;
    uint32_t size;

    if (!SFile::ReadFile(NULL, tocFile, &data, &size, 1, 1, NULL)) {
        status->Add(STATUS_ERROR, "Couldn't open %s", tocFile);
        return 0;
    }

    MD5Update(md5, static_cast<const uint8_t*>(data), size);

    const char* text = static_cast<const char*>(data);

    if (static_cast<uint8_t>(text[0]) == 0xEF && static_cast<uint8_t>(text[1]) == 0xBB && static_cast<uint8_t>(text[2]) == 0xBF) {
        text += 3;
    }

    CStatus tocStatus;
    char line[1024];

    do {
        SStrTokenize(&text, line, sizeof(line), "\r\n", NULL);

        if (!line[0]) {
            break;
        }

        if (line[0] != '#') {
            char path[260];
            SStrCopy(path, tocPath, sizeof(path));
            SStrPack(path, line, sizeof(path));

            size_t length = SStrLen(path);
            char* end = &path[length];

            if (path[length - 1] == ' ') {
                do {
                    if (end <= path) {
                        break;
                    }

                    end--;
                } while (end[-1] == ' ');
            }

            *end = '\0';

            FrameXML_ProcessFile(path, wrapper, md5, &tocStatus);

            if (s_progressCallback && s_progressFiles < s_progressTotal) {
                s_progressFiles++;
                s_progressCallback(static_cast<float>(s_progressFiles) / static_cast<float>(s_progressTotal), s_progressParam);
            }
        }
    } while (line[0]);

    XMLTree_Cleanup();
    SFile::Unload(data);

    if (s_debugLevel > 0 || tocStatus.m_highestSeverity > STATUS_INFO) {
        tocStatus.Prepend(STATUS_INFO, "** Loading table of contents %s", tocFile);
    }

    status->Add(tocStatus);

    return 1;
}
