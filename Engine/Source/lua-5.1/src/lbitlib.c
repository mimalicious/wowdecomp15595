#define lbitlib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"
#include "lualib.h"


typedef int Integer;
typedef unsigned int UInteger;


// OFFSET: 0x00A29F10/0x1407D1F20
static int bit_bnot (lua_State *L) {
  lua_pushnumber(L, (UInteger)~(Integer)luaL_checknumber(L, 1));
  return 1;
}


// OFFSET: 0x00A29F50/0x1407D1F60
static int bit_band (lua_State *L) {
  int n = lua_gettop(L), i;
  UInteger w = (Integer)luaL_checknumber(L, 1);
  for (i = 2; i <= n; i++)
    w &= (Integer)luaL_checknumber(L, i);
  lua_pushnumber(L, w);
  return 1;
}


// OFFSET: 0x00A29FD0/0x1407D1FF0
static int bit_bor (lua_State *L) {
  int n = lua_gettop(L), i;
  UInteger w = (Integer)luaL_checknumber(L, 1);
  for (i = 2; i <= n; i++)
    w |= (Integer)luaL_checknumber(L, i);
  lua_pushnumber(L, w);
  return 1;
}


// OFFSET: 0x00A2A050/0x1407D2080
static int bit_bxor (lua_State *L) {
  int n = lua_gettop(L), i;
  UInteger w = (Integer)luaL_checknumber(L, 1);
  for (i = 2; i <= n; i++)
    w ^= (Integer)luaL_checknumber(L, i);
  lua_pushnumber(L, w);
  return 1;
}


// OFFSET: 0x00A2A0D0/0x1407D2110
static int bit_lshift (lua_State *L) {
  lua_pushnumber(L, (UInteger)((Integer)luaL_checknumber(L, 1) << (UInteger)luaL_checknumber(L, 2)));
  return 1;
}


// OFFSET: 0x00A2A130/0x1407D2170
static int bit_rshift (lua_State *L) {
  lua_pushnumber(L, (Integer)((UInteger)luaL_checknumber(L, 1) >> (UInteger)luaL_checknumber(L, 2)));
  return 1;
}


// OFFSET: 0x00A2A190/0x1407D21D0
static int bit_arshift (lua_State *L) {
  lua_pushnumber(L, (Integer)luaL_checknumber(L, 1) >> (UInteger)luaL_checknumber(L, 2));
  return 1;
}


// OFFSET: 0x00A2A1E0/0x1407D2230
static int bit_mod (lua_State *L) {
  if ((Integer)luaL_checknumber(L, 2) == 0) {
    lua_pushnumber(L, 1 / lua_tonumber(L, 2));
    return 1;
  }
  lua_pushnumber(L, (Integer)luaL_checknumber(L, 1) % (Integer)luaL_checknumber(L, 2));
  return 1;
}


static const struct luaL_Reg bitlib[] = {
  {"bnot", bit_bnot},
  {"band", bit_band},
  {"bor", bit_bor},
  {"bxor", bit_bxor},
  {"lshift", bit_lshift},
  {"rshift", bit_rshift},
  {"arshift", bit_arshift},
  {"mod", bit_mod},
  {NULL, NULL}
};


// OFFSET: 0x00A2A260/0x1407D22D0
LUALIB_API int luaopen_bit (lua_State *L) {
  luaL_openlib(L, "bit", bitlib, 0);
  return 1;
}
