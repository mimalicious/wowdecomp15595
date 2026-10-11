#include "ChatBubbleFrame.h"
#include <cmath>
#include <new>
#include <typeinfo>
#include "Engine/Source/Frame/CBackdropGenerator.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Game/ValidateName.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Frame/CLayoutFrame.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Os/TimeManager.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/StaticSingleton.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/UIUtil/CameraManager.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/WorldFrame.h"

// STATIC: 0x00D16E24
static TSList<CGChatBubbleFrame> s_freeChatBubbles;

// STATIC: 0x00D16E30
static TSList<CGChatBubbleFrame> s_chatBubbles;

// STATIC: 0x00F76898
static uint8_t s_cameraPositionValid;

// STATIC: 0x00F7689C
static C3Vector s_cameraPosition;

CDataAllocator CGChatBubbleFrame::s_chatBubbleHeap(sizeof(CGChatBubbleFrame), 10);

struct RaidIconTag {
    const char* m_token;
    const char* m_icon;
};

// STATIC: 0x00C0B670
static const RaidIconTag s_raidIconTags[24] = {
    { "RAID_TARGET_1", "1" },
    { "RAID_TARGET_2", "2" },
    { "RAID_TARGET_3", "3" },
    { "RAID_TARGET_4", "4" },
    { "RAID_TARGET_5", "5" },
    { "RAID_TARGET_6", "6" },
    { "RAID_TARGET_7", "7" },
    { "RAID_TARGET_8", "8" },
    { "ICON_TAG_RAID_TARGET_STAR1", "1" },
    { "ICON_TAG_RAID_TARGET_CIRCLE1", "2" },
    { "ICON_TAG_RAID_TARGET_DIAMOND1", "3" },
    { "ICON_TAG_RAID_TARGET_TRIANGLE1", "4" },
    { "ICON_TAG_RAID_TARGET_MOON1", "5" },
    { "ICON_TAG_RAID_TARGET_SQUARE1", "6" },
    { "ICON_TAG_RAID_TARGET_CROSS1", "7" },
    { "ICON_TAG_RAID_TARGET_SKULL1", "8" },
    { "ICON_TAG_RAID_TARGET_STAR2", "1" },
    { "ICON_TAG_RAID_TARGET_CIRCLE2", "2" },
    { "ICON_TAG_RAID_TARGET_DIAMOND2", "3" },
    { "ICON_TAG_RAID_TARGET_TRIANGLE2", "4" },
    { "ICON_TAG_RAID_TARGET_MOON2", "5" },
    { "ICON_TAG_RAID_TARGET_SQUARE2", "6" },
    { "ICON_TAG_RAID_TARGET_CROSS2", "7" },
    { "ICON_TAG_RAID_TARGET_SKULL2", "8" },
};

// INLINED
void CGChatBubbleFrame::operator delete(void* ptr) {
    CGChatBubbleFrame::s_chatBubbleHeap.PutData(ptr, NULL, 0);
}

// PROVEN: 20523c8e IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGChatBubbleFrame::InitializeGame() {
}

// OFFSET: 0x009F0CE0
void CGChatBubbleFrame::FadeIn() {
    if (this->m_bubbleFlags & 0x5) {
        return;
    }

    uint8_t alpha = this->m_alpha;
    this->m_bubbleFlags = (this->m_bubbleFlags & ~0x3) | 0x1;
    this->m_fadeStart = FrameTime::GetCurTimeMs() - static_cast<int32_t>(alpha * (1.0f / 255.0f) * 250.0f);
}

// OFFSET: 0x009F0D50
void CGChatBubbleFrame::FadeOut(bool remove) {
    if (this->m_bubbleFlags & 0x4) {
        return;
    }

    if (remove) {
        this->m_bubbleFlags |= 0x4;
    }

    if (this->m_bubbleFlags & 0x2) {
        return;
    }

    uint8_t alpha = this->m_alpha;
    this->m_bubbleFlags = (this->m_bubbleFlags & ~0x3) | 0x2;
    this->m_fadeStart = FrameTime::GetCurTimeMs() - static_cast<int32_t>((255.0f - alpha) * (1.0f / 255.0f) * 250.0f);
}

