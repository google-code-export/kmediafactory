METASOURCES = AUTO

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kmediafactory_slideshow.la

# Headers needed by the plugin
AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib \
	$(all_includes)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)
AM_CFLAGS = $(STDCFLAG)

# Libs needed by the plugin
kmediafactory_slideshow_la_LIBADD = $(top_builddir)/lib/libkmf.la

# LD flags for the plugin
# -module says: this is a module, i.e. something you're going to dlopen
# so e.g. it has no version number like a normal shared lib would have.
kmediafactory_slideshow_la_LDFLAGS = \
	-module $(KDE_PLUGIN) \
	$(all_libraries)

# rc file containing the GUI for the plugin
pluginsdir = $(kde_datadir)/kmediafactory_slideshow
plugins_DATA = kmediafactory_slideshowui.rc

# Install the desktop file needed to detect the plugin
kde_services_DATA = kmediafactory_slideshow.desktop slideshowplugin.kcfg

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kmediafactory_slideshow.pot

# Srcs for the plugin
kmediafactory_slideshow_la_SOURCES = slideshowplugin.cpp slideshowobject.cpp \
	slideshowpluginsettings.kcfgc slideshowlayout.ui slideshowproperties.cpp
noinst_HEADERS = slideshowproperties.h
INCLUDES = -I$(top_srcdir)/ffmpeg/cppapi -I$(top_builddir)/lib
