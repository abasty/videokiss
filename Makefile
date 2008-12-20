INSTALL_PREFIX=/usr
BINARY=videokiss

debug :
	mkdir -p build/debug 2>/dev/null
	( cd build/debug ; cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../.. && make )
	
release :
	mkdir -p build/release 2>/dev/null
	( cd build/release ; cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ../.. && make )

install : release
	install -s build/release/${BINARY} ${INSTALL_PREFIX}/bin
	install ${BINARY}.desktop ${INSTALL_PREFIX}/share/applications/${BINARY}.desktop
	
uninstall :
	rm ${INSTALL_PREFIX}/bin/${BINARY}
	rm ${INSTALL_PREFIX}/share/applications/${BINARY}.desktop

clean :
	rm -rf build
	rm -f *.icon 2>/dev/null

splint :
	splint +unixlib -unrecog `pkg-config --cflags glib-2.0 gtk+-2.0 mlt-framework libglade-2.0` -I./lua/include -weak *.c