// PROVEN: 2f74ebde IDENTICAL/NONE
// OFFSET: 0x009F0B60
static int32_t ComputeDuration(const char* text, bool self) {
    int32_t perWord = self ? 500 : 750;

    if (!text || !*text) {
        return 0;
    }

    int32_t duration = perWord + (self ? 1000 : 2000);
    bool lastWasSpace = true;

    for (const char* c = text; *c; c++) {
        bool space = *c == ' ' || *c == '\t';

        if (space && !lastWasSpace) {
            duration += perWord;
        }

        lastWasSpace = space;
    }

    return duration;
}

// OFFSET: 0x009F0FB0
C3Vector CGChatBubbleFrame::GetUnitPosition() {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_unit, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit || !unit->m_worldObject || !unit->m_worldModel) {
        return C3Vector(0.0f, 0.0f, 0.0f);
    }

    if (unit->m_vehiclePassenger) {
        return unit->GetModelPosition();
    }

    return unit->GetPosition();
}

// OFFSET: 0x009F1320/0x14078B2C0
void CGChatBubbleFrame::Render(CGWorldFrame* worldFrame) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_unit, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit || !this->m_positionValid) {
        return;
    }

    C3Vector screen(0.0f, 0.0f, 0.0f);
    float threshold = NDCToDDCWidth(1.0f / (CoordinateGetAspectCompensation() * 1024.0f));
    C3Vector position = this->GetUnitPosition();
    position.z = this->m_height + position.z + 0.69999999f;

    if (unit->m_namePlateFrame) {
        position.z += 0.34999999f;
    }

    uint32_t onScreen;
    bool behind;
    worldFrame->GetScreenCoordinates(position, &screen, &onScreen, &behind);

    if (behind) {
        screen.x = -1000.0f;
        screen.y = -1000.0f;
        screen.z = -1000.0f;
    } else if (!onScreen) {
        return;
    }

    if (StaticSingleton<CCameraManager>::Instance()->m_legacyActive && !behind) {
        CLayoutFrame* layout = this->m_fontString;
        float top = NDCToDDCHeight(1.0f);
        top -= layout->GetHeight();
        top -= NDCToDDCWidth(128.0f / (CoordinateGetAspectCompensation() * 1024.0f));
        top -= NDCToDDCWidth(16.0f / (CoordinateGetAspectCompensation() * 1024.0f));
        float bottom = NDCToDDCWidth(1.0f / (CoordinateGetAspectCompensation() * 1024.0f) * 128.0f);
        bottom += NDCToDDCWidth(16.0f / (CoordinateGetAspectCompensation() * 1024.0f));
        bottom += NDCToDDCHeight(0.0f);
        float y = screen.y > bottom ? screen.y : bottom;

        if (top > y) {
            if (!(screen.y > bottom)) {
                screen.y = bottom;
            }
        } else {
            screen.y = top;
        }
    }

    screen.x = CSimpleTop::RoundToPixelWidth(screen.x);
    screen.y = CSimpleTop::RoundToPixelHeight(screen.y);
    screen.z -= worldFrame->m_layoutDepth;
    bool moveX = threshold <= fabs(screen.x - this->m_screenX);
    bool moveY = fabs(screen.y - this->m_screenY) >= threshold;
    bool moveDepth = fabs(screen.z - this->m_screenDepth) >= threshold;

    if (moveX || moveY) {
        if (moveX) {
            this->m_screenX = screen.x;
        }

        if (moveY) {
            this->m_screenY = screen.y;
        }

        this->m_fontString->SetPoint(FRAMEPOINT_BOTTOM, worldFrame, FRAMEPOINT_BOTTOMLEFT, this->m_screenX, this->m_screenY, 1);
    }

    if (moveDepth) {
        this->m_screenDepth = screen.z;
        this->SetFrameDepth(screen.z, false);
    }
}

