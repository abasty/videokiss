#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <gtk/gtk.h>
#include <glade/glade.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

extern char _binary_config_lua_start;
extern char _binary_config_lua_end;

# ifndef luaL_dobuffer
#  define luaL_dobuffer(L, s, sz) \
    (luaL_loadbuffer(L, (const char*)s, sz, (const char*)s) || lua_pcall(L, 0, LUA_MULTRET, 0))
# endif

int inigo(int argc, char **argv);

#define MAX_FORMATS_COUNT	32

typedef void (CallbackGetConfigListItem)(GtkWidget* widget);

typedef struct
{
	char* consumer;
	char* codecs;
} TypeFormat;

struct
{
	GladeXML *xml;
	lua_State *L;

	char* home_dir;
	char* clips_dir;
	char* themes_dir;
	char* movie_dir;

	TypeFormat formats[MAX_FORMATS_COUNT];
	int formats_count;

} globals;

int runLuaFile(lua_State *L, const char *name)
{
	int status = luaL_loadfile(L, name);
	if (status == 0)
		lua_call(L, 0, 1);
	return status;
}

char* getConfigString(const char* name)
{
	const char* string;

	lua_getglobal(globals.L, name);
	string = luaL_checkstring(globals.L, -1);
	if (string)
		string = strdup(string);
	lua_pop(globals.L, 1);

	return (char*) string;
}

char* getConfigFieldString(const char* name)
{
	char* string = NULL;

	lua_getfield(globals.L, -1, name);
	if (!lua_isnil(globals.L, -1))
		string = strdup(luaL_checkstring(globals.L, -1));
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
	lua_pop(globals.L, 1);
}

void getConfig(void)
{
	GtkWidget* widget;

	globals.home_dir = getConfigString("home_dir");

	globals.clips_dir = getConfigString("clips_dir");
	if (g_file_test(globals.clips_dir, G_FILE_TEST_IS_DIR))
	{
		widget = glade_xml_get_widget(globals.xml, "fcClips");
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(widget), globals.clips_dir);
	}

	globals.themes_dir = getConfigString("themes_dir");
	if (g_file_test(globals.themes_dir, G_FILE_TEST_IS_DIR))
	{
		widget = glade_xml_get_widget(globals.xml, "fcTheme");
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(widget), globals.themes_dir);
	}

	globals.movie_dir = getConfigString("movie_dir");
	if (g_file_test(globals.movie_dir, G_FILE_TEST_IS_DIR))
	{
		widget = glade_xml_get_widget(globals.xml, "fcOut");
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(widget), globals.movie_dir);
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
	int argc = 2;
	char* argv[] = {
		"inigo",
		"/home/alain/Desktop/Documents/SVN/dev/autoprod/xvid.avi",
		NULL
	};
		
	// TODO: run in another thread.
	inigo(argc, argv);
}

int main(int argc, char *argv[])
{
	// init gtk
	gtk_init(&argc, &argv);

	// load glade file
	globals.xml = glade_xml_new("autoprod.glade", NULL, NULL);
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
