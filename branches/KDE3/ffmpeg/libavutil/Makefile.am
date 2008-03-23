noinst_LTLIBRARIES = libavutil.la

INCLUDES = -I$(top_srcdir)/ffmpeg -I$(top_srcdir)/ffmpeg/libavcodec \
	$(all_includes)

libavutil_la_LDFLAGS = $(all_libraries)

AM_CPPFLAGS = -DHAVE_AV_CONFIG_H -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_GNU_SOURCE -DCONFIG_ENCODERS

AM_CFLAGS = -std=gnu99 -w

libavutil_la_SOURCES = \
	mathematics.c \
	integer.c \
	rational.c \
	intfloat_readwrite.c \
	crc.c

noinst_HEADERS = \
	avutil.h common.h intfloat_readwrite.h rational.h \
	bswap.h integer.h mathematics.h \
	crc.h
