#include "Engine/Source/Frame/CSimpleTop.h"
#include <cmath>
#include <cstring>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/FFX/FFX.h"
#include "Engine/Source/BLPFile/Types.h"
#include "Engine/Source/Event/CCharEvent.h"
#include "Engine/Source/Event/CFocusEvent.h"
#include "Engine/Source/Event/CImeEvent.h"
#include "Engine/Source/Event/CKeyEvent.h"
#include "Engine/Source/Event/CMouseEvent.h"
#include "Engine/Source/Event/CSizeEvent.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Frame/CFrameStrataNode.h"
#include "Engine/Source/Frame/CSimpleFont.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Os/OsIME.h"
#include "Engine/Source/Scrn/Scrn.h"
#include "Engine/Source/Services/Camera.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Source/Game/GameClient/FFXEffects.h"

// STATIC: 0x00DD3904
CSimpleTop* CSimpleTop::s_instance;

// STATIC: 0x00DD3900
uint32_t CSimpleTop::s_eventTime;

// STATIC: 0x00DD3908
FFX::Effect* CSimpleTop::s_colorShiftEffect;

// STATIC: 0x00DD390C
int32_t CSimpleTop::s_colorblindMode;

// STATIC: 0x00CA2CF8
static float s_colorShiftMatrices[8][9] = {
    { 0.567f, 0.433f, 0.0f, 0.558f, 0.442f, 0.0f, 0.0f, 0.242f, 0.758f },
    { 0.817f, 0.183f, 0.0f, 0.333f, 0.667f, 0.0f, 0.0f, 0.125f, 0.875f },
    { 0.625f, 0.375f, 0.0f, 0.7f, 0.3f, 0.0f, 0.0f, 0.3f, 0.7f },
    { 0.8f, 0.2f, 0.0f, 0.258f, 0.742f, 0.0f, 0.0f, 0.142f, 0.858f },
    { 0.95f, 0.05f, 0.0f, 0.0f, 0.433f, 0.567f, 0.0f, 0.475f, 0.525f },
    { 0.967f, 0.033f, 0.0f, 0.0f, 0.733f, 0.267f, 0.0f, 0.183f, 0.817f },
    { 0.299f, 0.587f, 0.114f, 0.299f, 0.587f, 0.114f, 0.299f, 0.587f, 0.114f },
    { 0.618f, 0.32f, 0.062f, 0.163f, 0.775f, 0.062f, 0.163f, 0.32f, 0.516f }
};

// STATIC: 0x00CA2E18
static const char* s_colorblindModeNames[8] = {
    "Protanopia          (1% of males)",
    "Protanomaly         (1% of males, 0.01% of females)",
    "Deuteranopia        (1% of males)",
    "Deuteranomaly       (6% of males, 0.4% of females)",
    "Tritanopia          (rare)",
    "Tritanomaly         (rare)",
    "Achromatopsia       (very rare)",
    "Achromatomaly       (very rare)"
};

static TSList<UNEEDED_TEXTURES, TSGetLink<UNEEDED_TEXTURES> > s_textureReleaseQueue;

// OFFSET: 0x005017B0/0x140147290
static void PaintScreen(void* param, const RECTF* rect, const RECTF* visible, float elapsedSec) {
    CSimpleTop* top = CSimpleTop::s_instance;
    top->OnLayerUpdate(elapsedSec);
    top->OnLayerRender();
    CSimpleTop::PurgeTextureReleaseQueue();
    top->CheckLoadingObjectsQueue();

    if (CSimpleTop::s_colorblindMode) {
        FFX::Start();
        FFX::DoPass(2);
        FFX::Finish();
    }
}

// OFFSET: 0x005015F0
void CSimpleTop::QueueTextureRelease(CTexture* texture) {
    UNEEDED_TEXTURES* node = s_textureReleaseQueue.NewNode(2, 0, 0);
    node->m_texture = texture;
}

// OFFSET: 0x00501770/0x1401471E0
void CSimpleTop::PurgeTextureReleaseQueue() {
    UNEEDED_TEXTURES* node = s_textureReleaseQueue.Head();

    while (node) {
        TextureRelease(node->m_texture);
        node = s_textureReleaseQueue.DeleteNode(node);
    }
}

// OFFSET: 0x00500CB0/0x1401463A0
void CSimpleTop::CheckLoadingObjectsQueue() {
    CSimpleFrame* frame = this->m_loadingObjects.Head();

    while (frame) {
        CSimpleFrame* next = this->m_loadingObjects.Next(frame);

        if (frame->IsObjectLoaded()) {
            this->m_loadingObjects.UnlinkNode(frame);
            frame->SignalLoaded();
            frame->MarkDirty();
        }

        frame = next;
    }
}

// OFFSET: 0x00500D50/0x140146480
void CSimpleTop::ClearLoadingObjectsQueue() {
    CSimpleFrame* frame = this->m_loadingObjects.Head();

    while (frame) {
        CSimpleFrame* next = this->m_loadingObjects.Next(frame);
        this->m_loadingObjects.UnlinkNode(frame);
        frame = next;
    }
}

// OFFSET: 0x00500DC0/0x140146510
void CSimpleTop::UpdateLoadingQueue(CSimpleFrame* frame, bool add) {
    if (frame->m_loadLink.IsLinked()) {
        this->m_loadingObjects.UnlinkNode(frame);
    }

    if (add) {
        this->m_loadingObjects.LinkToTail(frame);
    }
}

// INLINED
CSimpleTop* CSimpleTop::GetInstance() {
    return CSimpleTop::s_instance;
}

// PROVEN: 0e6a22e0 IDENTICAL/NONE
// OFFSET: 0x00464880
void CSimpleTop::GetCursorPosition(C2Vector& position) {
    NDCToDDC(this->m_mousePosition.m_x, this->m_mousePosition.m_y, &position.x, &position.y);
}

// OFFSET: 0x004FEDD0/0x140143BD0
void CSimpleTop::StopMoveOrResizeFrame() {
    if (this->m_moveResizeFrame) {
        this->m_moveResizeFrame->UnflattenFrame(this);
    }

    this->m_moveResizeReason = MOVERESIZE_REASON_NONE;
    this->m_moveResizeFrame = NULL;
}

