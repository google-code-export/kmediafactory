# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kmediafactory_output.la

# Srcs for the plugin
kmediafactory_output_la_SOURCES = \
	outputplugin.cpp \
	dvdauthorobject.cpp \
	dvddirectoryobject.cpp \
	k3bobject.cpp \
	dvdinfo.cpp \
	dvdinfolayout.ui

noinst_HEADERS = \
	dvdauthorobject.h \
	outputplugin.h \
	dvddirectoryobject.h \
	k3bobject.h \
	dvdinfo.h

# Headers needed by the plugin
AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib \
	$(all_includes)

# LD flags for the plugin
# -module says: this is a module, i.e. something you're going to dlopen
# so e.g. it has no version number like a normal shared lib would have.
kmediafactory_output_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# Libs needed by the plugin
kmediafactory_output_la_LIBADD = $(top_builddir)/lib/libkmf.la

METASOURCES = AUTO

AM_CFLAGS = $(STDCFLAG)

# rc file containing the GUI for the plugin
pluginsdir = $(kde_datadir)/kmediafactory_output
plugins_DATA = kmediafactory_outputui.rc

# Install the desktop file needed to detect the plugin
kde_services_DATA = kmediafactory_output.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kmediafactory_output.pot


