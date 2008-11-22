#include <gtk/gtk.h>
#include <glade/glade.h>

struct {
	GladeXML *xml;
} globals;


int main(int argc, char *argv[])
{
	GtkComboBox* cmbFormat;

	gtk_init(&argc, &argv);
	globals.xml = glade_xml_new("autoprod.glade", NULL, NULL);
	
	
	// Ecran (SDL)
/*XVid Basse Qualité
Personnalisée*/
	// gtk_combo_box_set_active(GTK_COMBO_BOX(cmb1), 0); 
	
	glade_xml_signal_autoconnect(globals.xml);
	
	cmbFormat = GTK_COMBO_BOX(glade_xml_get_widget(globals.xml, "cmbFormat"));
	gtk_combo_box_remove_text(cmbFormat, 0);
	
	// read config file and set widget default values, also set rules default values.
	
	gtk_main();
	return 0;
}