// PROVEN: f0370228 IDENTICAL/NONE
// OFFSET: 0x004FEE00/0x140143C10
void CSimpleTop::SetColorblindShader() {
    if (CSimpleTop::s_colorblindMode) {
        if (CSimpleTop::s_colorblindMode > 0 && static_cast<uint32_t>(CSimpleTop::s_colorblindMode) <= 8) {
            if (CSimpleTop::s_colorShiftEffect) {
                CSimpleTop::s_colorShiftEffect->SetParameters(9, reinterpret_cast<const int32_t*>(s_colorShiftMatrices[CSimpleTop::s_colorblindMode - 1]));
            }

            FFX::SetEffect(2, CSimpleTop::s_colorShiftEffect);
            return;
        }

        CSimpleTop::s_colorblindMode = 0;
    }

    FFX::SetEffect(2, NULL);
}

// OFFSET: 0x004FF0B0/0x140143E70
static int32_t ConsoleCommand_ColorblindShader(const char* command, const char* arguments) {
    if (arguments && *arguments) {
        int32_t mode = SStrToInt(arguments);

        if (mode < 0 || mode > 8) {
            ConsolePrintf("Invalid colorblind mode.");
            return 0;
        }

        CSimpleTop::s_colorblindMode = mode;
        CSimpleTop::SetColorblindShader();
        return 1;
    }

    ConsolePrintf("Colorblind Shader is %d.", CSimpleTop::s_colorblindMode);

    for (uint32_t i = 0; i < 8; i++) {
        ConsolePrintf("   %d - %s", i + 1, s_colorblindModeNames[i]);
    }

    return 1;
}

// PROVEN: 12f2bf1c IDENTICAL/DIFFERENT
// OFFSET: 0x004FF140/0x140143F10
void CSimpleTop::NotifyFrameLayerChanged(CSimpleFrame* frame, uint32_t drawLayer) {
    CFrameStrata* strata = this->m_strata[frame->m_strata];
    strata->m_levels[frame->m_level]->m_batchDirty |= 1 << drawLayer;
    strata->m_batchDirty = 1;
}

// PROVEN: 8d69f736 IDENTICAL/DIFFERENT
// OFFSET: 0x004FF180/0x140148BF0
void CSimpleTop::NotifyFrameDelayedLoad(CSimpleFrame* frame) {
    CFrameStrata* strata = this->m_strata[frame->m_strata];
    strata->m_levels[frame->m_level]->m_batchDirty |= 0x20;
    strata->m_batchDirty = 1;
}

// OFFSET: 0x004FF1C0
void CSimpleTop::SetCursor(MipBits* image) {
    if (image) {
        memcpy(g_theGxDevicePtr->CursorLock(), image->m_mip[0], sizeof(CGxDevice::s_cursor));
        g_theGxDevicePtr->CursorUnlock(0, 0);
        g_theGxDevicePtr->CursorSetVisible(1);
    }
}

// OFFSET: 0x004FF220
void CSimpleTop::MoveOrResizeFrame(const CMouseEvent& evt) {
    float offsetX = evt.m_data.m_mousePosition.x - this->m_moveResizeLast.x;
    float offsetY = evt.m_data.m_mousePosition.y - this->m_moveResizeLast.y;

    if (offsetX != 0.0f || offsetY != 0.0f) {
        this->m_moveResizeFrame->DragBy(offsetX, offsetY, this->m_moveResizePoint);
        this->m_moveResizeLast.x += offsetX;
        this->m_moveResizeLast.y += offsetY;
    }
}

// OFFSET: 0x004FF2C0/0x140143FE0
float CSimpleTop::RoundToPixelWidth(float ddcWidth) {
    if (fabs(ddcWidth) < 0.00000023841858f) {
        return ddcWidth;
    }

    CRect windowRect = { 0.0f, 0.0f, 0.0f, 0.0f };
    g_theGxDevicePtr->CapsWindowSizeInScreenCoords(windowRect);

    float windowWidth = windowRect.maxX - windowRect.minX;

    if (fabs(windowWidth) < 0.00000023841858f) {
        return ddcWidth;
    }

    int32_t pixels = static_cast<int32_t>(CMath::fuint_pi(DDCToNDCWidth(ddcWidth) * windowWidth));

    return NDCToDDCWidth(pixels / windowWidth);
}

// OFFSET: 0x004FF360/0x1401440D0
float CSimpleTop::RoundToPixelHeight(float ddcHeight) {
    if (fabs(ddcHeight) < 0.00000023841858f) {
        return ddcHeight;
    }

    CRect windowRect = { 0.0f, 0.0f, 0.0f, 0.0f };
    g_theGxDevicePtr->CapsWindowSizeInScreenCoords(windowRect);

    float windowHeight = windowRect.maxY - windowRect.minY;

    if (fabs(windowHeight) < 0.00000023841858f) {
        return ddcHeight;
    }

    int32_t pixels = static_cast<int32_t>(CMath::fuint_pi(DDCToNDCHeight(ddcHeight) * windowHeight));

    return NDCToDDCHeight(pixels / windowHeight);
}

// OFFSET: 0x004FF4D0/0x140144290
void CSimpleTop::HideFrame(CSimpleFrame* frame, int32_t a3) {
    if (this->m_moveResizeFrame == frame && !a3) {
        this->StopMoveOrResizeFrame();
    }

    frame->UnregisterForEvents(a3);

    CFrameStrata* strata = this->m_strata[frame->m_strata];

    if (frame->m_level < strata->m_levels.Count()) {
        strata->m_batchDirty |= strata->m_levels[frame->m_level]->RemoveFrame(frame);
        strata->m_levelsDirty = 1;
    }
}

// OFFSET: 0x004FF550/0x140144330
void CSimpleTop::SetMouseFocus(CSimpleFrame* frame) {
    CSimpleFrame* oldFocus = this->m_mouseFocus;

    if (frame != oldFocus) {
        this->m_mouseFocus = frame;

        if (oldFocus) {
            oldFocus->OnLayerCursorExit(0, 0);
        }

        if (frame) {
            frame->OnLayerCursorEnter(0);
        }
    }

    if (this->m_mouseCapture && this->m_mouseCapture != frame) {
        CMouseEvent evt;
        evt = this->m_mousePosition;
        evt.m_data.m_mousePosition.x = 0.0f;
        evt.m_data.m_mousePosition.y = 0.0f;
        evt.m_id = 0x400500C9;
        this->m_mouseCapture->OnLayerMouseUp(evt, NULL);
        this->m_mouseCapture = NULL;
    }
}

