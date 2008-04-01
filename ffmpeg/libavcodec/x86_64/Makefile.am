if HAVE_BUILTIN_VECTOR
SSE=-msse
endif

# Disable optimization and debugging. Won't compile with those in i386
AM_CFLAGS = -std=gnu99 -w -fomit-frame-pointer $(SSE) -Os -g1

# From xine Makefile.am to ffmpeg
# filter out -funroll-loops because it causes bad behavior of libavcodec
# g3 and O2 cause multiple build failures so override those here
CFLAGS := `echo @CFLAGS@ | sed -e '\
	s/-funroll-loops//g; \
	s/-g[0-9]/-g1/g; \
	s/-O[0-9]/-Os/g'`

AM_CPPFLAGS = \
	-DHAVE_AV_CONFIG_H -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
	-D_GNU_SOURCE -DCONFIG_DECODERS -DCONFIG_ENCODERS \
	-DCONFIG_MUXERS -DCONFIG_DEMUXERS \
	-I$(top_srcdir)/ffmpeg/libavcodec/x86_64 \
	-I$(top_srcdir)/ffmpeg/libavcodec \
	-I$(top_srcdir)/ffmpeg/libavutil \
	$(all_includes)

noinst_LTLIBRARIES = libmmx.la
libmmx_la_SOURCES = \
	cputest.c \
	fdct_mmx.c \
	dsputil_mmx.c \
	mpegvideo_mmx.c \
	idct_mmx.c \
	motion_est_mmx.c \
	simple_idct_mmx.c \
	fft_sse.c \
	vp3dsp_mmx.c \
	vp3dsp_sse2.c \
	idct_mmx_xvid.c \
	fft_3dn2.c \
	fft_3dn.c \
	snowdsp_mmx.c