/*
 * autoprod.h -- Autoprod header file
 * Copyright (C) 2008 Alain Basty
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef AUTPROD_H
#define AUTPROD_H

#include <libintl.h>

#include <glade/glade.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#define _(x) gettext(x)

#define DOMAINNAME "videokiss"

# ifndef luaL_dobuffer
#  define luaL_dobuffer(L, s, sz) \
    (luaL_loadbuffer(L, (const char*)s, sz, (const char*)s) || lua_pcall(L, 0, LUA_MULTRET, 0))
# endif

#define MAX_FORMATS_COUNT	32

#define STR_LUA_ERROR _("Lua script error. Check theme or config.")
#define STR_LUA_ERROR_TITLE _("Error")
#define STR_LUA_FILTER_NAME _("Themes")

typedef struct
{
	char* consumer;
	char* codecs;
} TypeFormat;


typedef struct
{
	GladeXML *xml;
	lua_State *L;
		
	GtkWindow* wndMain;

	const char* home_dir;
	
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