// OFFSET: 0x004FF610/0x140144460
int32_t CSimpleTop::StartMoveOrResizeFrame(CSimpleFrame* frame, MOVERESIZE_REASON reason, float x, float y, FRAMEPOINT point) {
    if (!frame->FlattenFrame(this, 0.0f, 0.0f, 0.0f, 0.0f)) {
        return 0;
    }

    frame->Raise();
    frame->SetFrameFlag(0x1000, true);

    this->m_moveResizeLast.x = x;
    this->m_moveResizeReason = reason;
    this->m_moveResizeLast.y = y;
    this->m_moveResizePoint = point;
    this->m_moveResizeFrame = frame;

    CMouseEvent evt;
    evt = this->m_mousePosition;
    this->MoveOrResizeFrame(evt);

    return 1;
}

// OFFSET: 0x004FF6D0/0x140144620
int32_t CSimpleTop::StartMoveOrResizeFrame(CSimpleFrame* frame, MOVERESIZE_REASON reason, float x, float y, int32_t resize) {
    FRAMEPOINT point = FRAMEPOINT_CENTER;

    if (resize) {
        if (frame->IsResizePending()) {
            frame->Resize(1);
        }

        CRect rect = { 0.0f, 0.0f, 0.0f, 0.0f };

        if (!frame->GetRect(&rect)) {
            return 0;
        }

        float offsetY = y - rect.minY;
        float quarterWidth = (rect.maxX - rect.minX) * 0.25f;
        float offsetX = x - rect.minX;
        float quarterHeight = (rect.maxY - rect.minY) * 0.25f;

        if (offsetX < quarterWidth) {
            if (offsetY < quarterHeight) {
                point = FRAMEPOINT_BOTTOMLEFT;
            } else if (quarterHeight * 3.0f <= offsetY) {
                point = FRAMEPOINT_TOPLEFT;
            } else {
                point = FRAMEPOINT_LEFT;
            }
        } else if (offsetX >= quarterWidth * 3.0f) {
            if (offsetY < quarterHeight) {
                point = FRAMEPOINT_BOTTOMRIGHT;
            } else if (3.0f * quarterHeight <= offsetY) {
                point = FRAMEPOINT_TOPRIGHT;
            } else {
                point = FRAMEPOINT_RIGHT;
            }
        } else if (offsetY < quarterHeight) {
            point = FRAMEPOINT_BOTTOM;
        } else if (3.0f * quarterHeight <= offsetY) {
            point = FRAMEPOINT_TOP;
        }
    }

    return this->StartMoveOrResizeFrame(frame, reason, x, y, point);
}

// OFFSET: 0x004FF810
int32_t CSimpleTop::OnChar(const void* data, void* param) {
    const EVENT_DATA_CHAR* charData = static_cast<const EVENT_DATA_CHAR*>(data);
    CSimpleTop* top = static_cast<CSimpleTop*>(param);
    int32_t handled = 0;

    CCharEvent evt;
    evt.m_data = *charData;
    evt.m_id = 0x40060067;

    for (int32_t strata = FRAME_STRATA_TOOLTIP; strata >= 0; strata--) {
        if (handled) {
            break;
        }

        CSimpleSortedArray<FRAMEPRIORITY*>& queue = top->m_eventqueue[SIMPLE_EVENT_CHAR][strata];
        queue.m_iterator = 0;

        while (queue.m_iterator < queue.m_count) {
            FRAMEPRIORITY** priority = &queue.m_array[queue.m_iterator++];

            if (!priority || handled) {
                break;
            }

            handled = (*priority)->m_frame->OnLayerChar(evt);
        }
    }

    return handled == 0;
}

// OFFSET: 0x004FF8B0
int32_t CSimpleTop::OnIme(const void* data, void* param) {
    const EVENT_DATA_IME* imeData = static_cast<const EVENT_DATA_IME*>(data);
    CSimpleTop* top = static_cast<CSimpleTop*>(param);
    int32_t handled = 0;

    CImeEvent evt;
    evt.m_data = *imeData;
    evt.m_id = 0x40060068;

    for (int32_t strata = FRAME_STRATA_TOOLTIP; strata >= 0; strata--) {
        if (handled) {
            break;
        }

        CSimpleSortedArray<FRAMEPRIORITY*>& queue = top->m_eventqueue[SIMPLE_EVENT_CHAR][strata];
        queue.m_iterator = 0;

        while (queue.m_iterator < queue.m_count) {
            FRAMEPRIORITY** priority = &queue.m_array[queue.m_iterator++];

            if (!priority || handled) {
                break;
            }

            handled = (*priority)->m_frame->OnLayerIme(evt);
        }
    }

    return handled == 0;
}

// OFFSET: 0x004FF950
int32_t CSimpleTop::OnKeyDown(const void* data, void* param) {
    const EVENT_DATA_KEY* keyData = static_cast<const EVENT_DATA_KEY*>(data);
    CSimpleTop* top = static_cast<CSimpleTop*>(param);
    int32_t handled = 0;

    CKeyEvent evt;
    evt.m_data = *keyData;
    CSimpleTop::s_eventTime = evt.m_data.m_time;
    evt.m_id = 0x40060064;

    if (top->m_keyDownCallback) {
        top->m_keyDownCallback(evt);
    }

    for (int32_t strata = FRAME_STRATA_TOOLTIP; strata >= 0; strata--) {
        if (handled) {
            break;
        }

        CSimpleSortedArray<FRAMEPRIORITY*>& queue = top->m_eventqueue[SIMPLE_EVENT_KEY][strata];
        queue.m_iterator = 0;

        while (queue.m_iterator < queue.m_count) {
            FRAMEPRIORITY** priority = &queue.m_array[queue.m_iterator++];

            if (!priority || handled) {
                break;
            }

            CSimpleFrame* frame = (*priority)->m_frame;
            handled = frame->OnLayerKeyDown(evt);

            if (handled) {
                top->m_keydownCapture[keyData->m_key] = frame;
            }
        }
    }

    return handled == 0;
}

// OFFSET: 0x004FFA20
int32_t CSimpleTop::OnKeyUp(const void* data, void* param) {
    const EVENT_DATA_KEY* keyData = static_cast<const EVENT_DATA_KEY*>(data);
    CSimpleTop* top = static_cast<CSimpleTop*>(param);
    CSimpleFrame* frame = top->m_keydownCapture[keyData->m_key];
    int32_t handled = 0;

    CSimpleTop::s_eventTime = keyData->m_time;

    if (frame) {
        CKeyEvent evt;
        evt.m_data = *keyData;
        evt.m_id = 0x40060066;
        frame->OnLayerKeyUp(evt);
        handled = 1;
    } else if (keyData->m_key == KEY_PRINTSCREEN) {
        handled = !CSimpleTop::OnKeyDown(data, param);
    }

    top->m_keydownCapture[keyData->m_key] = NULL;

    return !handled;
}

