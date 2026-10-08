#include "Engine/Source/Gxu/GxuDebugDraw.h"
#include <new>
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Scrn/Scrn.h"
#include "Engine/Source/Services/TextBlock.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

// STATIC: 0x00CA84F4
static float s_lodDistModifier = 1.0f;

// STATIC: 0x00CA84F8
static int32_t s_lodMode = 1;

// STATIC: 0x00DD8AFC
static GxuDebugTextObject* s_textObjectFreeList;

// STATIC: 0x00DD8B00
static GxuDebugObjectBlock<GxuDebugTextObject>* s_textObjectBlocks;

// STATIC: 0x00DD8B04
static bool s_textDrawerInitialized;

// STATIC: 0x00DD8B05
static bool s_text3DDrawerInitialized;

// STATIC: 0x00DD8B08
static GxuDebugText3DObject* s_text3DObjectFreeList;

// STATIC: 0x00DD8B0C
static GxuDebugObjectBlock<GxuDebugText3DObject>* s_text3DObjectBlocks;

// PROVEN: 1a9e7609 IDENTICAL/NONE
// OFFSET: 0x006120D0/0x14029D070
float GxuGetLODDistModifier() {
    return s_lodDistModifier;
}

// PROVEN: 21cdd17a IDENTICAL/NONE
// OFFSET: 0x006120E0
int32_t GxuGetLODMode() {
    return s_lodMode;
}

// PROVEN: 42691cb4 IDENTICAL/NONE
// OFFSET: 0x006120F0
void GxuDebugText::Draw(const C3Vector& viewPos) {
    GxuFontRenderBatch(this->m_batch);
}

// OFFSET: 0x00612100
void GxuDebugTextObject::Release() {
    GxuFontRemoveFromBatch(this->m_text.m_string);
    GxuFontDestroyString(this->m_text.m_string);
    this->m_text.m_string = NULL;
    GxuFontDestroyBatch(this->m_text.m_batch);
    this->m_text.m_batch = NULL;
    this->m_next = s_textObjectFreeList;
    s_textObjectFreeList = this;
}

// PROVEN: d8afad71 IDENTICAL/NONE
// OFFSET: 0x00612150
bool GxuDebugTextObject::IsAlive() {
    return this->m_time >= 0.0f;
}

// OFFSET: 0x00612170
void GxuDebugTextObject::Update(float elapsed) {
    this->m_time = this->m_time - elapsed;
}

// OFFSET: 0x00612180
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

// OFFSET: 0x00612540
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

// OFFSET: 0x006125E0
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
GxuDebugText::GxuDebugText() : m_x(0.0f), m_y(0.0f), m_text(NULL), m_batch(NULL), m_string(NULL) {
}

// INLINED
GxuDebugTextObject::GxuDebugTextObject() : m_next(NULL), m_time(0.0f) {
}

// INLINED
GxuDebugTextObject::GxuDebugTextObject(float x, float y, const char* text, const CImVector& color, float time) : m_next(NULL), m_text(x, y, text, color), m_time(time) {
}
