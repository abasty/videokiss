PROJECT(VIDEOKISS)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeScripts)

FIND_PACKAGE(GTK2 REQUIRED)
FIND_PACKAGE(Glade REQUIRED)
FIND_PACKAGE(MLT REQUIRED)

INCLUDE_DIRECTORIES(
	${VIDEOKISS_SOURCE_DIR}/lua/include
	${MLT_INCLUDE_DIRS}
	${GTK2_INCLUDE_DIRS}
	${GLADE_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
	${VIDEOKISS_SOURCE_DIR}/lua
)

SET(CONFIG_LUA "config.lua")

ADD_CUSTOM_COMMAND(
	OUTPUT  ${VIDEOKISS_BINARY_DIR}/${CONFIG_LUA}.o
	COMMAND ln -sf ${VIDEOKISS_SOURCE_DIR}/${CONFIG_LUA} .
	COMMAND objcopy --input binary --output elf32-i386 --binary-architecture i386 ${CONFIG_LUA} ${CONFIG_LUA}.o
	DEPENDS ${CONFIG_LUA}
	COMMENT "Objectifying ${CONFIG_LUA}..."
)

SET(VIDEOKISS_GLADE "videokiss.glade")

ADD_CUSTOM_COMMAND(
	OUTPUT  ${VIDEOKISS_BINARY_DIR}/${VIDEOKISS_GLADE}.o
	COMMAND ln -sf ${VIDEOKISS_SOURCE_DIR}/${VIDEOKISS_GLADE} .
	COMMAND objcopy --input binary --output elf32-i386 --binary-architecture i386 ${VIDEOKISS_GLADE} ${VIDEOKISS_GLADE}.o
	DEPENDS ${VIDEOKISS_GLADE}
	COMMENT "Objectifying ${VIDEOKISS_GLADE}..."
)

SET(VIDEOKISS_ICON "video-x-generic.png")

ADD_CUSTOM_COMMAND(
	OUTPUT  ${VIDEOKISS_SOURCE_DIR}/videokiss.icon
	COMMAND gdk-pixbuf-csource --raw --name=videokiss_icon ${VIDEOKISS_SOURCE_DIR}/${VIDEOKISS_ICON} >${VIDEOKISS_SOURCE_DIR}/videokiss.icon
	DEPENDS ${VIDEOKISS_ICON}
	COMMENT "Objectifying ${VIDEOKISS_ICON}..."
)

ADD_EXECUTABLE(
	# exe
	videokiss

	# sources
	videokiss.c
	inigo.c
	montage.c

	# headers
	videokiss.h
	
	# embedded objects(lua script, glade file)
	${VIDEOKISS_BINARY_DIR}/${CONFIG_LUA}.o
	${VIDEOKISS_BINARY_DIR}/${VIDEOKISS_GLADE}.o
	
	# app icon
	videokiss.icon
)

SET_TARGET_PROPERTIES(videokiss PROPERTIES COMPILE_FLAGS "-Wall")

ADD_DEFINITIONS(-DLUA_USE_LINUX)

TARGET_LINK_LIBRARIES(
	# exe
	videokiss

	# libs
	lua
	${GTK2_LIBRARIES}
	${GLADE_LIBRARIES}
	${MLT_LIBRARIES}
)

