#
# try to find MLT Library
#
#	MLT_INCLUDE_DIRS	- Directories to include to use MLT
#	MLT_LIBRARIES		- Files to link against to use MLT
#	MLT_FOUND				- MLT was found
###################################################################
#
#  Copyright (c) 2008 Alain Basty
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA  02110-1301, USA.
#
###################################################################
#
#  Copyright (c) 2008 Alain Basty
#  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in
#   the documentation and/or other materials provided with the
#   distribution.
#
# * Neither the name of the <ORGANIZATION> nor the names of its
#   contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

IF(UNIX)
	INCLUDE(UsePkgConfig)
	
	PKGCONFIG(mlt-framework _MLTIncDir _MLTLinkDir _MLTLinkFlags _MLTCflags)
	
	FIND_PATH(
		MLT_INCLUDE_DIR
		framework/mlt.h
		PATHS
			${_MLTIncDir}
			/usr/include/mlt
			/usr/local/include/mlt
	)
	
	MARK_AS_ADVANCED(MLT_INCLUDE_DIR)
	
	FIND_LIBRARY(MLT_LIBRARY
		NAMES mlt
		PATHS
			${_MLTLinkDir}
			/usr/lib
			/usr/local/lib
	)
	
	MARK_AS_ADVANCED(MLT_LIBRARY)
	
	IF(MLT_INCLUDE_DIR AND MLT_LIBRARY)
		SET(MLT_FOUND "YES")
		SET(MLT_INCLUDE_DIRS ${MLT_INCLUDE_DIR})
		SET(MLT_LIBRARIES ${MLT_LIBRARY})	
	ENDIF(MLT_INCLUDE_DIR AND MLT_LIBRARY)
	
	IF(MLT_FOUND)	
		IF(NOT MLT_FIND_QUIETLY)
			MESSAGE(STATUS "Found MLT libraries: ${MLT_LIBRARIES}")
			MESSAGE(STATUS "Found MLT inc dirs: ${MLT_INCLUDE_DIRS}")
		ENDIF(NOT MLT_FIND_QUIETLY)		
	ELSE(MLT_FOUND)	
		IF(MLT_FIND_REQUIRED)
			MESSAGE(FATAL_ERROR "Could not find MLT")
		ELSE(MLT_FIND_REQUIRED)
			IF(NOT MLT_FIND_QUIETLY)
				MESSAGE(STATUS "Could not find MLT")
			ENDIF(NOT MLT_FIND_QUIETLY)
		ENDIF(MLT_FIND_REQUIRED)
	ENDIF(MLT_FOUND)
ENDIF(UNIX)
