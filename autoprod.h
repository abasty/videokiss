//
// C Interface: autoprod
//
// Description: 
//
//
// Author: Alain Basty <alain.basty@free.fr>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef AUTPROD_H
#define AUTPROD_H

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

# ifndef luaL_dobuffer
#  define luaL_dobuffer(L, s, sz) \
    (luaL_loadbuffer(L, (const char*)s, sz, (const char*)s) || lua_pcall(L, 0, LUA_MULTRET, 0))
# endif

// run inigo producer
int inigo(int argc, char **argv);

// prepare arguments for the inigo producer
int montage(char* clips, char* theme, char* format, char* size, char* outFile);

#endif //AUTPROD_H
