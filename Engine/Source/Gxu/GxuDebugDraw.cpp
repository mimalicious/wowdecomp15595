#include "Engine/Source/Gxu/GxuDebugDraw.h"
#include <new>
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gxu/GxuDebugDrawShapes.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Scrn/Scrn.h"
#include "Engine/Source/Services/TextBlock.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

// STATIC: 0x00CA84F4/0x140B48508
static float s_lodDistModifier = 1.0f;

// STATIC: 0x00CA84F8/0x140B4850C
static int32_t s_lodMode = 1;

// STATIC: 0x00DD8AFC/0x140CBF310
static GxuDebugTextObject* s_textObjectFreeList;

// STATIC: 0x00DD8B00/0x140CBF318
static GxuDebugObjectBlock<GxuDebugTextObject>* s_textObjectBlocks;

// STATIC: 0x00DD8B04/0x140CBF320
static bool s_textDrawerInitialized;

// STATIC: 0x00DD8B05/0x140CBF321
static bool s_text3DDrawerInitialized;

// STATIC: 0x00DD8B08/0x140CBF328
static GxuDebugText3DObject* s_text3DObjectFreeList;

// STATIC: 0x00DD8B0C/0x140CBF330
static GxuDebugObjectBlock<GxuDebugText3DObject>* s_text3DObjectBlocks;

// STATIC: 0x00DD9E7C/0x140CC0758
static CGxShader* s_textureVertexShader;

// STATIC: 0x00DD9E80/0x140CC0760
static CGxShader* s_texturePixelShader;

// STATIC: 0x00DD9E84/0x140CC0768
static GxuDebugTextureObject* s_textureObjectFreeList;

// STATIC: 0x00DD9E88/0x140CC0770
static GxuDebugObjectBlock<GxuDebugTextureObject>* s_textureObjectBlocks;

// STATIC: 0x00DD9E8C/0x140CC0778
static bool s_textureDrawerInitialized;

// OFFSET: 0x006120D0/0x14029D070
float GxuGetLODDistModifier() {
    return s_lodDistModifier;
}

// OFFSET: 0x006120E0/0x14029D080
int32_t GxuGetLODMode() {
    return s_lodMode;
}

// OFFSET: 0x006120F0/0x14029D090
void GxuDebugText::Draw(const C3Vector& viewPos) {
    GxuFontRenderBatch(this->m_batch);
}

// OFFSET: 0x00612100/0x14029D0A0
void GxuDebugTextObject::Release() {
    GxuFontRemoveFromBatch(this->m_text.m_string);
    GxuFontDestroyString(this->m_text.m_string);
    this->m_text.m_string = NULL;
    GxuFontDestroyBatch(this->m_text.m_batch);
    this->m_text.m_batch = NULL;
    this->m_next = s_textObjectFreeList;
    s_textObjectFreeList = this;
}

// OFFSET: 0x00612150/0x14029D100
int32_t GxuDebugTextObject::IsAlive() {
    return this->m_time >= 0.0f;
}

// OFFSET: 0x00612170/0x14029D120
void GxuDebugTextObject::Update(float elapsed) {
    this->m_time = this->m_time - elapsed;
}

// OFFSET: 0x00612180/0x14029D130
void GxuDebugTextObject::Render() {
    if (this->m_time == 0.0f) {
        this->m_time = this->m_time - 0.001;
    }

    GxuAddToDrawList(GxuCat_2, &this->m_text);
}

// OFFSET: [0x006121F0/0x14029D1B0, 0x006121F0/0x14029D1B0]
GxuDebugText::~GxuDebugText() {
}

// OFFSET: 0x00612250/0x14029D250
GxuDebugText::GxuDebugText(float x, float y, const char* text, const CImVector& color) {
    this->m_x = x;
    this->m_y = y;
    this->m_text = text;
    this->m_batch = NULL;
    this->m_string = NULL;

    CGxFont* font = TextBlockGetFontPtr(ScrnGetStockFont(STOCK_PERFFONT));
    float height = ScrnGetStockFontHeight(STOCK_PERFFONT);
    this->m_batch = GxuFontCreateBatch(false, false, false);

    C3Vector position(this->m_x, this->m_y, 1.0f);
    GxuFontCreateString(font, text, 0.0f, position, 1.0f, height, 0.0f, this->m_string, GxVJ_Bottom, GxHJ_Left, 0x44, color, -0.45f, 1.0f, 1.0f, 0.0f);
    GxuFontAddToBatch(this->m_batch, this->m_string);
}

