//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damst�
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
//**************************************************************************
#ifndef QMEDIAFILE_H
#define QMEDIAFILE_H

#include "qdvdinfo.h"

/**
	@author Petri Damsten <petri.damsten@iki.fi>
*/
class KDE_EXPORT KMFMediaFile
{
  public:
    KMFMediaFile() {};
    ~KMFMediaFile() {};
    const QTime& duration() const { return m_duration; };
    double frameRate() const { return m_frameRate; };
    uint audioStreams() const { return m_audioStreams; };
    uint subtitles() const { return m_subtitles; };
    QDVD::VideoTrack::AspectRatio aspectRatio() const { return m_aspectRatio; };
    bool dvdCompatible() const { return m_dvdCompatible; };
    bool frame(QTime pos, QString output) const;

    static void clearCache() { m_cache.clear(); };
    static const KMFMediaFile& mediaFile(const QString& file);

  protected:
    KMFMediaFile(const QString& file);
    bool probe();

  private:
    QString m_file;
    QTime m_duration;
    double m_frameRate;
    uint m_audioStreams;
    uint m_subtitles;
    uint m_width;
    uint m_height;
    QString m_type;
    QDVD::VideoTrack::AspectRatio m_aspectRatio;
    bool m_dvdCompatible;

    static QMap<QString, KMFMediaFile> m_cache;
};

#endif
