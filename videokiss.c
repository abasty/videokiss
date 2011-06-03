/*
 * videokiss.c -- Main Video Kiss file
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
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/wait.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <glade/glade.h>

#include "videokiss.h"

#include "videokiss.icon.c"

// linked lua config file
extern char _binary_config_lua_start;
extern char _binary_config_lua_end;

// linked glade file
extern char _binary_videokiss_glade_start;
extern char _binary_videokiss_glade_end;

typedef void (*CallbackGetConfigListItem)(GtkWidget* widget);

TypeGlobals globals;

void on_SIGCHLD(int sig)
{
    (void) signal(SIGCHLD, on_SIGCHLD);
    while (wait3(NULL, WNOHANG, NULL) > 0);
}

char* getConfigString(const char* name)
{
	const char* string;

	lua_getglobal(globals.L, name);
	string = luaL_checkstring(globals.L, -1);
	if (string)
		string = g_strdup(string);
	lua_pop(globals.L, 1);

	return (char*) string;
}

char* getConfigFieldString(const char* name)
{
	char* string = NULL;

	lua_getfield(globals.L, -1, name);
	if (!lua_isnil(globals.L, -1))
		string = g_strdup(luaL_checkstring(globals.L, -1));
	lua_pop(globals.L, 1);
	return string;
}

char* getConfigFormatString(guint formatIndex, const char* name)
{
	char* string = NULL;

	// get consumer from lua
	lua_getglobal(globals.L, "formats");
	lua_pushinteger(globals.L, formatIndex);
	lua_gettable(globals.L, -2);				// get the format table on the stack
	if (lua_istable(globals.L, -1))
		string = getConfigFieldString(name);
	lua_pop(globals.L, 2);		// remove "formats" and the format item
	
	return string;
}

void callbackGetConfigListSizeItem(GtkWidget* widget)
{
	const char* string;
	string = luaL_checkstring(globals.L, -1);
	if (string)
		gtk_combo_box_append_text(GTK_COMBO_BOX(widget), string);
}

void callbackGetConfigListFormatItem(GtkWidget* widget)
{
	char* string = getConfigFieldString("name");
	if (string && globals.formats_count < MAX_FORMATS_COUNT)
	{
		gtk_combo_box_append_text(GTK_COMBO_BOX(widget), string);
		globals.formats[globals.formats_count].consumer = getConfigFieldString("consumer");
		globals.formats[globals.formats_count].codecs = getConfigFieldString("codecs");
		globals.formats_count++;
	}
	free(string);
}

void getConfigList(const char* name, CallbackGetConfigListItem itemCallback, GtkWidget* widget)
{
	lua_Integer index;

	lua_getglobal(globals.L, name);
	for (index = 1; TRUE ; index++)
	{
		lua_pushinteger(globals.L, index);
		lua_gettable(globals.L, -2);
		if (lua_isnil(globals.L, -1))
			break;
		(*itemCallback)(widget);
		lua_pop(globals.L, 1);
	}
	lua_pop(globals.L, 2);
}

void getConfig(void)
{
	GtkWidget* widget;
	GtkFileFilter *filter;
	char* clips_dir;
	char* theme_file;
	char* movie_dir;
	gboolean theme;

	clips_dir = getConfigString("clips_dir");
	if (g_file_test(clips_dir, G_FILE_TEST_IS_DIR))
	{
		widget = glade_xml_get_widget(globals.xml, "fcClips");
		(void) gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(widget), clips_dir);
	}

	widget = glade_xml_get_widget(globals.xml, "fcTheme");
	filter = gtk_file_filter_new();
	if (filter)
	{
		gtk_file_filter_add_pattern(filter, "*.lua");
		gtk_file_filter_set_name(filter, STR_LUA_FILTER_NAME);
		gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(widget), filter);
	}
	
	theme_file = getConfigString("theme_file");
	theme = g_file_test(theme_file, G_FILE_TEST_IS_REGULAR);
	if (theme)
	{
 		(void) gtk_file_chooser_select_filename(GTK_FILE_CHOOSER(widget), theme_file);
	}

  	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(globals.xml, "chkTheme")), FALSE);
  	gtk_widget_set_sensitive(widget,  FALSE);

	movie_dir = getConfigString("movie_dir");
	if (g_file_test(movie_dir, G_FILE_TEST_IS_DIR))
	{
		widget = glade_xml_get_widget(globals.xml, "fcOut");
		(void) gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(widget), movie_dir);
	}

	widget = glade_xml_get_widget(globals.xml, "cmbSize");
	gtk_combo_box_remove_text(GTK_COMBO_BOX(widget), 0);
	getConfigList("sizes", callbackGetConfigListSizeItem, widget);
	gtk_combo_box_set_active(GTK_COMBO_BOX(widget), 0);

	globals.formats_count = 0;
	widget = glade_xml_get_widget(globals.xml, "cmbFormat");
	gtk_combo_box_remove_text(GTK_COMBO_BOX(widget), 0);
	getConfigList("formats", callbackGetConfigListFormatItem, widget);
	gtk_combo_box_set_active(GTK_COMBO_BOX(widget), 0);
}

void saveConfig(void)
{
	FILE* file;
	GtkWidget* w;
	gchar* string;
	gchar* filename;
	
	filename = g_strconcat(globals.home_dir, "/.videokiss/default.lua", NULL);
	if (!filename)
		return;
		
	file = fopen(filename, "w");
	if (file)
	{	
		w = glade_xml_get_widget(globals.xml, "fcClips");
		string = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(w));
		fprintf(file, "clips_dir = \"%s\"\n", string);
		g_free(string);
		
		w = glade_xml_get_widget(globals.xml, "fcOut");
		string = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(w));
		fprintf(file, "movie_dir = \"%s\"\n", string);
		g_free(string);	
		
		w = glade_xml_get_widget(globals.xml, "fcTheme");
		string = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(w));
		fprintf(file, "theme_file = \"%s\"\n", string);
		g_free(string);	
		
		fclose(file);
	}
	
	g_free(filename);
}

void setDefaultFilename(void)
{
	gint index = gtk_combo_box_get_active(GTK_COMBO_BOX(glade_xml_get_widget(globals.xml, "cmbFormat"))) + 1;
	if (index >= 0)
	{
		gchar* ext = getConfigFormatString((guint) index, "ext");
		if (ext)
		{
			gchar* name = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(glade_xml_get_widget(globals.xml, "fcClips")));
			gchar* basename = g_path_get_basename(name);
			gchar* formatname = gtk_combo_box_get_active_text((GTK_COMBO_BOX(glade_xml_get_widget(globals.xml, "cmbFormat"))));
			const gchar* sizename = gtk_entry_get_text((GTK_ENTRY(glade_xml_get_widget(globals.xml, "entSize"))));
			gchar* filename = g_strconcat(basename, " - ", formatname, " - ", sizename, ".", ext, NULL);
			gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(globals.xml, "entOut")), filename);
			g_free(name);
			g_free(basename);
			g_free(filename);
			g_free(formatname);
		}
		g_free(ext);
	}
}

void on_cmbFormat_changed(GtkWidget *widget, gpointer user_data)
{
	gint index = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
	
	if (index >= 0)
	{
		gchar* codecs;
		gchar* size;
		gchar* formatCodecs;
		
		// make format box visible or not
		gtk_widget_set_sensitive(glade_xml_get_widget(globals.xml, "expFormat"), index > 0);
		
		// get codecs string and set it it in format entry
		codecs = getConfigFormatString((guint) index + 1, "codecs");
		formatCodecs = codecs ? codecs : "";
		gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(globals.xml, "entFormat")), formatCodecs);
		g_free(codecs);
		
		// get default size string and select it in the size list.
		size = getConfigFormatString((guint) index + 1, "size");
		if (size)
			gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(globals.xml, "entSize")), size);
		g_free(size);
		
		setDefaultFilename();
	}
}

void on_cmbSize_changed(GtkWidget *widget, gpointer user_data)
{
	setDefaultFilename();
}

void on_fcClips_file_set(GtkWidget *widget, gpointer user_data)
{
	setDefaultFilename();
}

void on_chkTheme_toggled(GtkWidget *widget, gpointer user_data)
{
	gboolean theme_active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
	
	gtk_widget_set_sensitive(glade_xml_get_widget(globals.xml, "fcTheme"), theme_active);
	if (!theme_active)
	{
		// reinstall default rules
		lua_getglobal(globals.L, "loadDefaultRules");
		lua_call(globals.L, 0, 0);
	}
}

void on_btnMontage_clicked(GtkWidget *widget, gpointer user_data)
{
	GtkWidget* w;
	gchar* clips = NULL;
	gchar* consumer = NULL;
	gchar* string;
	int width = 0, height = 0;
	char* end;
	gint index;
	gchar* theme = NULL;
	
	w = glade_xml_get_widget(globals.xml, "fcClips");
	clips = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(w));
	if (!clips && !*clips)
		return;
	
	// get size from UI
	w = glade_xml_get_widget(globals.xml, "cmbSize");
	string = gtk_combo_box_get_active_text(GTK_COMBO_BOX(w));
	if (string)
	{
		width = strtoull((char*) string, &end, 10);
		if (end && *end)
			height = strtoull(end + 1, NULL, 10);
	}
	if (width == 0)
		width = 720;
	if (height == 0)
		height = 576;
	g_free(string);

	// get format = consumer from lua(+ outFile) + codecs from UI 
	w = glade_xml_get_widget(globals.xml, "cmbFormat");
	index = gtk_combo_box_get_active(GTK_COMBO_BOX(w)) + 1;
	if (index >= 1)
	{
		gchar* file;
		// get consumer
		string = getConfigFormatString((guint) index, "consumer");
		
		// replace $file in consumer string to the out directory/file
		file = g_strstr_len(string, -1, "$file");
		if (file)
		{
			gchar* newString;
			gchar* fcOut;
			
			*file = 0;
			fcOut = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(glade_xml_get_widget(globals.xml, "fcOut")));
			newString = g_strconcat(
				string,
				fcOut,
				"/",
				gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(globals.xml, "entOut"))),
				file + 5,
				NULL
			);
			
			g_free(string);
			g_free(fcOut);
			string = newString;
		}
		
		// concat out format
		consumer = g_strconcat(
			string, 
			" ", 
			gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(globals.xml, "entFormat"))),
			NULL
		);
		
		g_free(string);
	}

	// get theme from UI
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(globals.xml, "chkTheme"))))
	{
		theme = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(glade_xml_get_widget(globals.xml, "fcTheme")));
	}
	else
	{
		theme = NULL;
	}
	
	// get output file if any = folder + name from UI
	montage(clips, theme, consumer, width, height);
	
	g_free(theme);
 	g_free(clips);
	g_free(consumer);
}

void on_wndMain_destroy(GtkWidget *widget, gpointer user_data)
{
	// save current config
	saveConfig();
	gtk_main_quit();
}

int main(int argc, char *argv[])
{
	int init;
	
	// avoid zombies
	(void) signal(SIGCHLD, on_SIGCHLD);
	
	// i18n
	(void) bindtextdomain(DOMAINNAME, LOCALEDIR);
	(void) textdomain(DOMAINNAME);
	
	globals.home_dir = g_get_home_dir();

	// relaunch in inigo mode	
	if (argc > 1)
		return 	inigo(argc, argv);

	// normal launch
	globals.cmdline = argv[0];
	
	// init gtk
	gtk_init(&argc, &argv);

	// load glade buffer
	globals.xml = glade_xml_new_from_buffer(&_binary_videokiss_glade_start, &_binary_videokiss_glade_end - &_binary_videokiss_glade_start, NULL, NULL);
	glade_xml_signal_autoconnect(globals.xml);

	// create Lua state
	globals.L = lua_open();
	if (globals.L == NULL)
	{
		fprintf(stderr, "%s %s", argv[0], "cannot create state: not enough memory\n");
		return EXIT_FAILURE;
	}
	luaL_openlibs(globals.L);

	// run inlined config script
	(void) luaL_dobuffer(globals.L, &_binary_config_lua_start, (size_t) (&_binary_config_lua_end - &_binary_config_lua_start));
	
	// call init lua function
	lua_getglobal(globals.L, "init");
	lua_call(globals.L, 0, 1);
	init = luaL_checkint(globals.L, -1);
	lua_pop(globals.L, 1);
	
	// create config directory if needed
	if (init)
	{
		gchar* dir = g_strconcat(globals.home_dir, "/.videokiss", NULL);
		mkdir(dir, 0700);
		g_free(dir);
	}
	
	// get config from Lua and update window
	getConfig();

	// last window updates and show all
	globals.wndMain = GTK_WINDOW(glade_xml_get_widget(globals.xml, "wndMain"));
  	gtk_window_set_icon(globals.wndMain,  gdk_pixbuf_new_from_inline(-1, videokiss_icon, FALSE, NULL));
 	gtk_widget_show(GTK_WIDGET(globals.wndMain));
	
	// run
	gtk_main();
	
	// finalize
	lua_close(globals.L);

	return 0;
}