// OFFSET: [0x00612340/0x14029D3A0, 0x00612340/0x14029D3A0]
GxuDebugTextObject::~GxuDebugTextObject() {
}

// OFFSET: 0x00612540/0x14029D590
void GxuShutdownDebugTextDrawer() {
    if (!s_textDrawerInitialized) {
        return;
    }

    s_textObjectFreeList = NULL;

    while (s_textObjectBlocks) {
        GxuDebugObjectBlock<GxuDebugTextObject>* block = s_textObjectBlocks;
        s_textObjectBlocks = block->m_next;
        block->m_objects.Clear();
        delete block;
    }

    s_textDrawerInitialized = false;
}

// OFFSET: 0x006125E0/0x14029D6E0
void GxuShutdownDebugText3DDrawer() {
    if (!s_text3DDrawerInitialized) {
        return;
    }

    s_text3DObjectFreeList = NULL;

    while (s_text3DObjectBlocks) {
        GxuDebugObjectBlock<GxuDebugText3DObject>* block = s_text3DObjectBlocks;
        s_text3DObjectBlocks = block->m_next;
        block->m_objects.Clear();
        delete block;
    }

    s_text3DDrawerInitialized = false;
}

// OFFSET: 0x00612770/0x14029D990
void GxuShutdownDebugDraw() {
    GxuShutdownDebugTextDrawer();
    GxuShutdownDebugText3DDrawer();
    GxuShutdownDebugTextureDrawer();
    GxuShutdownDebugShapeDrawer();

    for (uint32_t i = 0; i < GxuCats_Last; i++) {
        GxuClearDrawList(static_cast<EGxuDrawListCategory>(i));
    }
}

// OFFSET: 0x00612790/0x14029D9C0
void GxuDebugDrawText(float x, float y, const char* text, const CImVector& color, float time) {
    if (!s_textDrawerInitialized) {
        s_textDrawerInitialized = true;
    }

    GxuDebugTextObject* object = s_textObjectFreeList;

    if (!object) {
        GxuDebugObjectBlock<GxuDebugTextObject>* block = new (__FILE__, __LINE__) GxuDebugObjectBlock<GxuDebugTextObject>;
        block->m_objects.SetCount(16);

        for (uint32_t i = 0; i < 16; i++) {
            block->m_objects[i].m_next = object;
            object = &block->m_objects[i];
        }

        block->m_next = s_textObjectBlocks;
        s_textObjectBlocks = block;
    }

    s_textObjectFreeList = object->m_next;
    object->~GxuDebugTextObject();
    new (object) GxuDebugTextObject(x, y, text, color, time);
    g_scene.AddObject(object);
}

// INLINED
GxuDebugTextObject::GxuDebugTextObject() : m_next(NULL), m_time(0.0f) {
}

// INLINED
GxuDebugTextObject::GxuDebugTextObject(float x, float y, const char* text, const CImVector& color, float time) : m_next(NULL), m_text(x, y, text, color), m_time(time) {
}

// OFFSET: 0x00616E80/0x1402A3D60
void GxuShutdownDebugTextureDrawer() {
    if (!s_textureDrawerInitialized) {
        return;
    }

    s_textureObjectFreeList = NULL;

    while (s_textureObjectBlocks) {
        GxuDebugObjectBlock<GxuDebugTextureObject>* block = s_textureObjectBlocks;
        s_textureObjectBlocks = block->m_next;
        block->m_objects.Clear();
        delete block;
    }

    if (s_textureVertexShader) {
        g_theGxDevicePtr->ShaderRelease(s_textureVertexShader);
    }

    s_textureVertexShader = NULL;

    if (s_texturePixelShader) {
        g_theGxDevicePtr->ShaderRelease(s_texturePixelShader);
    }

    s_texturePixelShader = NULL;
    s_textureDrawerInitialized = false;
}
