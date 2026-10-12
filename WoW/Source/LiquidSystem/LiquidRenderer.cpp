#include "WoW/Source/LiquidSystem/Liquid.h"
#include <cmath>
#include <cstdlib>
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxLight.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00D298E4/0x140BE84C0
static float s_camSide = 1.0f;

// STATIC: 0x00D298E8/0x140BE84C4
static uint16_t s_drawStamp;

// STATIC: 0x00D298EC/0x140BE84C8
Liquid::CRenderer::DrawListSorter Liquid::CRenderer::s_drawListSorter[2] = { Liquid::CRenderer::CompareOpaque, Liquid::CRenderer::CompareTransparent };

// STATIC: 0x0100AAA0/0x140EE26A8
static Liquid::Object* s_closestReflectionObject;

// STATIC: 0x0100AAA4/0x140EE26B0
static Liquid::CRenderer* s_reflectionRenderer;

// STATIC: 0x0100AAA8/0x140EE26B8
static Liquid::CRenderer* s_renderer;

// STATIC: 0x0100AAAC/0x140EE26C0
static float s_closestReflectionDist;

// STATIC: 0x0100AAB0
static C3Vector s_camPos;

// INLINED
Liquid::CRenderer::CRenderer(uint32_t isReflection) {
    this->m_isReflection = isReflection;
}

// OFFSET: 0x00B3C240/0x140909160
void Liquid::RendererValidate() {
    s_drawStamp += 2;
}

// PROVEN: 705f4f0f IDENTICAL/UNPROVEN
// OFFSET: 0x00B3C260/0x140909180
void Liquid::SetCamPos(const C3Vector& pos) {
    s_camPos = pos;

    if (Liquid::CameraSubmerged()) {
        s_camSide = -1.0f;
    } else {
        s_camSide = 1.0f;
    }
}

// OFFSET: 0x00B3C2B0/0x1409091E0
float Liquid::GetClosestReflectionHeight() {
    if (s_closestReflectionObject) {
        return s_closestReflectionObject->m_reflectionPlane.d;
    }

    return 0.0f;
}

// PROVEN: 3bb558e0 IDENTICAL/DIFFERENT
// OFFSET: 0x00B3C360/0x1409092F0
int32_t Liquid::CRenderer::CompareOpaque(const void* a, const void* b) {
    const CInstance* l = *static_cast<CInstance* const*>(a);
    const CInstance* r = *static_cast<CInstance* const*>(b);

    if (reinterpret_cast<uintptr_t>(l->m_material) < reinterpret_cast<uintptr_t>(r->m_material)) {
        return -1;
    }

    if (reinterpret_cast<uintptr_t>(l->m_material) > reinterpret_cast<uintptr_t>(r->m_material)) {
        return 1;
    }

    if (reinterpret_cast<uintptr_t>(l->m_settings) < reinterpret_cast<uintptr_t>(r->m_settings)) {
        return -1;
    }

    if (reinterpret_cast<uintptr_t>(l->m_settings) > reinterpret_cast<uintptr_t>(r->m_settings)) {
        return 1;
    }

    if (l->m_isMesh < r->m_isMesh) {
        return -1;
    }

    if (l->m_isMesh > r->m_isMesh) {
        return 1;
    }

    if (reinterpret_cast<uintptr_t>(l->m_environment) < reinterpret_cast<uintptr_t>(r->m_environment)) {
        return -1;
    }

    if (reinterpret_cast<uintptr_t>(l->m_environment) > reinterpret_cast<uintptr_t>(r->m_environment)) {
        return 1;
    }

    if (l->m_transform.d3 > r->m_transform.d3) {
        return 1;
    }

    if (r->m_transform.d3 > l->m_transform.d3) {
        return -1;
    }

    return 0;
}

