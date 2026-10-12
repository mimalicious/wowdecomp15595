#include "Unit_C.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WoW/Source/Ui/NamePlateFrame.h"

// STATIC: 0x00DD6184/0x140CBC8E0
CVar* CGUnit_C::s_cvShowFootPrintParticles;

// STATIC: 0x00DD6180/0x140CBC8D8
CVar* CGUnit_C::s_cvPathingDistTolerance;

// STATIC: 0x00DD6174/0x140CBC8D0
CVar* CGUnit_C::s_cvPathSmoothing;

// OFFSET: 0x005AB790/0x14021DC10
void CGUnit_C::ClientInitialize() {
    CGNamePlateFrame::RegisterCVars();
    CGUnit_C::s_cvShowFootPrintParticles = CVar::Register("showfootprintparticles", "toggles rendering of footprint particles", 1, "1", NULL, 1, false, NULL, false);
    CGUnit_C::s_cvPathingDistTolerance = CVar::Register("pathDistTol", "Sets acceptable distance from pathing destination in yards", 0, "1", NULL, 4, false, NULL, false);
    CGUnit_C::s_cvPathSmoothing = CVar::Register("pathSmoothing", "NPC will round corners on ground paths", 0, "1", NULL, 4, false, NULL, false);
}
