debug :
	mkdir -p build/debug 2>/dev/null
	( cd build/debug ; cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../.. && make )
	
release :
	mkdir -p build/release 2>/dev/null
	( cd build/release ; cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ../.. && make )

clean :
	rm -rf build	

splint :
	splint +posixlib `pkg-config --cflags glib-2.0 --cflags gtk+-2.0 --cflags mlt-framework libglade-2.0` -I./lua/include -weak *.c
