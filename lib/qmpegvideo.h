//**************************************************************************
//   Copyright (C) 2004-2006 by Petri Damsten
//   petri.damsten@iki.fi
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
#ifndef QMPEGVIDEO_H
#define QMPEGVIDEO_H

#include <QTime>
#include <QStringList>
#include <QImage>
#include <QList>
#include "qdvdinfo.h"
#include <stdint.h>

class QMpegFile
{
  public:
    QMpegFile(const QString& file);
    QMpegFile() : width(0), height(0),
        aspect(QDVD::VideoTrack::Aspect_4_3), fps(0.0),
        size(0), audioStreamCount(0) {};

    int width;
    int height;
    QDVD::VideoTrack::AspectRatio aspect;
    double fps;
    QTime duration;
    QString filename;
    uint64_t size;
    int audioStreamCount;
    QImage frame(const QTime&) const;
};

class QMpegVideo : QList<QMpegFile>
{
  public:
    QMpegVideo();
    ~QMpegVideo();

    double frameRate() const;
    QTime duration(int index = -1) const;
    QStringList files() const;
    bool addFile(const QString& file);
    int audioStreamCount() const;
    int aspectRatio() const;
    bool isDVDCompatible() const;
    QImage frame(const QTime&) const;
    uint64_t size() const;
};

#endif
