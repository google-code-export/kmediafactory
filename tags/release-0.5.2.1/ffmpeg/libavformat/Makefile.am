noinst_LTLIBRARIES = libavformat.la

INCLUDES = -I$(top_srcdir)/ffmpeg -I$(top_srcdir)/ffmpeg/libavcodec \
	-I$(top_srcdir)/ffmpeg/libavformat -I$(top_srcdir)/ffmpeg/libavutil $(all_includes)

libavformat_la_LIBADD = $(OGG_LIBS)

libavformat_la_LDFLAGS = $(OGG_LDFLAGS) $(all_libraries)

AM_CPPFLAGS = -DHAVE_AV_CONFIG_H -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
	-D_GNU_SOURCE -DCONFIG_DECODERS -DCONFIG_ENCODERS \
	-DCONFIG_MUXERS -DCONFIG_DEMUXERS -DCONFIG_PROTOCOLS -DCONFIG_NETWORK

AM_CFLAGS = -std=gnu99 -w

if HAVE_LIBOGG
OGG=ogg.c
endif

# dc1394.c barpainet.c v4l2.c

libavformat_la_SOURCES = \
	utils.c cutils.c os_support.c allformats.c \
	mpeg.c mpegts.c mpegtsenc.c ffm.c crc.c img.c img2.c raw.c rm.c \
	avienc.c avidec.c wav.c mmf.c swf.c au.c gif.c mov.c mpjpeg.c dv.c \
	yuv4mpeg.c 4xm.c flvenc.c flvdec.c movenc.c psxstr.c idroq.c ipmovie.c \
	nut.c wc3movie.c mp3.c westwood.c segafilm.c idcin.c flic.c \
	sierravmd.c matroska.c sol.c electronicarts.c nsvdec.c asf.c asf-enc.c \
	ogg2.c oggparsevorbis.c oggparsetheora.c oggparseflac.c daud.c \
	pnm.c yuv.c png.c jpeg.c gifdec.c sgi.c \
	avio.c aviobuf.c file.c \
	framehook.c $(OGG) \
	aiff.c adtsenc.c mm.c voc.c tta.c avs.c rtsp.c rtp.c udp.c \
	rtpproto.c http.c tcp.c \
	nuv.c smacker.c oggparseogm.c

noinst_HEADERS = \
	asf.h avio.h dv1394.h ogg2.h rtp.h \
	avformat.h framehook.h os_support.h rtsp.h \
	avi.h dv.h mpegts.h qtpalette.h rtspcodes.h barpainet.h


