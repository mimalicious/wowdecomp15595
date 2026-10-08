/*
** $Id: lmathlib.c,v 1.67 2005/08/26 17:36:32 roberto Exp $
** Standard mathematical library
** See Copyright Notice in lua.h
*/


#include <stdlib.h>
#include <math.h>

#define lmathlib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


#undef PI
#define PI (3.14159265358979323846)
#define RADIANS_PER_DEGREE (PI/180.0)



// PROVEN: eb928bf3 IDENTICAL/UNPROVEN
// OFFSET: 0x00A2A290/0x1407D2300
static int math_abs (lua_State *L) {
  lua_pushnumber(L, fabs(luaL_checknumber(L, 1)));
  return 1;
}

// PROVEN: 4b071646 IDENTICAL/UNPROVEN
// OFFSET: 0x00A2A2C0/0x1407D2340
static int math_sin (lua_State *L) {
  lua_pushnumber(L, sin(luaL_checknumber(L, 1)));
  return 1;
}

// OFFSET: 0x00A2A2F0/0x1407D2370
static int math_sinh (lua_State *L) {
  lua_pushnumber(L, sinh(luaL_checknumber(L, 1)));
  return 1;
}

// PROVEN: eb1f19b7 IDENTICAL/UNPROVEN
// OFFSET: 0x00A2A320/0x1407D23A0
static int math_cos (lua_State *L) {
  lua_pushnumber(L, cos(luaL_checknumber(L, 1)));
  return 1;
}

// OFFSET: 0x00A2A350/0x1407D23D0
static int math_cosh (lua_State *L) {
  lua_pushnumber(L, cosh(luaL_checknumber(L, 1)));
  return 1;
}

// PROVEN: 3235195e IDENTICAL/UNPROVEN
// OFFSET: 0x00A2A380/0x1407D2400
static int math_tan (lua_State *L) {
  lua_pushnumber(L, tan(luaL_checknumber(L, 1)));
  return 1;
}

// OFFSET: 0x00A2A3B0/0x1407D2430
static int math_tanh (lua_State *L) {
  lua_pushnumber(L, tanh(luaL_checknumber(L, 1)));
  return 1;
}

// PROVEN: d0531571 IDENTICAL/NONE
// OFFSET: 0x00A2A3E0/0x1407D2460
static int math_asin (lua_State *L) {
  lua_pushnumber(L, asin(luaL_checknumber(L, 1)));
  return 1;
}

// PROVEN: 4fdaa66d IDENTICAL/NONE
// OFFSET: 0x00A2A410/0x1407D2490
static int math_acos (lua_State *L) {
  lua_pushnumber(L, acos(luaL_checknumber(L, 1)));
  return 1;
}

// PROVEN: a83893a5 IDENTICAL/UNPROVEN
// OFFSET: 0x00A2A440/0x1407D24C0
static int math_atan (lua_State *L) {
  lua_pushnumber(L, atan(luaL_checknumber(L, 1)));
  return 1;
}

