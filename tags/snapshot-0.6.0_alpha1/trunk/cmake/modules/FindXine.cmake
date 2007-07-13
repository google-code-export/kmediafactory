# - Try to find the XINE  library
# Once done this will define
#
#  XINE_FOUND - system has the XINE library
#  XINE_INCLUDE_DIR - the XINE include directory
#  XINE_LIBRARIES - The libraries needed to use XINE

# Copyright (c) 2006, Laurent Montel, <montel@kde.org>
# Copyright (c) 2006, Matthias Kretz, <kretz@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (XINE_INCLUDE_DIR AND XINE_LIBRARIES)
  # Already in cache, be silent
  set(XINE_FIND_QUIETLY TRUE)
endif (XINE_INCLUDE_DIR AND XINE_LIBRARIES)

FIND_PATH(XINE_INCLUDE_DIR xine.h
 /usr/include/
 /usr/local/include/
)


FIND_LIBRARY(XINE_LIBRARY NAMES xine
 PATHS
 /usr/lib
 /usr/local/lib
)


if (XINE_INCLUDE_DIR AND XINE_LIBRARY)
   set(XINE_FOUND TRUE)
endif (XINE_INCLUDE_DIR AND XINE_LIBRARY)


if (XINE_FOUND)
   if (NOT XINE_FIND_QUIETLY)
      message(STATUS "Found XINE: ${XINE_LIBRARY}")
   endif (NOT XINE_FIND_QUIETLY)
else (XINE_FOUND)
   if (XINE_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find XINE")
   endif (XINE_FIND_REQUIRED)
endif (XINE_FOUND)

MARK_AS_ADVANCED(XINE_INCLUDE_DIR XINE_LIBRARY)
