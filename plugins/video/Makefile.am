# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kmediafactory_video.la

# Srcs for the plugin
kmediafactory_video_la_SOURCES = \
	videoplugin.cpp \
	videoobject.cpp \
	videooptionslayout.ui \
	videooptions.cpp \
	chapters.cpp \
	chapterslayout.ui \
	kmfchapterlistview.cpp \
	autochapterslayout.ui \
	languageselectionlayout.ui \
	videopluginsettings.kcfgc \
	videoconfiglayout.ui \
	subtitleoptionslayout.ui \
	subtitleoptions.cpp \
	addchapterlayout.ui

noinst_HEADERS = \
	videooptions.h \
	chapters.h \
	kmfchapterlistview.h \
	subtitleoptions.h

kmediafactoryincludedir = $(includedir)/kmediafactory/plugin
kmediafactoryinclude_HEADERS = videoobject.h

# Headers needed by the plugin
AM_CPPFLAGS = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/ffmpeg \
	-I$(top_srcdir)/ffmpeg/libavcodec \
	-I$(top_srcdir)/ffmpeg/cppapi \
	$(all_includes)

# LD flags for the plugin
# -module says: this is a module, i.e. something you're going to dlopen
# so e.g. it has no version number like a normal shared lib would have.
kmediafactory_video_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# Libs needed by the plugin
kmediafactory_video_la_LIBADD = $(top_builddir)/lib/libkmf.la

METASOURCES = AUTO

AM_CFLAGS = $(STDCFLAG)

# rc file containing the GUI for the plugin
pluginsdir = $(kde_datadir)/kmediafactory_video
plugins_DATA = kmediafactory_videoui.rc

# Install the desktop file needed to detect the plugin
kde_services_DATA = kmediafactory_video.desktop videoplugin.kcfg

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kmediafactory_video.pot
