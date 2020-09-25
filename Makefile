export SOURCE_DIRS := $(CURDIR)
export HEADER_DIRS := $(SOURCE_DIRS)
export EXE_NAME := videokiss
export EXE_ICON := $(CURDIR)/video-x-generic.png
export CONFIG_FILE := config.lua
export GLADE_FILE := $(EXE_NAME).glade
export BUILD_DIR := $(CURDIR)/build
export I18N_DIR := $(BUILD_DIR)/i18n
export INSTALL_PREFIX := /usr
#export PACKAGES := glib-2.0 gtk+-2.0 mlt-framework libglade-2.0
export PACKAGES := glib-2.0 gtk+-3.0

all : debug

debug release: i18n
	@mkdir -p $(BUILD_DIR)/$@
	@$(MAKE) -s $@ -f Makefile.common TARGET=$@

# TODO create a rule for po files to mo files and move it to Makfile.common
i18n :
	@echo Creating mo files
	@mkdir -p $(I18N_DIR) 2>/dev/null
	@msgfmt fr.po -o $(I18N_DIR)/fr.mo

i18n-template : *.glade *.h
	xgettext -a *.h -o $(EXE_NAME).pot
	xgettext -j *.glade -o $(EXE_NAME).pot

i18n-template-fr : i18n-template
	msginit -l fr_FR.utf8

i18n-clean :
	rm *.po* 2>/dev/null

install :
	install -s build/release/$(EXE_NAME) $(INSTALL_PREFIX)/bin
	install $(EXE_NAME).desktop $(INSTALL_PREFIX)/share/applications/$(EXE_NAME).desktop
	install $(I18N_DIR)/fr.mo $(INSTALL_PREFIX)/share/locale/fr/LC_MESSAGES/$(EXE_NAME).mo

uninstall :
	rm $(INSTALL_PREFIX)/bin/$(EXE_NAME)
	rm $(INSTALL_PREFIX)/share/applications/$(EXE_NAME).desktop
	rm $(INSTALL_PREFIX)/share/locale/fr/LC_MESSAGES/$(EXE_NAME).mo

clean :
	@echo "Cleaning all targets."
	@-rm -rf $(BUILD_DIR)

.PHONY : all debug release clean i18n i18n-template i18n-template-fr i18n-clean

