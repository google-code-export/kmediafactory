project(kmediafactory)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

find_package(KDE4 REQUIRED)
include(KDE4Defaults)
include(MacroLibrary)

find_package(Xine REQUIRED)
find_package(Fontconfig REQUIRED)

#find_package(libdv)
#find_package(dvdread)
#find_package(xinerama)
#find_package(theora)
#find_package(ogg)
#find_package(unopkg)

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

link_directories(${KDE4_LIB_DIR})

include_directories(
    ${QDBUS_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${KDE4_INCLUDES}
)

configure_file(config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h )

add_subdirectory(kmediafactory)
add_subdirectory(lib)
add_subdirectory(kstore)
add_subdirectory(src)
add_subdirectory(plugins)
add_subdirectory(mimetypes)
add_subdirectory(icons)
