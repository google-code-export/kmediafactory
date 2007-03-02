SUBDIRS = pics

lib_LTLIBRARIES = libkmf.la

libkmf_la_SOURCES = color.cpp dvdauthorparser.cpp font.cpp kmffontchooser.cpp \
	kmfimageview.cpp kmflanguagewidgets.cpp kmfmultiurldialog.cpp kmfmultiurldialoglayout.ui \
	kmftime.cpp kmftoolbutton.cpp kmftools.cpp qdvdinfo.cpp qmediafile.cpp rect.cpp \
	run.cpp

noinst_HEADERS = color.h dvdauthorparser.h font.h kmf_stddef.h kmffontchooser.h \
	kmfmultiurldialog.h kmftime.h kmftoolbutton.h kmftools.h qmediafile.h rect.h run.h

kmediafactoryincludedir = $(includedir)/kmediafactory
kmediafactoryinclude_HEADERS = \
	qdvdinfo.h \
	kmflanguagewidgets.h \
	kmfimageview.h

AM_CPPFLAGS = \
	-DQT_PLUGIN \
	-I$(top_srcdir)/lib \
	$(FONTCONFIG_CPPFLAGS) \
	$(all_includes)

libkmf_la_LDFLAGS = $(FONTCONFIG_LDFLAGS) $(all_libraries)

libkmf_la_LIBADD = \
	$(top_builddir)/kmediafactory/libkmediafactoryinterfaces.la \
	$(top_builddir)/kstore/libkstore.la \
	$(FONTCONFIG_LIBS) \
	$(LIBMAGICK_LIBS) \
	$(DVDREAD_LIBS) \
	$(DV_LIBS) \
	$(LIB_KPARTS)

METASOURCES = AUTO

AM_CFLAGS = $(STDCFLAG)

kde_widget_LTLIBRARIES = kmfwidgets.la

kmfwidgets_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries)

kmfwidgets_la_SOURCES = kmfwidgets.cpp

kmfwidgets_la_LIBADD = \
	$(LIB_KIO) \
	libkmf.la

kmfwidgets.cpp: $(srcdir)/kmediafactory.widgets
	$(MAKEKDEWIDGETS) -o kmfwidgets.cpp $(srcdir)/kmediafactory.widgets

CLEANFILES = kmfwidgets.cpp

# i18n translation messages
messages: rc.cpp
	$(XGETTEXT) *.cpp *.h -o $(podir)/libkmf.pot
