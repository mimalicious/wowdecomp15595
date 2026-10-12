#include "NamePlateFrame.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <new>
#include <typeinfo>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/Frame/CSimpleStatusBar.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRect.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/HealthBar.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/WorldFrame.h"

// STATIC: 0x00F7636C/0x140E6C3C8
static CVar* s_cvNamePlateStyle;

// STATIC: 0x00F76368/0x140E6C3C0
static CVar* s_cvBloatTest;

// STATIC: 0x00F76364/0x140E6C3B8
static CVar* s_cvBloatNamePlates;

// STATIC: 0x00F76360/0x140E6C3B0
static CVar* s_cvBloatThreat;

// STATIC: 0x00F7635C/0x140E6C3A8
CVar* CGNamePlateFrame::s_cvNamePlateMotion;

// STATIC: 0x00F76358/0x140E6C3A0
static CVar* s_cvMaxAlgoPlates;

// STATIC: 0x00F76354/0x140E6C398
static CVar* s_cvRepositionFrequency;

// STATIC: 0x00F76350/0x140E6C390
static CVar* s_cvNamePlateOverlapH;

// STATIC: 0x00F7634C/0x140E6C388
static CVar* s_cvNamePlateOverlapV;

// STATIC: 0x00F76370/0x140E6C3D0
CGNamePlateFrame* CGUnit_C::s_namePlateFocus;

// STATIC: 0x00F76374/0x140E6C3D8
uint32_t CGUnit_C::s_namePlateFlags;

// STATIC: 0x00F76378/0x140E6C3DC
static int32_t s_namePlateCount;

// STATIC: 0x00F7637C/0x140E6C3E0
static uint32_t s_lastRepositionTime;

// STATIC: 0x00F76380/0x140E6C3E4
static uint32_t s_repositionCounter;

// STATIC: 0x00F76384/0x140E6C3E8
CDataAllocator CGNamePlateFrame::s_namePlateHeap(sizeof(CGNamePlateFrame), 5);

// STATIC: 0x00F76398/0x140E6C408
static TSGrowableArray<CGNamePlateFrame*> s_sortedPlates;

