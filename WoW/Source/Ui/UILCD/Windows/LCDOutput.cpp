#include "WoW/Source/Ui/UILCD/Windows/LCDOutput.h"
#include "WoW/Source/Ui/UILCD/Windows/LCDGfx.h"
#include "WoW/Source/Ui/UILCD/Windows/LCDPage.h"

// OFFSET: #0x00959C80
CLCDOutput::CLCDOutput() {
    this->m_pActivePage = NULL;
    this->m_pGlobalPage = NULL;
    this->m_dwButtonState = 0;
    this->m_pGfx = NULL;
    this->m_hDevice = LGLCD_INVALID_DEVICE;
    this->m_bSetAsForeground = 1;
    this->m_nPriority = LGLCD_PRIORITY_NORMAL;
    this->m_OpenByTypeContext.connection = 0;
    this->m_OpenByTypeContext.deviceType = 0;
    this->m_OpenByTypeContext.onSoftbuttonsChanged.softbuttonsChangedCallback = NULL;
    this->m_OpenByTypeContext.onSoftbuttonsChanged.softbuttonsChangedContext = NULL;
    this->m_OpenByTypeContext.device = 0;
}

// OFFSET: #0x0095A290
CLCDOutput::~CLCDOutput() {
    this->Close();
}

// OFFSET: #0x00959D20
void CLCDOutput::Close() {
    if (this->m_hDevice != LGLCD_INVALID_DEVICE) {
        this->OnClosingDevice(this->m_hDevice);
        lgLcdClose(this->m_hDevice);
        this->m_hDevice = LGLCD_INVALID_DEVICE;
    }
}

// OFFSET: #0x00959D20
void CLCDOutput::Shutdown() {
    this->Close();
}

// PROVEN: afa37274 IDENTICAL/NONE
// OFFSET: 0x00B3EAB0/0x14090C250
void CLCDOutput::SetGfx(CLCDGfxBase* gfx) {
    this->m_pGfx = gfx;
}

// PROVEN: 18bc7044 IDENTICAL/NONE
// OFFSET: 0x00B3EAF0
void CLCDOutput::SetGlobalPage(CLCDPage* page) {
    this->m_pGlobalPage = page;
}

// PROVEN: 8a0be9c0 IDENTICAL/NONE
// OFFSET: 0x00B3EB00/0x14090C2A0
int32_t CLCDOutput::IsOpened() {
    return this->m_hDevice != LGLCD_INVALID_DEVICE;
}

// OFFSET: #0x00959D70
HRESULT CLCDOutput::HandleErrorFromAPI(DWORD result) {
    switch (result) {
        case ERROR_SUCCESS:
            return S_OK;

        case ERROR_DEVICE_NOT_CONNECTED:
            if (this->m_hDevice != LGLCD_INVALID_DEVICE) {
                this->OnClosingDevice(this->m_hDevice);
                lgLcdClose(this->m_hDevice);
                this->m_hDevice = LGLCD_INVALID_DEVICE;
            }

            break;

        case RPC_S_PROTOCOL_ERROR:
            break;

        default:
            this->Close();
            break;
    }

    return static_cast<HRESULT>(result | 0x80000000);
}

// PROVEN: 91010f88 IDENTICAL/NONE
// OFFSET: 0x005EB640/0x14026CD90
int32_t CLCDOutput::DoesBitmapNeedUpdate(lgLcdBitmapHeader* bitmap) {
    return 1;
}

// PROVEN: 0b53931f IDENTICAL/NONE
// OFFSET: 0x00B3EB80/0x14090C320
void CLCDOutput::OnLCDButtonDown(int32_t button) {
    if (this->m_pActivePage) {
        this->m_pActivePage->OnLCDButtonDown(button);
    }
}

// PROVEN: 713f8e8c IDENTICAL/NONE
// OFFSET: 0x00B3EBA0/0x14090C340
void CLCDOutput::OnLCDButtonUp(int32_t button) {
    if (this->m_pActivePage) {
        this->m_pActivePage->OnLCDButtonUp(button);
    }
}

// OFFSET: #0x00959E30
int32_t CLCDOutput::GetDeviceId() {
    return this->m_hDevice;
}

// PROVEN: 9a34be3a IDENTICAL/NONE
// OFFSET: 0x00B3EBC0/0x14090C370
void CLCDOutput::ResetDeviceType() {
    this->m_OpenByTypeContext.deviceType = 0;
}

// PROVEN: b0477467 IDENTICAL/NONE
// OFFSET: 0x00B3EBD0
int32_t CLCDOutput::HasDeviceType() {
    return this->m_OpenByTypeContext.deviceType != 0;
}