// OFFSET: 0x004FFAC0
int32_t CSimpleTop::OnKeyDownRepeat(const void* data, void* param) {
    const EVENT_DATA_KEY* keyData = static_cast<const EVENT_DATA_KEY*>(data);
    CSimpleTop* top = static_cast<CSimpleTop*>(param);
    CSimpleFrame* frame = top->m_keydownCapture[keyData->m_key];
    int32_t handled = 0;

    if (frame) {
        CKeyEvent evt;
        evt.m_data = *keyData;
        evt.m_id = 0x40060065;
        frame->OnLayerKeyDownRepeat(evt);
        handled = 1;
    }

    return handled == 0;
}

// OFFSET: 0x004FFB30/0x140144B90
int32_t CSimpleTop::OnMouseMove(const void* data, void* param) {
    const EVENT_DATA_MOUSE* mouseData = static_cast<const EVENT_DATA_MOUSE*>(data);
    CSimpleTop* top = static_cast<CSimpleTop*>(param);
    int32_t (*callback)(CMouseEvent*) = top->m_mouseMoveCallback;

    CMouseEvent evt;
    evt = *mouseData;

    CSimpleFrame* oldFocus = top->m_mouseFocus;
    evt.m_id = 0x400500CA;

    CSimpleFrame* newFocus = NULL;
    int32_t motion = mouseData != &top->m_mousePosition;

    if (motion) {
        if (mouseData->m_x == top->m_mousePosition.m_x && mouseData->m_y == top->m_mousePosition.m_y) {
            return 0;
        }

        CSimpleTop::s_eventTime = mouseData->m_time;
    }

    memcpy(&top->m_mousePosition, mouseData, sizeof(top->m_mousePosition));

    if (top->m_moveResizeFrame) {
        CMouseEvent moveEvt;
        moveEvt = *mouseData;
        top->MoveOrResizeFrame(moveEvt);
        return 0;
    }

    if (callback && callback(&evt)) {
        return 0;
    }

    if (top->m_mouseCapture) {
        top->m_mouseCapture->OnLayerTrackUpdate(evt);
    }

    for (int32_t strata = FRAME_STRATA_TOOLTIP; strata >= 0; strata--) {
        if (newFocus) {
            break;
        }

        CSimpleSortedArray<FRAMEPRIORITY*>& queue = top->m_eventqueue[SIMPLE_EVENT_MOUSE][strata];
        queue.m_iterator = 0;

        while (queue.m_iterator < queue.m_count) {
            FRAMEPRIORITY** priority = &queue.m_array[queue.m_iterator++];

            if (!priority) {
                break;
            }

            CSimpleFrame* frame = (*priority)->m_frame;

            if (frame->OnLayerTrackUpdate(evt)) {
                newFocus = frame;
                break;
            }
        }
    }

    if (newFocus != oldFocus) {
        top->m_mouseFocus = newFocus;

        if (oldFocus) {
            oldFocus->OnLayerCursorExit(motion, 0);
        }

        if (newFocus) {
            newFocus->OnLayerCursorEnter(motion);
        }
    }

    return newFocus == NULL;
}

// OFFSET: 0x004FFCD0
int32_t CSimpleTop::OnMouseMoveRelative(const void* data, void* param) {
    const EVENT_DATA_MOUSE* mouseData = static_cast<const EVENT_DATA_MOUSE*>(data);
    CSimpleTop* top = static_cast<CSimpleTop*>(param);
    int32_t (*callback)(CMouseEvent*) = top->m_mouseMoveCallback;

    CMouseEvent evt;
    evt = *mouseData;
    evt.m_id = 0x400500CB;
    CSimpleTop::s_eventTime = mouseData->m_time;

    return !callback || !callback(&evt);
}

// OFFSET: 0x004FFD40
int32_t CSimpleTop::OnMouseUp(const void* data, void* param) {
    const EVENT_DATA_MOUSE* mouseData = static_cast<const EVENT_DATA_MOUSE*>(data);
    CSimpleTop* top = static_cast<CSimpleTop*>(param);
    CSimpleFrame* capture = top->m_mouseCapture;
    int32_t (*callback)(CMouseEvent*) = top->m_mouseButtonCallback;
    int32_t handled = 0;

    CMouseEvent evt;
    evt = *mouseData;
    CSimpleTop::s_eventTime = mouseData->m_time;
    evt.m_id = 0x400500C9;

    if (top->m_moveResizeReason == MOVERESIZE_REASON_LAYOUT || top->m_moveResizeReason == MOVERESIZE_REASON_TITLE) {
        top->StopMoveOrResizeFrame();
        return 0;
    }

    if (callback && !top->m_mouseCapture && callback(&evt)) {
        return 0;
    }

    if (capture) {
        capture->OnLayerMouseUp(evt, NULL);
        handled = 1;

        if (!evt.m_data.m_buttonState) {
            top->m_mouseCapture = NULL;
            return 0;
        }
    }

    return handled == 0;
}

// OFFSET: 0x004FFE40
int32_t CSimpleTop::OnMouseWheel(const void* data, void* param) {
    const EVENT_DATA_MOUSE* mouseData = static_cast<const EVENT_DATA_MOUSE*>(data);
    CSimpleTop* top = static_cast<CSimpleTop*>(param);
    int32_t handled = 0;

    CMouseEvent evt;
    evt = *mouseData;

    C2Vector pt;
    CSimpleTop::s_eventTime = mouseData->m_time;
    evt.m_id = 0x400500CD;
    NDCToDDC(top->m_mousePosition.m_x, top->m_mousePosition.m_y, &pt.x, &pt.y);

    for (int32_t strata = FRAME_STRATA_TOOLTIP; strata >= 0; strata--) {
        if (handled) {
            break;
        }

        CSimpleSortedArray<FRAMEPRIORITY*>& queue = top->m_eventqueue[SIMPLE_EVENT_MOUSEWHEEL][strata];
        queue.m_iterator = 0;

        while (queue.m_iterator < queue.m_count) {
            FRAMEPRIORITY** priority = &queue.m_array[queue.m_iterator++];

            if (!priority || handled) {
                break;
            }

            CSimpleFrame* frame = (*priority)->m_frame;

            if (frame->TestHitRect(pt)) {
                handled = frame->OnLayerMouseWheel(evt);
            }
        }
    }

    return handled == 0;
}