// PROVEN: 2ebd9172 IDENTICAL/DIFFERENT
// OFFSET: 0x00B3C3D0/0x140909360
int32_t Liquid::CRenderer::CompareTransparent(const void* a, const void* b) {
    const CInstance* l = *static_cast<CInstance* const*>(a);
    const CInstance* r = *static_cast<CInstance* const*>(b);

    if (reinterpret_cast<uintptr_t>(l->m_material) < reinterpret_cast<uintptr_t>(r->m_material)) {
        return -1;
    }

    if (reinterpret_cast<uintptr_t>(l->m_material) > reinterpret_cast<uintptr_t>(r->m_material)) {
        return 1;
    }

    if (reinterpret_cast<uintptr_t>(l->m_settings) < reinterpret_cast<uintptr_t>(r->m_settings)) {
        return -1;
    }

    if (reinterpret_cast<uintptr_t>(l->m_settings) > reinterpret_cast<uintptr_t>(r->m_settings)) {
        return 1;
    }

    if (l->m_isMesh < r->m_isMesh) {
        return -1;
    }

    if (l->m_isMesh > r->m_isMesh) {
        return 1;
    }

    if (reinterpret_cast<uintptr_t>(l->m_environment) < reinterpret_cast<uintptr_t>(r->m_environment)) {
        return -1;
    }

    if (reinterpret_cast<uintptr_t>(l->m_environment) > reinterpret_cast<uintptr_t>(r->m_environment)) {
        return 1;
    }

    if (l->m_transform.d3 > r->m_transform.d3) {
        return -1;
    }

    return r->m_transform.d3 > l->m_transform.d3;
}

// PROVEN: e1910044 DIFFERENT/IDENTICAL
// OFFSET: 0x00B3C440/0x1409093D0
int32_t Liquid::HasFakeReflection() {
    return s_renderer->m_instances[0].Count() + s_renderer->m_instances[1].Count() && Liquid::GetWaterDetail();
}

// PROVEN: e80d4e9e DIFFERENT/IDENTICAL
// OFFSET: 0x00B3C460/0x140909400
int32_t Liquid::HasRealReflection() {
    int32_t real = Liquid::GetWaterDetail() >= 3 && Liquid::GetReflectionMode();

    if (Liquid::CameraSubmerged()) {
        real = 0;
    }

    return s_reflectionRenderer->m_instances[0].Count() + s_reflectionRenderer->m_instances[1].Count() && real;
}

// PROVEN: 3d910c8b IDENTICAL/NONE
// OFFSET: 0x00B3C4B0
Liquid::CRenderer::~CRenderer() {
}

// PROVEN: d3bdb673 IDENTICAL/IDENTICAL
// OFFSET: 0x00B3C550/0x140909510
void Liquid::CRenderer::Draw(CInstance* instance) {
    if (instance->unk_0064 == s_drawStamp) {
        return;
    }

    instance->unk_0064 = s_drawStamp;

    if (instance->m_settings->m_transparent) {
        this->m_instances[1].New(instance);
        this->m_sorted[1] = 0;
    } else {
        this->m_instances[0].New(instance);
        this->m_sorted[0] = 0;
    }

    instance->m_material->OnAddToScene();
}

// PROVEN: 6eb60f5a IDENTICAL/IDENTICAL
// OFFSET: 0x00B3C5C0/0x140909580
void Liquid::RendererInit() {
    void* mem = SMemAlloc(sizeof(CRenderer), __FILE__, __LINE__, 0);
    s_renderer = mem ? new (mem) CRenderer(0) : NULL;

    mem = SMemAlloc(sizeof(CRenderer), __FILE__, __LINE__, 0);
    s_reflectionRenderer = mem ? new (mem) CRenderer(1) : NULL;
}

// OFFSET: 0x00B3C650/0x140909630
void Liquid::RendererDraw(CInstance* instance) {
    if (Liquid::GetWaterDetail() >= 3 && Liquid::GetReflectionMode()) {
        Object* object = instance->m_object;

        if (object->m_reflectionType == 1) {
            if (s_closestReflectionObject) {
                if (object == s_closestReflectionObject) {
                    s_reflectionRenderer->Draw(instance);
                    return;
                }

                C3Vector delta = instance->m_bounds.c - s_camPos;
                float dist = sqrt(delta.x * delta.x + delta.z * delta.z + delta.y * delta.y);
                float side = (object->m_reflectionPlane.n.y * s_camPos.y + object->m_reflectionPlane.n.z * s_camPos.z + object->m_reflectionPlane.n.x * s_camPos.x + object->m_reflectionPlane.d) * s_camSide;

                if (side <= 0.0f || s_closestReflectionDist <= dist) {
                    s_reflectionRenderer->Draw(instance);
                    return;
                }

                s_closestReflectionDist = dist;
            }

            s_closestReflectionObject = object;
            s_reflectionRenderer->Draw(instance);
            return;
        }
    }

    s_renderer->Draw(instance);
}