// OFFSET: #0x00959E60
int32_t CLCDOutput::OpenByType(lgLcdOpenByTypeContext& context) {
    if (this->m_hDevice != LGLCD_INVALID_DEVICE) {
        this->OnClosingDevice(this->m_hDevice);
        lgLcdClose(this->m_hDevice);
        this->m_hDevice = LGLCD_INVALID_DEVICE;
    }

    if (lgLcdOpenByType(&context)) {
        return 0;
    }

    this->m_hDevice = context.device;
    this->m_dwButtonState = 0;

    if (this->m_hDevice != LGLCD_INVALID_DEVICE) {
        lgLcdSetAsLCDForegroundApp(this->m_hDevice, this->m_bSetAsForeground);
    }

    this->m_OpenByTypeContext = context;
    this->OnOpenedDevice(this->m_hDevice);
    return 1;
}

// OFFSET: #0x00959F00
int32_t CLCDOutput::ReOpenDeviceType() {
    if (this->m_OpenByTypeContext.deviceType) {
        return this->OpenByType(this->m_OpenByTypeContext);
    }

    return 0;
}

// OFFSET: #0x00959F20
void CLCDOutput::ShowPage(CLCDPage* page, int32_t activate) {
    if (activate) {
        this->m_pActivePage = page;

        if (this->m_hDevice != LGLCD_INVALID_DEVICE) {
            lgLcdSetAsLCDForegroundApp(this->m_hDevice, this->m_bSetAsForeground);
        }

        this->OnPageShown(page);
    } else {
        page->SetExpiration(0);
        this->OnUpdate(GetTickCount());
    }
}

// OFFSET: #0x00959F80
int32_t CLCDOutput::Draw() {
    uint32_t priority = this->m_nPriority;

    if (!this->m_pActivePage || this->m_hDevice == LGLCD_INVALID_DEVICE || !priority) {
        return 1;
    }

    this->m_pGfx->BeginDraw();
    this->m_pGfx->ClearScreen();
    this->m_pActivePage->OnDraw(*this->m_pGfx);

    if (this->m_pGlobalPage) {
        this->m_pGlobalPage->OnDraw(*this->m_pGfx);
    }

    this->m_pGfx->EndDraw();
    lgLcdBitmapHeader* bitmap = this->m_pGfx->GetLCDScreen();

    if (this->DoesBitmapNeedUpdate(bitmap)) {
        this->HandleErrorFromAPI(lgLcdUpdateBitmap(this->m_hDevice, bitmap, priority));
    }

    return this->m_hDevice != LGLCD_INVALID_DEVICE;
}