// OFFSET: 0x004FFF30
int32_t CSimpleTop::OnFocusChanged(const void* data, void* param) {
    const EVENT_DATA_FOCUS* focusData = static_cast<const EVENT_DATA_FOCUS*>(data);
    CSimpleTop* top = static_cast<CSimpleTop*>(param);
    void (*callback)(const CFocusEvent&) = top->m_focusChangedCallback;

    OsIMESync();

    if (callback) {
        CFocusEvent evt;
        evt.m_data = *focusData;
        evt.m_id = 0x40040065;
        callback(evt);
    }

    return 1;
}

// OFFSET: 0x00500280/0x140148E80
CSimpleFrame* CFrameStrata::GetHitFrame(const C2Vector& pt) {
    for (uint32_t level = this->m_topLevel; level > 0;) {
        level--;
        CFrameStrataNode* node = this->m_levels[level];

        for (CSimpleFrame* frame = node->m_frames.Head(); frame; frame = node->m_frames.Next(frame)) {
            if (frame->TestHitRect(pt)) {
                while (!(frame->m_frameFlags & 1)) {
                    frame = frame->m_parent;

                    if (!frame) {
                        return NULL;
                    }
                }

                return frame;
            }
        }
    }

    return NULL;
}

// OFFSET: 0x00500330/0x140145640
int32_t CFrameStrata::FrameOccluded(CSimpleFrame* frame) {
    CRect frameRect = { 0.0f, 0.0f, 0.0f, 0.0f };
    CRect otherRect = { 0.0f, 0.0f, 0.0f, 0.0f };

    for (uint32_t level = frame->m_level; level < this->m_topLevel; level++) {
        CFrameStrataNode* node = this->m_levels[level];

        for (CSimpleFrame* other = node->m_frames.Head(); other; other = node->m_frames.Next(other)) {
            if (other == frame) {
                continue;
            }

            CSimpleFrame* parent = other->m_parent;

            while (parent && parent != frame) {
                parent = parent->m_parent;
            }

            if (parent) {
                continue;
            }

            frame->GetRect(&frameRect);
            other->GetRect(&otherRect);
            frameRect = frameRect & otherRect;

            if (frameRect.maxY > frameRect.minY && frameRect.maxX > frameRect.minX) {
                return 1;
            }
        }
    }

    return 0;
}

// OFFSET: 0x005004B0
void CFrameStrata::CheckOcclusion() {
    for (uint32_t level = 0; level < this->m_topLevel; level++) {
        CFrameStrataNode* node = this->m_levels[level];

        for (CSimpleFrame* frame = node->m_frames.Head(); frame; frame = node->m_frames.Next(frame)) {
            if (frame->m_frameFlags & 1) {
                frame->SetFrameFlag(0x10, this->FrameOccluded(frame));
            }
        }
    }
}

// OFFSET: 0x00500530
int32_t CFrameStrata::BuildBatches(int32_t noMouseCapture) {
    if (this->m_levelsDirty) {
        this->CheckOcclusion();
        this->m_levelsDirty = 0;
    }

    if (!this->m_batchDirty) {
        return this->m_batchDirty;
    }

    this->m_batchDirty = 0;

    for (uint32_t level = 0; level < this->m_topLevel; level++) {
        if (this->m_levels[level]->BuildBatches()) {
            this->m_batchDirty = 1;
        }
    }

    return this->m_batchDirty;
}

// PROVEN: 4339851a IDENTICAL/NONE
// OFFSET: 0x00500580
void CFrameStrata::RenderBatches() {
    for (uint32_t level = 0; level < this->m_topLevel; level++) {
        CFrameStrataNode* node = this->m_levels[level];

        for (CRenderBatch* batch = node->m_renderList.Head(), * nextBatch; !node->m_renderList.IsEnd(batch); batch = nextBatch) {
            nextBatch = node->m_renderList.RawNext(batch);
            CSimpleRender::DrawBatch(batch);
        }
    }
}

// OFFSET: 0x005005E0/0x140145960
void CSimpleTop::RegisterFrame(CSimpleFrame* frame) {
    this->m_frames.LinkToTail(frame);
}

// OFFSET: 0x005005F0/0x140145970
void CSimpleTop::UnregisterFrame(CSimpleFrame* frame) {
    if (this->m_moveResizeFrame == frame) {
        this->StopMoveOrResizeFrame();
    }

    frame->UnregisterForEvents(0);

    CFrameStrata* strata = this->m_strata[frame->m_strata];

    if (frame->m_level < strata->m_levels.Count()) {
        strata->m_batchDirty |= strata->m_levels[frame->m_level]->RemoveFrame(frame);
        strata->m_levelsDirty = 1;
    }

    this->m_frames.UnlinkNode(frame);
}

// OFFSET: 0x00500660
void CSimpleTop::RegisterForCleanup(CSimpleFrame* frame) {
    this->m_destroyed.LinkToTail(frame);
}

// OFFSET: 0x00500670/0x140145A70
void CSimpleTop::CompressStrata(int32_t strata) {
    if (strata < FRAME_STRATA_WORLD || strata > FRAME_STRATA_TOOLTIP) {
        return;
    }

    uint32_t topLevel = this->m_strata[strata]->m_topLevel;
    int32_t emptyLevel = -1;
    uint32_t level = 0;

    while (level < topLevel) {
        int32_t used = this->m_strata[strata]->m_levels[level]->m_frames.Head() != NULL;

        if (!used) {
            for (CSimpleFrame* frame = this->m_frames.Head(); frame; frame = this->m_frames.Next(frame)) {
                if (frame->m_strata == strata && frame->m_level == level) {
                    used = 1;
                    break;
                }
            }
        }

        if (!used) {
            if (emptyLevel == -1) {
                emptyLevel = level;
            }

            level++;
            continue;
        }

        if (emptyLevel == -1) {
            level++;
            continue;
        }

        uint32_t gap = level - emptyLevel;

        for (uint32_t moveLevel = level; moveLevel < topLevel; moveLevel++) {
            for (CSimpleFrame* frame = this->m_frames.Head(); frame; frame = this->m_frames.Next(frame)) {
                if (frame->m_strata == strata && frame->m_level == moveLevel) {
                    frame->SetFrameLevel(moveLevel - gap, 0);
                }
            }
        }

        topLevel -= gap;
        emptyLevel = -1;
    }

    if (emptyLevel != -1) {
        topLevel = emptyLevel;
    }

    this->m_strata[strata]->m_topLevel = topLevel;
}

