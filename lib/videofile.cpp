//
// This file is based upon moviedecoder.cpp, from ffmpegthumbnailer. 
// The copyright notice for this follows...
//

//    Copyright (C) 2010 Dirk Vanden Boer <dirk.vdb@gmail.com>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "videofile.h"
#include <algorithm>
#include <QtCore/QFile>

#include <stdint.h>
#ifndef UINT64_C
#define UINT64_C(value)   value##LL
#endif

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/log.h>
#include <libswscale/swscale.h>
}

VideoFile::VideoFile()
         : videoStreamId(-1)
         , avFromatContext(0L)
         , avVideoCodecContext(0L)
         , avVideoCodec(0L)
         , avVideoStream(0L)
         , avFrame(0L)
         , frameBuffer(0L)
         , avPacket(0L)
         , numAudioStreams(0)
{
}

VideoFile::~VideoFile()
{
    close();
}

bool VideoFile::open(const QString& filename)
{
    static bool init=false;
    if(!init) {
        av_register_all();
        avcodec_init();
        avcodec_register_all();
        av_log_set_level(0);
        init=true;
    }

    close();

    if ( av_open_input_file(&avFromatContext, QFile::encodeName(filename).constData(), 0L, 0, 0L) != 0 ||
         av_find_stream_info(avFromatContext) < 0) {
        close();
    }
    else {
        if(initialize()) {
            avFrame = avcodec_alloc_frame();
            return decodeVideoFrame();
        }
    }
    
    return false;
}

void VideoFile::close()
{
    if (avVideoCodecContext) {
        avcodec_close(avVideoCodecContext);
        avVideoCodecContext = 0L;
    }

    if (avFromatContext) {
        av_close_input_file(avFromatContext);
        avFromatContext = 0L;
    }

    if (avPacket) {
        av_free_packet(avPacket);
        delete avPacket;
        avPacket = 0L;
    }

    if (avFrame) {
        av_free(avFrame);
        avFrame = 0L;
    }

    if (frameBuffer) {
        av_free(frameBuffer);
        frameBuffer = 0L;
    }

    videoStreamId = -1;
    numAudioStreams = 0;
}

QString VideoFile::getCodec()
{
    return avVideoCodec ? QString(avVideoCodec->name) : QString();
}

bool VideoFile::initialize()
{
    for (unsigned int i = 0; i < avFromatContext->nb_streams; ++i) {
        if (AVMEDIA_TYPE_VIDEO==avFromatContext->streams[i]->codec->codec_type) {
            avVideoStream = avFromatContext->streams[i];
            videoStreamId = i;
        }
        else if (AVMEDIA_TYPE_AUDIO==avFromatContext->streams[i]->codec->codec_type) {
            numAudioStreams++;
        }
    }

    if (-1==videoStreamId) {
        return false;
    }

    avVideoCodecContext = avFromatContext->streams[videoStreamId]->codec;
    avVideoCodec = avcodec_find_decoder(avVideoCodecContext->codec_id);

    if (!avVideoCodec) {
        // set to 0L, otherwise avcodec_close(avVideoCodecContext) crashes
        avVideoCodecContext = 0L;
        return false;
    }

    avVideoCodecContext->workaround_bugs = 1;

    return avcodec_open(avVideoCodecContext, avVideoCodec)>=0;
}

int VideoFile::getWidth()
{
    return avVideoCodecContext ? avVideoCodecContext->width : -1;
}

int VideoFile::getHeight()
{
    return avVideoCodecContext ? avVideoCodecContext->height : -1;
}

double VideoFile::getDuration()
{
    return avFromatContext ? (double)avFromatContext->duration / (double)AV_TIME_BASE : 0;
}

double VideoFile::getAspect()
{
    // This is pixel aspect ratio!!! Really want 4/3 16/9, etc!!!
    return avVideoStream || 0==avVideoCodecContext->sample_aspect_ratio.den ? (double)avVideoCodecContext->sample_aspect_ratio.num/(double)avVideoCodecContext->sample_aspect_ratio.den : 0.0;
}

double VideoFile::getFrameRate()
{
    return avVideoStream ||0==avVideoStream->r_frame_rate.den ? (double)avVideoStream->r_frame_rate.num/(double)avVideoStream->r_frame_rate.den : 0.0;
}

