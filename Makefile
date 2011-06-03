#export INSTALL_PREFIX := /usr
#export BINARY := videokiss
#export I18N_BUILD := build/i18n

export SOURCE_DIRS := $(CURDIR)
export HEADER_DIRS := $(SOURCE_DIRS)
export EXE_NAME := videokiss
export EXE_ICON := $(CURDIR)/video-x-generic.png
export CONFIG_FILE := config.lua
export GLADE_FILE := $(EXE_NAME).glade
export BUILD_DIR := $(CURDIR)/build
export PACKAGES := glib-2.0 gtk+-2.0 mlt-framework libglade-2.0

all : debug

debug release:
	@mkdir -p $(BUILD_DIR)/$@
	@$(MAKE) -s $@ -f Makefile.common TARGET=$@

clean :
	@echo "Cleaning all targets."
	@-rm -rf $(BUILD_DIR)
	
.PHONY : all debug release clean

