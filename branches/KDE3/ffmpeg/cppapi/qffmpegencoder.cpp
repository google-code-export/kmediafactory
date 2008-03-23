/*
 * Libavformat API example: Output a media file in any supported
 * libavformat format. The default codecs are used.
 *
 * Copyright (c) 2003 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <kdebug.h>
#include <qstringlist.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "qffmpegencoder.h"

#define STREAM_PIX_FMT PIX_FMT_YUV420P /* default pix_fmt */

void QFFmpegEncoder::reset()
{
  samples = 0;
  audio_outbuf = 0;
  audio_outbuf_size = 0;
  audio_input_frame_size = 0;
  picture = 0;
  tmp_picture = 0;
  video_outbuf = 0;
  frame_count = 0;
  video_outbuf_size = 0;

  m_videoCodecId = CODEC_ID_NONE;
  m_oFormat = "";
  m_videoBitRate = 6000000;
  m_videoRcMaxRate = 9000000;
  m_videoRcMinRate = 0;
  m_videoRcBufferSize = 224*1024*8;
  m_muxPacketSize= 2048;
  m_muxRate = 10080000;
  m_gopSize = 18;
  m_frameRate = 25;
  m_frameRateBase = 1;
  m_resolution = QSize(720, 576);
  m_audioCodecId = CODEC_ID_NONE;
  m_audioSampleRate = 48000;
  m_audioBitRate = 128000;
  m_duration = 1.0;
  m_frameAspectRatio = 4.0 / 3.0;
}

void QFFmpegEncoder::setOutput(int type)
{
  if(type & QF_DVD)
  {
    m_oFormat = "dvd";
    m_videoCodecId = CODEC_ID_MPEG2VIDEO;
  }

  if(type & QF_NTSC)
  {
    m_resolution = QSize(720, 480);
    m_frameRate = 30000;
    m_frameRateBase = 1001;
    m_gopSize = 15;
  }
  else // pal
  {
    m_resolution = QSize(720, 576);
    m_frameRate = 25;
    m_frameRateBase = 1;
    m_gopSize = 18;
  }

  m_videoBitRate = 6000000;
  m_videoRcMaxRate = 9000000;
  m_videoRcMinRate = 0;
  m_videoRcBufferSize = 224*1024*8;
  m_muxPacketSize= 2048;
  m_muxRate = 10080000;

  if(type & QF_MP2)
  {
    m_audioBitRate = 128000;
    m_audioCodecId = CODEC_ID_MP2;
  }
  else // ac3
  {
    m_audioBitRate = 448000;
    m_audioCodecId = CODEC_ID_AC3;
  }

  m_audioSampleRate = 48000;

  if(type & QF_4_3)
    m_frameAspectRatio = 4.0 / 3.0;
  else
    m_frameAspectRatio = 16.0 / 9.0;
}

/**************************************************************/
/* audio output */

/*
 * add an audio output stream
 */
AVStream* QFFmpegEncoder::add_audio_stream(AVFormatContext *oc)
{
  AVCodecContext *c;
  AVStream *st;

  st = av_new_stream(oc, 1);
  if (!st)
  {
    kdDebug() << "Could not alloc stream" << endl;
    return false;
  }

  c = st->codec;
  c->codec_id = (CodecID)m_audioCodecId;
  c->codec_type = CODEC_TYPE_AUDIO;

  /* put sample parameters */
  c->bit_rate = m_audioBitRate;
  c->sample_rate = m_audioSampleRate;
  c->channels = 2;
  return st;
}

#define MAX_AUDIO_PACKET_SIZE (128 * 1024)

bool QFFmpegEncoder::open_audio(AVFormatContext*, AVStream *st)
{
  AVCodecContext *c;
  AVCodec *codec;

  c = st->codec;

  /* find the audio encoder */
  codec = avcodec_find_encoder(c->codec_id);
  if (!codec)
  {
    kdDebug() << "codec not found" << endl;
    return false;
  }

  /* open it */
  if (avcodec_open(c, codec) < 0)
  {
    kdDebug() << "could not open codec" << endl;
    return false;
  }

  audio_outbuf_size = 4 * MAX_AUDIO_PACKET_SIZE;
  audio_outbuf = (uint8_t*)av_malloc(audio_outbuf_size);

  /* ugly hack for PCM codecs (will be removed ASAP with new PCM
     support to compute the input frame size in samples */
  if (c->frame_size <= 1)
  {
    audio_input_frame_size = audio_outbuf_size / c->channels;
    switch(st->codec->codec_id)
    {
        case CODEC_ID_PCM_S16LE:
        case CODEC_ID_PCM_S16BE:
        case CODEC_ID_PCM_U16LE:
        case CODEC_ID_PCM_U16BE:
          audio_input_frame_size >>= 1;
          break;
        default:
          break;
    }
  }
  else
  {
    audio_input_frame_size = c->frame_size;
  }
  samples = (int16_t*)av_malloc(2 * MAX_AUDIO_PACKET_SIZE);

  memset(audio_outbuf, 0, 4 * MAX_AUDIO_PACKET_SIZE);
  memset(samples, 0, 2 * MAX_AUDIO_PACKET_SIZE);
  return true;
}

