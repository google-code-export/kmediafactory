# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kmediafactory_dvimport.la

# Srcs for the plugin
kmediafactory_dvimport_la_SOURCES = \
	dvimportplugin.cpp \
	dvimportpluginsettings.kcfgc \
	progresslayout.ui

noinst_HEADERS = \
	dvimportplugin.h

# Headers needed by the plugin
AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/ffmpeg/cppapi \
	-I$(top_srcdir)/plugins/video \
	-I$(top_builddir)/plugins/video \
	$(DV_CPPFLAGS) \
	$(all_includes)

# LD flags for the plugin
# -module says: this is a module, i.e. something you're going to dlopen
# so e.g. it has no version number like a normal shared lib would have.
kmediafactory_dvimport_la_LDFLAGS = \
	-module $(KDE_PLUGIN) \
	$(DV_LDFLAGS) \
	$(all_libraries)

# Libs needed by the plugin
kmediafactory_dvimport_la_LIBADD = \
	$(top_builddir)/plugins/video/kmediafactory_video.la

METASOURCES = AUTO

AM_CFLAGS = $(STDCFLAG)

# rc file containing the GUI for the plugin
pluginsdir = $(kde_datadir)/kmediafactory_dvimport
plugins_DATA = kmediafactory_dvimportui.rc

# Install the desktop file needed to detect the plugin
kde_services_DATA = kmediafactory_dvimport.desktop dvimportplugin.kcfg

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kmediafactory_dvimport.pot

