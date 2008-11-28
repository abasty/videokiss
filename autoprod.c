#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <glade/glade.h>

#include "autoprod.h"

// linked lua config file
extern char _binary_config_lua_start;
extern char _binary_config_lua_end;

// linked glade file
extern char _binary_autoprod_glade_start;
extern char _binary_autoprod_glade_end;

typedef void (CallbackGetConfigListItem)(GtkWidget* widget);

TypeGlobals globals;

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

void getConfigList(const char* name, CallbackGetConfigListItem* itemCallback, GtkWidget* widget)
{
	int index;

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
	char* clips_dir;
	char* themes_dir;
	char* movie_dir;

	globals.home_dir = getConfigString("home_dir");

	clips_dir = getConfigString("clips_dir");
	if (g_file_test(clips_dir, G_FILE_TEST_IS_DIR))
	{
		widget = glade_xml_get_widget(globals.xml, "fcClips");
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(widget), clips_dir);
	}

	themes_dir = getConfigString("themes_dir");
	if (g_file_test(themes_dir, G_FILE_TEST_IS_DIR))
	{
		widget = glade_xml_get_widget(globals.xml, "fcTheme");
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(widget), themes_dir);
	}

	movie_dir = getConfigString("movie_dir");
	if (g_file_test(movie_dir, G_FILE_TEST_IS_DIR))
	{
		widget = glade_xml_get_widget(globals.xml, "fcOut");
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(widget), movie_dir);
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

void on_cmbFormat_changed(GtkComboBox *widget, gpointer user_data)
{
	gboolean visible = gtk_combo_box_get_active(GTK_COMBO_BOX(widget)) > 0;
	gtk_widget_set(glade_xml_get_widget(globals.xml, "vbxFormat"), "visible", visible);
}

void on_btnMontage_clicked(GtkComboBox *widget, gpointer user_data)
{
	GtkWidget* w;
	gchar* clips;
	
	w = glade_xml_get_widget(globals.xml, "fcClips");
	clips = gtk_file_chooser_get_uri(GTK_FILE_CHOOSER(w));
	if (g_str_has_prefix(clips, "file://"))
	{
		gchar* string;
		int width, height;
		char* end;
		gint integer;
		
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

#if 0	 	
	 	// get theme from UI
	 	
	 	// get format = consumer from lua(+ outFile) + codecs from UI 
 		w = glade_xml_get_widget(globals.xml, "cmbFormat");
		integer = gtk_combo_box_get_active(GTK_COMBO_BOX(w));
 		if (integer >= 0)
 		{
 			// get consumer from lua
			lua_getglobal(globals.L, "formats");
			lua_pushinteger(globals.L, integer + 1);
			lua_gettable(globals.L, -2);				// get the format table on the stack
			if (lua_istable(globals.L, -1))
			{
			}
			lua_pop(globals.L, 2);		// remove "formats" and the format item
 			
 		}
 		
	 	
	 	// get output file if any = folder + name from UI
#endif
		
		montage(clips + 7, NULL, "sdl", width, height);
 	}
 	g_free(clips);
}

int main(int argc, char *argv[])
{
	// relaunch in inigo mode	
	if (argc > 1)
		return 	inigo(argc, argv);

	// normal launch
	globals.cmdline = argv[0];
	
	// init gtk
	gtk_init(&argc, &argv);

	// load glade buffer
	globals.xml = glade_xml_new_from_buffer(&_binary_autoprod_glade_start, &_binary_autoprod_glade_end - &_binary_autoprod_glade_start, NULL, NULL);
	
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
	luaL_dobuffer(globals.L, &_binary_config_lua_start, &_binary_config_lua_end - &_binary_config_lua_start);

	// get config from Lua
	getConfig();

	// run
	gtk_main();

	// finalize
	lua_close(globals.L);

	return 0;
}
