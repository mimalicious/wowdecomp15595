#ifndef ENGINE_SOURCE_FRAME_CSIMPLEEDITBOX_H
#define ENGINE_SOURCE_FRAME_CSIMPLEEDITBOX_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFontedFrame.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleHyperlinkedFrame.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/CRect.h"

class CEvent;
class linkObject;
class CSimpleFontString;
class CSimpleMessageFrame;
class CSimpleTexture;

class CSimpleEditBox : public CSimpleHyperlinkedFrame, public CSimpleFontedFrame {
    public:
    struct EditHistory {
        /* 0000 */ char* m_text;
        /* 0004 */ const char* m_taint;

        EditHistory();
        EditHistory(const EditHistory& source);
        ~EditHistory();
    };

    class EventHandler {
        public:
        virtual void unk_0000() = 0;
        virtual void unk_0004() = 0;
        virtual void unk_0008() = 0;
        virtual void unk_000C() = 0;
        virtual void OnEvent(const CEvent& evt) = 0;
    };

    struct EventCallback {
        /* 0000 */ uint32_t m_id;
        /* 0004 */ CSimpleEditBox::EventHandler* m_handler;
    };

    static CDataAllocator s_allocator;
    static CSimpleEditBox* s_currentFocus;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;
    static const char* s_inputLanguageNames[4];

    static void operator delete(void* ptr);
    static void ClearKeyboardFocus(CSimpleEditBox* editBox, bool runScript);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static int32_t GetObjectType();
    static void RegisterScriptMethods(lua_State* L);
    static void SetKeyboardFocus(CSimpleEditBox* editBox);

    /* 0204 */ uint32_t m_editBoxFlags;
    /* 0208 */ uint32_t m_dirtyBits;
    /* 020C */ const char* m_textTaint;
    /* 0210 */ CSimpleFontedFrameFont* m_font;
    /* 0214 */ CSimpleFontString* m_fontString;
    /* 0218 */ char* m_text;
    /* 021C */ uint32_t* m_textInfo;
    /* 0220 */ char* m_textHidden;
    /* 0224 */ int32_t m_textLength;
    /* 0228 */ int32_t m_textMaxLength;
    /* 022C */ int32_t m_textMaxLetters;
    /* 0230 */ uint32_t m_textSize;
    /* 0234 */ int32_t m_visiblePosition;
    /* 0238 */ int32_t m_visibleLength;
    /* 023C */ CSimpleTexture* m_highlightTextures[3];
    /* 0248 */ int32_t m_highlightLeft;
    /* 024C */ int32_t m_highlightRight;
    /* 0250 */ int32_t m_highlightDrag;
    /* 0254 */ CSimpleTexture* m_cursorTexture;
    /* 0258 */ int32_t m_cursorPos;
    /* 025C */ float m_cursorBlinkSpeed;
    /* 0260 */ float m_blinkElapsedTime;
    /* 0264 */ TSGrowableArray<uint32_t> m_visibleLines;
    /* 0274 */ int32_t m_historyLines;
    /* 0278 */ int32_t m_historyIndex;
    /* 027C */ TSFixedArray<CSimpleEditBox::EditHistory> m_history;
    /* 0288 */ CSimpleEditBox::EventCallback m_enterPressedCallback;
    /* 0290 */ CSimpleEditBox::EventCallback m_escapePressedCallback;
    /* 0298 */ CSimpleEditBox::EventCallback m_spacePressedCallback;
    /* 02A0 */ CSimpleEditBox::EventCallback m_tabPressedCallback;
    /* 02A8 */ CSimpleEditBox::EventCallback m_textChangedCallback;
    /* 02B0 */ CSimpleEditBox::EventCallback m_textSetCallback;
    /* 02B8 */ int32_t m_imeComposition;
    /* 02BC */ int32_t m_inputLanguage;
    /* 02C0 */ CSimpleTexture* m_clauseHighlight;
    /* 02C4 */ int32_t m_clauseStart;
    /* 02C8 */ int32_t m_clauseEnd;
    /* 02CC */ CSimpleMessageFrame* m_candidatesFrame;
    /* 02D0 */ CSimpleTexture* m_candidateHighlight;
    /* 02D4 */ CRect m_textInsets;
    /* 02E4 */ FrameScript_Object::ScriptIx m_onEnterPressed;
    /* 02E5 */ FrameScript_Object::ScriptIx m_onEscapePressed;
    /* 02E6 */ FrameScript_Object::ScriptIx m_onSpacePressed;
    /* 02E7 */ FrameScript_Object::ScriptIx m_onTabPressed;
    /* 02E8 */ FrameScript_Object::ScriptIx m_onTextChanged;
    /* 02E9 */ FrameScript_Object::ScriptIx m_onTextSet;
    /* 02EA */ FrameScript_Object::ScriptIx m_onCursorChanged;
    /* 02EB */ FrameScript_Object::ScriptIx m_onInputLanguageChanged;
    /* 02EC */ FrameScript_Object::ScriptIx m_onEditFocusGained;
    /* 02ED */ FrameScript_Object::ScriptIx m_onEditFocusLost;
    /* 02EE */ FrameScript_Object::ScriptIx m_onCharComposition;

