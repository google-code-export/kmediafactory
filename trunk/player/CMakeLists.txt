#INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/lib ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )

set(kxinewidget_STAT_SRCS
    kxinewidget.cpp
    postfilter.cpp
)

kde4_automoc(${kxinewidget_STAT_SRCS})
kde4_add_library(kxinewidget STATIC ${kxinewidget_STAT_SRCS})


########### next target ###############

set(kmediafactoryplayer_SRCS
    main.cpp
    kmediafactoryplayer.cpp
    kmfplayer.cpp
    positionslider.cpp
)

kde4_automoc(${kmediafactoryplayer_SRCS})
kde4_add_ui_files(kmediafactoryplayer_SRCS kmfplayer.ui )

kde4_add_executable(kmediafactoryplayer ${kmediafactoryplayer_SRCS})

target_link_libraries(kmediafactoryplayer ${QT_AND_KDECORE_LIBS})

install(FILES kmediafactoryplayerui.rc DESTINATION ${DATA_INSTALL_DIR}/kmediafactory)
install(FILES kmediafactoryplayer.desktop DESTINATION ${XDG_APPS_DIR})
install(TARGETS kmediafactoryplayer DESTINATION ${BIN_INSTALL_DIR} )

#KDE_PLACEHOLDER()



#original Makefile.am contents follow:

## set the include path for X, qt and KDE
#INCLUDES = -I$(top_srcdir)/lib $(all_includes)
#
## these are the headers for your project
#noinst_HEADERS = kmediafactoryplayer.h postfilter.h kmfplayer.h kxinewidget.h
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#AM_CFLAGS = $(STDCFLAG)
#
#noinst_LTLIBRARIES = libkxinewidget.la
#libkxinewidget_la_SOURCES = kxinewidget.cpp postfilter.cpp
#
##########################################################################
## APPLICATION SECTION
##########################################################################
## this is the program that gets installed.  it's name is used for all
## of the other Makefile.am variables
#bin_PROGRAMS = kmediafactoryplayer
#
## the application source, library search path, and link libraries
#kmediafactoryplayer_SOURCES = main.cpp \
#	kmediafactoryplayer.cpp kmfplayer.cpp kmfplayerlayout.ui positionslider.cpp
#kmediafactoryplayer_LDFLAGS = $(KDE_RPATH) $(all_libraries)
#kmediafactoryplayer_LDADD = \
#	libkxinewidget.la $(X_PRE_LIBS) $(LIB_KDEUI) \
#	$(LIB_KIO) $(XINE_LIBS)
#
## this is where the desktop file will go
#shelldesktopdir = $(kde_appsdir)/Utilities
#shelldesktop_DATA = kmediafactoryplayer.desktop
#
## this is where the shell's XML-GUI resource file goes
#shellrcdir = $(kde_datadir)/kmediafactory
#shellrc_DATA = kmediafactoryplayerui.rc
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp *.h -o $(podir)/kmediafactory_player.pot