/* prepare a 16 bit dummy audio frame of 'frame_size' samples and
   'nb_channels' channels */
void QFFmpegEncoder::get_audio_frame(int16_t *, int, int)
{
  /*
  int j, i;
  int16_t *q;

  q = samples;
  for(j=0;j<frame_size;j++)
  {
    for(i = 0; i < nb_channels; i++)
      *q++ = 0;
  }
  */
}

bool QFFmpegEncoder::write_audio_frame(AVFormatContext *oc, AVStream *st)
{
  AVCodecContext *c;
  AVPacket pkt;
  av_init_packet(&pkt);

  c = st->codec;

  get_audio_frame(samples, audio_input_frame_size, c->channels);

  pkt.size= avcodec_encode_audio(c, audio_outbuf, audio_outbuf_size, samples);

  pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
  pkt.flags |= PKT_FLAG_KEY;
  pkt.stream_index= st->index;
  pkt.data= audio_outbuf;

  /* write the compressed frame in the media file */
  if (av_interleaved_write_frame(oc, &pkt) != 0)
  {
    kdDebug() << "Error while writing audio frame" << endl;
    return false;
  }
  return true;
}

void QFFmpegEncoder::close_audio(AVFormatContext *, AVStream *st)
{
  avcodec_close(st->codec);

  av_free(samples);
  av_free(audio_outbuf);
}

/**************************************************************/
/* video output */

/* add a video output stream */
AVStream* QFFmpegEncoder::add_video_stream(AVFormatContext *oc)
{
  AVCodecContext *c;
  AVStream *st;

  oc->packet_size = m_muxPacketSize;
  oc->mux_rate = m_muxRate;

  st = av_new_stream(oc, 0);
  if (!st)
  {
    kdDebug() << "Could not alloc stream" << endl;
    return false;
  }

  c = st->codec;
  c->codec_id = (CodecID)m_videoCodecId;
  c->codec_type = CODEC_TYPE_VIDEO;

  /* put sample parameters */
  c->bit_rate = m_videoBitRate;
  /* resolution must be a multiple of two */
  c->width = m_resolution.width();
  c->height = m_resolution.height();
  /* frames per second */
  c->time_base.den = m_frameRate;
  c->time_base.num = m_frameRateBase;
  c->gop_size = m_gopSize;
  c->sample_aspect_ratio =
      av_d2q(m_frameAspectRatio * m_resolution.height() / m_resolution.width(),
             255);
  c->rc_max_rate = m_videoRcMaxRate;
  c->rc_min_rate = m_videoRcMinRate;
  c->rc_buffer_size = m_videoRcBufferSize;

  c->pix_fmt = STREAM_PIX_FMT;
  if (c->codec_id == CODEC_ID_MPEG2VIDEO)
  {
    /* just for testing, we also add B frames */
    c->max_b_frames = 2;
  }
  if (c->codec_id == CODEC_ID_MPEG1VIDEO)
  {
    /* needed to avoid using macroblocks in which some coeffs overflow
       this doesnt happen with normal video, it just happens here as the
       motion of the chroma plane doesnt match the luma plane */
    c->mb_decision=2;
  }
  // some formats want stream headers to be seperate
  if(!strcmp(oc->oformat->name, "mp4") || !strcmp(oc->oformat->name, "mov") ||
     !strcmp(oc->oformat->name, "3gp"))
    c->flags |= CODEC_FLAG_GLOBAL_HEADER;

  return st;
}

AVFrame* QFFmpegEncoder::alloc_picture(int pix_fmt, int width, int height)
{
  AVFrame *picture;
  uint8_t *picture_buf;
  int size;

  picture = avcodec_alloc_frame();
  if (!picture)
    return NULL;
  size = avpicture_get_size(pix_fmt, width, height);
  picture_buf = (uint8_t*)av_malloc(size);
  if (!picture_buf)
  {
    av_free(picture);
    return NULL;
  }
  avpicture_fill((AVPicture *)picture, picture_buf,
                 pix_fmt, width, height);
  return picture;
}

