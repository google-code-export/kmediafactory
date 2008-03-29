# set the include path for X, qt and KDE
INCLUDES = -I$(top_srcdir)/lib $(all_includes)

# these are the headers for your project
noinst_HEADERS = kmediafactoryplayer.h postfilter.h kmfplayer.h kxinewidget.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO
AM_CFLAGS = $(STDCFLAG)

noinst_LTLIBRARIES = libkxinewidget.la
libkxinewidget_la_SOURCES = kxinewidget.cpp postfilter.cpp

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kmediafactoryplayer

# the application source, library search path, and link libraries
kmediafactoryplayer_SOURCES = main.cpp \
	kmediafactoryplayer.cpp kmfplayer.cpp kmfplayerlayout.ui positionslider.cpp
kmediafactoryplayer_LDFLAGS = $(KDE_RPATH) $(all_libraries)
kmediafactoryplayer_LDADD = \
	libkxinewidget.la $(X_PRE_LIBS) $(LIB_KDEUI) \
	$(LIB_KIO) $(XINE_LIBS)

# this is where the desktop file will go
shelldesktopdir = $(kde_appsdir)/Utilities
shelldesktop_DATA = kmediafactoryplayer.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir = $(kde_datadir)/kmediafactory
shellrc_DATA = kmediafactoryplayerui.rc

messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kmediafactory_player.pot