METASOURCES = AUTO
INCLUDES= \
	-I$(top_srcdir)/src \
	$(all_includes)

# The library containing the plugin base class
lib_LTLIBRARIES =  libkmediafactoryinterfaces.la

libkmediafactoryinterfaces_la_SOURCES = plugin.cpp projectinterface.cpp \
							uiinterface.cpp kmfobject.cpp
libkmediafactoryinterfaces_la_LIBADD =

libkmediafactoryinterfaces_la_LDFLAGS = $(all_libraries) -version-info 0:0:0

AM_CFLAGS = $(STDCFLAG)

# headerfiles are installed in a kmediafactory subdirectory for convenience
kmediafactoryincludedir = $(includedir)/kmediafactory
kmediafactoryinclude_HEADERS = plugin.h projectinterface.h uiinterface.h \
	kmfobject.h

# Definition of the service type
kde_servicetypes_DATA = kmediafactoryplugin.desktop

noinst_HEADERS =