// STATIC: 0x00C0A270/0x140A03400
static const int32_t s_greenLevelRanges[20] = { 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

// STATIC: 0x00D16AA8/0x140BD10D0
static float s_namePlateWidth = 0.09f;

// STATIC: 0x00D16AAC/0x140BD10D4
static float s_namePlateHeight = 0.025f;

// STATIC: 0x00D16ACC/0x140BD110C
static CImVector s_friendlyPlayerColor = CImVector(0xFF, 0x00, 0x00, 0xFF);

// STATIC: 0x00D16AD0/0x140BD1110
static CImVector s_friendlyColor = CImVector(0x00, 0xFF, 0x00, 0xFF);

// STATIC: 0x00D16AD4/0x140BD1114
static CImVector s_neutralColor = CImVector(0x00, 0xFF, 0xFF, 0xFF);

// STATIC: 0x00D16AD8/0x140BD1118
static CImVector s_hostileColor = CImVector(0x00, 0x00, 0xFF, 0xFF);

// STATIC: 0x00D16ADC/0x140BD111C
static CImVector s_levelTextColor = CImVector(0xFF, 0xFF, 0xFF, 0xFF);

// STATIC: 0x00D16AE0/0x140BD1120
static CImVector s_classColors[12] = {
    CImVector(0x00, 0x00, 0x00, 0xFF),
    CImVector(0x6D, 0x9B, 0xC6, 0xFF),
    CImVector(0xBA, 0x8C, 0xF4, 0xFF),
    CImVector(0x72, 0xD3, 0xAA, 0xFF),
    CImVector(0x68, 0xF4, 0xFF, 0xFF),
    CImVector(0xFF, 0xFF, 0xFF, 0xFF),
    CImVector(0x3A, 0x1E, 0xC4, 0xFF),
    CImVector(0xDD, 0x70, 0x00, 0xFF),
    CImVector(0xEF, 0xCC, 0x68, 0xFF),
    CImVector(0xC9, 0x82, 0x93, 0xFF),
    CImVector(0x00, 0x00, 0x00, 0xFF),
    CImVector(0x0A, 0x7C, 0xFF, 0xFF),
};

// STATIC: 0x00D16B10/0x140BD1160
TSList<CGNamePlateFrame> CGNamePlateFrame::s_namePlates;

// STATIC: 0x00D16B1C/0x140BD1168
TSList<CGNamePlateFrame> CGNamePlateFrame::s_unusedNamePlates;

// INLINED
void CGNamePlateFrame::operator delete(void* ptr) {
    CGNamePlateFrame::s_namePlateHeap.PutData(ptr, NULL, 0);
}

// OFFSET: 0x009E70D0/0x14077F990
static float NamePlateScreenCenterDistanceSquared(C3Vector position) {
    static C3Vector s_screenCenter(0.4f, 0.3f, 0.0f);

    C3Vector delta(position.x - s_screenCenter.x, position.y - s_screenCenter.y, position.z - s_screenCenter.z);
    C2Vector delta2(delta);

    return delta2.x * delta2.x + delta2.y * delta2.y;
}

// PROVEN: f4dfa202 IDENTICAL/IDENTICAL
// OFFSET: 0x009E7180/0x14077FA50
void CGNamePlateFrame::RegisterCVars() {
    s_cvNamePlateStyle = CVar::Register("NamePlateStlye", "Template used to create nameplates", 257, "", NULL, 1, false, NULL, false);
    s_cvBloatTest = CVar::Register("bloatTest", "makes nameplates larger 1.0 + x", 1, "0.0", NULL, 1, false, NULL, false);
    s_cvBloatNamePlates = CVar::Register("bloatnameplates", "makes nameplates larger depending on threat %", 1, "0.0", NULL, 1, false, NULL, false);
    s_cvBloatThreat = CVar::Register("bloatthreat", "makes nameplates larger depending on gaining/losing threat", 1, "1", NULL, 1, false, NULL, false);
    CGNamePlateFrame::s_cvNamePlateMotion = CVar::Register("nameplateMotion", "Defines the movement/collison model for nameplates", 17, "1", NULL, 1, false, NULL, false);
    s_cvMaxAlgoPlates = CVar::Register("maxAlgoplates", "max number nameplates on screen before overlapping nameplates are used", 17, "25", NULL, 1, false, NULL, false);
    s_cvRepositionFrequency = CVar::Register("repositionfrequency", "how many frames to skip per nameplate reposition", 1, "1", NULL, 1, false, NULL, false);
    s_cvNamePlateOverlapH = CVar::Register("nameplateOverlapH", "Percentage amount for horizontal overlap of nameplates", 17, "0.8", NULL, 1, false, NULL, false);
    s_cvNamePlateOverlapV = CVar::Register("nameplateOverlapV", "Percentage amount for vertical overlap of nameplates", 17, "0.8", NULL, 1, false, NULL, false);
}

// PROVEN: c903ef18 IDENTICAL/IDENTICAL
// OFFSET: 0x009E72E0/0x14077FCA0
CGNamePlateFrame* CGNamePlateFrame::GetNamePlateFocus() {
    return CGUnit_C::s_namePlateFocus;
}

// PROVEN: f184d2ce IDENTICAL/IDENTICAL
// OFFSET: 0x009E72F0/0x14077FCB0
void CGUnit_C::NamePlateSetResortAll(bool enable) {
    if (enable) {
        s_namePlateFlags |= 0x1;
    } else {
        s_namePlateFlags &= ~0x1;
    }
}

// PROVEN: 453941c5 IDENTICAL/IDENTICAL
// OFFSET: 0x009E7310/0x14077FCD0
void CGUnit_C::NamePlateUpdateLevels(bool enable) {
    if (enable) {
        s_namePlateFlags |= 0x2;
    } else {
        s_namePlateFlags &= ~0x2;
    }
}

// PROVEN: 8c91925f IDENTICAL/IDENTICAL
// OFFSET: 0x009E7330/0x14077FCF0
void CGUnit_C::NamePlateUpdateColor(bool enable) {
    if (enable) {
        s_namePlateFlags |= 0x4;
    } else {
        s_namePlateFlags &= ~0x4;
    }
}

// PROVEN: 131c46b7 IDENTICAL/IDENTICAL
// OFFSET: 0x009E7350/0x14077FD10
uint32_t CGUnit_C::GetNamePlateFlags() {
    return s_namePlateFlags;
}

// PROVEN: c23b60cd IDENTICAL/IDENTICAL
// OFFSET: 0x009E7360/0x14077FD20
void CGNamePlateFrame::UpdateNameDisplay(const CGUnit_C* unit) {
    if (this->m_unitNameText) {
        this->m_unitNameText->SetText(const_cast<CGUnit_C*>(unit)->GetUnitName(NULL, 1), 1);
    }
}

// PROVEN: df659d44 IDENTICAL/IDENTICAL
// OFFSET: 0x009E7390/0x14077FD60
void CGNamePlateFrame::UpdateCastBarSize() {
    if (this->m_scripted) {
        return;
    }

    float barOffsetY = this->m_notInterruptible ? -0.003125f : 0.0035f;
    this->m_castBar->SetPoint(FRAMEPOINT_BOTTOMRIGHT, this->m_castBarBorder, FRAMEPOINT_BOTTOMRIGHT, this->GetWidth() / s_namePlateWidth * -0.003125f, this->GetHeight() * (barOffsetY / s_namePlateHeight), 1);

    float iconOffsetY = this->m_notInterruptible ? 0.0f : this->GetHeight() / s_namePlateHeight * 0.0066f;
    this->m_castBarIcon->SetPoint(FRAMEPOINT_CENTER, this->m_castBarBorder, FRAMEPOINT_BOTTOMLEFT, this->GetWidth() / s_namePlateWidth * 0.0092f, iconOffsetY, 1);
}

// OFFSET: 0x009E74A0/0x14077FEF0
static int32_t UpdateStackDepth(CGNamePlateFrame* frame, int32_t depth) {
    if (!frame) {
        return 0;
    }

    int32_t result = depth + 1;

    if (frame->m_below) {
        result = UpdateStackDepth(frame->m_below, depth + 1);
    }

    frame->m_stackDepth = frame->m_stackDepth <= result ? result : frame->m_stackDepth;
    return result;
}

// PROVEN: d15a7fc9 IDENTICAL/NONE
// OFFSET: 0x009E74E0
void CGNamePlateFrame::SetFrameStrata(FRAME_STRATA strata) {
    CSimpleFrame::SetFrameStrata(strata);
}

// PROVEN: 357851f8 DIFFERENT/IDENTICAL
// OFFSET: 0x009E7610/0x1407800F0
static void SetNumberText(CSimpleFontString* string, int32_t value) {
    char text[32];
    SStrPrintf(text, sizeof(text), "%d", value);
    string->SetText(text, 1);
}

// PROVEN: a7ab68ac IDENTICAL/NONE
// OFFSET: 0x009E7650
void CGNamePlateFrame::SetScreenRect(const CRect& rect) {
    if (this->m_screenRect.minY != rect.minY || this->m_screenRect.minX != rect.minX || this->m_screenRect.maxY != rect.maxY || this->m_screenRect.maxX != rect.maxX) {
        this->m_screenRect = rect;
        this->m_screenRectChanged = 1;
    }
}

// PROVEN: 335b5a8b IDENTICAL/IDENTICAL
// OFFSET: 0x009E76D0/0x140780130
bool CGNamePlateFrame::IsBloatThreat() {
    return s_cvBloatThreat->m_intValue > 0;
}

// PROVEN: dc24b233 IDENTICAL/IDENTICAL
// OFFSET: 0x009E76E0/0x140780140
int32_t CGNamePlateFrame::GetNamePlateMotion() {
    return CGNamePlateFrame::s_cvNamePlateMotion->m_intValue;
}

// PROVEN: b1837e23 IDENTICAL/IDENTICAL
// OFFSET: 0x009E76F0/0x140780150
float CGNamePlateFrame::GetBloatNamePlates() {
    return s_cvBloatNamePlates->m_floatValue;
}

// OFFSET: 0x009E7700/0x140780160
bool CGNamePlateFrame::SetBloat(float bloat, bool force) {
    if (fabs(bloat) < 0.001f) {
        this->m_bloatTime = 0;
    }

    if (this->m_scripted) {
        float scale = sqrt(bloat + 1.0f);
        this->m_bloat = bloat;
        this->SetWidth(scale * s_namePlateWidth);
        this->SetHeight(s_namePlateHeight * scale);
        return false;
    }

    if (fabs(bloat - this->m_bloat) < 0.001f && !force) {
        return false;
    }

    float scale = sqrt(bloat + 1.0f);
    this->m_bloat = bloat;
    this->SetWidth(scale * s_namePlateWidth);
    this->SetHeight(s_namePlateHeight * scale);

    this->m_castBarBorder->SetWidth(this->GetWidth());
    this->m_castBarBorder->SetHeight(this->GetHeight());
    this->m_castBarShield->SetWidth(this->GetWidth());
    this->m_castBarShield->SetHeight(this->GetHeight());

    this->m_healthBar->SetWidth(this->GetWidth() * 0.804f);
    this->m_healthBar->SetHeight(this->GetHeight() * 0.281f);
    this->m_healthBar->SetPoint(FRAMEPOINT_BOTTOMLEFT, this, FRAMEPOINT_BOTTOMLEFT, this->GetWidth() * 0.031f, this->GetHeight() * 0.125f, 1);
    this->m_levelText->SetPoint(FRAMEPOINT_CENTER, this, FRAMEPOINT_BOTTOMRIGHT, this->GetWidth() * -0.08949998f, this->GetHeight() * 0.2655f + s_namePlateHeight * 0.019f, 1);

    this->m_castBar->SetWidth(this->GetWidth() * 0.804f);
    this->m_castBar->SetHeight(this->GetHeight() * 0.281f);
    this->m_castBarIcon->SetPoint(FRAMEPOINT_CENTER, this->m_castBarBorder, FRAMEPOINT_BOTTOMLEFT, this->GetWidth() * 0.0585f, this->GetHeight() * 0.284f, 1);
    this->m_castBarBorder->SetPoint(FRAMEPOINT_CENTER, this, FRAMEPOINT_CENTER, 0.0f, -(this->m_castBarBorder->GetHeight() * 0.5f), 1);
    this->m_castBarShield->SetPoint(FRAMEPOINT_CENTER, this->m_castBarBorder, FRAMEPOINT_CENTER, 0.0f, -(this->m_castBarShield->GetHeight() * 0.5f), 1);
    this->m_castBar->SetPoint(FRAMEPOINT_BOTTOMRIGHT, this->m_castBarBorder, FRAMEPOINT_BOTTOMRIGHT, this->GetWidth() * 0.0585f, this->GetHeight() * 0.284f, 1);

    this->m_targetingFrameFlash->SetWidth(this->GetWidth() * 1.1f);
    this->m_targetingFrameFlash->SetHeight(this->GetHeight() * 1.1f);
    this->m_targetingFrameFlash->SetPoint(FRAMEPOINT_BOTTOMLEFT, this, FRAMEPOINT_BOTTOMLEFT, this->GetWidth() * -0.06f, this->GetHeight() * -0.37f, 1);

    this->m_eliteNameplateIcon->SetWidth(this->GetWidth() / s_namePlateWidth * 0.0294f);
    this->m_eliteNameplateIcon->SetHeight(this->GetHeight() / s_namePlateHeight * 0.0215f);
    this->m_eliteNameplateIcon->SetPoint(FRAMEPOINT_CENTER, this->m_levelText, FRAMEPOINT_CENTER, 0.003f, -0.001f, 1);

    this->UpdateCastBarSize();
    return true;
}

// PROVEN: 87c23b41 IDENTICAL/IDENTICAL
// OFFSET: 0x009E7C00/0x1407807E0
void CGNamePlateFrame::TriggerDamageFeedback() {
    CImVector color(0, 0, 0xFF, 0xFF);
    this->m_damageFeedbackTime = 5000;

    if (this->m_unitNameText) {
        this->m_unitNameText->SetVertexColor(color);
    }
}

// PROVEN: 930fe4f4 IDENTICAL/DIFFERENT
// OFFSET: 0x009E7C40/0x140780820
static const CImVector* GetDifficultyColor(int32_t level, int32_t playerLevel) {
    static CImVector s_difficultyColors[5] = {
        CImVector(0x19, 0x19, 0xFF, 0xFF),
        CImVector(0x3F, 0x7F, 0xFF, 0xFF),
        CImVector(0x00, 0xFF, 0xFF, 0xFF),
        CImVector(0x3F, 0xB2, 0x3F, 0xFF),
        CImVector(0x7F, 0x7F, 0x7F, 0xFF),
    };

    int32_t diff = level - playerLevel;
    uint32_t bracket = playerLevel / 5;
    int32_t greenRange = bracket < 20 ? s_greenLevelRanges[bracket] : 8;

    if (diff >= 5) {
        return &s_difficultyColors[0];
    }

    if (diff >= 3) {
        return &s_difficultyColors[1];
    }

    if (diff >= -2) {
        return &s_difficultyColors[2];
    }

    if (-diff <= greenRange) {
        return &s_difficultyColors[3];
    }

    return &s_difficultyColors[4];
}

// PROVEN: ba57a66e IDENTICAL/IDENTICAL
// OFFSET: 0x009E7D30/0x140780940
void CGNamePlateFrame::UpdateEliteIcon(CGUnit_C* unit) {
    if (!this->m_eliteNameplateIcon) {
        return;
    }

    uint32_t classification = unit->GetCreatureClassification();

    if (unit->IsBossMob() || classification == 2 || classification == 1) {
        this->m_eliteNameplateIcon->Show();
    } else {
        this->m_eliteNameplateIcon->Hide();
    }
}

// PROVEN: 55034d75 IDENTICAL/IDENTICAL
// OFFSET: 0x009E7DA0/0x1407809E0
void CGNamePlateFrame::UpdateRaidTargetDisplay(uint32_t raidTarget) {
    if (!this->m_raidTargetingIcon) {
        return;
    }

    if (raidTarget >= 28) {
        this->m_raidTargetingIcon->Hide();
        return;
    }

    float left = (raidTarget & 3) * 0.25f;
    float top = (raidTarget >> 2) * 0.25f;

    CRect texCoords;
    texCoords.minX = left;
    texCoords.maxX = left + 0.25f;
    texCoords.minY = top;
    texCoords.maxY = top + 0.25f;
    this->m_raidTargetingIcon->SetTexCoord(texCoords);
    this->m_raidTargetingIcon->Show();
}

// OFFSET: 0x009E7E50/0x140780AA0
void CGNamePlateFrame::HideCastBar() {
    if (!this->m_castBar->m_shown) {
        return;
    }

    this->m_castBar->Hide();
    this->m_casting = 0;
    this->m_castHoldTime = 0.0f;
    this->m_castFadeTime = 0.0f;
}

// PROVEN: 05b5c263 DIFFERENT/IDENTICAL
// OFFSET: 0x009E7E90/0x140780AE0
void CGNamePlateFrame::SetScreenCoordsAndRectangle(C3Vector position) {
    CSimpleFrame* parent = this->m_parent;
    this->SetPoint(FRAMEPOINT_BOTTOM, parent, FRAMEPOINT_BOTTOMLEFT, position.x, position.y, 1);
    this->SetFrameDepth(position.z - parent->m_layoutDepth, true);

    CRect rect;
    rect.minY = position.y - this->m_halfHeight;
    rect.minX = position.x - this->m_halfWidth;
    rect.maxY = this->m_halfHeight + position.y;
    rect.maxX = this->m_halfWidth + position.x;
    this->SetScreenRect(rect);
}

// OFFSET: 0x009E7F40/0x140780C00
static int32_t __cdecl CompareNamePlates(const void* a, const void* b) {
    CGNamePlateFrame* frameA = *static_cast<CGNamePlateFrame* const*>(a);
    CGNamePlateFrame* frameB = *static_cast<CGNamePlateFrame* const*>(b);
    CGUnit_C* unitA = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(frameA->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    CGUnit_C* unitB = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(frameB->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    bool playerA = (unitA->m_obj->m_type & TYPEMASK_PLAYER) != 0;
    bool playerB = (unitB->m_obj->m_type & TYPEMASK_PLAYER) != 0;

    if (playerA != playerB) {
        return playerA ? -1 : 1;
    }

    uint32_t classificationA = unitA->GetCreatureClassification();
    uint32_t classificationB = unitB->GetCreatureClassification();
    bool eliteA = unitA->IsBossMob() || classificationA == 2 || classificationA == 1;
    bool eliteB = unitB->IsBossMob() || classificationB == 2 || classificationB == 1;

    if (eliteA != eliteB) {
        return eliteA ? -1 : 1;
    }

    float yA = frameA->m_sortY > 0.0f ? frameA->m_sortY : frameA->m_position.y;
    float yB = frameB->m_sortY <= 0.0f ? frameB->m_position.y : frameB->m_sortY;

    return yB <= yA ? 1 : -1;
}

// PROVEN: 62c7e379 IDENTICAL/DIFFERENT
// OFFSET: 0x009E8180/0x140780E50
static int32_t GetAdjustedLevel(CGPlayer_C* player, CGUnit_C* unit) {
    int32_t level = static_cast<int32_t>(unit->m_unit->m_unitFieldLevel);

    if (level > 0 && player && (player->m_player->m_playerBytes3[1] || player->m_player->m_playerFakeInebriation) && player->CanAttack(unit)) {
        level -= RoundToInt64(player->GetDrunknessTurnValue() * 5.0f);

        if (level < 1) {
            return 1;
        }
    }

    return level;
}

// PROVEN: cd7978df IDENTICAL/DIFFERENT
// OFFSET: 0x009E81F0/0x140780ED0
void CGNamePlateFrame::OnGainFocus() {
    if (this->m_namePlateGlow) {
        this->m_namePlateGlow->Show();
    }

    if (this->m_damageFeedbackTime <= 0 && this->m_unitNameText) {
        CImVector color(0xFFFFFF00);
        this->m_unitNameText->SetVertexColor(color);
    }

    this->EnableDrawLayer(4);
}

// PROVEN: c182cdcf IDENTICAL/DIFFERENT
// OFFSET: 0x009E8250/0x140780F40
void CGNamePlateFrame::OnLoseFocus() {
    if (this->m_namePlateGlow) {
        this->m_namePlateGlow->Hide();
    }

    if (this->m_damageFeedbackTime <= 0 && this->m_unitNameText) {
        CImVector color(0xFFFFFFFF);
        this->m_unitNameText->SetVertexColor(color);
    }

    this->DisableDrawLayer(4);
}

// OFFSET: 0x009E82B0/0x140780FB0
void CGNamePlateFrame::OnLayerUpdate(float elapsedSec) {
    if (CGNamePlateFrame::s_cvNamePlateMotion->m_intValue == 1) {
        float delta = this->m_targetOffsetY - this->m_offset.y;
        float distance = fabs(delta);

        if (distance > 0.00001f) {
            float step = elapsedSec * 0.05f;

            if (distance <= step) {
                step = delta;
            } else if (delta <= 0.0f) {
                step = -step;
            }

            C3Vector position = this->m_position;
            this->m_offset.y += step;
            this->m_sortY = this->m_offset.y + position.y;
            this->SetScreenCoordsAndRectangle(C3Vector(this->m_offset.x + position.x, this->m_offset.y + position.y, this->m_offset.z + position.z));
        }
    }

    if (this->m_guid == CGGameUI::m_lockedTarget) {
        this->SetFrameLevel(20, 1);
    } else {
        this->SetFrameLevel(10, 1);
    }

    int32_t feedbackTime = this->m_damageFeedbackTime;

    if (feedbackTime >= 0) {
        this->m_damageFeedbackTime = feedbackTime - static_cast<int32_t>(elapsedSec * 1000.0f);

        if (this->m_damageFeedbackTime <= 0 && this->m_unitNameText) {
            if (this->m_namePlateGlow && this->m_namePlateGlow->m_visible) {
                this->m_unitNameText->SetVertexColorRGBA(1.0f, 1.0f, 0.0f, 1.0f);
            } else {
                this->m_unitNameText->SetVertexColorRGBA(1.0f, 1.0f, 1.0f, 1.0f);
            }
        }
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && (!CGGameUI::m_lockedTarget || CGGameUI::m_lockedTarget == unit->m_obj->m_guid)) {
        this->SetFrameAlpha(0xFF);
    } else {
        this->SetFrameAlpha(0x7F);
    }

    CSimpleFrame::OnLayerUpdate(elapsedSec);

    uint8_t threatStatus = 1;

    if (unit && CGGameUI::m_threatWarningEnabled) {
        WOWGUID player = ClntObjMgrGetActivePlayer();
        unit->CalculateThreat(player, &threatStatus, NULL, NULL, NULL);
    }

    if (this->m_targetingFrameFlash) {
        if (threatStatus > 1) {
            this->m_targetingFrameFlash->SetVertexColor(*reinterpret_cast<const CImVector*>(CGTooltip::GetThreatStateColor(threatStatus)));
            this->m_targetingFrameFlash->Show();
        } else {
            this->m_targetingFrameFlash->Hide();
        }
    }

    if (!g_showVKeyCastbarCVar->m_intValue) {
        this->HideCastBar();
        return;
    }

    CImVector color(0x00000000);

    if (this->m_castHoldTime > 0.0f) {
        this->m_castHoldTime -= elapsedSec;

        if (this->m_castHoldTime <= 0.0f) {
            this->m_castHoldTime = 0.0f;
        }

        return;
    }

    if (this->m_castFadeTime > 0.0f) {
        this->m_castFadeTime -= elapsedSec;

        if (this->m_castFadeTime <= 0.0f) {
            this->HideCastBar();
            return;
        }

        if (this->m_castBarBorder) {
            this->m_castBarBorder->SetVertexAlpha(static_cast<uint8_t>(this->m_castBarBorder->GetVertexAlpha() * this->m_castFadeTime));
        }

        if (this->m_castBarShield) {
            this->m_castBarShield->SetVertexAlpha(static_cast<uint8_t>(this->m_castBarBorder->GetVertexAlpha() * this->m_castFadeTime));
        }

        if (this->m_castBarIcon) {
            this->m_castBarIcon->SetVertexAlpha(static_cast<uint8_t>(this->m_castBarIcon->GetVertexAlpha() * this->m_castFadeTime));
        }

        this->m_castBar->GetStatusBarColor(color);
        color.a = static_cast<uint8_t>(color.a * this->m_castFadeTime);
        this->m_castBar->SetStatusBarColor(color);
        return;
    }

    if (!this->m_casting) {
        this->HideCastBar();
        return;
    }

    CSimpleStatusBar* castBar = this->m_castBar;
    float value = castBar->m_value;

    if (this->m_channeling) {
        value -= elapsedSec;

        if (value < castBar->m_minValue) {
            value = castBar->m_minValue;
            this->m_castHoldTime = 0.0f;
            this->m_castFadeTime = 1.0f;
            color.Set(1.0f, 0.0f, 1.0f, 0.0f);
            castBar->SetStatusBarColor(color);
            this->m_casting = 0;
        }
    } else {
        value += elapsedSec;

        if (value > castBar->m_maxValue) {
            value = castBar->m_maxValue;
            this->m_castHoldTime = 0.0f;
            this->m_castFadeTime = 1.0f;
            color.Set(1.0f, 0.0f, 1.0f, 0.0f);
            castBar->SetStatusBarColor(color);
            this->m_casting = 0;
        }
    }

    this->m_castBar->SetValue(value);
}

// OFFSET: 0x009E87F0/0x1407815C0
void CGNamePlateFrame::UpdateColorDisplay(CGUnit_C* unit) {
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (g_ShowClassColorInNameplateCVar->m_intValue && (unit->m_obj->m_type & TYPEMASK_PLAYER) && unit->UnitReaction(player) <= 1) {
        this->m_healthBarColor = s_classColors[unit->m_unit->m_unitFieldBytes0[1]];
    } else if (unit->UnitReaction(player) <= 1) {
        this->m_healthBarColor = s_hostileColor;
    } else if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
        this->m_healthBarColor = s_friendlyPlayerColor;
    } else if (player && unit->IsFriendly(player)) {
        this->m_healthBarColor = s_friendlyColor;
    } else {
        this->m_healthBarColor = s_neutralColor;
    }

    this->m_healthBar->SetStatusBarColor(this->m_healthBarColor);
}

// OFFSET: 0x009E88D0/0x1407816C0
void CGNamePlateFrame::UpdateLevelDisplay(CGUnit_C* unit) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    int32_t level = GetAdjustedLevel(player, unit);
    bool showLevel;

    if (unit->IsBossMob()) {
        showLevel = false;
    } else {
        showLevel = (unit->UnitReaction(player) <= 1 && CGPlayer_C::UnitIsTrivial(unit)) || unit->UnitReaction(player) > 1 || static_cast<int32_t>(player->m_unit->m_unitFieldLevel) > level - 10;
    }

    if (this->m_levelText) {
        if (!this->m_targetingFrameSkull) {
            this->m_levelText->Show();
            this->m_levelText->SetVertexColor(*GetDifficultyColor(level, player->m_unit->m_unitFieldLevel));
            SetNumberText(this->m_levelText, level);
            this->m_targetingFrameSkull->Hide();
            return;
        }

        if (showLevel) {
            this->m_levelText->Show();
            this->m_levelText->SetVertexColor(*GetDifficultyColor(level, player->m_unit->m_unitFieldLevel));
            SetNumberText(this->m_levelText, level);
            this->m_targetingFrameSkull->Hide();
            return;
        }

        this->m_levelText->Hide();
        this->m_targetingFrameSkull->Show();
    } else if (this->m_targetingFrameSkull && !showLevel) {
        this->m_targetingFrameSkull->Show();
    }
}

// OFFSET: 0x009E8A30/0x1407818F0
void CGNamePlateFrame::SetCastBarValues(float minValue, float maxValue, float value, int32_t spellId, bool channel, bool notInterruptible) {
    if (maxValue < value) {
        return;
    }

    this->m_castBar->SetMinMaxValues(minValue, maxValue);
    this->m_castBar->SetValue(value);
    this->m_channeling = channel;

    SpellRec* spellRec = g_spellDB.GetRecord(spellId);

    if (!spellRec || (spellRec->m_attributes & 0x20)) {
        this->HideCastBar();
        return;
    }

    CImVector color(0x00000000);

    if (channel ? minValue != value : maxValue != value) {
        color.Set(1.0f, 1.0f, 0.7f, 0.0f);
        this->m_castBar->SetStatusBarColor(color);
        this->m_casting = 1;
        this->m_castHoldTime = 0.0f;
        this->m_castFadeTime = 0.0f;
    } else {
        this->m_castHoldTime = 0.0f;
        this->m_castFadeTime = 1.0f;
        color.Set(1.0f, 1.0f, 0.0f, 0.0f);
        this->m_castBar->SetStatusBarColor(color);
        this->m_casting = 0;
    }

    color = CImVector(0xFFFFFFFF);

    if (this->m_castBarIcon) {
        this->m_castBarIcon->SetVertexColor(color);
        SpellIconRec* iconRec = g_spellIconDB.GetRecord(spellRec->m_spellIconID);

        if (iconRec) {
            this->m_castBarIcon->SetTexture(iconRec->m_textureFilename, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        }
    }

    this->m_notInterruptible = notInterruptible;
    CSimpleTexture* hidden;

    if (this->m_notInterruptible) {
        if (this->m_castBarShield) {
            this->m_castBarShield->SetVertexColor(color);
            this->m_castBarShield->Show();
        }

        hidden = this->m_castBarBorder;
    } else {
        if (this->m_castBarBorder) {
            this->m_castBarBorder->SetVertexColor(color);
            this->m_castBarBorder->Show();
        }

        hidden = this->m_castBarShield;
    }

    if (hidden) {
        hidden->Hide();
    }

    this->UpdateCastBarSize();
    this->m_castBar->Show();

    if (this->m_castBarIcon) {
        this->m_castBarIcon->Show();
    }
}

// PROVEN: 66d4e6e4 IDENTICAL/IDENTICAL
// OFFSET: 0x009E8E80/0x140781C40
void CGUnit_C::NamePlateClearAll() {
    CGNamePlateFrame* frame = CGNamePlateFrame::s_namePlates.Head();

    while (!CGNamePlateFrame::s_namePlates.IsEnd(frame)) {
        CGNamePlateFrame* next = CGNamePlateFrame::s_namePlates.RawNext(frame);
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(frame->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            unit->DestroyNamePlate();
        }

        frame = next;
    }
}

// OFFSET: 0x009E8EE0/0x140781CB0
void CGUnit_C::SetNamePlateFocus(const C2Vector& position) {
    if (CGInputControl::GetActive()->m_flags & 0x6000003) {
        return;
    }

    CGNamePlateFrame* focus = NULL;

    for (CGNamePlateFrame* frame = CGNamePlateFrame::s_namePlates.Head(); frame; frame = CGNamePlateFrame::s_namePlates.Next(frame)) {
        if (frame->m_guid == CGGameUI::m_lockedTarget) {
            continue;
        }

        if (!frame->PtInFrameRect(position)) {
            continue;
        }

        if (!focus || focus->m_position.z > frame->m_position.z) {
            focus = frame;
        }
    }

    if (focus == CGUnit_C::s_namePlateFocus) {
        return;
    }

    if (CGUnit_C::s_namePlateFocus) {
        CGUnit_C::s_namePlateFocus->OnLoseFocus();
    }

    CGUnit_C::s_namePlateFocus = focus;

    if (focus) {
        focus->OnGainFocus();
    }
}

// PROVEN: 960191b4 IDENTICAL/IDENTICAL
// OFFSET: 0x009E8FD0/0x140781DD0
void CGUnit_C::ClearNamePlateFocus() {
    if (CGUnit_C::s_namePlateFocus) {
        CGUnit_C::s_namePlateFocus->OnLoseFocus();
    }

    CGUnit_C::s_namePlateFocus = NULL;
}

// PROVEN: 450c42e5 IDENTICAL/DIFFERENT
// OFFSET: 0x009E8FF0/0x140781E00
void CGNamePlateFrame::ReleaseNameplateFrame(CGNamePlateFrame* frame) {
    if (frame) {
        CGNamePlateFrame::s_unusedNamePlates.LinkToTail(frame);
    }
}

// OFFSET: 0x009E9010/0x140781E70
void CGNamePlateFrame::NamePlateSort(CGNamePlateFrame* frame) {
    CGNamePlateFrame::s_namePlates.UnlinkNode(frame);

    float distance = NamePlateScreenCenterDistanceSquared(frame->m_position);

    for (CGNamePlateFrame* node = CGNamePlateFrame::s_namePlates.Head(); node; node = CGNamePlateFrame::s_namePlates.Next(node)) {
        if (NamePlateScreenCenterDistanceSquared(node->m_position) <= distance) {
            CGNamePlateFrame::s_namePlates.LinkNode(frame, 2, node);
            return;
        }
    }

    CGNamePlateFrame::s_namePlates.LinkToTail(frame);
}

// PROVEN: 9507edeb IDENTICAL/NONE
// OFFSET: 0x009E9240
void CGNamePlateFrame::DestroyUnusedNameplateFrames() {
    CGNamePlateFrame* frame = CGNamePlateFrame::s_unusedNamePlates.Head();

    while (!CGNamePlateFrame::s_unusedNamePlates.IsEnd(frame)) {
        CGNamePlateFrame* next = CGNamePlateFrame::s_unusedNamePlates.RawNext(frame);
        CGNamePlateFrame::s_unusedNamePlates.UnlinkNode(frame);
        delete frame;
        frame = next;
    }
}

// OFFSET: 0x009E92C0/0x1407821F0
void CGNamePlateFrame::Initialize(CGUnit_C* unit) {
    if (this->m_unitNameText) {
        this->m_unitNameText->SetVertexColor(CImVector(0xFFFFFFFF));
    }

    this->m_guid = unit->m_obj->m_guid;

    if (this->m_healthBar) {
        this->m_healthBar->SetUnit(unit);
    }

    this->SetBloat(s_cvBloatTest->m_floatValue, true);
    this->m_castBar->SetMinMaxValues(0.0f, 0.0f);
    this->m_castBar->SetValue(0.0f);

    CImVector castColor(0xFFB200FF);
    this->m_castBar->SetStatusBarColor(castColor);

    if (this->m_namePlateGlow) {
        this->m_namePlateGlow->SetAllPoints(this, 1);
    }

    this->m_damageFeedbackTime = 0;
    this->m_offset.x = 0.0f;
    this->m_offset.y = 0.0f;
    this->m_offset.z = 0.0f;

    if (this->m_unitNameText) {
        this->m_unitNameText->SetText(unit->GetUnitName(NULL, 1), 1);
    }

    this->UpdateColorDisplay(unit);
    this->UpdateLevelDisplay(unit);
    this->UpdateEliteIcon(unit);
    this->UpdateRaidTargetDisplay(GetRaidTargetIndexFromGuid(unit->m_obj->m_guid));
    this->m_sortY = 0.0f;
    this->m_targetOffsetY = 0.0f;
    this->m_below = NULL;
    this->m_stackDepth = 0;
}

// OFFSET: 0x009E9430/0x140782370
bool CGUnit_C::UnitNameplatesReposition(CGWorldFrame* worldFrame) {
    uint32_t elapsed = FrameTime::GetCurTimeMs() - s_lastRepositionTime;
    float overlapH = s_cvNamePlateOverlapH->m_floatValue;
    float overlapV = s_cvNamePlateOverlapV->m_floatValue;
    int32_t count = 0;

    for (CGNamePlateFrame* frame = CGNamePlateFrame::s_namePlates.Tail(); frame; frame = CGNamePlateFrame::s_namePlates.Prev(frame)) {
        count++;

        if (frame->m_bloatTime <= static_cast<int32_t>(elapsed)) {
            frame->SetBloat(0.0f, false);
        } else {
            frame->m_bloatTime -= elapsed;
        }
    }

    int32_t motion = CGNamePlateFrame::s_cvNamePlateMotion->m_intValue;

    if (count <= s_cvMaxAlgoPlates->m_intValue && motion == 1) {
        s_lastRepositionTime = FrameTime::GetCurTimeMs();
        s_sortedPlates.SetCount(0);

        for (CGNamePlateFrame* frame = CGNamePlateFrame::s_namePlates.Tail(); frame; frame = CGNamePlateFrame::s_namePlates.Prev(frame)) {
            frame->m_offset = C3Vector(0.0f, frame->m_offset.y, 0.0f);
            frame->GetSize(frame->m_halfWidth, frame->m_halfHeight, 0);
            frame->m_halfHeight *= 0.5f;
            frame->m_halfWidth *= 0.5f;

            C3Vector position = frame->m_position;
            float height = frame->m_halfHeight * overlapV;
            float width = frame->m_halfWidth * overlapH;
            frame->m_overlapRect.minY = position.y - height;
            frame->m_overlapRect.minX = position.x - width;
            frame->m_overlapRect.maxY = height + position.y;
            frame->m_overlapRect.maxX = width + position.x;
            s_sortedPlates.Add(1, &frame);
            frame->m_below = NULL;
            frame->m_stackDepth = 0;
            frame->m_targetOffsetY = 0.0f;
        }

        qsort(s_sortedPlates.m_data, s_sortedPlates.Count(), sizeof(CGNamePlateFrame*), CompareNamePlates);

        for (uint32_t i = 0; i < s_sortedPlates.Count(); i++) {
            CGNamePlateFrame* frame = s_sortedPlates.m_data[i];

            for (uint32_t j = 0; j < i; j++) {
                CGNamePlateFrame* other = s_sortedPlates.m_data[j];
                CRect overlap = frame->m_overlapRect & other->m_overlapRect;

                if (overlap.maxY > overlap.minY && overlap.maxX > overlap.minX) {
                    float oldOffset = frame->m_targetOffsetY;
                    frame->m_targetOffsetY = (other->m_halfHeight + frame->m_halfHeight) * overlapV + 0.001f + (other->m_targetOffsetY + other->m_position.y) - frame->m_position.y;
                    float shift = frame->m_targetOffsetY - oldOffset;
                    frame->m_overlapRect.minY += shift;
                    frame->m_overlapRect.maxY += shift;
                    frame->m_below = other;
                }
            }

            for (uint32_t j = 0; j < i; j++) {
                CGNamePlateFrame* other = s_sortedPlates.m_data[j];

                if (other->m_targetOffsetY + other->m_position.y > frame->m_targetOffsetY + frame->m_position.y) {
                    memmove(&s_sortedPlates.m_data[j + 1], &s_sortedPlates.m_data[j], (i - j) * sizeof(CGNamePlateFrame*));
                    s_sortedPlates.m_data[j] = frame;
                    break;
                }
            }
        }

        for (int32_t i = s_sortedPlates.Count() - 1; i > 0; i--) {
            CGNamePlateFrame* frame = s_sortedPlates.m_data[i];

            if (!frame->m_stackDepth) {
                UpdateStackDepth(frame, 0);
            }
        }

        for (uint32_t i = 0; i < s_sortedPlates.Count(); i++) {
            CGNamePlateFrame* frame = s_sortedPlates.m_data[i];
            CGNamePlateFrame* below = frame->m_below;

            if (below && (frame->m_stackDepth > 5 || below->m_stackDepth == -1)) {
                float scale = frame->m_stackDepth > 5 ? 0.7f : 1.0f;
                float y = (below->m_halfHeight + frame->m_halfHeight) * (scale * overlapV) + (below->m_targetOffsetY + below->m_position.y);
                frame->m_targetOffsetY = y - frame->m_position.y >= 0.0f ? y - frame->m_position.y : 0.0f;
                frame->m_stackDepth = -1;
            }

            frame->m_stackDepth = 0;
        }

        float maxStep = static_cast<float>(elapsed * 0.001 * 0.05000000074505806);

        for (CGNamePlateFrame* frame = CGNamePlateFrame::s_namePlates.Tail(); frame; frame = CGNamePlateFrame::s_namePlates.Prev(frame)) {
            float delta = frame->m_targetOffsetY - frame->m_offset.y;
            float step = delta;

            if (fabs(delta) > maxStep) {
                step = delta <= 0.0f ? -maxStep : maxStep;
            }

            frame->m_offset.y += step;
            frame->m_sortY = frame->m_position.y + frame->m_offset.y;
        }
    } else if (count <= s_cvMaxAlgoPlates->m_intValue && motion == 2) {
        s_repositionCounter = (s_repositionCounter + 1) % (static_cast<uint8_t>(s_cvRepositionFrequency->m_intValue) + 1);

        if (!s_repositionCounter) {
            s_lastRepositionTime = FrameTime::GetCurTimeMs();

            for (CGNamePlateFrame* frame = CGNamePlateFrame::s_namePlates.Tail(); frame; frame = CGNamePlateFrame::s_namePlates.Prev(frame)) {
                C3Vector position = frame->m_position;
                frame->m_offset = C3Vector(0.0f, 0.0f, 0.0f);
                frame->GetSize(frame->m_halfWidth, frame->m_halfHeight, 0);
                frame->m_halfWidth *= 0.5f;
                frame->m_halfHeight *= 0.5f;

                float height = frame->m_halfHeight * 0.5f;
                frame->m_overlapRect.minY = position.y - height;
                frame->m_overlapRect.minX = position.x - frame->m_halfWidth;
                frame->m_overlapRect.maxY = height + position.y;
                frame->m_overlapRect.maxX = frame->m_halfWidth + position.x;
            }

            for (CGNamePlateFrame* frame = CGNamePlateFrame::s_namePlates.Tail(); frame; frame = CGNamePlateFrame::s_namePlates.Prev(frame)) {
                C3Vector position = frame->m_position;

                for (CGNamePlateFrame* other = CGNamePlateFrame::s_namePlates.Tail(); other; other = CGNamePlateFrame::s_namePlates.Prev(other)) {
                    if (other == frame) {
                        continue;
                    }

                    CRect overlap = other->m_overlapRect & frame->m_overlapRect;

                    if (!(overlap.maxY > overlap.minY) || !(overlap.maxX > overlap.minX)) {
                        continue;
                    }

                    C3Vector delta(position.x - other->m_position.x, position.y - other->m_position.y, position.z - other->m_position.z);

                    if (delta.x != 0.0f) {
                        float scale = fabs(frame->m_halfWidth / delta.x);
                        delta.x *= scale;
                        delta.y *= scale;
                        delta.z *= scale;
                    }

                    if (fabs(delta.y) > frame->m_halfHeight) {
                        float scale = fabs(frame->m_halfHeight / delta.y);
                        delta.x *= scale;
                        delta.y *= scale;
                        delta.z *= scale;
                    }

                    float ratio = (overlap.maxY - overlap.minY) * (overlap.maxX - overlap.minX) / ((frame->m_overlapRect.maxX - frame->m_overlapRect.minX) * (frame->m_overlapRect.maxY - frame->m_overlapRect.minY));
                    frame->m_offset.x += ratio * delta.x;
                    frame->m_offset.y += ratio * delta.y;
                    frame->m_offset.z += ratio * delta.z;
                }
            }
        }
    } else {
        for (CGNamePlateFrame* frame = CGNamePlateFrame::s_namePlates.Head(); !CGNamePlateFrame::s_namePlates.IsEnd(frame); frame = CGNamePlateFrame::s_namePlates.RawNext(frame)) {
            frame->m_offset = C3Vector(0.0f, 0.0f, 0.0f);
        }
    }

    bool inCombat = false;
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        inCombat = player->IsInCombat() != 0;
    }

    for (CGNamePlateFrame* frame = CGNamePlateFrame::s_namePlates.Tail(); frame; frame = CGNamePlateFrame::s_namePlates.Prev(frame)) {
        frame->SetScreenCoordsAndRectangle(C3Vector(frame->m_position.x + frame->m_offset.x, frame->m_offset.y + frame->m_position.y, frame->m_offset.z + frame->m_position.z));

        uint8_t alpha = 0xFF;
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(frame->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            alpha = unit->GetNamePlateAlpha(inCombat);
        }

        frame->SetFrameAlpha(alpha);
    }

    CLayoutFrame::ResizePending();
    return true;
}

// OFFSET: 0x009EA630/0x1407837E0
void CGNamePlateFrame::LoadScriptedTemplate(linkObject* node) {
    CStatus status;
    this->PreLoadXML(node, &status);
    this->LoadXML(node, &status);
    this->PostLoadXML(node, &status);

    uint32_t nameLength = SStrLen(this->GetName());

    for (CSimpleRegion* region = this->m_regions.Head(); region;) {
        CSimpleRegion* next = this->m_regions.Next(region);

        if (region->GetName() && SStrLen(region->GetName()) > nameLength) {
            char suffix[128];
            SStrCopy(suffix, region->GetName() + nameLength, sizeof(suffix));

            if (!SStrCmp(suffix, "Name", STORM_MAX_STR)) {
                this->m_unitNameText = static_cast<CSimpleFontString*>(region);
            } else if (!SStrCmp(suffix, "Level", STORM_MAX_STR)) {
                this->m_levelText = static_cast<CSimpleFontString*>(region);
            } else if (!SStrCmp(suffix, "Aggro", STORM_MAX_STR)) {
                this->m_targetingFrameFlash = static_cast<CSimpleTexture*>(region);
            } else if (!SStrCmp(suffix, "CastBar", STORM_MAX_STR)) {
                this->m_castBarBorder = static_cast<CSimpleTexture*>(region);
            } else if (!SStrCmp(suffix, "CastBarShield", STORM_MAX_STR)) {
                this->m_castBarShield = static_cast<CSimpleTexture*>(region);
            } else if (!SStrCmp(suffix, "CastBarSpellIcon", STORM_MAX_STR)) {
                this->m_castBarIcon = static_cast<CSimpleTexture*>(region);
            } else if (!SStrCmp(suffix, "Highlight", STORM_MAX_STR)) {
                this->m_namePlateGlow = static_cast<CSimpleTexture*>(region);
            } else if (!SStrCmp(suffix, "BossTexture", STORM_MAX_STR)) {
                this->m_targetingFrameSkull = static_cast<CSimpleTexture*>(region);
            } else if (!SStrCmp(suffix, "RaidTarget", STORM_MAX_STR)) {
                this->m_raidTargetingIcon = static_cast<CSimpleTexture*>(region);
            } else if (!SStrCmp(suffix, "Elite", STORM_MAX_STR)) {
                this->m_eliteNameplateIcon = static_cast<CSimpleTexture*>(region);
            }
        }

        region = next;
    }

    for (SIMPLEFRAMENODE* child = this->m_children.Head(); child;) {
        CSimpleFrame* frame = child->m_frame;
        SIMPLEFRAMENODE* next = this->m_children.Next(child);

        if (frame->GetName() && SStrLen(frame->GetName()) > nameLength) {
            char suffix[128];
            SStrCopy(suffix, frame->GetName() + nameLength, sizeof(suffix));

            if (!SStrCmp(suffix, "CastBar", STORM_MAX_STR)) {
                this->m_castBar = static_cast<CSimpleStatusBar*>(frame);
            }
        }

        child = next;
    }

    this->m_scripted = 1;
}

// OFFSET: 0x009EAAE0/0x140783BB0
CGNamePlateFrame::CGNamePlateFrame(CSimpleFrame* parent) : CSimpleFrame(parent), m_bloatTime(0), m_damageFeedbackTime(0) {
    this->SetProtected();
    this->SetDontReturnDepth(1);

    char name[32];
    SStrPrintf(name, sizeof(name), "NamePlate%d", ++s_namePlateCount);
    this->SetName(name);

    this->m_guid.m_guid = 0;
    this->m_namePlateGlow = NULL;
    this->m_targetingFrameSkull = NULL;
    this->m_raidTargetingIcon = NULL;
    this->m_castBarBorder = NULL;
    this->m_castBarShield = NULL;
    this->m_castBarIcon = NULL;
    this->m_unitNameText = NULL;
    this->m_levelText = NULL;
    this->m_healthBar = NULL;
    this->m_castBar = NULL;
    this->m_eliteNameplateIcon = NULL;
    this->m_targetingFrameFlash = NULL;
    this->m_position.x = 0.0f;
    this->m_position.y = 0.0f;
    this->m_position.z = 0.0f;
    this->m_casting = 0;
    this->m_channeling = 0;
    this->m_healthBarColor.Set(0.0f, 0.0f, 0.0f, 0.0f);
    this->m_damageFeedbackTime = 0;
    this->m_castHoldTime = 0.0f;
    this->m_castFadeTime = 0.0f;
    this->m_scripted = 0;
    this->SetWidth(s_namePlateWidth);
    this->SetHeight(s_namePlateHeight);

    const char* style = s_cvNamePlateStyle->m_stringValue.m_str;

    if (style && *style) {
        XMLNode node(NULL, "Frame");
        node.SetAttribute("inherits", style);
        linkObject* compiled = FrameXML_CompileXML_Region(&node);

        if (compiled) {
            this->LoadScriptedTemplate(compiled);
        } else {
            s_cvNamePlateStyle->Reset();
        }
    }

    if (!this->m_scripted) {
        CRect texCoords(0.0f, 0.0f, 0.0f, 0.0f);

        void* flashData = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
        this->m_targetingFrameFlash = flashData ? new (flashData) CSimpleTexture(this, 0, 0, true) : NULL;
        this->m_targetingFrameFlash->SetBlendMode(GxBlend_Alpha);
        this->m_targetingFrameFlash->SetTexture("Interface\\TargetingFrame\\UI-TargetingFrame-Flash", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        texCoords.minX = 0.0f;
        texCoords.maxX = 0.555f;
        texCoords.minY = 0.53f;
        texCoords.maxY = 0.6f;
        this->m_targetingFrameFlash->SetTexCoord(texCoords);

        void* borderData = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
        CSimpleTexture* healthBorder = borderData ? new (borderData) CSimpleTexture(this, 2, 0, true) : NULL;
        healthBorder->SetAllPoints(this, 1);
        healthBorder->SetTexture("Interface\\Tooltips\\Nameplate-Border", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        healthBorder->SetBlendMode(GxBlend_Alpha);

        void* castBorderData = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
        this->m_castBarBorder = castBorderData ? new (castBorderData) CSimpleTexture(this, 2, 0, true) : NULL;
        this->m_castBarBorder->SetTexture("Interface\\Tooltips\\Nameplate-Border", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        texCoords.minX = 1.0f;
        texCoords.maxX = 0.0f;
        texCoords.minY = 0.0f;
        texCoords.maxY = 1.0f;
        this->m_castBarBorder->SetTexCoord(texCoords);
        this->m_castBarBorder->SetBlendMode(GxBlend_Alpha);

        void* shieldData = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
        this->m_castBarShield = shieldData ? new (shieldData) CSimpleTexture(this, 2, 0, true) : NULL;
        this->m_castBarShield->SetTexture("Interface\\Tooltips\\Nameplate-CastBar-Shield", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        this->m_castBarShield->SetBlendMode(GxBlend_Alpha);

        void* iconData = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
        this->m_castBarIcon = iconData ? new (iconData) CSimpleTexture(this, 3, 0, true) : NULL;
        this->m_castBarIcon->SetWidth(0.01f);
        this->m_castBarIcon->SetHeight(0.01f);

        void* glowData = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
        this->m_namePlateGlow = glowData ? new (glowData) CSimpleTexture(this, 4, 0, true) : NULL;
        this->m_namePlateGlow->SetBlendMode(GxBlend_Add);
        this->m_namePlateGlow->SetTexture("Interface\\Tooltips\\Nameplate-Glow", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        this->m_namePlateGlow->SetAllPoints(this, 1);
        this->m_namePlateGlow->Hide();

        void* nameData = CSimpleFontString::s_allocator.GetData(0, typeid(CSimpleFontString).raw_name(), SLOG_OBJECT);
        this->m_unitNameText = nameData ? new (nameData) CSimpleFontString(this, 2, true) : NULL;
        this->m_unitNameText->SetFont(FrameScript_GetText("NAMEPLATE_FONT", -1, GENDER_NOT_APPLICABLE), 0.009f, 0, false);
        this->m_unitNameText->AddShadow(CImVector(0x00, 0x00, 0x00, 0xFF), C2Vector(0.001f, -0.001f));
        this->m_unitNameText->SetFrame(this, 3, 0, true);
        this->m_unitNameText->SetPoint(FRAMEPOINT_BOTTOM, this, FRAMEPOINT_CENTER, 0.0f, 0.0f, 1);

        void* levelData = CSimpleFontString::s_allocator.GetData(0, typeid(CSimpleFontString).raw_name(), SLOG_OBJECT);
        this->m_levelText = levelData ? new (levelData) CSimpleFontString(this, 2, true) : NULL;
        this->m_levelText->SetFont(FrameScript_GetText("NAMEPLATE_FONT", -1, GENDER_NOT_APPLICABLE), 0.009f, 0, false);
        this->m_levelText->SetVertexColor(s_levelTextColor);
        this->m_levelText->AddShadow(CImVector(0x00, 0x00, 0x00, 0xFF), C2Vector(0.001f, -0.001f));
        this->m_levelText->SetFrame(this, 3, 0, true);

        void* skullData = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
        this->m_targetingFrameSkull = skullData ? new (skullData) CSimpleTexture(this, 3, 0, true) : NULL;
        this->m_targetingFrameSkull->SetBlendMode(GxBlend_Alpha);
        this->m_targetingFrameSkull->SetTexture("Interface\\TargetingFrame\\UI-TargetingFrame-Skull", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        this->m_targetingFrameSkull->SetWidth(0.01f);
        this->m_targetingFrameSkull->SetHeight(0.01f);
        this->m_targetingFrameSkull->SetPoint(FRAMEPOINT_CENTER, this->m_levelText, FRAMEPOINT_CENTER, 0.0f, 0.0f, 1);

        void* raidData = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
        this->m_raidTargetingIcon = raidData ? new (raidData) CSimpleTexture(this, 2, 0, true) : NULL;
        this->m_raidTargetingIcon->SetBlendMode(GxBlend_Alpha);
        this->m_raidTargetingIcon->SetTexture("Interface\\TargetingFrame\\UI-RaidTargetingIcons", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        this->m_raidTargetingIcon->SetWidth(0.02f);
        this->m_raidTargetingIcon->SetHeight(0.02f);
        this->m_raidTargetingIcon->SetPoint(FRAMEPOINT_RIGHT, healthBorder, FRAMEPOINT_LEFT, 0.0f, 0.0f, 1);

        void* eliteData = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
        this->m_eliteNameplateIcon = eliteData ? new (eliteData) CSimpleTexture(this, 1, 0, true) : NULL;
        this->m_eliteNameplateIcon->SetTexture("Interface\\Tooltips\\EliteNameplateIcon", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        texCoords.minX = 0.0f;
        texCoords.minY = 0.0f;
        texCoords.maxX = 0.578125f;
        texCoords.maxY = 0.84375f;
        this->m_eliteNameplateIcon->SetTexCoord(texCoords);
    }

    if (!this->m_healthBar) {
        void* healthData = CGSimpleHealthBar::s_simpleHealthBarHeap.GetData(0, typeid(CGSimpleHealthBar).raw_name(), SLOG_OBJECT);
        this->m_healthBar = healthData ? new (healthData) CGSimpleHealthBar(this) : NULL;
        this->m_healthBar->SetBarTexture("Interface\\TargetingFrame\\UI-TargetingFrame-BarFill", 2, 0);
        this->m_healthBar->SetDontReturnDepth(1);
        this->m_healthBar->SetFrameLevel(this->m_level - 1, 1);
    }

    if (!this->m_castBar) {
        void* castBarData = CSimpleStatusBar::s_allocator.GetData(0, typeid(CSimpleStatusBar).raw_name(), SLOG_OBJECT);
        this->m_castBar = castBarData ? new (castBarData) CSimpleStatusBar(this) : NULL;
        this->m_castBar->SetBarTexture("Interface\\TargetingFrame\\UI-TargetingFrame-BarFill", 2, 0);
        this->m_castBar->SetDontReturnDepth(1);
        this->m_castBar->SetFrameLevel(this->m_level - 1, 1);

        if (!this->m_scripted) {
            this->m_castBarBorder->SetParent(this->m_castBar);
            this->m_castBarShield->SetParent(this->m_castBar);
            this->m_castBarIcon->SetParent(this->m_castBar);
        }
    }

    this->DisableEvent(SIMPLE_EVENT_MOUSE);
}

// OFFSET: [0x009EB470, 0x009EB480]
CGNamePlateFrame::~CGNamePlateFrame() {
}

// OFFSET: 0x009EB4F0/0x140784900
CGNamePlateFrame* CGNamePlateFrame::GetNewNameplateFrame(CSimpleFrame* parent) {
    CGNamePlateFrame* frame = CGNamePlateFrame::s_unusedNamePlates.Head();

    if (frame) {
        CGNamePlateFrame::s_unusedNamePlates.UnlinkNode(frame);
        return frame;
    }

    void* data = CGNamePlateFrame::s_namePlateHeap.GetData(0, typeid(CGNamePlateFrame).raw_name(), SLOG_OBJECT);
    return data ? new (data) CGNamePlateFrame(parent) : NULL;
}

// PROVEN: 6a7621fc IDENTICAL/NONE
// OFFSET: 0x005ACCA0
void CGNamePlateFrame::SetPosition(const C3Vector& position) {
    if (this->m_position.x != position.x || this->m_position.y != position.y || this->m_position.z != position.z) {
        this->m_position = position;
        this->m_positionChanged = 1;
    }
}
