#include "WoW/Source/Ui/UILCD/Windows/LCDConnection.h"
#include <cstring>
#include "WoW/Source/Ui/UILCD/Windows/LCDGfx.h"
#include "WoW/Source/Ui/UILCD/Windows/LCDOutput.h"

// STATIC: 0x0100AAC8
LONG CLCDConnection::g_lInitCount;

// OFFSET: 0x00B3F460
CLCDConnection::CLCDConnection() {
    this->m_hConnection = LGLCD_INVALID_CONNECTION;
    this->m_pColorOutput = NULL;
    this->m_pGfxColor = NULL;
    this->m_pMonoOutput = NULL;
    this->m_pGfxMono = NULL;
    this->unk_0040 = 0;
    this->m_pSoftbuttonsChangedCtx = NULL;
    InitializeCriticalSection(&this->m_csCallback);
}

// OFFSET: [0x00B3F4B0/0x14090CE40, 0x00B3F580/0x14090CF60]
CLCDConnection::~CLCDConnection() {
    this->Disconnect();

    if (this->m_pMonoOutput) {
        delete this->m_pMonoOutput;
        this->m_pMonoOutput = NULL;
    }

    if (this->m_pColorOutput) {
        delete this->m_pColorOutput;
        this->m_pColorOutput = NULL;
    }

    if (this->m_pGfxMono) {
        delete this->m_pGfxMono;
        this->m_pGfxMono = NULL;
    }

    if (this->m_pGfxColor) {
        delete this->m_pGfxColor;
        this->m_pGfxColor = NULL;
    }
}

// OFFSET: 0x00B3F830/0x14090D300
int32_t CLCDConnection::InitializeConnection(lgLcdConnectContextEx& connectContext, lgLcdSoftbuttonsChangedContext* softbuttonsChangedContext) {
    if (!connectContext.dwAppletCapabilitiesSupported || (connectContext.dwAppletCapabilitiesSupported & LGLCD_APPLET_CAP_BW)) {
        this->m_pMonoOutput = this->AllocMonoOutput();
        this->m_pGfxMono = new CLCDGfxMono();
        this->m_pGfxMono->Initialize();
        this->m_pMonoOutput->SetGfx(this->m_pGfxMono);
    }

    if (connectContext.dwAppletCapabilitiesSupported & LGLCD_APPLET_CAP_QVGA) {
        this->m_pColorOutput = this->AllocColorOutput();
        this->m_pGfxColor = new CLCDGfxColor();
        this->m_pGfxColor->Initialize();
        this->m_pColorOutput->SetGfx(this->m_pGfxColor);
    }

    if (InterlockedIncrement(&CLCDConnection::g_lInitCount) == 1 && lgLcdInit() != ERROR_SUCCESS) {
        InterlockedDecrement(&CLCDConnection::g_lInitCount);
        return 0;
    }

    memset(&this->m_lcdConnectCtxEx, 0, sizeof(this->m_lcdConnectCtxEx));
    this->m_lcdConnectCtxEx.onNotify.notificationCallback = NULL;
    this->m_lcdConnectCtxEx.appFriendlyName = NULL;
    this->m_lcdConnectCtxEx.isPersistent = FALSE;
    this->m_lcdConnectCtxEx.isAutostartable = FALSE;
    this->m_lcdConnectCtxEx.dwAppletCapabilitiesSupported = 0;
    this->m_lcdConnectCtxEx.dwReserved1 = 0;
    this->m_lcdConnectCtxEx.onNotify.notifyContext = NULL;
    this->m_lcdConnectCtxEx.connection = LGLCD_INVALID_CONNECTION;
    memcpy(&this->m_lcdConnectCtxEx, &connectContext, sizeof(this->m_lcdConnectCtxEx));
    this->m_pSoftbuttonsChangedCtx = softbuttonsChangedContext;

    if (!this->m_lcdConnectCtxEx.onNotify.notificationCallback) {
        this->m_lcdConnectCtxEx.onNotify.notificationCallback = CLCDConnection::_OnNotificationCallback;
        this->m_lcdConnectCtxEx.onNotify.notifyContext = this;
    }

    this->ConnectEx();

    if (this->m_hConnection != LGLCD_INVALID_CONNECTION) {
        Sleep(50);
        this->Update();
    }

    return 1;
}

// OFFSET: 0x00B3F150/0x14090CA10
void CLCDConnection::Shutdown() {
    this->Disconnect();

    if (!InterlockedDecrement(&CLCDConnection::g_lInitCount)) {
        lgLcdDeInit();
    }

    this->m_pSoftbuttonsChangedCtx = NULL;
}

// PROVEN: 02e1b70c IDENTICAL/NONE
// OFFSET: 0x00B3F200/0x14090CAD0
int32_t CLCDConnection::IsConnected() {
    return this->m_hConnection != LGLCD_INVALID_CONNECTION;
}

