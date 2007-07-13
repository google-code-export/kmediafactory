project(kmediafactory)

set(KMF_MAJOR_VERSION "0")
set(KMF_MINOR_VERSION "6")
set(KMF_BUILD_VERSION "0_alpha1")
set(VERSION "${KMF_MAJOR_VERSION}.${KMF_MINOR_VERSION}.${KMF_BUILD_VERSION}")

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)

find_package(Xine REQUIRED)
find_package(Fontconfig REQUIRED)
find_package(LibDvdread)

macro_bool_to_01(LIBDVDREAD_FOUND HAVE_LIBDVDREAD)

check_function_exists(va_copy VA_COPY)

find_program(EXTRACTRC_EXECUTABLE extractrc)
find_program(MSGFMT_EXECUTABLE msgfmt)
find_program(MSGMERGE_EXECUTABLE msgmerge)
find_program(XGETTEXT_EXECUTABLE xgettext)
find_program(ZIP_EXECUTABLE zip)
find_program(DVDAUTHOR_EXECUTABLE dvdauthor)
find_program(MPEG2ENC_EXECUTABLE mpeg2enc)
find_program(DVDSLIDESHOW_EXECUTABLE dvd-slideshow)

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
  "commit.sh"
  "dist.sh"
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
