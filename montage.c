/*
 * montage.c -- Autoprod file
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

#include <stdlib.h>
#include <glib.h>
#include <ftw.h>
#include <sys/types.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include "videokiss.h"

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

int montage(char* clips, char* theme, char* format, int width, int height)
{
	int status = 0;
	guint i;
	guint len_consumer;
	guint len_inigo;
	gchar* consumer = NULL;
	gchar** consumer_split = NULL;
	gint consumer_splitc;

	int argc;
	char** argv = NULL;
	char** argvp;

	// Get file listing from directory "clips"
	files = g_ptr_array_new();
	g_ptr_array_add(files, clips);
	ftw(clips, callbackFtw, 10);

	// sort the list
	g_ptr_array_sort(files, callbackSort);

	// get the lua montage function
	lua_getglobal(globals.L, "montage");

	// Create a lua table
	lua_createtable(globals.L, (int) files->len, 0);

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
	if (lua_pcall(globals.L, 2, 1, 0) != 0)
	{
 		GtkWidget* dialog = gtk_message_dialog_new(globals.wndMain, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", STR_LUA_ERROR);
 		gtk_window_set_title(GTK_WINDOW(dialog), STR_LUA_ERROR_TITLE);
 		(void) gtk_dialog_run(GTK_DIALOG(dialog));
 		gtk_widget_destroy(dialog);
	}

	// free files
	(void) g_ptr_array_free(files, TRUE);

	// The Lua function loaded the theme and returned a table of inigo strings
	if (lua_type(globals.L, 1) != LUA_TTABLE)
	{
		status = 1;
		goto finalize;
	}
	
	// format the inigo consumer string
	consumer = g_strdup_printf("-consumer %s width=%d height=%d", format, width, height);
	if (!g_shell_parse_argv(consumer, &consumer_splitc, &consumer_split, NULL))
	{
		status = 2;
		goto finalize;
	}
	
	// allocate argv
	len_consumer = g_strv_length(consumer_split);
	len_inigo = (guint) lua_objlen(globals.L, 1);
	argc = (int) (len_consumer + len_inigo + 1);
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
	
 	if (fork() == 0)
 	{
 		execvp(globals.cmdline, argv);
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
