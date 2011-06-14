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

#ifndef VIDEOFILE_H
#define VIDEOFILE_H

#include <inttypes.h>
#include <QtGui/QImage>
#include <QtCore/QString>

struct AVFormatContext;
struct AVCodecContext;
struct AVCodec;
struct AVStream;
struct AVFrame;
struct AVPacket;

class VideoFile
{
    public:

    VideoFile();
    ~VideoFile();

    bool    open(const QString& filename);
    void    close();

    bool    seek(int64_t milliseconds);
    bool    decodeVideoFrame();
    QImage  getFrame();

    QString getCodec();
    int     getWidth();
    int     getHeight();
    double  getDuration();
    double  getAspect();
    double  getFrameRate();
    int     getNumAudioStreams();
    
    bool    isOpen() { return 0L!=avFrame; }

    private:

    bool initialize();    
    bool decodeVideoPacket();
    bool getVideoPacket();
    bool convertFrame();

    private:

    int             videoStreamId;
    AVFormatContext *avFromatContext;
    AVCodecContext  *avVideoCodecContext;
    AVCodec         *avVideoCodec;
    AVStream        *avVideoStream;
    AVFrame         *avFrame;
    uint8_t         *frameBuffer;
    AVPacket        *avPacket;
    int             numAudioStreams;
};

#endif

