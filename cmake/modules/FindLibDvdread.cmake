# LIBDVDREAD_FOUND - system has the LIBDVDREAD library
# LIBDVDREAD_INCLUDE_DIR - the LIBDVDREAD include directory
# LIBDVDREAD_LIBRARY - The libraries needed to use LIBDVDREAD

IF (LIBDVDREAD_INCLUDE_DIR AND LIBDVDREAD_LIBRARIES)
  SET(LIBDVDREAD_FIND_QUIETLY TRUE)
ENDIF (LIBDVDREAD_INCLUDE_DIR AND LIBDVDREAD_LIBRARIES)

FIND_PATH(LIBDVDREAD_INCLUDE_DIR dvdread/ifo_read.h
  /usr/include/
  /usr/local/include/
)

FIND_LIBRARY(LIBDVDREAD_LIBRARY NAMES dvdread libdvdread
  PATHS
  /usr/lib/
  /usr/local/lib/
)

IF (LIBDVDREAD_INCLUDE_DIR AND LIBDVDREAD_LIBRARY)
  SET(LIBDVDREAD_FOUND TRUE)
ENDIF (LIBDVDREAD_INCLUDE_DIR AND LIBDVDREAD_LIBRARY)

IF (LIBDVDREAD_FOUND)
  IF (NOT LIBDVDREAD_FIND_QUIETLY)
    MESSAGE (STATUS "Found LIBDVDREAD: ${LIBDVDREAD_LIBRARY}")
  ENDIF (NOT LIBDVDREAD_FIND_QUIETLY)
ELSE (LIBDVDREAD_FOUND)
  IF (LIBDVDREAD_FIND_REQUIRED)
    MESSAGE (FATAL_ERROR "Could NOT find LIBDVDREAD")
  ENDIF (LIBDVDREAD_FIND_REQUIRED)
ENDIF (LIBDVDREAD_FOUND)

MARK_AS_ADVANCED(LIBDVDREAD_INCLUDE_DIR LIBDVDREAD_LIBRARY)