// OFFSET: 0x009F1050
bool CGChatBubbleFrame::Update(CGUnit_C* player, CGUnit_C* unit) {
    if (!player) {
        return false;
    }

    CM2Model* model = unit->m_worldModel;
    uintptr_t entity = unit->m_worldObject;

    if (!entity || !model) {
        return false;
    }

    if (!model->IsLoaded() || !(model->m_fFlags & 0x4000)) {
        return true;
    }

    if (CGGameUI::m_visible) {
        this->Show();
    } else {
        this->Hide();
    }

    C3Vector position = this->GetUnitPosition();
    C3Vector delta = player->GetPosition() - position;

    if ((World::QueryObjectVisible(entity) && delta.z * delta.z + delta.x * delta.x + delta.y * delta.y < 625.0f) || !(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        if (this->m_fadeAlpha < 1.0f) {
            this->FadeIn();
        }
    } else if (this->m_fadeAlpha > 0.0f) {
        this->FadeOut(false);
    }

    uint32_t now = FrameTime::GetCurTimeMs();

    if (!(this->m_bubbleFlags & 0x2) && static_cast<int32_t>(now - this->m_showTime) > this->m_duration + 250) {
        this->FadeOut(true);
    }

    uint32_t flags = this->m_bubbleFlags;

    if (flags & 0x1) {
        uint32_t elapsed = now - this->m_fadeStart;
        float alpha = 1.0f;

        if (elapsed < 250) {
            double fraction = static_cast<double>(elapsed) * 0.0040000002;

            if (fraction < 1.0) {
                alpha = static_cast<float>(fraction);
            }
        } else {
            this->m_bubbleFlags = flags & ~0x1;
        }

        this->m_fadeAlpha = alpha;
    } else if (flags & 0x2) {
        uint32_t elapsed = now - this->m_fadeStart;

        if (elapsed < 250) {
            double fraction = static_cast<double>(elapsed) * 0.0040000002;
            float faded = fraction >= 1.0 ? 1.0f : static_cast<float>(fraction);
            this->m_fadeAlpha = 1.0f - faded;
        } else {
            if (flags & 0x4) {
                return false;
            }

            this->m_bubbleFlags = flags & ~0x2;
            this->m_fadeAlpha = 0.0f;
        }
    }

    float fade = CGWorldFrame::GetUIFadeAmount();
    this->SetFrameAlpha(static_cast<uint8_t>(static_cast<int32_t>(this->m_fadeAlpha * fade * 255.0f)));
    C3Vector attachment;

    if (unit->IsMounted()) {
        attachment = unit->GetAttachmentPosition(29, NULL);
    } else {
        attachment = unit->GetAttachmentPosition(18, NULL);
    }

    this->m_height = attachment.z - unit->GetModelPosition().z;
    this->m_positionValid = 1;
    return true;
}

// OFFSET: 0x009F0BC0/0x14078AA30
bool CGChatBubbleFrame::AppendRaidIconTexture(char* dest, char* tag, uint32_t size) {
    for (uint32_t i = 0; i < sizeof(s_raidIconTags) / sizeof(s_raidIconTags[0]); i++) {
        if (!_stricmp(FrameScript_GetText(s_raidIconTags[i].m_token, -1, GENDER_NOT_APPLICABLE), tag)) {
            SStrPack(dest, "|TInterface\\TargetingFrame\\UI-RaidTargetingIcon_", size);
            SStrPack(dest, s_raidIconTags[i].m_icon, size);
            SStrPack(dest, ":0|t", size);
            return true;
        }
    }

    return false;
}

// OFFSET: 0x009F0DE0/0x14078ACB0
void CGChatBubbleFrame::ParseRaidIcons(char* text, char* dest, uint32_t size) {
    if (!text) {
        return;
    }

    *dest = '\0';
    char* open = strchr(text, '{');

    while (open && *open) {
        uint32_t prefix = open - text;

        if (prefix) {
            uint32_t length = strlen(dest);
            SStrPack(dest, text, size);

            if (length + prefix < size) {
                dest[length + prefix] = '\0';
            }
        }

        char* tagStart = open + 1;
        char* close = strchr(tagStart, '}');
        bool replaced = false;

        if (close) {
            char tag[256];
            SStrCopy(tag, tagStart, sizeof(tag));
            tag[close - tagStart] = '\0';

            if (CGChatBubbleFrame::AppendRaidIconTexture(dest, tag, size)) {
                text = close + 1;
                replaced = true;
            }
        }

        if (!replaced) {
            SStrPack(dest, "{", size);
            text = tagStart;
        }

        open = strchr(text, '{');
    }

    SStrPack(dest, text, size);
}

// OFFSET: 0x009F1660/0x14078B750
void CGChatBubbleFrame::Initialize(const WOWGUID& unit, const char* text, const CImVector& color) {
    char buffer[3000];
    char parsed[3000];
    StripHyperlinks(LanguageProcess(text), buffer, sizeof(buffer));
    this->m_fontString->SetVertexColor(color);
    this->ParseRaidIcons(buffer, parsed, sizeof(parsed));
    this->m_fontString->SetText(parsed, 1);
    this->m_fontString->SetWidth(0.0f);
    this->m_fontString->SetHeight(0.0f);

    if (s_locale == 1) {
        this->m_fontString->SetJustifyH(1);
    }

    float minWidth = NDCToDDCWidth(16.0f / (CoordinateGetAspectCompensation() * 1024.0f));
    minWidth += minWidth;
    float width = this->m_fontString->GetWidth();

    if (0.2f < width) {
        width = 0.2f;
    } else if (minWidth > width) {
        width = minWidth;
    }

    this->m_fontString->SetWidth(width);
    this->m_height = 0.0f;
    this->m_positionValid = 0;
    this->m_showTime = FrameTime::GetCurTimeMs();
    this->m_duration = ComputeDuration(buffer, unit == ClntObjMgrGetActivePlayer());
    this->m_bubbleFlags = 0x8;
    this->m_unit = unit;
    this->m_fadeAlpha = 0.0f;
    this->SetFrameAlpha(0);
    this->FadeIn();
}

// OFFSET: 0x009F1C20/0x14078BF20
void CGChatBubbleFrame::OnWorldRender(CGWorldFrame* worldFrame) {
    CGChatBubbleFrame* next;

    for (CGChatBubbleFrame* bubble = s_chatBubbles.Head(); bubble; bubble = next) {
        next = s_chatBubbles.Next(bubble);
        bubble->Render(worldFrame);
    }

    CLayoutFrame::ResizePending();
}

// OFFSET: 0x009F1870/0x14078B9D0
CGChatBubbleFrame::CGChatBubbleFrame(CSimpleFrame* parent) : CSimpleFrame(parent) {
    this->m_screenX = 0.0f;
    this->m_screenY = 0.0f;
    this->m_screenDepth = 0.0f;
    this->SetFrameFlag(0x40000, 1);

    void* data = SMemAlloc(sizeof(CBackdropGenerator), __FILE__, __LINE__, 0);
    CBackdropGenerator* backdrop = data ? new (data) CBackdropGenerator() : NULL;
    backdrop->m_bgFile.Copy("Interface\\Tooltips\\ChatBubble-Background");
    backdrop->m_pieces = 0xFF;
    backdrop->m_tile = 1;
    backdrop->m_edgeFile.Copy("Interface\\Tooltips\\ChatBubble-Backdrop");
    backdrop->m_alphaMode = GxBlend_Alpha;
    float inset = NDCToDDCWidth(16.0f / (CoordinateGetAspectCompensation() * 1024.0f));
    backdrop->m_insetLeft = inset;
    backdrop->m_insetRight = inset;
    backdrop->m_insetTop = inset;
    backdrop->m_insetBottom = inset;
    backdrop->m_edgeSize = NDCToDDCWidth(16.0f / (CoordinateGetAspectCompensation() * 1024.0f));
    backdrop->m_tileSize = NDCToDDCWidth(16.0f / (CoordinateGetAspectCompensation() * 1024.0f));
    this->SetBackdrop(backdrop);

    void* textureData = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    CSimpleTexture* tail = textureData ? new (textureData) CSimpleTexture(this, 2, 0, true) : NULL;
    tail->SetTexture("Interface\\Tooltips\\ChatBubble-Tail", 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
    tail->SetWidth(NDCToDDCWidth(16.0f / (CoordinateGetAspectCompensation() * 1024.0f)));
    tail->SetHeight(NDCToDDCWidth(16.0f / (CoordinateGetAspectCompensation() * 1024.0f)));
    tail->SetPoint(FRAMEPOINT_TOPRIGHT, this, FRAMEPOINT_BOTTOM, 0.0f, NDCToDDCWidth(4.0f / (CoordinateGetAspectCompensation() * 1024.0f)), 1);

    void* stringData = CSimpleFontString::s_allocator.GetData(0, typeid(CSimpleFontString).raw_name(), SLOG_OBJECT);
    this->m_fontString = stringData ? new (stringData) CSimpleFontString(this, 2, true) : NULL;
    this->m_fontString->SetFont(FrameScript_GetText("NAMEPLATE_FONT", -1, GENDER_NOT_APPLICABLE), 0.01f, 0, false);
    this->m_fontString->SetNonSpaceWrap(1);
    this->SetPoint(FRAMEPOINT_TOPLEFT, this->m_fontString, FRAMEPOINT_TOPLEFT, -0.01f, 0.01f, 1);
    this->SetPoint(FRAMEPOINT_BOTTOMRIGHT, this->m_fontString, FRAMEPOINT_BOTTOMRIGHT, 0.01f, -0.01f, 1);
}

// OFFSET: 0x009F1B60
void CGChatBubbleFrame::ShutdownGame() {
    s_freeChatBubbles.Clear();
    s_cameraPositionValid = 0;
}

// OFFSET: 0x009F1B80/0x14078BDF0
static void InsertSorted(CGChatBubbleFrame* bubble) {
    for (CGChatBubbleFrame* node = s_chatBubbles.Head(); node; node = s_chatBubbles.Next(node)) {
        if (static_cast<int32_t>(node->m_showTime - bubble->m_showTime) >= 0) {
            s_chatBubbles.LinkNode(bubble, 2, node);
            return;
        }
    }

    s_chatBubbles.LinkToTail(bubble);
}

// OFFSET: 0x009F1C90/0x14078C010
CGChatBubbleFrame* CGChatBubbleFrame::GetNewChatBubble(CSimpleFrame* parent) {
    CGChatBubbleFrame* bubble = s_freeChatBubbles.Head();

    if (!bubble) {
        void* data = CGChatBubbleFrame::s_chatBubbleHeap.GetData(0, typeid(CGChatBubbleFrame).raw_name(), SLOG_OBJECT);
        bubble = data ? new (data) CGChatBubbleFrame(parent) : NULL;
    }

    s_chatBubbles.LinkToTail(bubble);
    return bubble;
}

// OFFSET: 0x009F1C70/0x14078BF80
void CGChatBubbleFrame::Remove(CGChatBubbleFrame* bubble) {
    bubble->Hide();
    s_freeChatBubbles.LinkToTail(bubble);
}

// INLINED
void CGChatBubbleFrame::ClearAll() {
    CGChatBubbleFrame* next;

    for (CGChatBubbleFrame* bubble = s_chatBubbles.Head(); bubble; bubble = next) {
        next = s_chatBubbles.Next(bubble);
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(bubble->m_unit, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            unit->ChatBubbleRemove();
        } else {
            CGChatBubbleFrame::Remove(bubble);
        }
    }
}

// OFFSET: 0x009F1E80/0x14078C240
void CGChatBubbleFrame::OnWorldLayerUpdate() {
    if (!s_chatBubbles.Head()) {
        return;
    }

    C3Vector camera = StaticSingleton<CCameraManager>::Instance()->m_shot.m_position;
    bool cameraMoved = false;

    if (!s_cameraPositionValid || camera.x != s_cameraPosition.x || camera.y != s_cameraPosition.y || camera.z != s_cameraPosition.z) {
        cameraMoved = true;
        s_cameraPositionValid = 1;
        s_cameraPosition = camera;
    }

    TSList<CGChatBubbleFrame> moved;
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGChatBubbleFrame* next;

    for (CGChatBubbleFrame* bubble = s_chatBubbles.Head(); bubble; bubble = next) {
        next = s_chatBubbles.Next(bubble);
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(bubble->m_unit, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!unit) {
            CGChatBubbleFrame::Remove(bubble);
            continue;
        }

        if (!bubble->Update(player, unit)) {
            unit->ChatBubbleRemove();
            continue;
        }

        if ((bubble->m_bubbleFlags & 0x8) || cameraMoved) {
            moved.LinkToTail(bubble);
            bubble->m_bubbleFlags &= ~0x8;
        }
    }

    if (!moved.Head()) {
        return;
    }

    for (CGChatBubbleFrame* bubble = moved.Head(); bubble; bubble = next) {
        next = moved.Next(bubble);
        InsertSorted(bubble);
    }

    int32_t level = 2;

    for (CGChatBubbleFrame* bubble = s_chatBubbles.Head(); bubble; bubble = next) {
        next = s_chatBubbles.Next(bubble);
        bubble->SetFrameLevel(level++, 1);
    }
}

// INLINED
CGChatBubbleFrame::~CGChatBubbleFrame() {
}
