SUBDIRS = pics

lib_LTLIBRARIES = libkmf.la

libkmf_la_SOURCES = \
	qdvdinfo.cpp \
	color.cpp \
	rect.cpp \
	font.cpp \
	dvdauthorparser.cpp \
	kmflanguagewidgets.cpp \
	kmfimageview.cpp kmftime.cpp \
	kmffontchooser.cpp \
	kmftools.cpp \
	kmfmultiurldialoglayout.ui \
	kmfmultiurldialog.cpp \
	kmftoolbutton.cpp

noinst_HEADERS = \
	color.h \
	rect.h \
	font.h \
	dvdauthorparser.h \
	kmftime.h \
	kmffontchooser.h \
	kmftools.h \
	kmfmultiurldialog.h \
	kmftoolbutton.h \
	kmf_stddef.h

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
	$(top_builddir)/ffmpeg/cppapi/libqffmpeg.la \
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
