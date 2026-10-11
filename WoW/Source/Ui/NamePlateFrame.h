#ifndef WOW_SOURCE_UI_NAMEPLATEFRAME_H
#define WOW_SOURCE_UI_NAMEPLATEFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Source/Object/WGUID.h"

class CGSimpleHealthBar;
class CGUnit_C;
class CSimpleFontString;
class CSimpleStatusBar;
class CSimpleTexture;
class CVar;
class linkObject;

class CGNamePlateFrame : public TSLinkedNode<CGNamePlateFrame>, public CSimpleFrame {
    public:
    static CDataAllocator s_namePlateHeap;
    static TSList<CGNamePlateFrame> s_namePlates;
    static TSList<CGNamePlateFrame> s_unusedNamePlates;
    static CVar* s_cvNamePlateMotion;

    static void operator delete(void* ptr);
    static CGNamePlateFrame* GetNamePlateFocus();
    static CGNamePlateFrame* GetNewNameplateFrame(CSimpleFrame* parent);
    static void ReleaseNameplateFrame(CGNamePlateFrame* frame);
    static void DestroyUnusedNameplateFrames();
    static void NamePlateSort(CGNamePlateFrame* frame);
    static void RegisterCVars();
    static bool IsBloatThreat();
    static int32_t GetNamePlateMotion();
    static float GetBloatNamePlates();

    /* 01F8 */ CRect m_overlapRect;
    /* 0208 */ C3Vector m_offset;
    /* 0214 */ int32_t m_bloatTime;
    /* 0218 */ float m_sortY;
    /* 021C */ float m_targetOffsetY;
    /* 0220 */ CGNamePlateFrame* m_below;
    /* 0224 */ int32_t m_stackDepth;
    /* 0228 */ float m_bloat;
    /* 022C */ float m_halfHeight;
    /* 0230 */ float m_halfWidth;
    /* 0238 */ WOWGUID m_guid;
    /* 0240 */ CSimpleTexture* m_namePlateGlow;
    /* 0244 */ CSimpleTexture* m_targetingFrameSkull;
    /* 0248 */ CSimpleTexture* m_raidTargetingIcon;
    /* 024C */ CSimpleTexture* m_castBarBorder;
    /* 0250 */ CSimpleTexture* m_castBarShield;
    /* 0254 */ CSimpleTexture* m_targetingFrameFlash;
    /* 0258 */ CSimpleTexture* m_eliteNameplateIcon;
    /* 025C */ CSimpleTexture* m_castBarIcon;
    /* 0260 */ CSimpleFontString* m_unitNameText;
    /* 0264 */ CSimpleFontString* m_levelText;
    /* 0268 */ CGSimpleHealthBar* m_healthBar;
    /* 026C */ CSimpleStatusBar* m_castBar;
    /* 0270 */ C3Vector m_position;
    /* 027C */ CImVector m_healthBarColor;
    /* 0280 */ int32_t m_damageFeedbackTime;
    /* 0284 */ float m_castHoldTime;
    /* 0288 */ float m_castFadeTime;
    /* 028C */ CRect m_screenRect;
    /* 029C */ uint32_t m_casting : 1;
    /* 029C */ uint32_t m_channeling : 1;
    /* 029C */ uint32_t m_positionChanged : 1;
    /* 029C */ uint32_t m_screenRectChanged : 1;
    /* 029C */ uint32_t m_notInterruptible : 1;
    /* 02A0 */ uint8_t m_scripted;

    ~CGNamePlateFrame();
    void OnLayerUpdate(float elapsedSec) override;
    virtual void SetFrameStrata(FRAME_STRATA strata);

    CGNamePlateFrame(CSimpleFrame* parent);
    void HideCastBar();
    void Initialize(CGUnit_C* unit);
    void LoadScriptedTemplate(linkObject* node);
    void OnGainFocus();
    void OnLoseFocus();
    bool SetBloat(float bloat, bool force);
    void SetCastBarValues(float minValue, float maxValue, float value, int32_t spellId, bool channel, bool notInterruptible);
    void SetPosition(const C3Vector& position);
    void SetScreenCoordsAndRectangle(C3Vector position);
    void SetScreenRect(const CRect& rect);
    void TriggerDamageFeedback();
    void UpdateCastBarSize();
    void UpdateColorDisplay(CGUnit_C* unit);
    void UpdateEliteIcon(CGUnit_C* unit);
    void UpdateLevelDisplay(CGUnit_C* unit);
    void UpdateNameDisplay(const CGUnit_C* unit);
    void UpdateRaidTargetDisplay(uint32_t raidTarget);
};

#endif
