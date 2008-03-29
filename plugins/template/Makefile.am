INCLUDES = -I$(top_srcdir)/ffmpeg -I$(top_srcdir)/ffmpeg/cppapi \
	-I$(top_srcdir)/ffmpeg/libavcodec -I$(top_srcdir)/ffmpeg/libavutil -I$(top_srcdir)/lib -I$(top_srcdir) \
	$(all_includes)

METASOURCES = AUTO
SUBDIRS = preview_6 preview_3 preview_1 simple

AM_CPPFLAGS = $(all_includes)
KDE_CXXFLAGS = $(USE_EXCEPTIONS)
AM_CFLAGS = $(STDCFLAG)

# Install this plugin in the KDE modules directory
kde_module_LTLIBRARIES = kmediafactory_template.la

# Srcs for the plugin
kmediafactory_template_la_SOURCES = templateplugin.cpp templateobject.cpp \
			kmfwidget.cpp kmflabel.cpp kmfframe.cpp kmfmenupage.cpp kmfbutton.cpp kmfmenu.cpp \
			kmfimage.cpp QMImage.cpp kmftemplatebase.cpp kmfwidgetfactory.cpp kmflayout.cpp \
			kmfgeometry.cpp kmfintl.cpp kmftemplate.cpp kconfigxml.cpp templatepluginsettings.kcfgc \
		templateconfiglayout.ui newstuffobject.cpp templatenewstuff.cpp

# Libs needed by the plugin
kmediafactory_template_la_LIBADD = \
	$(top_builddir)/lib/libkmf.la \
	-lqui \
	$(LIB_KNEWSTUFF)

# LD flags for the plugin
# -module says: this is a module, i.e. something you're going to dlopen
# so e.g. it has no version number like a normal shared lib would have.
kmediafactory_template_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

# rc file containing the GUI for the plugin
pluginsdir = $(kde_datadir)/kmediafactory_template
plugins_DATA = kmediafactory_templateui.rc templateplugin.kcfg

# Install the desktop file needed to detect the plugin
kde_services_DATA = kmediafactory_template.desktop

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/kmediafactory_template.pot

kmft-messages:
	list='$(SUBDIRS)'; for subdir in $$list; do \
		(cd $$subdir && make kmft-messages) \
	done

kmft-merge:
	list='$(SUBDIRS)'; for subdir in $$list; do \
		(cd $$subdir && make kmft-merge) \
	done

noinst_HEADERS = templateobject.h kmflabel.h kmfframe.h kmfmenupage.h \
		kmfbutton.h kmfmenu.h kmftemplatebase.h kmfwidgetfactory.h kmflayout.h kmfintl.h \
		kmftemplate.h kconfigxml.h newstuffobject.h templatenewstuff.h


