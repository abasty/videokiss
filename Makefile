debug :
	mkdir -p build/debug 2>/dev/null
	( cd build/debug ; cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../.. && make )
	
release :
	mkdir -p build/release 2>/dev/null
	( cd build/release ; cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ../.. && make )

install :
	( cd build/release && make install)

clean :
	rm -rf build
	rm -f *.icon 2>/dev/null

splint :
	splint +unixlib -unrecog `pkg-config --cflags glib-2.0 gtk+-2.0 mlt-framework libglade-2.0` -I./lua/include -weak *.c