// PROVEN: ea9fc070 IDENTICAL/NONE
// OFFSET: 0x00B3F1C0/0x14090CA90
void CLCDConnection::Disconnect() {
    if (this->m_pColorOutput) {
        this->m_pColorOutput->Close();
    }

    if (this->m_pMonoOutput) {
        this->m_pMonoOutput->Close();
    }

    if (this->m_hConnection != LGLCD_INVALID_CONNECTION) {
        lgLcdDisconnect(this->m_hConnection);
        this->m_hConnection = LGLCD_INVALID_CONNECTION;
    }
}

// OFFSET: [0x00B3F440/0x14090CD90]
CLCDOutput* CLCDConnection::AllocMonoOutput() {
    return new CLCDOutput();
}

// OFFSET: [0x00B3F440/0x14090CD90]
CLCDOutput* CLCDConnection::AllocColorOutput() {
    return new CLCDOutput();
}

// PROVEN: 44a25712 IDENTICAL/NONE
// OFFSET: 0x00B3F220/0x14090CB00
CLCDOutput* CLCDConnection::MonoOutput() {
    return this->m_pMonoOutput;
}

// PROVEN: 46da3199 IDENTICAL/IDENTICAL
// OFFSET: 0x00529DA0/0x140512ED0
CLCDOutput* CLCDConnection::ColorOutput() {
    return this->m_pColorOutput;
}

// OFFSET: 0x00B3F180/0x14090CA40
void CLCDConnection::ConnectEx() {
    this->Disconnect();

    if (this->m_hConnection != LGLCD_INVALID_CONNECTION) {
        return;
    }

    if (lgLcdConnectEx(&this->m_lcdConnectCtxEx) == ERROR_SUCCESS) {
        this->m_hConnection = this->m_lcdConnectCtxEx.connection;
    } else {
        this->m_hConnection = LGLCD_INVALID_CONNECTION;
    }
}

// PROVEN: 60a692cf IDENTICAL/NONE
// OFFSET: 0x00B3F210/0x14050FC70
int32_t CLCDConnection::GetConnectionId() {
    return this->m_hConnection;
}

// OFFSET: 0x00B3F230/0x14090CB10
void CLCDConnection::OnDeviceRemoval(uint32_t displayType) {
    CLCDOutput** output;

    if (displayType == LGLCD_DEVICE_BW) {
        output = &this->m_pMonoOutput;
    } else if (displayType == LGLCD_DEVICE_QVGA) {
        output = &this->m_pColorOutput;
    } else {
        return;
    }

    if (*output) {
        (*output)->ResetDeviceType();
        (*output)->Close();
    }
}

// OFFSET: 0x00B3F260/0x14090CB50
void CLCDConnection::OnNotification(uint32_t notification, uint32_t param1) {
    switch (notification) {
        case LGLCD_NOTIFICATION_DEVICE_ARRIVAL:
            this->OnDeviceArrival(param1);
            break;

        case LGLCD_NOTIFICATION_DEVICE_REMOVAL:
            this->OnDeviceRemoval(param1);
            break;

        case LGLCD_NOTIFICATION_CLOSE_CONNECTION:
            this->Disconnect();
            break;

        case LGLCD_NOTIFICATION_APPLET_DISABLED:
            this->OnAppletDisabled();
            break;

        case LGLCD_NOTIFICATION_APPLET_ENABLED:
            this->OnAppletEnabled();
            break;
    }
}

// OFFSET: 0x00B3F2D0/0x14090CBB0
void CLCDConnection::OnSoftButtonEvent(int32_t device, uint32_t buttons) {
    if (this->m_pMonoOutput && device == this->m_pMonoOutput->GetDeviceId()) {
        this->m_pMonoOutput->OnSoftButtonEvent(buttons);
    } else if (this->m_pColorOutput && device == this->m_pColorOutput->GetDeviceId()) {
        this->m_pColorOutput->OnSoftButtonEvent(buttons);
    }
}

// PROVEN: fca4e27e IDENTICAL/NONE
// OFFSET: 0x00B3F330/0x14090CC10
void CLCDConnection::FreeMonoOutput() {
    if (this->m_pMonoOutput) {
        delete this->m_pMonoOutput;
        this->m_pMonoOutput = NULL;
    }
}

// PROVEN: cfb52040 IDENTICAL/NONE
// OFFSET: 0x00B3F350/0x14090CC40
void CLCDConnection::FreeColorOutput() {
    if (this->m_pColorOutput) {
        delete this->m_pColorOutput;
        this->m_pColorOutput = NULL;
    }
}

// OFFSET: 0x00B3F370/0x14090CC70
int32_t CLCDConnection::Initialize(lgLcdConnectContext& connectContext, lgLcdSoftbuttonsChangedContext* softbuttonsChangedContext) {
    this->m_pMonoOutput = new CLCDOutput();
    this->m_pGfxMono = new CLCDGfxMono();
    this->m_pGfxMono->Initialize();
    this->m_pMonoOutput->SetGfx(this->m_pGfxMono);

    lgLcdConnectContextEx connectContextEx;
    memset(&connectContextEx, 0, sizeof(connectContextEx));
    connectContextEx.appFriendlyName = connectContext.appFriendlyName;
    connectContextEx.isPersistent = connectContext.isPersistent;
    connectContextEx.isAutostartable = connectContext.isAutostartable;
    connectContextEx.onConfigure = connectContext.onConfigure;
    connectContextEx.connection = LGLCD_INVALID_CONNECTION;
    connectContextEx.dwAppletCapabilitiesSupported = LGLCD_APPLET_CAP_BW;
    return this->InitializeConnection(connectContextEx, softbuttonsChangedContext);
}