// OFFSET: 0x005007B0/0x140145C30
int32_t CSimpleTop::RaiseFrame(CSimpleFrame* frame, int32_t checkOcclusion) {
    while (!(frame->m_frameFlags & 1)) {
        frame = frame->m_parent;

        if (!frame) {
            return 0;
        }
    }

    if (checkOcclusion) {
        if (!(frame->m_layoutFlags & 0x1) || frame->IsResizePending()) {
            frame->Resize(1);
        }

        frame->SetFrameFlag(0x10, this->m_strata[frame->m_strata]->FrameOccluded(frame));
    }

    if (frame->m_frameFlags & 0x10) {
        this->CompressStrata(frame->m_strata);
        frame->SetFrameLevel(this->m_strata[frame->m_strata]->m_topLevel, 1);
        this->m_checkFocus = 1;
    }

    return 1;
}

// OFFSET: 0x00500880/0x140145D30
int32_t CSimpleTop::StartMoveOrResizeFrame(const CMouseEvent& evt, MOVERESIZE_REASON reason, int32_t resize) {
    CSimpleFrame* frame = NULL;

    for (int32_t strata = FRAME_STRATA_FULLSCREEN_DIALOG; strata >= 0; strata--) {
        if (frame) {
            break;
        }

        C2Vector pt;
        pt.x = evt.m_data.m_mousePosition.x;
        pt.y = evt.m_data.m_mousePosition.y;
        frame = this->m_strata[strata]->GetHitFrame(pt);
    }

    if (!frame) {
        return 0;
    }

    if (resize) {
        if (!(frame->m_frameFlags & 0x200)) {
            return 0;
        }
    } else if (!(frame->m_frameFlags & 0x100)) {
        return 0;
    }

    return this->StartMoveOrResizeFrame(frame, reason, evt.m_data.m_mousePosition.x, evt.m_data.m_mousePosition.y, resize);
}

// OFFSET: 0x00500930/0x140145EC0
void CSimpleTop::OnLayerUpdate(float elapsedSec) {
    CSimpleFrame* frame;

    while ((frame = this->m_destroyed.Head()) != NULL) {
        this->m_destroyed.UnlinkNode(frame);
        frame->AllowAttributeChanges(0);
    }

    CLayoutFrame::ResizePending();

    for (int32_t strata = 0; strata < FRAME_STRATA_TOOLTIP + 1; strata++) {
        CFrameStrata* frameStrata = this->m_strata[strata];

        for (uint32_t level = 0; level < frameStrata->m_topLevel; level++) {
            frameStrata->m_levels[level]->OnLayerUpdate(elapsedSec);
        }
    }

    CLayoutFrame::ResizePending();

    if (this->m_checkFocus) {
        this->m_checkFocus = 0;
        CSimpleTop::OnMouseMove(&this->m_mousePosition, this);
    }
}

// OFFSET: 0x00500A30/0x140145FF0
void CSimpleTop::OnLayerRender() {
    C2Vector offset;
    CameraSetupScreenProjection(this->m_rect, offset, 0.0f, 0);

    for (int32_t strata = 0; strata < FRAME_STRATA_TOOLTIP + 1; strata++) {
        this->m_strata[strata]->BuildBatches(this->m_mouseCapture == NULL);
        this->m_strata[strata]->RenderBatches();
    }
}

// OFFSET: 0x00500AA0/0x1401460C0
int32_t CSimpleTop::OnMouseDown(const void* data, void* param) {
    const EVENT_DATA_MOUSE* mouseData = static_cast<const EVENT_DATA_MOUSE*>(data);
    CSimpleTop* top = static_cast<CSimpleTop*>(param);
    int32_t (*callback)(CMouseEvent*) = top->m_mouseButtonCallback;

    CMouseEvent evt;
    evt = *mouseData;
    CSimpleTop::s_eventTime = mouseData->m_time;
    evt.m_id = 0x400500C8;
    memcpy(&top->m_mousePosition, mouseData, sizeof(top->m_mousePosition));

    if (top->m_moveResizeEnabled && (EventIsKeyDown(KEY_LCONTROL) || EventIsKeyDown(KEY_LALT))) {
        top->StartMoveOrResizeFrame(evt, MOVERESIZE_REASON_LAYOUT, EventIsKeyDown(KEY_LCONTROL));
        return 0;
    }

    if (callback && !top->m_mouseCapture && callback(&evt)) {
        return 0;
    }

    CSimpleFrame* frame = top->m_mouseCapture;

    if (!frame) {
        frame = top->m_mouseFocus;

        if (!frame) {
            return 1;
        }
    }

    frame->Raise();

    if (frame->m_titleRegion) {
        C2Vector pt;
        pt.x = evt.m_data.m_mousePosition.x;
        pt.y = evt.m_data.m_mousePosition.y;

        if (frame->m_titleRegion->PtInFrameRect(pt)) {
            top->StartMoveOrResizeFrame(frame, MOVERESIZE_REASON_TITLE, evt.m_data.m_mousePosition.x, evt.m_data.m_mousePosition.y, FRAMEPOINT_CENTER);
            return 0;
        }
    }

    top->m_mouseCapture = frame;
    frame->OnLayerMouseDown(evt, NULL);

    return 0;
}

// OFFSET: 0x00500C20
int32_t CSimpleTop::OnDisplaySizeChanged(const void* data, void* param) {
    const EVENT_DATA_SIZE* sizeData = static_cast<const EVENT_DATA_SIZE*>(data);
    CSimpleTop* top = static_cast<CSimpleTop*>(param);
    int32_t (*callback)(const CSizeEvent&) = top->m_displaySizeCallback;

    GxuFontWindowSizeChanged();

    for (CSimpleFrame* frame = top->m_frames.Head(); frame; frame = top->m_frames.Next(frame)) {
        frame->OnScreenSizeChanged();
    }

    if (callback) {
        CSizeEvent evt;
        evt.m_data = *sizeData;
        evt.m_id = 0x40040064;
        callback(evt);
    }

    return 1;
}

