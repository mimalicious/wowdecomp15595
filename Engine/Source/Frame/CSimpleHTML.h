#ifndef ENGINE_SOURCE_FRAME_CSIMPLEHTML_H
#define ENGINE_SOURCE_FRAME_CSIMPLEHTML_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFontedFrame.h"
#include "Engine/Source/Frame/CSimpleHyperlinkedFrame.h"
#include "Engine/Source/Frame/Types.h"
#include "Engine/Source/Storm/H/stpl.h"

class CLayoutFrame;
class CRect;
class CSimpleRegion;
class CStatus;
class XMLNode;
class linkObject;
struct lua_State;

struct CONTENTNODE : public TSLinkedNode<CONTENTNODE> {
    /* 0008 */ int32_t m_type;
    /* 000C */ CSimpleRegion* m_region;
};

class CSimpleHTML : public CSimpleHyperlinkedFrame, public CSimpleFontedFrame {
    public:
    static CDataAllocator s_allocator;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static void operator delete(void* ptr);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static int32_t GetObjectType();
    static void RegisterScriptMethods(lua_State* L);

    /* 01F8 */ TSList<CONTENTNODE, TSGetLink<CONTENTNODE> > m_content;
    /* 0204 */ CLayoutFrame* m_layoutAnchor;
    /* 0208 */ float m_layoutOffset;
    /* 020C */ CSimpleFontedFrameFont* m_fonts[NUM_HTML_TEXT_TYPES];
    /* 021C */ char* m_hyperlinkFormat;
    /* 0220 */ TSExplicitList<CSimpleHyperlinkButton, offsetof(CSimpleHyperlinkButton, m_hyperlinkLink)> m_hyperlinks;
    /* 022C */ int32_t m_hyperlinksDirty;
    /* 0230 */ bool m_hyperlinksEnabled;

    virtual ~CSimpleHTML();
    virtual int32_t GetScriptMetaTable();
    virtual bool IsA(int32_t type);
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
    virtual void OnLayerUpdate(float elapsedSec);
    virtual void LoadXML(linkObject* node, CStatus* status);
    virtual void OnFrameSizeChanged(const CRect& rect);
    virtual void FontUpdated(CSimpleFontedFrameFont* font, int32_t flags);

    CSimpleHTML(CSimpleFrame* parent);
    void AddText(const char* text, HTML_TEXT_TYPE type, uint32_t justify);
    void ClearContent();
    void ParseBODY(XMLNode* node, CStatus* status);
    void ParseIMG(XMLNode* node, CStatus* status);
    void ParseP(XMLNode* node, HTML_TEXT_TYPE type, CStatus* status);
    void RefreshHyperlinks();
    void SetHyperlinkFormat(const char* format);
    bool SetText(const char* text, CStatus* status);
};

#include "Engine/Source/Frame/CSimpleHTML.inl"

#endif