bool QFFmpegEncoder::open_video(AVFormatContext *oc, AVStream *st)
{
  AVCodec *codec;
  AVCodecContext *c;

  c = st->codec;

  /* find the video encoder */
  codec = avcodec_find_encoder(c->codec_id);
  if (!codec)
  {
    kdDebug() << "codec not found" << endl;
    return false;
  }

  /* open the codec */
  if (avcodec_open(c, codec) < 0)
  {
    kdDebug() << "could not open codec" << endl;
    return false;
  }

  video_outbuf = NULL;
  if (!(oc->oformat->flags & AVFMT_RAWPICTURE))
  {
    /* allocate output buffer */
    /* XXX: API change will be done */
    int size= c->width * c->height;
    video_outbuf_size = 1024 * 256;
    video_outbuf_size = FFMAX(video_outbuf_size, 4 * size);
    video_outbuf = (uint8_t*)av_malloc(video_outbuf_size);
  }

  /* allocate the encoded raw picture */
  picture = alloc_picture(c->pix_fmt, c->width, c->height);
  if (!picture)
  {
    kdDebug() << "Could not allocate picture" << endl;
    return false;
  }

  /* if the output format is not YUV420P, then a temporary YUV420P
     picture is needed too. It is then converted to the required
     output format */
  tmp_picture = NULL;
  if (c->pix_fmt != PIX_FMT_YUV420P)
  {
    tmp_picture = alloc_picture(PIX_FMT_YUV420P, c->width, c->height);
    if (!tmp_picture)
    {
      kdDebug() << "Could not allocate temporary picture" << endl;
      return false;
    }
  }
  return true;
}

/* prepare a dummy image */
void QFFmpegEncoder::fill_yuv_image(AVFrame *, int, int, int)
{
  /*
  int x, y, i;

  i = frame_index;

  // Y
  for(y=0;y<height;y++)
  {
    for(x=0;x<width;x++)
    {
      pict->data[0][y * pict->linesize[0] + x] = 16; //x + y + i * 3;
    }
  }

  // Cb and Cr
  for(y=0;y<height/2;y++)
  {
    for(x=0;x<width/2;x++)
    {
      pict->data[1][y * pict->linesize[1] + x] = 128; //128 + y + i * 2;
      pict->data[2][y * pict->linesize[2] + x] = 128; //64 + x + i * 5;
    }
  }
  */
}

bool QFFmpegEncoder::write_video_frame(AVFormatContext *oc, AVStream *st)
{
  int out_size, ret;
  AVCodecContext *c;

  c = st->codec;

  if (frame_count >= ((int)(m_duration * m_frameRate / m_frameRateBase)))
  {
    /* no more frame to compress. The codec has a latency of a few
       frames if using B frames, so we get the last frames by
       passing the same picture again */
  }
  else
  {
    if (c->pix_fmt != PIX_FMT_YUV420P)
    {
      /* as we only generate a YUV420P picture, we must convert it
         to the codec pixel format if needed */
      fill_yuv_image(tmp_picture, frame_count, c->width, c->height);
      img_convert((AVPicture *)picture, c->pix_fmt,
                  (AVPicture *)tmp_picture, PIX_FMT_YUV420P,
                  c->width, c->height);
    }
    else
    {
      fill_yuv_image(picture, frame_count, c->width, c->height);
    }
  }


  if (oc->oformat->flags & AVFMT_RAWPICTURE)
  {
    kdDebug() << "Raw video" << endl;
    /* raw video case. The API will change slightly in the near
       futur for that */
    AVPacket pkt;
    av_init_packet(&pkt);

    pkt.flags |= PKT_FLAG_KEY;
    pkt.stream_index= st->index;
    pkt.data= (uint8_t *)picture;
    pkt.size= sizeof(AVPicture);

    ret = av_write_frame(oc, &pkt);
  }
  else
  {
    /* encode the image */
    out_size = avcodec_encode_video(c, video_outbuf, video_outbuf_size,
                                    picture);
    /* if zero size, it means the image was buffered */
    if (out_size != 0)
    {
      AVPacket pkt;
      av_init_packet(&pkt);

      pkt.pts= av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
      if(c->coded_frame->key_frame)
        pkt.flags |= PKT_FLAG_KEY;
      pkt.stream_index= st->index;
      pkt.data= video_outbuf;
      pkt.size= out_size;

      /* write the compressed frame in the media file */
      ret = av_interleaved_write_frame(oc, &pkt);
    }
    else
    {
      ret = 0;
    }
  }
  if (ret != 0)
  {
    kdDebug() << "Error while writing video frame" << endl;
    return false;
  }
  frame_count++;
  return true;
}

