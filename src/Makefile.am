## Makefile.am for kmediafactory

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = kmediafactory

# set the include path for X, qt and KDE
INCLUDES = \
	-I$(top_srcdir) \
	-I$(top_srcdir)/lib \
	$(all_includes)

# the library search path.
kmediafactory_LDFLAGS = $(KDE_RPATH) $(all_libraries)

AM_CPPFLAGS = -DHAVE_VA_COPY
AM_CFLAGS = $(STDCFLAG)

# the libraries to link against.
kmediafactory_LDADD = \
	$(top_builddir)/lib/libkmf.la \
	$(LIB_KHTML) \
	$(LIB_KNEWSTUFF)

# which sources should be compiled for kmediafactory
kmediafactory_SOURCES = kmfuiinterface.cpp kmfprojectinterface.cpp	main.cpp \
	tools.cpp toolslayout.ui kmfapplication.cpp 	mediapagelayout.ui mediapage.cpp \
	templatepagelayout.ui templatepage.cpp 	outputpagelayout.ui outputpage.cpp kmediafactory.cpp \
	kmfproject.cpp 	projectoptionslayout.ui 	projectoptions.cpp kmficonview.cpp \
		kmflogger.cpp logview.cpp logviewlayout.ui kmfprogressitem.cpp \
		kmfprogresslistview.cpp kmediafactorysettings.kcfgc kmfoptionslayout.ui 	sizewidget.cpp \
		sizewidgetlayout.ui kmfdcopinterface.cpp kmfdcopinterface.skel 	toolpropertieslayout.ui \
		toolproperties.cpp kmfnewstuff.cpp

# these are the headers for your project
noinst_HEADERS = kmediafactory.h kmfproject.h projectoptions.h \
		kmfprojectinterface.h kmfuiinterface.h kmflogger.h logview.h kmfprogressitem.h \
		kmfprogresslistview.h sizewidget.h kmfdcopinterface.h tools.h kmfnewstuff.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kmediafactory.pot

# this is where the kdelnk file will go
kdelnkdir   = $(kde_appsdir)/Utilities
kdelnk_DATA = kmediafactory.desktop kmediafactory.kcfg

# this is where the XML-GUI resource file goes
rcdir = $(kde_datadir)/kmediafactory
rc_DATA = kmediafactoryui.rc
