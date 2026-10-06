#include "Unit_C.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"

// STATIC: #0x00CA11A0
CVar* CGUnit_C::s_cvShowFootPrintParticles;

// STATIC: #0x00CA1194
CVar* CGUnit_C::s_cvPathingDistTolerance;

// OFFSET: #0x00715330
void CGUnit_C::ClientInitialize() {
    CGUnit_C::s_cvShowFootPrintParticles = CVar::Register("showfootprintparticles", "toggles rendering of footprint particles", 1, "1", NULL, 1, false, NULL, false);
    CGUnit_C::s_cvPathingDistTolerance = CVar::Register("pathDistTol", "Sets acceptable distance from pathing destination in yards", 0, "1", NULL, 4, false, NULL, false);
}
