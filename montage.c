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
// #include <string.h>
#include <glib.h>
#include <ftw.h>

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

void callbackFiles(gpointer data, gpointer user_data)
{
	g_print("%s\n", (const gchar*) data);
}

int montage(char* clips, char* theme, char* format, char* size, char* outFile)
{
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
	
	// parse the sorted array of files
	g_ptr_array_foreach(files, callbackFiles, NULL);
	
	// Create a lua table and call the Lua montage function with the table and the theme as params
	
	// The Lua function loaded the theme and returned a table of inigo strings
	
	// format the inigo consumer string with
	
	// allocate a argv array and add the consumer and the inigo strings
	
	// call inigo
		
// 	inigo(argc, argv);
	
	// free argv
	
	// pop inigo strings 
	
	// free files
	 g_ptr_array_free(files, TRUE);
}