// PROVEN: 8b249c1e IDENTICAL/DIFFERENT
// OFFSET: 0x00500F20/0x1401466C0
void CSimpleTop::NotifyFrameMovedOrResized(CSimpleFrame* frame) {
    this->m_strata[frame->m_strata]->m_levelsDirty = 1;

    if (this->m_moveResizeFrame) {
        this->RaiseFrame(this->m_moveResizeFrame, 0);
    }

    this->m_checkFocus = 1;
}

// PROVEN: ef5e7907 IDENTICAL/DIFFERENT
// OFFSET: 0x00500F70/0x140146720
void CSimpleTop::RegisterForEvent(CSimpleFrame* frame, CSimpleEventType event, int32_t a4, int32_t priority) {
    CSimpleSortedArray<FRAMEPRIORITY*>& queue = this->m_eventqueue[event][frame->m_strata];

    if (priority == -1) {
        priority = frame->m_level;
    }

    FRAMEPRIORITY* framePriority = static_cast<FRAMEPRIORITY*>(SMemAlloc(sizeof(FRAMEPRIORITY), __FILE__, __LINE__, 0));
    framePriority->m_frame = frame;
    framePriority->m_priority = priority;
    queue.Insert(framePriority);

    if (!a4 && event == SIMPLE_EVENT_MOUSE) {
        this->m_checkFocus = 1;
    }
}

// OFFSET: 0x00500FF0/0x1401467D0
void CSimpleTop::UnregisterForEvent(CSimpleFrame* frame, CSimpleEventType event, int32_t a4) {
    CSimpleSortedArray<FRAMEPRIORITY*>& queue = this->m_eventqueue[event][frame->m_strata];

    for (uint32_t i = 0; i < queue.m_count; i++) {
        FRAMEPRIORITY* framePriority = queue.m_array[i];

        if (framePriority->m_frame != frame) {
            continue;
        }

        queue.Remove(i);
        delete framePriority;

        if (a4) {
            return;
        }

        if (event == SIMPLE_EVENT_KEY) {
            for (uint32_t key = 0; key < sizeof(this->m_keydownCapture) / sizeof(this->m_keydownCapture[0]); key++) {
                if (this->m_keydownCapture[key] == frame) {
                    this->m_keydownCapture[key] = NULL;
                }
            }
        } else if (event == SIMPLE_EVENT_MOUSE) {
            if (frame == this->m_mouseCapture) {
                this->m_mouseCapture = NULL;
            }

            if (frame == this->m_mouseFocus) {
                this->m_mouseFocus = NULL;
                this->m_checkFocus = 1;

                if (frame->m_frameState != 3) {
                    frame->OnLayerCursorExit(0, 1);
                }
            }
        }

        return;
    }
}

// PROVEN: 01adf7ec IDENTICAL/DIFFERENT
// OFFSET: 0x005010F0/0x1401469C0
void CSimpleTop::EnableEvents() {
    EventRegisterEx(EVENT_ID_CHAR, &CSimpleTop::OnChar, this, 1.0f);
    EventRegisterEx(EVENT_ID_IME, &CSimpleTop::OnIme, this, 1.0f);
    EventRegisterEx(EVENT_ID_KEYDOWN, &CSimpleTop::OnKeyDown, this, 1.0f);
    EventRegisterEx(EVENT_ID_KEYUP, &CSimpleTop::OnKeyUp, this, 1.0f);
    EventRegisterEx(EVENT_ID_KEYDOWN_REPEATING, &CSimpleTop::OnKeyDownRepeat, this, 1.0f);
    EventRegisterEx(EVENT_ID_MOUSEMOVE, &CSimpleTop::OnMouseMove, this, 1.0f);
    EventRegisterEx(EVENT_ID_MOUSEMOVE_RELATIVE, &CSimpleTop::OnMouseMoveRelative, this, 1.0f);
    EventRegisterEx(EVENT_ID_MOUSEDOWN, &CSimpleTop::OnMouseDown, this, 1.0f);
    EventRegisterEx(EVENT_ID_MOUSEUP, &CSimpleTop::OnMouseUp, this, 1.0f);
    EventRegisterEx(EVENT_ID_MOUSEWHEEL, &CSimpleTop::OnMouseWheel, this, 1.0f);
    EventRegisterEx(EVENT_ID_SIZE, &CSimpleTop::OnDisplaySizeChanged, this, 1.0f);
    EventRegisterEx(EVENT_ID_FOCUS, &CSimpleTop::OnFocusChanged, this, 1.0f);
}

// PROVEN: e3e3d1c4 IDENTICAL/UNPROVEN
// OFFSET: 0x00501200/0x140146B20
void CSimpleTop::DisableEvents() {
    EventUnregisterEx(EVENT_ID_CHAR, &CSimpleTop::OnChar, this, 0xFFFFFFFF);
    EventUnregisterEx(EVENT_ID_IME, &CSimpleTop::OnIme, this, 0xFFFFFFFF);
    EventUnregisterEx(EVENT_ID_KEYDOWN, &CSimpleTop::OnKeyDown, this, 0xFFFFFFFF);
    EventUnregisterEx(EVENT_ID_KEYUP, &CSimpleTop::OnKeyUp, this, 0xFFFFFFFF);
    EventUnregisterEx(EVENT_ID_KEYDOWN_REPEATING, &CSimpleTop::OnKeyDownRepeat, this, 0xFFFFFFFF);
    EventUnregisterEx(EVENT_ID_MOUSEMOVE, &CSimpleTop::OnMouseMove, this, 0xFFFFFFFF);
    EventUnregisterEx(EVENT_ID_MOUSEMOVE_RELATIVE, &CSimpleTop::OnMouseMoveRelative, this, 0xFFFFFFFF);
    EventUnregisterEx(EVENT_ID_MOUSEDOWN, &CSimpleTop::OnMouseDown, this, 0xFFFFFFFF);
    EventUnregisterEx(EVENT_ID_MOUSEUP, &CSimpleTop::OnMouseUp, this, 0xFFFFFFFF);
    EventUnregisterEx(EVENT_ID_MOUSEWHEEL, &CSimpleTop::OnMouseWheel, this, 0xFFFFFFFF);
    EventUnregisterEx(EVENT_ID_SIZE, &CSimpleTop::OnDisplaySizeChanged, this, 0xFFFFFFFF);
    EventUnregisterEx(EVENT_ID_FOCUS, &CSimpleTop::OnFocusChanged, this, 0xFFFFFFFF);
}

