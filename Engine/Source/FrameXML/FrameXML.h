#ifndef ENGINE_SOURCE_FRAMEXML_FRAMEXML_H
#define ENGINE_SOURCE_FRAMEXML_FRAMEXML_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Base/Types.h"
#include "Engine/Source/Storm/H/stpl.h"

class CSimpleFrame;
class Frame;
class XMLNode;
class linkObject;
struct MD5_CTX;
struct XMLTree;

class CFrameXMLStatus : public CStatus {
    public:
    virtual void Add(STATUS_TYPE severity, const char* format, ...);
};

struct FrameFactoryNode : public TSHashObject<FrameFactoryNode, HASHKEY_STRI> {
    /* 0018 */ CSimpleFrame* (*m_factory)(CSimpleFrame* parent);
    /* 001C */ bool m_unique;
};

struct HashedNode : public TSHashObject<HashedNode, HASHKEY_STRI> {
    /* 0018 */ linkObject* m_node;
    /* 001C */ const char* m_taint;
    /* 0020 */ bool m_locked;

    ~HashedNode();
};

class XMLHashNodeRef {
    public:
    /* 0000 */ bool m_locked;
    /* 0004 */ linkObject* m_node;
    /* 0008 */ const char* m_taint;
    /* 000C */ char m_name[128];

    XMLHashNodeRef();
    ~XMLHashNodeRef();
    linkObject* Acquire(const char* name);
};

CSimpleFrame* Create_SimpleButton(CSimpleFrame* parent);
CSimpleFrame* Create_SimpleCheckButton(CSimpleFrame* parent);
CSimpleFrame* Create_SimpleColorSelect(CSimpleFrame* parent);
CSimpleFrame* Create_SimpleEditBox(CSimpleFrame* parent);
CSimpleFrame* Create_SimpleFrame(CSimpleFrame* parent);
CSimpleFrame* Create_SimpleHTML(CSimpleFrame* parent);
CSimpleFrame* Create_SimpleMessageFrame(CSimpleFrame* parent);
CSimpleFrame* Create_SimpleModel(CSimpleFrame* parent);
CSimpleFrame* Create_SimpleMovieFrame(CSimpleFrame* parent);
CSimpleFrame* Create_SimpleScrollFrame(CSimpleFrame* parent);
CSimpleFrame* Create_SimpleScrollingMessageFrame(CSimpleFrame* parent);
CSimpleFrame* Create_SimpleSlider(CSimpleFrame* parent);
CSimpleFrame* Create_SimpleStatusBar(CSimpleFrame* parent);
const char* FindParent(const Frame* frame);
void FrameXML_ClearFactories();
CSimpleFrame* FrameXML_CreateFrame(linkObject* node, CSimpleFrame* parent, CStatus* status);
int32_t FrameXML_CreateFrames(const char* tocFile, const char* wrapper, MD5_CTX* md5, CStatus* status);
void FrameXML_FreeHashNodes();
int32_t FrameXML_GetDebugLevel();
int32_t FrameXML_GuessNumFiles(const char* toc);
XMLTree* FrameXML_LoadXML(const char* filename, MD5_CTX* md5, CStatus* status);
int32_t FrameXML_ProcessFile(const char* filename, const char* wrapper, MD5_CTX* md5, CStatus* status);
int32_t FrameXML_RegisterDefault();
int32_t FrameXML_RegisterFactory(const char* name, CSimpleFrame* (*factory)(CSimpleFrame* parent), bool unique);
void FrameXML_RegisterLoadProgressCallback(void (*callback)(float progress, void* param), void* param, uint32_t total);
void FrameXML_ReleaseHashNode(const char* name);
void FrameXML_SetDebugLevel(int32_t level);
bool FrameXML_StoreHashNode(linkObject* node, const char* name, const char* taint, CStatus* status);

#include "Engine/Source/FrameXML/FrameXML.inl"

#endif
