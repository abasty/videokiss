#include <gtk/gtk.h>
#include <glade/glade.h>

int main(int argc, char *argv[])
{
	GladeXML *xml;

	gtk_init(&argc, &argv);
	xml = glade_xml_new("autoprod.glade", NULL, NULL);
	glade_xml_signal_autoconnect(xml);
	gtk_main();
	return 0;
}