// PROVEN: 6a0d9e2e IDENTICAL/NONE
// OFFSET: 0x00B3EDB0/0x14090C5D0
void CLCDOutput::OnSoftButtonEvent(uint32_t buttons) {
    if (this->m_pGfx->GetFamily() == LGLCD_DEVICE_FAMILY_QVGA_BASIC) {
        if ((this->m_dwButtonState & LGLCDBUTTON_LEFT) && !(buttons & LGLCDBUTTON_LEFT)) {
            this->OnLCDButtonUp(LGLCDBUTTON_LEFT);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_LEFT) && (buttons & LGLCDBUTTON_LEFT)) {
            this->OnLCDButtonDown(LGLCDBUTTON_LEFT);
        }

        if ((this->m_dwButtonState & LGLCDBUTTON_RIGHT) && !(buttons & LGLCDBUTTON_RIGHT)) {
            this->OnLCDButtonUp(LGLCDBUTTON_RIGHT);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_RIGHT) && (buttons & LGLCDBUTTON_RIGHT)) {
            this->OnLCDButtonDown(LGLCDBUTTON_RIGHT);
        }

        if ((this->m_dwButtonState & LGLCDBUTTON_OK) && !(buttons & LGLCDBUTTON_OK)) {
            this->OnLCDButtonUp(LGLCDBUTTON_OK);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_OK) && (buttons & LGLCDBUTTON_OK)) {
            this->OnLCDButtonDown(LGLCDBUTTON_OK);
        }

        if ((this->m_dwButtonState & LGLCDBUTTON_CANCEL) && !(buttons & LGLCDBUTTON_CANCEL)) {
            this->OnLCDButtonUp(LGLCDBUTTON_CANCEL);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_CANCEL) && (buttons & LGLCDBUTTON_CANCEL)) {
            this->OnLCDButtonDown(LGLCDBUTTON_CANCEL);
        }

        if ((this->m_dwButtonState & LGLCDBUTTON_UP) && !(buttons & LGLCDBUTTON_UP)) {
            this->OnLCDButtonUp(LGLCDBUTTON_UP);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_UP) && (buttons & LGLCDBUTTON_UP)) {
            this->OnLCDButtonDown(LGLCDBUTTON_UP);
        }

        if ((this->m_dwButtonState & LGLCDBUTTON_DOWN) && !(buttons & LGLCDBUTTON_DOWN)) {
            this->OnLCDButtonUp(LGLCDBUTTON_DOWN);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_DOWN) && (buttons & LGLCDBUTTON_DOWN)) {
            this->OnLCDButtonDown(LGLCDBUTTON_DOWN);
        }

        if ((this->m_dwButtonState & LGLCDBUTTON_MENU) && !(buttons & LGLCDBUTTON_MENU)) {
            this->OnLCDButtonUp(LGLCDBUTTON_MENU);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_MENU) && (buttons & LGLCDBUTTON_MENU)) {
            this->OnLCDButtonDown(LGLCDBUTTON_MENU);
        }
    } else {
        if ((this->m_dwButtonState & LGLCDBUTTON_BUTTON0) && !(buttons & LGLCDBUTTON_BUTTON0)) {
            this->OnLCDButtonUp(LGLCDBUTTON_BUTTON0);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_BUTTON0) && (buttons & LGLCDBUTTON_BUTTON0)) {
            this->OnLCDButtonDown(LGLCDBUTTON_BUTTON0);
        }

        if ((this->m_dwButtonState & LGLCDBUTTON_BUTTON1) && !(buttons & LGLCDBUTTON_BUTTON1)) {
            this->OnLCDButtonUp(LGLCDBUTTON_BUTTON1);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_BUTTON1) && (buttons & LGLCDBUTTON_BUTTON1)) {
            this->OnLCDButtonDown(LGLCDBUTTON_BUTTON1);
        }

        if ((this->m_dwButtonState & LGLCDBUTTON_BUTTON2) && !(buttons & LGLCDBUTTON_BUTTON2)) {
            this->OnLCDButtonUp(LGLCDBUTTON_BUTTON2);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_BUTTON2) && (buttons & LGLCDBUTTON_BUTTON2)) {
            this->OnLCDButtonDown(LGLCDBUTTON_BUTTON2);
        }

        if ((this->m_dwButtonState & LGLCDBUTTON_BUTTON3) && !(buttons & LGLCDBUTTON_BUTTON3)) {
            this->OnLCDButtonUp(LGLCDBUTTON_BUTTON3);
        }

        if (!(this->m_dwButtonState & LGLCDBUTTON_BUTTON3) && (buttons & LGLCDBUTTON_BUTTON3)) {
            this->OnLCDButtonDown(LGLCDBUTTON_BUTTON3);
        }
    }

    this->m_dwButtonState = buttons;
}

// OFFSET: #0x0095A2E0
void CLCDOutput::OnUpdate(uint32_t timestamp) {
    if (this->m_pActivePage) {
        this->m_pActivePage->OnUpdate(timestamp);
    }

    if (!this->m_pActivePage || !this->m_pActivePage->HasExpired()) {
        return;
    }

    this->m_pActivePage = NULL;
    this->OnPageExpired(NULL);

    for (CLCDBASE_NODE* node = this->m_Objects.Head(); node; node = this->m_Objects.Next(node)) {
        CLCDPage* page = static_cast<CLCDPage*>(node->m_pObject);

        if (!page->HasExpired()) {
            this->m_pActivePage = page;

            if (this->m_hDevice != LGLCD_INVALID_DEVICE) {
                lgLcdSetAsLCDForegroundApp(this->m_hDevice, this->m_bSetAsForeground);
            }

            this->OnPageShown(page);
            break;
        }
    }

    if (this->m_pActivePage) {
        return;
    }

    this->OnEnteringIdle();

    if (this->m_hDevice != LGLCD_INVALID_DEVICE) {
        this->m_pGfx->ClearScreen();
        lgLcdUpdateBitmap(this->m_hDevice, this->m_pGfx->GetLCDScreen(), 0);
    }
}

// PROVEN: 1030497a IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CLCDOutput::OnPageShown(CLCDCollection* page) {
}

// PROVEN: 68dde66a IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CLCDOutput::OnPageExpired(CLCDCollection* page) {
}

// PROVEN: 7b8ed439 IDENTICAL/NONE
// OFFSET: 0x00401000
void CLCDOutput::OnEnteringIdle() {
}

// PROVEN: 5eda7916 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CLCDOutput::OnClosingDevice(int32_t device) {
}

// PROVEN: 16a5474d IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CLCDOutput::OnOpenedDevice(int32_t device) {
}