int VideoFile::getNumAudioStreams()
{
    return numAudioStreams;
}

bool VideoFile::seek(int64_t milliseconds)
{
    int64_t timestamp = int64_t((AV_TIME_BASE * (((double)milliseconds)/1000.0))+0.5);

    if (timestamp < 0) {
        timestamp = 0;
    }

    if (av_seek_frame(avFromatContext, -1, timestamp, 0) >= 0) {
        avcodec_flush_buffers(avFromatContext->streams[videoStreamId]->codec);
    }
    else {
        return false;
    }

    int  keyFrameAttempts = 0;
    bool gotFrame = false;
    
    do {
        int count = 0;
        gotFrame = 0;

        while (!gotFrame && count < 20) {
            getVideoPacket();
            gotFrame = decodeVideoPacket();
            ++count;
        }
        
        ++keyFrameAttempts;
    }
    while ((!gotFrame || !avFrame->key_frame) && keyFrameAttempts < 200);
    
    return gotFrame;
}

bool VideoFile::decodeVideoFrame()
{
    bool frameFinished = false;

    while(!frameFinished && getVideoPacket()) {
        frameFinished = decodeVideoPacket();
    }

    return frameFinished;
}

bool VideoFile::decodeVideoPacket()
{
    if (avPacket->stream_index != videoStreamId) {
        return false;
    }

    avcodec_get_frame_defaults(avFrame);
    
    int frameFinished;
    
    if (avcodec_decode_video2(avVideoCodecContext, avFrame, &frameFinished, avPacket) < 0) {
        return false;
    }

    return frameFinished > 0;
}

bool VideoFile::getVideoPacket()
{
    bool framesAvailable = true;
    bool frameDecoded    = false;
    int  attempts        = 0;

    if (avPacket) {
        av_free_packet(avPacket);
        delete avPacket;
    }

    avPacket = new AVPacket();

    while (framesAvailable && !frameDecoded && (attempts++ < 1000))  {
        framesAvailable = av_read_frame(avFromatContext, avPacket) >= 0;
        if (framesAvailable) {
            frameDecoded = avPacket->stream_index == videoStreamId;
            if (!frameDecoded) {
                av_free_packet(avPacket);
            }
        }
    }
    
    return frameDecoded;
}

QImage VideoFile::getFrame()
{
    if (avFrame->interlaced_frame) {
        avpicture_deinterlace((AVPicture*) avFrame, (AVPicture*) avFrame, avVideoCodecContext->pix_fmt,
                              avVideoCodecContext->width, avVideoCodecContext->height);
    }

    if(convertFrame()) {
        int    width=getWidth(), 
               height=getHeight();
        QImage img(width, height, QImage::Format_RGB888);
        for(int y=0 ; y<height; y++) {
            memcpy(img.scanLine(y), (void *)((avFrame->data[0])+(y*avFrame->linesize[0])), width*3);
        }
        return img;
    }
    
    return QImage();
}

bool VideoFile::convertFrame()
{
    int        width=getWidth(),
               height=getHeight();
    SwsContext *scaleContext = sws_getContext(avVideoCodecContext->width, avVideoCodecContext->height,
                                              avVideoCodecContext->pix_fmt, width, height,
                                              PIX_FMT_RGB24, SWS_BICUBIC, 0L, 0L, 0L);

    if (0L == scaleContext) {
        return false;
    }

    AVFrame *convertedFrame       = avcodec_alloc_frame();
    uint8_t *convertedFrameBuffer = reinterpret_cast<uint8_t*>(av_malloc(avpicture_get_size(PIX_FMT_RGB24, width, width)));

    avpicture_fill((AVPicture*) convertedFrame, convertedFrameBuffer, PIX_FMT_RGB24, width, height);
    sws_scale(scaleContext, avFrame->data, avFrame->linesize, 0, avVideoCodecContext->height,
              convertedFrame->data, convertedFrame->linesize);
    sws_freeContext(scaleContext);

    av_free(avFrame);
    av_free(frameBuffer);
    
    avFrame     = convertedFrame;
    frameBuffer = convertedFrameBuffer;
    return true;
}
