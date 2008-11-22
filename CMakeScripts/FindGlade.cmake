#	GLADE_INCLUDE_DIRS	- Directories to include to use glade
#	GLADE_LIBRARIES		- Files to link against to use glade
#	GLADE_FOUND				- Glade was found

SET( GTK2_FIND_REQUIRED ${GLADE_FIND_REQUIRED})
include( FindGTK2)

FIND_PATH( GLADE_INCLUDE_DIR glade/glade.h
	PATHS /opt/gtk/include
			/opt/gnome/include
			/usr/include
			/usr/local/include
	PATH_SUFFIXES libglade-2.0
	NO_DEFAULT_PATH
)
MARK_AS_ADVANCED( GLADE_INCLUDE_DIR)

find_library( GLADE_LIBRARY 
	NAMES glade-2.0 
	PATHS /opt/gtk
			/opt/gnome
			/usr
			/usr/local
	PATH_SUFFIXES lib lib64
	NO_DEFAULT_PATH
)
MARK_AS_ADVANCED( GLADE_LIBRARY)

IF( GTK2_FOUND AND GLADE_INCLUDE_DIR AND GLADE_LIBRARY)
	 
	SET( GLADE_FOUND "YES" )
	
	SET( GLADE_INCLUDE_DIRS 
		${GLADE_INCLUDE_DIR}
		${GTK2_INCLUDE_DIRS}
	)

	SET( GLADE_LIBRARIES 
		${GLADE_LIBRARY}
		${GTK2_LIBRARIES}
	)

ENDIF( GTK2_FOUND AND GLADE_INCLUDE_DIR AND GLADE_LIBRARY)

if( GLADE_FOUND)

	if( NOT GLADE_FIND_QUIETLY)
		message( STATUS "Found Glade libraries: ${GLADE_LIBRARIES}")
		message( STATUS "Found Glade inc dirs: ${GLADE_INCLUDE_DIRS}")
	endif( NOT GLADE_FIND_QUIETLY)
	
else( GLADE_FOUND)

	if( GLADE_FIND_REQUIRED)
		message( FATAL_ERROR "Could not find Glade")
	else( GLADE_FIND_REQUIRED)
		if( NOT GLADE_FIND_QUIETLY)
			message( STATUS "Could not find Glade")
		endif( NOT GLADE_FIND_QUIETLY)
	endif( GLADE_FIND_REQUIRED)
	
endif( GLADE_FOUND)
