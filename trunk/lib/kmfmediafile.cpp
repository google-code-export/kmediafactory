// **************************************************************************
//   Copyright (C) 2006 by Petri Damsten
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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
// **************************************************************************

#include "kmfmediafile.h"

#include <QtCore/QMap>
#include <QtCore/QRegExp>

#include <KDebug>

#include "kmftime.h"
#include "videofile.h"

QMap<QString, KMFMediaFile> KMFMediaFile::m_cache;

KMFMediaFile::KMFMediaFile(const QString &file)
{
    m_file = file;
    m_aspectRatio = QDVD::VideoTrack::Aspect_Unknown;
    m_frameRate = 0;
    m_audioStreams = 0;
    m_dvdCompatible = false;
    m_duration = KMF::Time(0);
    m_resolution = QSize(0, 0);

    if (!m_file.isEmpty()) {
        probe();
    }
}

bool checkRatio(double orig, double ratio)
{
    // Checlk returned ratio nis with +/- 1% of desired ratio...
    return orig>(ratio*0.99) && orig<(ratio*1.01);
}

bool KMFMediaFile::probe()
{
    VideoFile video;
    if(video.open(m_file)) {
        double ratio=video.getAspect();
        if (checkRatio(ratio, 16.0/9.0)) {
            m_aspectRatio = QDVD::VideoTrack::Aspect_16_9;
        } else if (checkRatio(ratio, 4.0/3.0)) {
            m_aspectRatio = QDVD::VideoTrack::Aspect_4_3;
        } else {
            m_aspectRatio = QDVD::VideoTrack::Aspect_Unknown;
        }
        m_frameRate = video.getFrameRate();
        m_audioStreams = video.getNumAudioStreams();
        m_dvdCompatible = video.getCodec()=="mpeg2video";
        m_duration = KMF::Time(video.getDuration());
        m_resolution = QSize(video.getWidth(), video.getHeight());
        return true;
    }

    return false;
}

QImage KMFMediaFile::frame(QTime pos) const
{
    VideoFile video;
    if(video.open(m_file)) {
        video.seek(KMF::Time(pos).toMSec());
        return (video.getFrame());
    }
    return QImage();
}

const KMFMediaFile &KMFMediaFile::mediaFile(const QString &file)
{
    if (!m_cache.contains(file)) {
        m_cache[file] = KMFMediaFile(file);
    }

    return m_cache[file];
}