// PROVEN: 7857dcee IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CLCDConnection::OnAppletEnabled() {
}

// PROVEN: 7480ab95 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CLCDConnection::OnAppletDisabled() {
}

// PROVEN: 840872af IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CLCDConnection::OnConfigure() {
}

// PROVEN: 2b1b6a6a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CLCDConnection::OnCallbackEvent() {
}

// OFFSET: 0x00B3F5B0/0x14090CFB0
DWORD WINAPI CLCDConnection::_OnNotificationCallback(int connection, const PVOID pContext, DWORD notificationCode, DWORD notifyParm1, DWORD notifyParm2, DWORD notifyParm3, DWORD notifyParm4) {
    CLCDConnection* self = static_cast<CLCDConnection*>(pContext);

    EnterCriticalSection(&self->m_csCallback);
    CB_EVENT_NODE* node = self->m_callbackEvents.NewNode(2, 0, 0);
    node->m_event.m_code = notificationCode;
    node->m_event.m_type = 2;
    node->m_event.m_param1 = notifyParm1;
    node->m_event.m_param2 = notifyParm2;
    LeaveCriticalSection(&self->m_csCallback);

    self->OnCallbackEvent();
    return 0;
}

// OFFSET: 0x00B3F670/0x14090D0C0
DWORD WINAPI CLCDConnection::_OnSoftButtonsCallback(int device, DWORD buttons, const PVOID context) {
    CLCDConnection* self = static_cast<CLCDConnection*>(context);

    EnterCriticalSection(&self->m_csCallback);
    CB_EVENT_NODE* node = self->m_callbackEvents.NewNode(2, 0, 0);
    node->m_event.m_code = device;
    node->m_event.m_type = 0;
    node->m_event.m_param1 = buttons;
    LeaveCriticalSection(&self->m_csCallback);

    self->OnCallbackEvent();
    return 0;
}

// OFFSET: 0x00B3F730/0x14090D1D0
void CLCDConnection::OnDeviceArrival(uint32_t displayType) {
    CLCDOutput** output;

    if (displayType == LGLCD_DEVICE_BW) {
        output = &this->m_pMonoOutput;
    } else if (displayType == LGLCD_DEVICE_QVGA) {
        output = &this->m_pColorOutput;
    } else {
        return;
    }

    if (!*output) {
        return;
    }

    lgLcdOpenByTypeContext context;
    memset(&context, 0, sizeof(context));
    context.connection = this->m_hConnection;
    context.deviceType = displayType;
    context.onSoftbuttonsChanged.softbuttonsChangedCallback = CLCDConnection::_OnSoftButtonsCallback;
    context.onSoftbuttonsChanged.softbuttonsChangedContext = this;
    context.device = LGLCD_INVALID_DEVICE;

    if (this->m_pSoftbuttonsChangedCtx) {
        context.onSoftbuttonsChanged = *this->m_pSoftbuttonsChangedCtx;
    }

    (*output)->OpenByType(context);
}

// OFFSET: 0x00B3F990
int32_t CLCDConnection::PopEvent(CB_EVENT& event) {
    memset(&event, 0, sizeof(event));
    EnterCriticalSection(&this->m_csCallback);
    CB_EVENT_NODE* node = this->m_callbackEvents.Head();

    if (!node) {
        LeaveCriticalSection(&this->m_csCallback);
        return 0;
    }

    event = node->m_event;
    this->m_callbackEvents.DeleteNode(node);
    LeaveCriticalSection(&this->m_csCallback);
    return 1;
}

// OFFSET: 0x00B3FA20/0x14090D4D0
void CLCDConnection::Update() {
    if (this->m_hConnection == LGLCD_INVALID_CONNECTION) {
        this->ConnectEx();
    }

    CB_EVENT event;

    while (this->PopEvent(event)) {
        if (event.m_type == 0) {
            this->OnSoftButtonEvent(event.m_code, event.m_param1);
        } else if (event.m_type == 1) {
            this->OnConfigure();
        } else if (event.m_type == 2) {
            this->OnNotification(event.m_code, event.m_param1);
        }
    }

    for (int32_t i = 0; i < 2; i++) {
        CLCDOutput** output = &this->m_pMonoOutput;

        if (i) {
            output = &this->m_pColorOutput;
        }

        if (!*output) {
            continue;
        }

        if ((*output)->IsOpened()) {
            (*output)->OnUpdate(GetTickCount());
            (*output)->Draw();
        }

        if (!(*output)->IsOpened() && (*output)->HasDeviceType()) {
            (*output)->ReOpenDeviceType();
        }
    }
}
