#include "WoW/Source/DayNight/DayNight.h"
#include <cmath>
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/DayNight/DNClouds.h"
#include "WoW/Source/DayNight/DNGlare.h"
#include "WoW/Source/DayNight/DNOverrideSky.h"
#include "WoW/Source/DayNight/DNPlanet.h"
#include "WoW/Source/DayNight/DNSky.h"
#include "WoW/Source/DayNight/DNStars.h"
#include "WoW/Source/LiquidSystem/Liquid.h"

// OFFSET: 0x006532E0
void DayNight::RenderGlares() {
    if (DayNight::g_dnInfo.m_showSky && (!Liquid::CameraSubmerged() || !Liquid::GetWaterDetail())) {
        DayNight::g_sunGlare.DNGlare::Update(DayNight::g_dnInfo.m_tickSec);
        DayNight::g_sunGlare.DNGlare::Render();
        DayNight::g_moonGlare.DNGlare::Update(DayNight::g_dnInfo.m_tickSec);
        DayNight::g_moonGlare.DNGlare::Render();
    }
}

// OFFSET: 0x00653340
void DayNight::Billboard(const C3Vector& dir, C44Matrix& mat) {
    mat.a0 = dir.x;
    mat.a1 = dir.y;
    mat.a2 = dir.z;

    float ax = mat.a0;
    float ay = mat.a1;
    float az = mat.a2;
    float scale = 1.0f / sqrt(ax * ax + ay * ay + az * az);
    mat.a0 = ax * scale;
    mat.a1 = ay * scale;
    mat.a2 = az * scale;

    mat.b0 = -(ay * scale);
    mat.b1 = mat.a0;
    mat.b2 = 0.0f;

    if (fabs(mat.a0 * mat.b0) <= 0.0000099999997f) {
        mat.b0 = 0.0f;
        mat.b1 = 1.0f;
        mat.b2 = 0.0f;
    } else {
        float bx = mat.b0;
        float invLength = 1.0f / sqrt(bx * bx + mat.b1 * mat.b1);
        mat.b0 = bx * invLength;
        mat.b1 = invLength * mat.b1;
    }

    mat.c0 = mat.a1 * mat.b2 - mat.a2 * mat.b1;
    mat.c1 = mat.a2 * mat.b0 - mat.a0 * mat.b2;
    mat.c2 = mat.a0 * mat.b1 - mat.a1 * mat.b0;
}

// OFFSET: 0x006534B0
void DayNight::DrawSky(DNOverrideSky* sky, float weight) {
    if (!sky || !sky->m_model || !sky->m_model->IsDrawable() || weight <= 0.0f) {
        return;
    }

    sky->m_model->SetAnimating(1);

    CM2Model* model = sky->m_model;
    int32_t attached = model->m_attachParent != NULL;

    if (attached) {
        model->m_fFlags |= 0x40;
    } else {
        model->m_fFlags |= 0x8;
    }

    if (attached) {
        model->m_fFlags |= 0x10000;
    } else {
        model->m_fFlags |= 0x8000;
    }

    sky->m_model->m_fadeAlpha = weight;

    C3Vector cameraPos(0.0f, 0.0f, 0.0f);
    DNOverrideSky::s_scene->Animate(cameraPos);
    DNOverrideSky::s_scene->BeginDraw();
    DNOverrideSky::s_scene->Draw(M2PASS_0);
    DNOverrideSky::s_scene->Draw(M2PASS_1);
    DNOverrideSky::s_scene->Draw(M2PASS_2);
    DNOverrideSky::s_scene->EndDraw();

    sky->m_model->SetAnimating(0);

    model = sky->m_model;
    attached = model->m_attachParent != NULL;

    if (attached) {
        model->m_fFlags &= ~0x40u;
    } else {
        model->m_fFlags &= ~0x8u;
    }

    if (attached) {
        model->m_fFlags &= ~0x10000u;
    } else {
        model->m_fFlags &= ~0x8000u;
    }
}

// OFFSET: 0x006535D0
void DayNight::RenderSky(const char* skyboxName, bool interior) {
    DNInfo& info = DayNight::g_dnInfo;

    if (!info.m_showSky || DayNight::g_exteriorUnderwater || (info.m_liquidStatus && !Liquid::GetWaterDetail())) {
        return;
    }

    if (skyboxName) {
        DayNight::SetBlendSky(info.m_timeMs, 0, skyboxName, 0, info.m_interiorFogBlend);
        DayNight::SetBlendSky(info.m_timeMs, 1, NULL, 0, 0.0f);
    }

    float minX = 0.0f;
    float maxX = 0.0f;
    float minY = 0.0f;
    float maxY = 0.0f;
    float minZ = 0.0f;
    float maxZ = 0.0f;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);
    GxXformSetViewport(minX, maxX, minY, maxY, 0.99902344f, 1.0f);

    int32_t drawSky = 1;

    if (info.m_skybox && info.m_skybox->m_model && info.m_skybox->m_model->IsDrawable() && info.m_skyboxWeight > 0.99000001f) {
        drawSky = 0;
    }

    for (uint32_t i = 0; i < 3; i++) {
        DNOverrideSky* sky = info.m_blendSkybox[i];

        if (sky && sky->m_model && sky->m_model->IsDrawable() && info.m_blendSkyboxWeight[i] > 0.99000001f && !info.m_blendSkyboxFlags[i]) {
            drawSky = 0;
        }
    }

    if (drawSky) {
        DayNight::g_stars.Render(interior);

        if (!interior) {
            DayNight::g_planets[0].Render();
            DayNight::g_planets[1].Render();
            DayNight::g_planets[2].Render();
        }

        DayNight::g_sky.Render();
        DayNight::g_clouds.Render();
    }

    if (DNOverrideSky::s_scene) {
        if (interior) {
            DNOverrideSky::s_scene->m_flags |= 0x8;
        } else {
            DNOverrideSky::s_scene->m_flags &= ~0x8u;
        }

        uint32_t delta = info.m_timeMs - DNOverrideSky::s_lastTimeMs;
        DNOverrideSky::s_scene->AdvanceTime(delta);
        DNOverrideSky::s_lastTimeMs += delta;

        int32_t minute = static_cast<int32_t>(info.m_dayFraction * 1440.0f);

        for (uint32_t i = 0; i < 3; i++) {
            DayNight::SyncSkyAnimWithTime(info.m_blendSkybox[i], minute);
        }

        DayNight::SyncSkyAnimWithTime(info.m_skybox, minute);

        if (!info.m_skybox || info.m_skyboxWeight < 1.0f) {
            for (uint32_t i = 0; i < 3; i++) {
                DayNight::DrawSky(info.m_blendSkybox[i], info.m_blendSkyboxWeight[i]);
            }
        }

        if (info.m_skybox && info.m_skyboxWeight > 0.0f) {
            DayNight::DrawSky(info.m_skybox, info.m_skyboxWeight);
        }
    }

    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
}
