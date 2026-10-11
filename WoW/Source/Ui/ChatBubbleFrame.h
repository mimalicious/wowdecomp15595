#ifndef WOW_SOURCE_UI_CHATBUBBLEFRAME_H
#define WOW_SOURCE_UI_CHATBUBBLEFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Object/WGUID.h"

class CGUnit_C;
class CGWorldFrame;
class CImVector;
class CSimpleFontString;

class CGChatBubbleFrame : public TSLinkedNode<CGChatBubbleFrame>, public CSimpleFrame {
    public:
    static CDataAllocator s_chatBubbleHeap;

    static void operator delete(void* ptr);
    static bool AppendRaidIconTexture(char* dest, char* tag, uint32_t size);
    static void ClearAll();
    static CGChatBubbleFrame* GetNewChatBubble(CSimpleFrame* parent);
    static void OnWorldRender(CGWorldFrame* worldFrame);
    static void OnWorldLayerUpdate();
    static void InitializeGame();
    static void Remove(CGChatBubbleFrame* bubble);
    static void ShutdownGame();

    /* 01F8 */ CSimpleFontString* m_fontString;
    /* 01FC */ uint32_t m_bubbleFlags;
    /* 0200 */ float m_screenX;
    /* 0204 */ float m_screenY;
    /* 0208 */ float m_screenDepth;
    /* 020C */ uint32_t m_showTime;
    /* 0210 */ uint32_t m_fadeStart;
    /* 0214 */ int32_t m_duration;
    /* 0218 */ WOWGUID m_unit;
    /* 0220 */ float m_height;
    /* 0224 */ uint8_t m_positionValid;
    /* 0228 */ float m_fadeAlpha;

    ~CGChatBubbleFrame();

    CGChatBubbleFrame(CSimpleFrame* parent);
    void FadeIn();
    void FadeOut(bool remove);
    C3Vector GetUnitPosition();
    void ParseRaidIcons(char* text, char* dest, uint32_t size);
    void Initialize(const WOWGUID& unit, const char* text, const CImVector& color);
    void Render(CGWorldFrame* worldFrame);
    bool Update(CGUnit_C* player, CGUnit_C* unit);
};

#endif
