//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damstén
//   petri.damsten@iki.fi
//
//   Heavily based on:
//   Libavformat API example: Output a media file in any supported
//   libavformat format. The default codecs are used.
//
//   Copyright (c) 2003 Fabrice Bellard//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the
//   Free Software Foundation, Inc.,
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************

#ifndef QFFMPEGENCODER_H
#define QFFMPEGENCODER_H

#include "libavformat/avformat.h"
#include <qstring.h>
#include <qdatetime.h>
#include <qsize.h>

// At the moment this is just a dummy encoder to create silent audio files.
// Hopefully it will become real encoder at some point :-)

enum { QF_DVD = 1, QF_PAL = 2, QF_NTSC = 4, QF_AC3 = 8, QF_MP2 = 16,
       QF_4_3 = 32, QF_16_9 = 64 };

class QFFmpegEncoder
{
  public:
    QFFmpegEncoder() : samples(0), audio_outbuf(0), picture(0),
        tmp_picture(0), video_outbuf(0)
        { reset(); };
    virtual ~QFFmpegEncoder() {};

    bool write(QString file);
    void setResolution(QSize res) { m_resolution = res; };
    void setFrameRate(int rate, int base)
        { m_frameRate = rate; m_frameRateBase = base; };
    void setOutput(int type);
    void reset();

  protected:
    AVStream* add_audio_stream(AVFormatContext *oc);
    bool open_audio(AVFormatContext *oc, AVStream *st);
    void get_audio_frame(int16_t *samples, int frame_size, int nb_channels);
    bool write_audio_frame(AVFormatContext *oc, AVStream *st);
    void close_audio(AVFormatContext *oc, AVStream *st);
    AVStream* add_video_stream(AVFormatContext *oc);
    AVFrame* alloc_picture(int pix_fmt, int width, int height);
    bool open_video(AVFormatContext *oc, AVStream *st);
    void fill_yuv_image(AVFrame *pict, int frame_index, int width, int height);
    bool write_video_frame(AVFormatContext *oc, AVStream *st);
    void close_video(AVFormatContext *oc, AVStream *st);

  private:
    int16_t* samples;
    uint8_t* audio_outbuf;
    int audio_outbuf_size;
    int audio_input_frame_size;
    AVFrame* picture;
    AVFrame* tmp_picture;
    uint8_t* video_outbuf;
    int frame_count;
    int video_outbuf_size;

    int m_videoCodecId;
    QString m_oFormat;
    int m_videoBitRate;
    int m_videoRcMaxRate;
    int m_videoRcMinRate;
    int m_videoRcBufferSize;
    int m_gopSize;
    int m_muxPacketSize;
    int m_muxRate;
    int m_frameRate;
    int m_frameRateBase;
    QSize m_resolution;
    double m_frameAspectRatio;

    int m_audioCodecId;
    int m_audioSampleRate;
    int m_audioBitRate;

    double m_duration;
};

#endif
