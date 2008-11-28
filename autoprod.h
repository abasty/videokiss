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

#include <glade/glade.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

# ifndef luaL_dobuffer
#  define luaL_dobuffer(L, s, sz) \
    (luaL_loadbuffer(L, (const char*)s, sz, (const char*)s) || lua_pcall(L, 0, LUA_MULTRET, 0))
# endif

#define MAX_FORMATS_COUNT	32

typedef struct
{
	char* consumer;
	char* codecs;
} TypeFormat;


typedef struct
{
	GladeXML *xml;
	lua_State *L;

	char* home_dir;
	
	TypeFormat formats[MAX_FORMATS_COUNT];
	int formats_count;
	
	char* cmdline;

} TypeGlobals;

extern TypeGlobals globals;

// run inigo producer
int inigo(int argc, char **argv);

// prepare arguments for the inigo producer
int montage(char* clips, char* theme, char* format, int width, int height);

#endif //AUTPROD_H
