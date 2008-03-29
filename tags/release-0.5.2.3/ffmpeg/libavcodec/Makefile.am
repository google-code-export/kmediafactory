if HAVE_MMX
if ARCH_X86_64
MMX=x86_64/libmmx.la
SUBDIRS = x86_64
else
MMX=i386/libmmx.la
SUBDIRS = i386
endif
endif

noinst_LTLIBRARIES = libavcodec.la

INCLUDES = -I$(top_srcdir)/ffmpeg/libavcodec -I$(top_srcdir)/ffmpeg/libavutil \
	-I$(top_srcdir)/ffmpeg/ $(all_includes)

libavcodec_la_LIBADD = $(THEORA_LIBS) $(MMX)

libavcodec_la_LDFLAGS = $(THEORA_LDFLAGS) $(all_libraries)

AM_CPPFLAGS = -DHAVE_AV_CONFIG_H -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
	-D_GNU_SOURCE -DCONFIG_DECODERS -DCONFIG_ENCODERS \
	-DCONFIG_MUXERS -DCONFIG_DEMUXERS $(THEORA_CPPFLAGS)

if HAVE_BUILTIN_VECTOR
SSE=-msse
endif

AM_CFLAGS = -std=gnu99 -w $(SSE)

if HAVE_LIBTHEORA
THEORA=oggtheora.c
endif

libavcodec_la_SOURCES = bitstream.c utils.c mem.c allcodecs.c 	mpegvideo.c \
	jrevdct.c jfdctfst.c jfdctint.c	mpegaudio.c ac3enc.c mjpeg.c resample.c \
	resample2.c dsputil.c 	motion_est.c imgconvert.c imgresample.c 	mpeg12.c \
	mpegaudiodec.c pcm.c simple_idct.c 	ratecontrol.c adpcm.c eval.c error_resilience.c \
	fft.c mdct.c raw.c golomb.c cabac.c	dpcm.c adx.c faandct.c parser.c g726.c \
	vp3dsp.c h264idct.c rangecoder.c pnm.c h263.c msmpeg4.c 	h263dec.c dvdsub.c \
	dvbsub.c dvbsubdec.c	opt.c 	vorbis.c dv.c h261.c h264.c flac.c 	svq1.c huffyuv.c \
	asv1.c ffv1.c snow.c lcl.c sonic.c vc9.c 	rv10.c wmadec.c indeo2.c indeo3.c \
	tscc.c ulti.c qdrw.c 	xl.c qpeg.c loco.c wnv1.c aasc.c fraps.c mace.c cyuv.c \
	vp3.c vcr1.c cljr.c 4xm.c roqvideo.c interplayvideo.c 	xan.c rpza.c cinepak.c \
	msrle.c msvideo1.c vqavideo.c 	smc.c flicvideo.c truemotion1.c ra144.c ra288.c \
	qtrle.c alac.c ws-snd1.c idcinvideo.c 8bps.c shorten.c vmdav.c $(THEORA) \
	truemotion2.c qdm2.c bmp.c cook.c dvdsubenc.c truespeech.c \
	zmbv.c avs.c lzo.c tta.c mmvideo.c cscd.c \
	nuv.c rtjpeg.c smacker.c kmvc.c

noinst_HEADERS = \
	ac3.h h261data.h mpegaudiodectab.h sp5x.h \
	ac3tab.h h263data.h mpegaudiotab.h svq1_cb.h \
	avcodec.h h264data.h mpegvideo.h svq1_vlc.h \
	bitstream.h imgconvert_template.h msmpeg4data.h truemotion1data.h \
	cabac.h indeo2data.h opt.h ulti_cb.h \
	dsputil.h indeo3data.h ra144.h vc9data.h \
	dvdata.h mpeg12data.h ra288.h vorbis.h \
	faandct.h mpeg4data.h rangecoder.h vp3data.h \
	golomb.h mpegaudio.h simple_idct.h wmadata.h cookdata.h \
	internal.h rtjpeg.h snow.h