// PROVEN: 47492e71 IDENTICAL/IDENTICAL
// OFFSET: 0x00B3C750/0x140909740
void Liquid::RendererDestroy() {
    CRenderer* renderer = s_renderer;

    if (renderer) {
        renderer->~CRenderer();
        SMemFree(renderer, "delete", -1, 0);
    }

    s_renderer = NULL;

    renderer = s_reflectionRenderer;

    if (renderer) {
        renderer->~CRenderer();
        SMemFree(renderer, "delete", -1, 0);
    }

    s_reflectionRenderer = NULL;
}

// OFFSET: 0x00B3C7B0/0x140909830
void Liquid::CRenderer::Flush(uint32_t time, const C3Vector& camPos, uint32_t pass) {
    uint32_t list = pass == 2 ? 1 : pass;
    TSGrowableArray<CInstance*>& instances = this->m_instances[list];
    uint32_t count = instances.Count();

    if (!count) {
        return;
    }

    Liquid::UpdateSurfaceGen(time);

    CGxLight lights[4];

    for (uint32_t i = 0; i < 4; i++) {
        g_theGxDevicePtr->Light(i, lights[i]);
    }

    if (!this->m_sorted[list]) {
        qsort(instances.m_data, count, sizeof(CInstance*), CRenderer::s_drawListSorter[list]);
        this->m_sorted[list] = 1;
    }

    IMaterial* lastMaterial = NULL;
    CMaterialSettings* lastSettings = NULL;
    IEnvironment* lastEnvironment = NULL;
    uint16_t lastMesh = 0xFFFF;

    g_theGxDevicePtr->RsPush();
    GxXformPush(GxXform_Tex1);
    GxXformPush(GxXform_Tex0);
    GxXformPush(GxXform_World);

    for (uint32_t i = 0; i < count; i++) {
        CInstance* instance = instances.m_data[i];
        IMaterial* material = instance->m_material;

        if (lastMaterial == material && lastSettings == instance->m_settings) {
            if (lastMesh == instance->m_isMesh) {
                if (lastEnvironment != instance->m_environment) {
                    material->SetupLighting(camPos, instance);
                    lastEnvironment = instance->m_environment;
                }
            } else {
                if (instance->m_geomFactory->GetWorldSpace()) {
                    instance->m_material->SetupDefaultTransform(camPos);
                }

                lastMesh = instance->m_isMesh;
            }
        } else {
            uint32_t reflection = this->m_isReflection;

            if (!reflection && instance->m_object->m_reflectionType == 2) {
                reflection = 2;
            }

            if (!material->SetupRender(time, static_cast<ERenderPass>(pass), camPos, instance->m_environment, instance->m_settings, reflection)) {
                while (i + 1 < count && instances.m_data[i + 1]->m_material == instance->m_material && instances.m_data[i + 1]->m_settings == instance->m_settings) {
                    i++;
                }

                continue;
            }

            lastMaterial = instance->m_material;
            lastSettings = instance->m_settings;

            if (instance->m_geomFactory->GetWorldSpace()) {
                instance->m_material->SetupDefaultTransform(camPos);
            }

            lastMesh = instance->m_isMesh;
            instance->m_material->SetupLighting(camPos, instance);
            lastEnvironment = instance->m_environment;
        }

        lastMaterial->m_render(pass, camPos, instance);
    }

    GxXformPop(GxXform_Tex1);
    GxXformPop(GxXform_Tex0);
    GxXformPop(GxXform_World);
    g_theGxDevicePtr->RsPop();

    if (pass != 2) {
        if (instances.m_data) {
            SMemFree(instances.m_data, typeid(CInstance*).raw_name(), SLOG_OBJECT, 0);
        }

        instances.m_alloc = 0;
        instances.m_count = 0;
        instances.m_data = NULL;
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (lights[i].m_flags & 0x1) {
            C3Vector pos(0.0f, 0.0f, 0.0f);
            g_theGxDevicePtr->LightSet(i, lights[i], pos);
            g_theGxDevicePtr->LightEnable(i, 1);
        }
    }
}

// PROVEN: dd0bdcfc IDENTICAL/IDENTICAL
// OFFSET: 0x00B3CCC0/0x140909C50
void Liquid::RendererFlush(uint32_t time, const C3Vector& camPos, uint32_t pass) {
    s_renderer->Flush(time, camPos, pass);
    s_reflectionRenderer->Flush(time, camPos, pass);

    if (pass == 1) {
        s_closestReflectionObject = NULL;
        s_closestReflectionDist = 0.0f;
    }
}
