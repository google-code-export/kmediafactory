noinst_LTLIBRARIES = libkstore.la

INCLUDES = $(all_includes)

libkstore_la_LIBADD = $(LIB_KIO)

libkstore_la_LDFLAGS = $(all_libraries)

KDE_CXXFLAGS = $(USE_RTTI)
AM_CFLAGS = $(STDCFLAG)

libkstore_la_SOURCES = \
	koStore.cc \
	koTarStore.cc \
	koDirectoryStore.cc \
	koZipStore.cc \
	koStoreBase.cc

include_HEADERS = \
	koStore.h \
	koStoreDevice.h