void QFFmpegEncoder::close_video(AVFormatContext*, AVStream *st)
{
  avcodec_close(st->codec);
  av_free(picture->data[0]);
  av_free(picture);
  if (tmp_picture)
  {
    av_free(tmp_picture->data[0]);
    av_free(tmp_picture);
  }
  av_free(video_outbuf);
}

/**************************************************************/
/* media file output */

bool QFFmpegEncoder::write(QString file)
{
  AVOutputFormat *fmt;
  AVFormatContext *oc;
  AVStream *audio_st, *video_st;
  double audio_pts, video_pts;
  int i;

  /* initialize libavcodec, and register all codecs and formats */
  av_register_all();

  /* auto detect the output format from the name. default is
     mpeg. */
  if(m_oFormat.isEmpty())
    fmt = guess_format(NULL, (const char*)file.local8Bit(), NULL);
  else
    fmt = guess_format((const char*)m_oFormat.local8Bit(), NULL, NULL);

  if (!fmt)
  {
    kdDebug() << "Could not deduce output format from file"
        " extension: using MPEG." << endl;
    fmt = guess_format("mpeg", NULL, NULL);
  }
  if (!fmt)
  {
    kdDebug() << "Could not find suitable output format" << endl;
    return false;
  }

  /* allocate the output media context */
  oc = av_alloc_format_context();
  if (!oc)
  {
    kdDebug() << "Memory error" << endl;
    return false;
  }
  oc->oformat = fmt;
  snprintf(oc->filename, sizeof(oc->filename), "%s", oc->filename);

  /* add the audio and video streams using the default format codecs
     and initialize the codecs */
  video_st = NULL;
  audio_st = NULL;
  if (fmt->video_codec != CODEC_ID_NONE)
  {
    if(m_videoCodecId == CODEC_ID_NONE)
      m_videoCodecId = fmt->video_codec;
    video_st = add_video_stream(oc);
  }
  if (fmt->audio_codec != CODEC_ID_NONE)
  {
    if(m_audioCodecId == CODEC_ID_NONE)
      m_audioCodecId = fmt->audio_codec;
    audio_st = add_audio_stream(oc);
  }

  /* set the output parameters (must be done even if no
     parameters). */
  if (av_set_parameters(oc, NULL) < 0)
  {
    kdDebug() << "Invalid output format parameters" << endl;
    return false;
  }

  dump_format(oc, 0, oc->filename, 1);

  /* now that all the parameters are set, we can open the audio and
     video codecs and allocate the necessary encode buffers */
  if (video_st)
    if(!open_video(oc, video_st))
      return false;
  if (audio_st)
    if(!open_audio(oc, audio_st))
      return false;

  /* open the output file, if needed */
  if (!(fmt->flags & AVFMT_NOFILE))
  {
    if (url_fopen(&oc->pb, (const char*)file.local8Bit(), URL_WRONLY) < 0)
    {
      kdDebug() << "Could not open " << file << endl;
      return false;
    }
  }

  /* write the stream header, if any */
  av_write_header(oc);

  for(;;)
  {
    /* compute current audio and video time */
    if (audio_st)
      audio_pts = (double)audio_st->pts.val *
          audio_st->time_base.num / audio_st->time_base.den;
    else
      audio_pts = 0.0;

    if (video_st)
      video_pts = (double)video_st->pts.val *
          video_st->time_base.num / video_st->time_base.den;
    else
      video_pts = 0.0;

    if ((!audio_st || audio_pts >= m_duration) &&
        (!video_st || video_pts >= m_duration))
      break;

    /* write interleaved audio and video frames */
    if (!video_st || (video_st && audio_st && audio_pts < video_pts))
    {
      if(!write_audio_frame(oc, audio_st))
        return false;
    }
    else
    {
      if(!write_video_frame(oc, video_st))
        return false;
    }
  }

  /* close each codec */
  if (video_st)
    close_video(oc, video_st);
  if (audio_st)
    close_audio(oc, audio_st);

  /* write the trailer, if any */
  av_write_trailer(oc);

  /* free the streams */
  for(i = 0; i < oc->nb_streams; i++)
  {
    av_freep(&oc->streams[i]);
  }

  if (!(fmt->flags & AVFMT_NOFILE))
  {
    /* close the output file */
    url_fclose(&oc->pb);
  }

  /* free the stream */
  av_free(oc);

  return 0;
}
