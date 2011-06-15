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
#include "config.h"

#ifdef HAVE_FFMPEG_SWSCALE
#include "videofile.h"
#else
#include "run.h"
#endif

QMap<QString, KMFMediaFile> KMFMediaFile::m_cache;

KMFMediaFile::KMFMediaFile(const QString &file)
{
    m_file = file;

    if (!m_file.isEmpty()) {
        probe();
    }
}

#ifdef HAVE_FFMPEG_SWSCALE
bool checkRatio(double orig, double ratio)
{
    // Checlk returned ratio nis with +/- 1% of desired ratio...
    return orig>(ratio*0.99) && orig<(ratio*1.01);
}

#endif

bool KMFMediaFile::probe()
{
#ifdef HAVE_FFMPEG_SWSCALE
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
    }
#else
    Run run(QString("kmf_info \"%1\"").arg(m_file));

    if (run.exitStatus() == 0) {
        QStringList lines = run.output().split('\n');
        QMap<QString, QString> info;

        for (QStringList::Iterator it = lines.begin(); it != lines.end(); ++it) {
            QStringList keyAndValue = it->split('=');

            if (keyAndValue.count() == 2) {
                info[keyAndValue[0]] = keyAndValue[1];
            }
        }

        m_aspectRatio =
            (QDVD::VideoTrack::AspectRatio)(info["ASPECT_RATIO"].toInt());
        m_frameRate = info["FRAME_RATE"].toFloat();
        m_audioStreams = info["AUDIO_STREAMS"].toInt();
        m_dvdCompatible = (info["DVD_COMPATIBLE"] == "1");
        m_duration = KMF::Time(info["DURATION"].toDouble());
        m_resolution = QSize(info["WIDTH"].toInt(), info["HEIGHT"].toInt());

        /*
         * kDebug() << "File: " << m_file;
         * kDebug() << "Output: " << run.output();
         * kDebug() << "Aspect: " << m_aspectRatio;
         * kDebug() << "Frame rate: " << m_frameRate;
         * kDebug() << "Audio: " << m_audioStreams;
         * kDebug() << "Compatible: " << m_dvdCompatible;
         * kDebug() << "Duration: " << m_duration;
         */
        return true;
    }
#endif
    return false;
}

bool KMFMediaFile::frame(QTime pos, QString output) const
{
#ifdef HAVE_FFMPEG_SWSCALE
    VideoFile video;
    if(video.open(m_file)) {
        video.seek(KMF::Time(pos).toMSec());
        QImage img(video.getFrame());
        
        if(!img.isNull()) {
            return img.save(output, output.endsWith(".pnm") ? "PPM" : 0L);
        }
    }
    return false;
#else
    Run run(QString("kmf_frame \"%1\" %2 \"%3\"").arg(m_file)
            .arg(KMF::Time(pos).toString()).arg(output));

    return (run.exitStatus() == 0);
#endif
}

const KMFMediaFile &KMFMediaFile::mediaFile(const QString &file)
{
    if (!m_cache.contains(file)) {
        m_cache[file] = KMFMediaFile(file);
    }

    return m_cache[file];
}
