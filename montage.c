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
 	g_ptr_array_add(files, g_strdup(fpath));
	return 0;
}

gint callbackSort(gconstpointer a, gconstpointer b)
{
	// the sort array callback receives pointers to array elements. Array elements are pointer to char
 	return g_ascii_strcasecmp(*(const gchar **) a, *(const gchar **) b);
}

int montage(char* clips, char* theme, char* format, char* size, char* outFile)
{
	guint i;
	
	int argc = 2;
	char* argv[] = {
		"inigo",
		"/home/alain/Desktop/Documents/SVN/dev/autoprod/xvid.avi",
		NULL
	};
	
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
	lua_pop(globals.L, 1);
	
	// format the inigo consumer string with
	
	// allocate a argv array and add the consumer and the inigo strings
	
	// call inigo
		
// 	inigo(argc, argv);
	
	// free argv
	
	// pop inigo strings 
	
}

