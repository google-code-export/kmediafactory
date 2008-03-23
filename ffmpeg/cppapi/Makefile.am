noinst_LTLIBRARIES = libqffmpeg.la

INCLUDES = $(all_includes)

AM_CPPFLAGS = \
	$(FFMPEG_CPPFLAGS) \
	$(all_includes)

libqffmpeg_la_LIBADD = $(FFMPEG_LIBS) $(LIB_DVDREAD)

libqffmpeg_la_LDFLAGS = $(FFMPEG_LDFLAGS) $(all_libraries)

libqffmpeg_la_SOURCES = ffmpeg.c qffmpeg.cpp dopr.c qffmpegconverter.cpp \
			qffmpeglogger.cpp qffmpegencoder.cpp

noinst_HEADERS = ffmpeg.h qffmpeg.h dopr.h qffmpegconverter.h qffmpeglogger.h qffmpegencoder.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO
