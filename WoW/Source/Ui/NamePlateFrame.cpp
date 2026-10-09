#include "NamePlateFrame.h"
#include <new>
#include <typeinfo>
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/Frame/CSimpleStatusBar.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/HealthBar.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"

// STATIC: #0x00AA34B8
static int32_t s_greenLevelRanges[20] = { 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

// STATIC: #0x00B2DA0C
static CImVector s_friendlyPlayerColor = CImVector(0xFF, 0x00, 0x00, 0xFF);

// STATIC: #0x00B2DA10
static CImVector s_friendlyColor = CImVector(0x00, 0xFF, 0x00, 0xFF);

// STATIC: #0x00B2DA14
static CImVector s_neutralColor = CImVector(0x00, 0xFF, 0xFF, 0xFF);

// STATIC: #0x00B2DA18
static CImVector s_hostileColor = CImVector(0x00, 0x00, 0xFF, 0xFF);

// STATIC: #0x00B2DA1C
static CImVector s_levelTextColor = CImVector(0xFF, 0xFF, 0xFF, 0xFF);

// STATIC: #0x00B2DA20
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

// STATIC: #0x00B2DA70
static float s_namePlateHeight = 0.025f;

// STATIC: #0x00B2DA74
static float s_namePlateWidth = 0.1f;

// STATIC: #0x00DCEC44
CDataAllocator CGNamePlateFrame::s_namePlateHeap(sizeof(CGNamePlateFrame), 5);

// INLINED
void CGNamePlateFrame::operator delete(void* ptr) {
    CGNamePlateFrame::s_namePlateHeap.PutData(ptr, NULL, 0);
}

// PROVEN: c903ef18 IDENTICAL/NONE
// OFFSET: 0x009E72E0/0x14077FCA0
CGNamePlateFrame* CGNamePlateFrame::GetNamePlateFocus() {
    return CGUnit_C::s_namePlateFocus;
}

// OFFSET: 0x009E7360/0x14077FD20
void CGNamePlateFrame::UpdateNameDisplay(const CGUnit_C* unit) {
    this->m_unitNameText->SetText(const_cast<CGUnit_C*>(unit)->GetUnitName(NULL, 1), 1);
}

// OFFSET: #0x0098E5B0
void CGNamePlateFrame::TriggerDamageFeedback() {
    CImVector color(0, 0, 0xFF, 0xFF);
    this->m_damageFeedbackTime = 5000;
    this->m_unitNameText->SetVertexColor(color);
}

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

// OFFSET: #0x0098E6E0
void CGNamePlateFrame::UpdateEliteIcon(CGUnit_C* unit) {
    uint32_t classification = unit->GetCreatureClassification();

    if (unit->IsBossMob() || classification == 2 || classification == 1) {
        this->m_eliteNameplateIcon->Show();
    } else {
        this->m_eliteNameplateIcon->Hide();
    }
}

// OFFSET: #0x0098E740
void CGNamePlateFrame::UpdateRaidTargetDisplay(uint32_t raidTarget) {
    if (raidTarget < 8) {
        float left = static_cast<int32_t>(raidTarget & 3) * 0.25f;
        float top = static_cast<int32_t>(raidTarget >> 2) * 0.25f;

        CRect texCoords;
        texCoords.minX = left;
        texCoords.maxX = left + 0.25f;
        texCoords.minY = top;
        texCoords.maxY = top + 0.25f;
        this->m_raidTargetingIcon->SetTexCoord(texCoords);
        this->m_raidTargetingIcon->Show();
    } else {
        this->m_raidTargetingIcon->Hide();
    }
}

// OFFSET: #0x0098E7E0
void CGNamePlateFrame::HideCastBar() {
    if (!this->m_castBar->m_shown) {
        return;
    }

    this->m_castBar->Hide();
    this->m_castBarBorder->Hide();
    this->m_castBarShield->Hide();
    this->m_castBarIcon->Hide();
    this->m_castHoldTime = 0.0f;
    this->m_casting = 0;
    this->m_castFadeTime = 0.0f;
}

// OFFSET: #0x0098E8A0
static int32_t GetAdjustedLevel(CGPlayer_C* player, CGUnit_C* unit) {
    int32_t level = static_cast<int32_t>(unit->m_unit->m_unitFieldLevel);

    if (level <= 0 || !player) {
        return level;
    }

    if (!player->m_player->m_playerBytes3[1] && !player->m_player->m_playerFakeInebriation) {
        return level;
    }

    if (!player->CanAttack(unit)) {
        return level;
    }

    level -= RoundToInt64(player->GetDrunknessTurnValue() * 5.0f);

    return level >= 1 ? level : 1;
}

// OFFSET: #0x0098E910
void CGNamePlateFrame::OnGainFocus() {
    this->m_namePlateGlow->Show();

    if (this->m_damageFeedbackTime <= 0) {
        CImVector color;
        color.Set(1.0f, 1.0f, 1.0f, 0.0f);
        this->m_unitNameText->SetVertexColor(color);
    }

    this->EnableDrawLayer(4);
}

// OFFSET: #0x0098E980
void CGNamePlateFrame::OnLoseFocus() {
    this->m_namePlateGlow->Hide();

    if (this->m_damageFeedbackTime <= 0) {
        CImVector color;
        color.Set(1.0f, 1.0f, 1.0f, 1.0f);
        this->m_unitNameText->SetVertexColor(color);
    }

    this->DisableDrawLayer(4);
}

// OFFSET: #0x0098E9F0
void CGNamePlateFrame::OnLayerUpdate(float elapsedSec) {
    if (this->m_guid == CGGameUI::m_lockedTarget) {
        this->SetFrameLevel(20, 1);
    } else {
        this->SetFrameLevel(10, 1);
    }

    int32_t feedbackTime = this->m_damageFeedbackTime;

    if (feedbackTime >= 0) {
        this->m_damageFeedbackTime = feedbackTime - static_cast<int32_t>(elapsedSec * 1000.0f);

        if (this->m_damageFeedbackTime <= 0) {
            if (this->m_namePlateGlow->m_visible) {
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

    if (threatStatus > 1) {
        this->m_targetingFrameFlash->SetVertexColor(*reinterpret_cast<const CImVector*>(CGTooltip::GetThreatStateColor(threatStatus)));
        this->m_targetingFrameFlash->Show();
    } else {
        this->m_targetingFrameFlash->Hide();
    }

    if (!g_showVKeyCastbarCVar->m_intValue) {
        this->HideCastBar();
        return;
    }

    CImVector color;

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

        this->m_castBarBorder->SetVertexAlpha(static_cast<uint8_t>(this->m_castBarBorder->GetVertexAlpha() * this->m_castFadeTime));
        this->m_castBarShield->SetVertexAlpha(static_cast<uint8_t>(this->m_castBarBorder->GetVertexAlpha() * this->m_castFadeTime));
        this->m_castBarIcon->SetVertexAlpha(static_cast<uint8_t>(this->m_castBarIcon->GetVertexAlpha() * this->m_castFadeTime));
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

// OFFSET: #0x0098EF10
void CGNamePlateFrame::UpdateLevelDisplay(CGUnit_C* unit) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    int32_t level = GetAdjustedLevel(player, unit);

    if (unit->IsBossMob() || ((unit->UnitReaction(player) > 1 || !CGPlayer_C::UnitIsTrivial(unit)) && unit->UnitReaction(player) <= 1 && static_cast<int32_t>(player->m_unit->m_unitFieldLevel) <= level - 10)) {
        this->m_levelText->Hide();
        this->m_targetingFrameSkull->Show();
    } else {
        this->m_levelText->Show();
        this->m_targetingFrameSkull->Hide();
    }

    if (!this->m_levelText->m_shown) {
        return;
    }

    this->m_levelText->SetVertexColor(*GetDifficultyColor(level, player->m_unit->m_unitFieldLevel));

    char text[32];
    SStrPrintf(text, sizeof(text), "%d", level);
    this->m_levelText->SetText(text, 1);
}

// OFFSET: 0x009E8A30/0x1407818F0
void CGNamePlateFrame::SetCastBarValues(float minValue, float maxValue, float value, int32_t spellId, bool channel, bool notInterruptible) {
    if (maxValue < value) {
        return;
    }

    this->m_castBar->SetMinMaxValues(minValue, maxValue);
    this->m_castBar->SetValue(value);
    this->m_channeling = channel;

    SpellRec spellRec;

    if (!spellId || !g_spellDB.GetRecord(spellId, &spellRec) || (spellRec.m_attributes & 0x20)) {
        this->HideCastBar();
        return;
    }

    CImVector color;

    if (channel ? minValue != value : maxValue != value) {
        color.Set(1.0f, 1.0f, 0.7f, 0.0f);
        this->m_castBar->SetStatusBarColor(color);
        this->m_castHoldTime = 0.0f;
        this->m_casting = 1;
        this->m_castFadeTime = 0.0f;
    } else {
        this->m_castHoldTime = 0.0f;
        this->m_castFadeTime = 1.0f;
        color.Set(1.0f, 1.0f, 0.0f, 0.0f);
        this->m_castBar->SetStatusBarColor(color);
        this->m_casting = 0;
    }

    color.Set(1.0f, 1.0f, 1.0f, 1.0f);
    this->m_castBarIcon->SetVertexColor(color);

    float iconOffsetY;

    if (notInterruptible) {
        this->m_castBarBorder->Hide();
        this->m_castBarShield->Show();
        this->m_castBarShield->SetVertexColor(color);
        this->m_castBar->SetPoint(FRAMEPOINT_BOTTOMRIGHT, this->m_castBarBorder, FRAMEPOINT_BOTTOMRIGHT, -0.003125f, -0.003125f, 1);
        iconOffsetY = 0.0f;
    } else {
        this->m_castBarBorder->Show();
        this->m_castBarBorder->SetVertexColor(color);
        this->m_castBarShield->Hide();
        this->m_castBar->SetPoint(FRAMEPOINT_BOTTOMRIGHT, this->m_castBarBorder, FRAMEPOINT_BOTTOMRIGHT, -0.003125f, 0.003125f, 1);
        iconOffsetY = 0.0071f;
    }

    this->m_castBarIcon->SetPoint(FRAMEPOINT_CENTER, this->m_castBarBorder, FRAMEPOINT_BOTTOMLEFT, 0.0092f, iconOffsetY, 1);
    this->m_castBar->Show();
    this->m_castBarIcon->Show();

    SpellIconRec* iconRec = g_spellIconDB.GetRecord(spellRec.m_spellIconID);

    if (iconRec) {
        this->m_castBarIcon->SetTexture(iconRec->m_textureFilename, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
    }
}

// OFFSET: #0x0098F390
void CGNamePlateFrame::Initialize(CGUnit_C* unit) {
    this->m_guid = unit->m_obj->m_guid;
    this->m_unitNameText->SetVertexColor(CImVector(0xFF, 0xFF, 0xFF, 0xFF));
    this->SetWidth(s_namePlateWidth);
    this->SetHeight(s_namePlateHeight);

    float barOffsetX = s_namePlateWidth * 0.031f;
    float barOffsetY = s_namePlateHeight * 0.125f;
    this->m_healthBar->SetPoint(FRAMEPOINT_BOTTOMLEFT, this, FRAMEPOINT_BOTTOMLEFT, barOffsetX, barOffsetY, 1);
    this->m_healthBar->SetWidth(s_namePlateWidth * 0.804f);
    this->m_healthBar->SetHeight(s_namePlateHeight * 0.281f);
    this->m_healthBar->SetUnit(unit);

    this->m_castBarBorder->SetWidth(this->GetWidth());
    this->m_castBarBorder->SetHeight(this->GetHeight());
    this->m_castBarBorder->SetPoint(FRAMEPOINT_CENTER, this, FRAMEPOINT_CENTER, 0.0f, -(this->m_castBarBorder->GetHeight() * 0.5f), 1);

    this->m_castBarShield->SetWidth(this->GetWidth());
    this->m_castBarShield->SetHeight(this->GetHeight());
    this->m_castBarShield->SetPoint(FRAMEPOINT_CENTER, this->m_castBarBorder, FRAMEPOINT_CENTER, 0.0f, -(this->m_castBarShield->GetHeight() * 0.5f), 1);

    this->m_castBar->SetPoint(FRAMEPOINT_BOTTOMRIGHT, this->m_castBarBorder, FRAMEPOINT_BOTTOMRIGHT, -barOffsetX, barOffsetY, 1);
    this->m_castBar->SetWidth(s_namePlateWidth * 0.804f);
    this->m_castBar->SetHeight(s_namePlateHeight * 0.281f);
    this->m_castBar->SetMinMaxValues(0.0f, 0.0f);
    this->m_castBar->SetValue(0.0f);

    CImVector castColor;
    castColor.Set(1.0f, 0.7f, 0.0f, 1.0f);
    this->m_castBar->SetStatusBarColor(castColor);

    float levelOffsetX = s_namePlateWidth * -0.092f;
    float levelOffsetY = s_namePlateHeight * 0.284f;
    this->m_levelText->SetPoint(FRAMEPOINT_CENTER, this, FRAMEPOINT_BOTTOMRIGHT, levelOffsetX, levelOffsetY, 1);
    this->m_castBarIcon->SetPoint(FRAMEPOINT_CENTER, this->m_castBarBorder, FRAMEPOINT_BOTTOMLEFT, -levelOffsetX, levelOffsetY, 1);
    this->m_namePlateGlow->SetAllPoints(this, 1);

    this->m_damageFeedbackTime = 0;
    this->m_unitNameText->SetText(unit->GetUnitName(NULL, 1), 1);
    this->UpdateColorDisplay(unit);
    this->UpdateLevelDisplay(unit);
    this->UpdateEliteIcon(unit);
    this->UpdateRaidTargetDisplay(GetRaidTargetIndexFromGuid(unit->m_obj->m_guid));
}

// OFFSET: #0x0098F790
CGNamePlateFrame::CGNamePlateFrame(CSimpleFrame* parent) : CSimpleFrame(parent), m_damageFeedbackTime(0) {
    this->SetProtected();
    this->SetFrameFlag(0x40000, 1);
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
    this->m_position.x = 0.0f;
    this->m_position.y = 0.0f;
    this->m_position.z = 0.0f;
    this->m_healthBarColor.Set(0.0f, 0.0f, 0.0f, 0.0f);
    this->m_castHoldTime = 0.0f;
    this->m_castFadeTime = 0.0f;

    CRect texCoords = { 0.0f, 0.0f, 0.0f, 0.0f };
    this->m_damageFeedbackTime = 0;
    this->m_casting = 0;
    this->m_channeling = 0;

    void* flashData = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    this->m_targetingFrameFlash = flashData ? new (flashData) CSimpleTexture(this, 0, 0, true) : NULL;
    this->m_targetingFrameFlash->SetBlendMode(GxBlend_Alpha);
    this->m_targetingFrameFlash->SetTexture("Interface\\TargetingFrame\\UI-TargetingFrame-Flash", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
    this->m_targetingFrameFlash->SetWidth(0.11f);
    this->m_targetingFrameFlash->SetHeight(0.029f);
    this->m_targetingFrameFlash->SetPoint(FRAMEPOINT_TOP, this, FRAMEPOINT_TOP, -0.001f, -0.0065f, 1);
    texCoords.minX = 0.0f;
    texCoords.minY = 0.53f;
    texCoords.maxX = 0.555f;
    texCoords.maxY = 0.6f;
    this->m_targetingFrameFlash->SetTexCoord(texCoords);

    void* borderData = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    CSimpleTexture* healthBorder = borderData ? new (borderData) CSimpleTexture(this, 2, 0, true) : NULL;
    healthBorder->SetAllPoints(this, 1);
    healthBorder->SetTexture("Interface\\Tooltips\\Nameplate-Border", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
    healthBorder->SetBlendMode(GxBlend_Alpha);

    void* healthData = CGSimpleHealthBar::s_simpleHealthBarHeap.GetData(0, typeid(CGSimpleHealthBar).raw_name(), SLOG_OBJECT);
    this->m_healthBar = healthData ? new (healthData) CGSimpleHealthBar(this) : NULL;
    this->m_healthBar->SetBarTexture("Interface\\TargetingFrame\\UI-TargetingFrame-BarFill", 2, 0);
    this->m_healthBar->SetFrameFlag(0x40000, 1);
    this->m_healthBar->SetFrameLevel(this->m_level - 1, 1);

    void* castBorderData = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    this->m_castBarBorder = castBorderData ? new (castBorderData) CSimpleTexture(this, 2, 0, true) : NULL;
    this->m_castBarBorder->SetTexture("Interface\\Tooltips\\Nameplate-Border", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
    texCoords.minX = 1.0f;
    texCoords.minY = 0.0f;
    texCoords.maxX = 0.0f;
    texCoords.maxY = 1.0f;
    this->m_castBarBorder->SetTexCoord(texCoords);
    this->m_castBarBorder->SetBlendMode(GxBlend_Alpha);

    void* shieldData = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    this->m_castBarShield = shieldData ? new (shieldData) CSimpleTexture(this, 2, 0, true) : NULL;
    this->m_castBarShield->SetTexture("Interface\\Tooltips\\Nameplate-CastBar-Shield", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
    this->m_castBarShield->SetBlendMode(GxBlend_Alpha);

    void* castBarData = CSimpleStatusBar::s_allocator.GetData(0, typeid(CSimpleStatusBar).raw_name(), SLOG_OBJECT);
    this->m_castBar = castBarData ? new (castBarData) CSimpleStatusBar(this) : NULL;
    this->m_castBar->SetBarTexture("Interface\\TargetingFrame\\UI-TargetingFrame-BarFill", 2, 0);
    this->m_castBar->SetFrameFlag(0x40000, 1);
    this->m_castBar->SetFrameLevel(this->m_level - 1, 1);

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
    this->m_unitNameText->SetFont(FrameScript_GetText("NAMEPLATE_FONT", -1, GENDER_NOT_APPLICABLE), 0.01f, 0, false);
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
    this->m_eliteNameplateIcon->SetWidth(0.0294f);
    this->m_eliteNameplateIcon->SetHeight(0.0215f);
    this->m_eliteNameplateIcon->SetPoint(FRAMEPOINT_CENTER, this->m_levelText, FRAMEPOINT_CENTER, 0.003f, -0.001f, 1);
    texCoords.minX = 0.0f;
    texCoords.minY = 0.0f;
    texCoords.maxX = 0.578125f;
    texCoords.maxY = 0.84375f;
    this->m_eliteNameplateIcon->SetTexCoord(texCoords);

    this->DisableEvent(SIMPLE_EVENT_MOUSE);
}

// OFFSET: [#0x0098FFE0, #0x00990030]
CGNamePlateFrame::~CGNamePlateFrame() {
}
