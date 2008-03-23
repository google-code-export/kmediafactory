noinst_LTLIBRARIES = libqffmpeg.la

INCLUDES = -I$(top_srcdir)/ffmpeg -I$(top_srcdir)/ffmpeg/libavcodec \
	-I$(top_srcdir)/ffmpeg/libavutil $(all_includes)

libqffmpeg_la_LIBADD = $(top_builddir)/ffmpeg/libavformat/libavformat.la \
	$(top_builddir)/ffmpeg/libavcodec/libavcodec.la $(top_builddir)/ffmpeg/libavutil/libavutil.la $(LIB_DVDREAD)

libqffmpeg_la_LDFLAGS = $(all_libraries)

AM_CFLAGS = -std=gnu99 -w

AM_CPPFLAGS = -DHAVE_AV_CONFIG_H

libqffmpeg_la_SOURCES = ffmpeg.c qffmpeg.cpp dopr.c qffmpegconverter.cpp \
			qffmpeglogger.cpp qffmpegencoder.cpp

noinst_HEADERS = ffmpeg.h qffmpeg.h dopr.h qffmpegconverter.h qffmpeglogger.h qffmpegencoder.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO
