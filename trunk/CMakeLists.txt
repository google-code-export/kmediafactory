project(kmediafactory)

macro(IS_VERSION version)
  math(EXPR V1 "(((${ARGV1}) << 16) | ((${ARGV2}) << 8) | (${ARGV3}))")
  math(EXPR V2 "(((${ARGV4}) << 16) | ((${ARGV5}) << 8) | (${ARGV6}))")
  if (${V2} GREATER ${V1} OR ${V2} EQUAL ${V1})
     set(${version} TRUE)
  else (${V2} GREATER ${V1} OR ${V2} EQUAL ${V1})
     set(${version} FALSE)
  endif (${V2} GREATER ${V1} OR ${V2} EQUAL ${V1})
endmacro(IS_VERSION)

set(KMF_MAJOR_VERSION "0")
set(KMF_MINOR_VERSION "6")
set(KMF_BUILD_VERSION "0")
set(VERSION "${KMF_MAJOR_VERSION}.${KMF_MINOR_VERSION}.${KMF_BUILD_VERSION}")

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)

check_function_exists(va_copy VA_COPY)

# Required packages
find_package(Fontconfig REQUIRED)
macro_log_feature(FONTCONFIG_FOUND "fontconfig" "Get font information" "http://www.fontconfig.org/" TRUE "" "")

find_package(MSGFMT REQUIRED)
macro_log_feature(MSGFMT_FOUND "gettext" "For translations" "http://www.gnu.org/software/gettext/" TRUE "" "")
find_program(MSGFMT_EXECUTABLE msgfmt)
find_program(MSGMERGE_EXECUTABLE msgmerge)
find_program(XGETTEXT_EXECUTABLE xgettext)

find_program(ZIP_EXECUTABLE zip)
macro_log_feature(ZIP_EXECUTABLE "zip" "For packing templates" "http://www.info-zip.org/" TRUE "" "")

find_program(DVDAUTHOR_EXECUTABLE dvdauthor)
macro_log_feature(DVDAUTHOR_EXECUTABLE "dvdauthor" "For making DVDs" "http://dvdauthor.sourceforge.net/" TRUE "0.6.11" "")

find_program(FFMPEG_EXECUTABLE ffmpeg)
macro_log_feature(FFMPEG_EXECUTABLE "ffmpeg" "For extracting frames from videos" "http://ffmpeg.mplayerhq.hu/" TRUE "" "")

find_program(MPEG2ENC_EXECUTABLE mpeg2enc)
macro_log_feature(MPEG2ENC_EXECUTABLE "mpeg2enc" "For making mpeg files" "http://mjpeg.sourceforge.net/" TRUE "" "")

find_program(MPLAYER_EXECUTABLE mpeg2enc)
macro_log_feature(MPLAYER_EXECUTABLE "mplayer" "For getting info from video file" "http://www.mplayerhq.hu/" TRUE "" "")

# Optional packages
find_package(LibDvdread)
macro_log_feature(LIBDVDREAD_FOUND "libdvdread" "For reading dvd info" "http://www.dtek.chalmers.se/groups/dvd/downloads.shtml" FALSE "" "")
macro_bool_to_01(LIBDVDREAD_FOUND HAVE_LIBDVDREAD)

find_program(DVDSLIDESHOW_EXECUTABLE dvd-slideshow)
macro_log_feature(DVDSLIDESHOW_EXECUTABLE "dvd-slideshow" "For making slideshows" "http://dvd-slideshow.sourceforge.net/wiki/Main_Page" FALSE "" "")

find_program(K3B_EXECUTABLE k3b)
macro_log_feature(K3B_EXECUTABLE "k3b" "For burning DVDs" "http://www.k3b.org/" FALSE "" "")

find_program(XINE_EXECUTABLE xine)
macro_log_feature(XINE_EXECUTABLE "xine" "For DVD preview" "http://xinehq.de/" FALSE "" "")

find_program(KAFFEINE_EXECUTABLE kaffeine)
macro_log_feature(KAFFEINE_EXECUTABLE "kaffeine" "For DVD preview" "http://kaffeine.kde.org/" FALSE "" "")

find_program(OO_EXECUTABLE soffice)
macro_log_feature(OO_EXECUTABLE "openoffice.org" "For converting OpenOffice docs to slideshow" "http://www.openoffice.org/" FALSE "" "also needs ghostscript")

find_program(GS_EXECUTABLE gs)
macro_log_feature(GS_EXECUTABLE "ghostscript" "For converting pdf to slideshow" "http://pages.cs.wisc.edu/~ghost/" FALSE "" "")

# Are we using this?
find_program(EXTRACTRC_EXECUTABLE extractrc)

add_definitions(
    ${QT_DEFINITIONS}
    ${QT_QTDBUS_DEFINITIONS}
    ${KDE4_DEFINITIONS}
    -DHAVE_CONFIG_H=1
)

include_directories(
    ${QDBUS_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${KDE4_INCLUDES}
)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )

add_subdirectory(lib)
add_subdirectory(kstore)
add_subdirectory(kmediafactory)
add_subdirectory(src)
add_subdirectory(plugins)
add_subdirectory(mimetypes)
add_subdirectory(icons)
add_subdirectory(scripts)
add_subdirectory(media)
add_subdirectory(po)
add_subdirectory(doc)

macro_display_feature_log()

# CPack
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "KMediaFactory is easy to use template based dvd authoring tool.")
set(CPACK_PACKAGE_VENDOR "KMediaFactory Team")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "KMediaFactory ${VERSION}")
set(CPACK_PACKAGE_EXECUTABLES "kmediafactory")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "kmediafactory-${VERSION}")
SET(CPACK_GENERATOR "TBZ2")
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_IGNORE_FILES
  ".svn/"
  "debug/"
  "templates/"
  "distros/"
  "package/"
  "commit.sh"
  "links.sh"
  "snapshot.sh"
  "kmediafactory.kdevelop"
  "kmediafactory.kdevelop.pcs"
  "kmediafactory.kdevses"
  "snapshot.changelog"
  ".*.tar.bz2"
  "~$"
)
include(CPack)
