PROJECT(AUTOPROD)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeScripts)

FIND_PACKAGE(GTK2 REQUIRED)
FIND_PACKAGE(Glade REQUIRED)
FIND_PACKAGE(MLT REQUIRED)

INCLUDE_DIRECTORIES(
	${AUTOPROD_SOURCE_DIR}/lua/include
	${MLT_INCLUDE_DIRS}
	${GTK2_INCLUDE_DIRS}
	${GLADE_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
	${AUTOPROD_SOURCE_DIR}/lua
)

ADD_CUSTOM_COMMAND(
	OUTPUT  ${AUTOPROD_BINARY_DIR}/config.lua.o
	COMMAND ln -sf ${AUTOPROD_SOURCE_DIR}/config.lua .
	COMMAND objcopy --input binary --output elf32-i386 --binary-architecture i386 config.lua config.lua.o
	DEPENDS config.lua
	COMMENT "Objectifying config.lua..."
)

ADD_EXECUTABLE(
	# exe
	autoprod

	# sources
	autoprod.c
	inigo.c
	montage.c

	# headers
	autoprod.h
	
	# objects
	${AUTOPROD_BINARY_DIR}/config.lua.o
)

ADD_DEFINITIONS(-DLUA_USE_LINUX)

TARGET_LINK_LIBRARIES(
	# exe
	autoprod

	# libs
	lua
	${GTK2_LIBRARIES}
	${GLADE_LIBRARIES}
	${MLT_LIBRARIES}
)

