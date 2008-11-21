debug :
	mkdir -p build/debug 2>/dev/null
	( cd build/debug ; cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ../.. && make )
	
release :
	mkdir -p build/release 2>/dev/null
	( cd build/release ; cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ../.. && make )

clean :
	rm -rf build	