// PROVEN: 5c278da5 IDENTICAL/UNPROVEN
// OFFSET: 0x00A2A470/0x1407D24F0
static int math_atan2 (lua_State *L) {
  lua_pushnumber(L, atan2(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
  return 1;
}

// PROVEN: 91f218c7 IDENTICAL/NONE
// OFFSET: 0x00A2A4B0/0x1407D2550
static int math_ceil (lua_State *L) {
  lua_pushnumber(L, ceil(luaL_checknumber(L, 1)));
  return 1;
}

// PROVEN: f52477b2 IDENTICAL/NONE
// OFFSET: 0x00A2A4E0/0x1407D25C0
static int math_floor (lua_State *L) {
  lua_pushnumber(L, floor(luaL_checknumber(L, 1)));
  return 1;
}

// PROVEN: 8f0aa313 IDENTICAL/NONE
// OFFSET: 0x00A2A510/0x1407D2630
static int math_fmod (lua_State *L) {
  lua_pushnumber(L, fmod(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
  return 1;
}

// PROVEN: ed9a2def IDENTICAL/UNPROVEN
// OFFSET: 0x00A2A560/0x1407D2690
static int math_modf (lua_State *L) {
  double ip;
  double fp = modf(luaL_checknumber(L, 1), &ip);
  lua_pushnumber(L, ip);
  lua_pushnumber(L, fp);
  return 2;
}

// PROVEN: d47fde2d IDENTICAL/UNPROVEN
// OFFSET: 0x00A2A5B0/0x1407D26E0
static int math_sqrt (lua_State *L) {
  lua_pushnumber(L, sqrt(luaL_checknumber(L, 1)));
  return 1;
}

// PROVEN: 651bb26c IDENTICAL/NONE
// OFFSET: 0x00A2A5E0/0x1407D2710
static int math_pow (lua_State *L) {
  lua_pushnumber(L, pow(luaL_checknumber(L, 1), luaL_checknumber(L, 2)));
  return 1;
}

// PROVEN: 1616cdf6 IDENTICAL/UNPROVEN
// OFFSET: 0x00A2A630/0x1407D2770
static int math_log (lua_State *L) {
  lua_pushnumber(L, log(luaL_checknumber(L, 1)));
  return 1;
}

// PROVEN: b5f3a494 IDENTICAL/UNPROVEN
// OFFSET: 0x00A2A660/0x1407D27A0
static int math_log10 (lua_State *L) {
  lua_pushnumber(L, log10(luaL_checknumber(L, 1)));
  return 1;
}

// PROVEN: 57391248 IDENTICAL/UNPROVEN
// OFFSET: 0x00A2A690/0x1407D27D0
static int math_exp (lua_State *L) {
  lua_pushnumber(L, exp(luaL_checknumber(L, 1)));
  return 1;
}

// PROVEN: f653d959 IDENTICAL/NONE
// OFFSET: 0x00A2A6D0/0x1407D2800
static int math_deg (lua_State *L) {
  lua_pushnumber(L, luaL_checknumber(L, 1)/RADIANS_PER_DEGREE);
  return 1;
}

// PROVEN: d60e0a45 IDENTICAL/NONE
// OFFSET: 0x00A2A700/0x1407D2840
static int math_rad (lua_State *L) {
  lua_pushnumber(L, luaL_checknumber(L, 1)*RADIANS_PER_DEGREE);
  return 1;
}

// PROVEN: ff1b06c3 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2A730/0x1407D2880
static int math_frexp (lua_State *L) {
  int e;
  lua_pushnumber(L, frexp(luaL_checknumber(L, 1), &e));
  lua_pushinteger(L, e);
  return 2;
}

// PROVEN: 01664e6f IDENTICAL/DIFFERENT
// OFFSET: 0x00A2A770/0x1407D28C0
static int math_ldexp (lua_State *L) {
  lua_pushnumber(L, ldexp(luaL_checknumber(L, 1), luaL_checkint(L, 2)));
  return 1;
}



// PROVEN: 9230314d IDENTICAL/NONE
// OFFSET: 0x00A2A7B0/0x1407D2910
static int math_min (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  lua_Number dmin = luaL_checknumber(L, 1);
  int i;
  for (i=2; i<=n; i++) {
    lua_Number d = luaL_checknumber(L, i);
    if (d < dmin)
      dmin = d;
  }
  lua_pushnumber(L, dmin);
  return 1;
}


// PROVEN: 2e0f66fa IDENTICAL/NONE
// OFFSET: 0x00A2A820/0x1407D2990
static int math_max (lua_State *L) {
  int n = lua_gettop(L);  /* number of arguments */
  lua_Number dmax = luaL_checknumber(L, 1);
  int i;
  for (i=2; i<=n; i++) {
    lua_Number d = luaL_checknumber(L, i);
    if (d > dmax)
      dmax = d;
  }
  lua_pushnumber(L, dmax);
  return 1;
}


// PROVEN: aead1376 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2A890/0x1407D2A10
static int math_random (lua_State *L) {
  /* the `%' avoids the (rare) case of r==1, and is needed also because on
     some systems (SunOS!) `rand()' may return a value larger than RAND_MAX */
  lua_Number r = (lua_Number)(rand()%RAND_MAX) / (lua_Number)RAND_MAX;
  switch (lua_gettop(L)) {  /* check number of arguments */
    case 0: {  /* no arguments */
      lua_pushnumber(L, r);  /* Number between 0 and 1 */
      break;
    }
    case 1: {  /* only upper limit */
      int u = luaL_checkint(L, 1);
      luaL_argcheck(L, 1<=u, 1, "interval is empty");
      lua_pushnumber(L, floor(r*u)+1);  /* int between 1 and `u' */
      break;
    }
    case 2: {  /* lower and upper limits */
      int l = luaL_checkint(L, 1);
      int u = luaL_checkint(L, 2);
      luaL_argcheck(L, l<=u, 2, "interval is empty");
      lua_pushnumber(L, floor(r*(u-l+1))+l);  /* int between `l' and `u' */
      break;
    }
    default: return luaL_error(L, "wrong number of arguments");
  }
  return 1;
}


static const luaL_Reg mathlib[] = {
  {"abs",   math_abs},
  {"acos",  math_acos},
  {"asin",  math_asin},
  {"atan2", math_atan2},
  {"atan",  math_atan},
  {"ceil",  math_ceil},
  {"cosh",   math_cosh},
  {"cos",   math_cos},
  {"deg",   math_deg},
  {"exp",   math_exp},
  {"floor", math_floor},
  {"fmod",   math_fmod},
  {"frexp", math_frexp},
  {"ldexp", math_ldexp},
  {"log10", math_log10},
  {"log",   math_log},
  {"max",   math_max},
  {"min",   math_min},
  {"modf",   math_modf},
  {"pow",   math_pow},
  {"rad",   math_rad},
  {"random",     math_random},
  {"sinh",   math_sinh},
  {"sin",   math_sin},
  {"sqrt",  math_sqrt},
  {"tanh",   math_tanh},
  {"tan",   math_tan},
  {NULL, NULL}
};


/*
** Open math library
*/
// OFFSET: %0x0096CA10
LUALIB_API int luaopen_math (lua_State *L) {
  luaL_register(L, LUA_MATHLIBNAME, mathlib);
  lua_pushnumber(L, PI);
  lua_setfield(L, -2, "pi");
  lua_pushnumber(L, HUGE_VAL);
  lua_setfield(L, -2, "huge");
#if defined(LUA_COMPAT_MOD)
  lua_getfield(L, -1, "fmod");
  lua_setfield(L, -2, "mod");
#endif
  return 1;
}