// INLINED
CFrameStrata::CFrameStrata() {
    this->m_batchDirty = 0;
    this->m_levelsDirty = 0;
    this->m_topLevel = 0;
}

// OFFSET: 0x005014E0/0x140146F10
void CFrameStrata::AddFrame(CSimpleFrame* frame) {
    uint32_t level = frame->m_level;

    if (level >= this->m_levels.Count()) {
        uint32_t count = this->m_levels.Count();
        this->m_levels.SetCount(level + 1);

        for (; count <= level; count++) {
            this->m_levels[count] = new (SMemAlloc(sizeof(CFrameStrataNode), __FILE__, __LINE__, 0)) CFrameStrataNode;
        }
    }

    if (level >= this->m_topLevel) {
        this->m_topLevel = level + 1;
    }

    CFrameStrataNode* node = this->m_levels[level];

    if (!frame->m_strataLink.IsLinked()) {
        TSExplicitList<CSimpleFrame, offsetof(CSimpleFrame, m_strataLink)>* list = node->m_pendingFrame ? &node->m_pendingFrames : &node->m_frames;
        list->LinkToTail(frame);

        if (!(frame->m_frameFlags & 0x2000)) {
            node->m_batchDirty = -1;
        }
    }

    this->m_levelsDirty = 1;
    this->m_batchDirty |= node->m_batchDirty != 0;
}

// OFFSET: 0x00501800/0x1401472F0
CSimpleTop::CSimpleTop() {
    this->m_mouseFocus = NULL;
    this->m_mouseCapture = NULL;
    this->m_checkFocus = 1;
    this->m_mouseButtonName = NULL;
    this->m_mouseButtonCallback = NULL;
    this->m_mouseMoveCallback = NULL;
    this->m_displaySizeCallback = NULL;
    this->m_focusChangedCallback = NULL;
    this->m_keyDownCallback = NULL;
    this->m_allowProtectedActions = 1;
    this->unk_1260 = 0;
    this->m_actionBlockedCallback = NULL;

    CSimpleTop::s_instance = this;
    ConsoleCommandRegister("colorblindShader", ConsoleCommand_ColorblindShader, CATEGORY_DEBUG, NULL, NULL);
    CSimpleTop::s_colorShiftEffect = new (__FILE__, __LINE__) EffectColorShift();
    CSimpleTop::SetColorblindShader();
    ScrnLayerCreate(NULL, 1.0f, 4, NULL, &PaintScreen, &this->m_screenLayer);
    this->EnableEvents();

    memset(this->m_keydownCapture, 0, sizeof(this->m_keydownCapture));

    NDCToDDC(1.0f, 1.0f, &this->m_rect.maxX, &this->m_rect.maxY);
    this->m_layoutFlags |= 0x1;

    this->m_moveResizeEnabled = 0;
    this->m_moveResizeFrame = NULL;
    this->m_moveResizeReason = MOVERESIZE_REASON_NONE;
    this->m_moveResizePoint = FRAMEPOINT_TOPLEFT;
    this->m_moveResizeLast.x = 0.0f;
    this->m_moveResizeLast.y = 0.0f;

    for (int32_t strata = 0; strata < FRAME_STRATA_TOOLTIP + 1; strata++) {
        this->m_strata[strata] = new (SMemAlloc(sizeof(CFrameStrata), __FILE__, __LINE__, 0)) CFrameStrata;
    }

    this->m_mousePosition.m_mode = MOUSE_MODE_NORMAL;
    this->m_mousePosition.m_button = static_cast<MOUSEBUTTON>(0);
    this->m_mousePosition.m_buttonState = 0;
    this->m_mousePosition.m_metaKeyState = 0;
    this->m_mousePosition.m_flags = 0;
    this->m_mousePosition.m_x = 0.0f;
    this->m_mousePosition.m_y = 0.0f;
    this->m_mousePosition.m_wheelDistance = 0;
    this->m_mousePosition.m_time = 0;

    float x;
    float y;
    EventInputGetMousePosition(&x, &y);
    DDCToNDC(x, y, &this->m_mousePosition.m_x, &this->m_mousePosition.m_y);

    CSimpleRender::Init();
    CSimpleTexture::Init();
}

// PROVEN: c2171ee9 IDENTICAL/DIFFERENT
// OFFSET: 0x005015C0/0x140147030
void CSimpleTop::ShowFrame(CSimpleFrame* frame, int32_t a3) {
    this->m_strata[frame->m_strata]->AddFrame(frame);
    frame->RegisterForEvents(a3);
}

// OFFSET: 0x00501700/0x140147130
CFrameStrata::~CFrameStrata() {
    for (uint32_t level = 0; level < this->m_levels.Count(); level++) {
        delete this->m_levels[level];
        this->m_levels[level] = NULL;
    }
}

// OFFSET: [0x00501A90/0x1401475E0, 0x00501CF0/0x140147920]
CSimpleTop::~CSimpleTop() {
    CSimpleTop::PurgeTextureReleaseQueue();
    this->ClearLoadingObjectsQueue();

    CSimpleFrame* frame;

    while ((frame = this->m_destroyed.Head()) != NULL) {
        this->m_destroyed.UnlinkNode(frame);
    }

    while ((frame = this->m_frames.Head()) != NULL) {
        this->m_frames.UnlinkNode(frame);
        delete frame;
    }

    for (int32_t strata = 0; strata < FRAME_STRATA_TOOLTIP + 1; strata++) {
        delete this->m_strata[strata];
        this->m_strata[strata] = NULL;
    }

    CSimpleFontString::FreeEmbededTextureStorage();
    CSimpleFont::Destroy();
    CSimpleTexture::Destroy();
    CSimpleRender::Destroy();

    this->DisableEvents();
    HandleClose(reinterpret_cast<HOBJECT>(this->m_screenLayer));
    ConsoleCommandUnregister("colorblindShader");
    FFX::SetEffect(2, NULL);

    if (CSimpleTop::s_colorShiftEffect) {
        delete CSimpleTop::s_colorShiftEffect;
    }

    CSimpleTop::s_colorShiftEffect = NULL;
    CSimpleTop::s_instance = NULL;
}

// PROVEN: b2ac43a5 IDENTICAL/DIFFERENT
// OFFSET: 0x00A9AA30/0x1402123D0
int32_t CSimpleTop::LowerFrame(CSimpleFrame* frame) {
    return 0;
}

// PROVEN: 0cc3f3ae IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CSimpleTop::ValidateDeletedFrame(CSimpleFrame* frame) {
}
