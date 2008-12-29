INSTALL_PREFIX=/usr
BINARY=videokiss
I18N_BUILD=build/i18n

debug : i18n
	mkdir -p build/debug 2>/dev/null
	( cd build/debug ; cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../.. && make )
	
release : i18n
	mkdir -p build/release 2>/dev/null
	( cd build/release ; cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ../.. && make )
	
i18n :
	mkdir -p ${I18N_BUILD} 2>/dev/null
	msgfmt fr.po -o ${I18N_BUILD}/fr.mo

install : 
	install -s build/release/${BINARY} ${INSTALL_PREFIX}/bin
	install ${BINARY}.desktop ${INSTALL_PREFIX}/share/applications/${BINARY}.desktop
	install ${I18N_BUILD}/fr.mo ${INSTALL_PREFIX}/share/locale/fr/LC_MESSAGES/${BINARY}.mo
	
uninstall :
	rm ${INSTALL_PREFIX}/bin/${BINARY}
	rm ${INSTALL_PREFIX}/share/applications/${BINARY}.desktop
	rm ${INSTALL_PREFIX}/share/locale/fr/LC_MESSAGES/${BINARY}.mo

clean :
	rm -rf build
	rm -f *.icon 2>/dev/null

i18n-template : *.glade *.h
	xgettext -a *.h -o ${BINARY}.pot
	xgettext -j *.glade -o ${BINARY}.pot
	
i18n-template-fr : i18n-template
	msginit -l fr_FR.utf8
	
i18n-clean :
	rm *.po* 2>/dev/null
	
splint :
	splint +unixlib -unrecog `pkg-config --cflags glib-2.0 gtk+-2.0 mlt-framework libglade-2.0` -I./lua/include -weak *.c