    virtual ~CSimpleEditBox();
    virtual int32_t GetScriptMetaTable();
    virtual FrameScript_Object::ScriptIx* GetScriptByName(const char* name, FrameScript_Object::ScriptData& data);
    virtual bool IsA(int32_t type);
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
    virtual void GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse);
    virtual void OnLayerShow();
    virtual void OnLayerHide();
    virtual void OnLayerUpdate(float elapsedSec);
    virtual int32_t OnLayerTrackUpdate(const CMouseEvent& evt);
    virtual void OnFrameSizeChanged(float width, float height);
    virtual int32_t OnLayerIme(const CImeEvent& evt);
    virtual int32_t OnLayerKeyDownRepeat(const CKeyEvent& evt);
    virtual int32_t OnLayerChar(const CCharEvent& evt);
    virtual int32_t OnLayerKeyDown(const CKeyEvent& evt);
    virtual int32_t OnLayerKeyUp(const CKeyEvent& evt);
    virtual int32_t OnLayerMouseDown(const CMouseEvent& evt, const char* btn);
    virtual int32_t OnLayerMouseUp(const CMouseEvent& evt, const char* btn);
    virtual void LoadXML(linkObject* node, CStatus* status);
    virtual void FontUpdated(CSimpleFontedFrameFont* font, int32_t flags);
    virtual void RefreshHyperlinks();
    virtual void Enable(int32_t enabled);

    CSimpleEditBox(CSimpleFrame* parent);
    void AddHistoryLine(const char* text, const char* taint);
    void BackwardHistory();
    void ClearHistory();
    int32_t ConvertCoordinateToIndex(float x, float y, int32_t& index);
    void CopyToClipboard();
    void AnchorCandidatesFrame();
    void CreateCandidatesFrame();
    void CreateClauseHighlight();
    void Delete(int32_t count, int32_t script);
    void DeleteBackward(int32_t script);
    void DeleteBackwardWord(int32_t script);
    void DeleteForward(int32_t script);
    void DeleteForwardWord(int32_t script);
    void DeleteHighlight(int32_t script);
    void DeleteSubstring(int32_t left, int32_t right, int32_t script);
    void DeleteToEnd(int32_t script);
    void DeleteToStart(int32_t script);
    void DisableIME();
    void ExtendHighlight(int32_t count);
    void ForwardHistory();
    const char* GetInputLanguageString();
    int32_t GetLenToNum(int32_t offset, int32_t length);
    int32_t GetNumToLen(int32_t offset, int32_t count, bool skipHyperlinks);
    void GetEditTextInsets(float& left, float& right, float& top, float& bottom);
    void GrowText(int32_t length);
    void HighlightText(int32_t left, int32_t right);
    void Insert(const char* text, const char* taint, int32_t runScripts, int32_t highlight, int32_t script);
    void Insert(uint32_t ch);
    void MakeTextVisible(int32_t position, float offset, float width);
    void Move(int32_t count, int32_t highlight);
    void MoveBackward(int32_t highlight);
    void MoveBackwardWord(int32_t highlight);
    void MoveForward(int32_t highlight);
    void MoveForwardWord(int32_t highlight);
    void MoveLine(int32_t count, int32_t highlight);
    void MoveToEnd(int32_t highlight);
    void MoveToLineEnd(int32_t highlight);
    void MoveToLineStart(int32_t highlight);
    void MoveToStart(int32_t highlight);
    void OnFocusChange(bool focus, bool runScript);
    void PasteFromClipboard();
    int32_t PopulateCandidates(uint32_t listMask);
    void RunOnCharCompositionScript(const char* text);
    void RunOnCursorChangedScript(float x, float y, float width, float height);
    void RunOnEnterPressedScript();
    void RunOnEscapePressedScript();
    void RunOnSpacePressedScript();
    void RunOnTabPressedScript();
    void RunOnTextChangedScript();
    void RunOnTextSetScript(const char* taint);
    void SetAltArrowKeyMode(int32_t enable);
    void SetAutoFocus(int32_t enable);
    void SetCountInvisibleLetters(int32_t enable);
    void SetCursorPosition(int32_t position);
    void SetEditTextInsets(float left, float right, float top, float bottom);
    void SetHighlightColor(const CImVector& color);
    void SetHistoryLines(int32_t lines);
    void SetMultiLine(int32_t enable);
    void SetNumeric(int32_t enable);
    void SetPassword(int32_t enable);
    void SetText(const char* text, const char* taint);
    void ToggleInputLanguage();
    void UpdateClauseInfo();
    void UpdateDirtyBits();
    void UpdateHighlightArea(CSimpleRegion* region, int32_t left, int32_t right);
    void UpdateLanguageIndicator();
    void UpdateSizes();
    void UpdateTextInfo();
    void UpdateVisibleCursor();
    void UpdateVisibleHighlight();
    void UpdateVisibleText();
};

#include "Engine/Source/Frame/CSimpleEditBox.inl"

#endif
