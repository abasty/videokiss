/*
*  C Implementation: montage
*
* Description:
*
*
* Author: Alain Basty <alain.basty@free.fr>, (C) 2008
*
* Copyright: See COPYING file that comes with this distribution
*
*/

#include <stdlib.h>
#include <glib.h>
#include <ftw.h>

#include "autoprod.h"

GPtrArray* files;

int callbackFtw(const char *fpath, const struct stat *sb, int typeflag)
{
	if (fpath[0] != '.' && g_strstr_len(fpath, -1, "/.") == NULL)
		g_ptr_array_add(files, g_strdup(fpath));
	return 0;
}

gint callbackSort(gconstpointer a, gconstpointer b)
{
	// the sort array callback receives pointers to array elements. Array elements are pointer to char
	return g_ascii_strcasecmp(* (const gchar **) a, * (const gchar **) b);
}

void stackDump(lua_State *L)
{
	int i;
	int top = lua_gettop(L);
	for (i = 1; i <= top; i++)    /* repeat for each level */
	{
		int t = lua_type(L, i);
		switch (t)
		{

			case LUA_TSTRING:  /* strings */
				printf("'%s'", lua_tostring(L, i));
				break;

			case LUA_TBOOLEAN:  /* booleans */
				printf(lua_toboolean(L, i) ? "true" : "false");
				break;

			case LUA_TNUMBER:  /* numbers */
				printf("%g", lua_tonumber(L, i));
				break;

			default:  /* other values */
				printf("%s", lua_typename(L, t));
				break;

		}
		printf("   ");     /* put a separator */
	}
	printf("\n");     /* end the listing */
}

int montage(char* clips, char* theme, char* format, int width, int height)
{
	int status = 0;
	guint i;
	guint len_consumer;
	guint len_inigo;
	gchar* consumer = NULL;
	gchar** consumer_split = NULL;

	int argc;
	char** argv = NULL;
	char** argvp;

	// Get file listing from directory "clips"
	files = g_ptr_array_new();
	ftw(clips, callbackFtw, 10);

	// sort the list
	g_ptr_array_sort(files, callbackSort);

	// get the lua montage function
	lua_getglobal(globals.L, "montage");

	// Create a lua table
	lua_createtable(globals.L, files->len, 0);

	// parse the sorted array of files and add them to the lua table
	for (i = 0; i < files->len; i++)
	{
		lua_pushinteger(globals.L, i);
		lua_pushstring(globals.L, (char*) g_ptr_array_index(files, i));		// string are copied by lua
		lua_settable(globals.L, -3);
	}

	// add theme
	lua_pushstring(globals.L, theme);

	// call the Lua montage function with the table and the theme as params
	lua_call(globals.L, 2, 1);

	// free files
	g_ptr_array_free(files, TRUE);

	// The Lua function loaded the theme and returned a table of inigo strings
	// TODO do something smart with the result
	if (lua_type(globals.L, 1) != LUA_TTABLE)
	{
		status = 1;
		goto finalize;
	}
	
	// format the inigo consumer string with
	consumer = g_strdup_printf("-consumer %s width=%d height=%d", format, width, height);
	consumer_split = g_strsplit(consumer, " ", 0);
	
	// allocate argv
	len_consumer = g_strv_length(consumer_split);
	len_inigo = (guint) lua_objlen(globals.L, 1);
	argc = len_consumer + len_inigo + 1;
	argvp = argv = (char**) g_malloc(sizeof(char*) * (argc +  1));
	
	// add first (unused) arg to argv
	*argvp++ = g_strdup("autoprod");
	
	// add consumer_split to argv
	for (i = 0; i < len_consumer; i++)
		*argvp++ = g_strdup(consumer_split[i]);
		
	// add montage table to argv
	for (i = 0; i < len_inigo; i++)
	{
		lua_pushinteger(globals.L, i + 1);
		lua_gettable(globals.L, 1);
		*argvp++ = g_strdup(luaL_checkstring(globals.L, -1));
		lua_pop(globals.L, 1);
	}
	
	// add NULL to argv
	*argvp++ = NULL;
	
//   	for (argvp = argv; *argvp; argvp++)
//   		g_print("%s ", *argvp);
// 	g_print("\n");

 	if (fork() == 0)
 	{
 		execv(globals.cmdline, argv);
 		exit(1);
 	}

finalize:
	// remove lua result
	lua_pop(globals.L, 1);
	
	// free consumer
	g_free(consumer);
	
	// free consumer split
	g_strfreev(consumer_split);
	
	// free inigo args
  	g_strfreev(argv);
	
	return status;
}
