//**************************************************************************
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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************

#include "qmediafile.h"
#include "run.h"
#include "kmftime.h"
#include "kdebug.h"
#include <qregexp.h>
#include <qmap.h>

QMap<QString, KMFMediaFile> KMFMediaFile::m_cache;

KMFMediaFile::KMFMediaFile(const QString& file)
{
  m_file = file;
  if(!m_file.isEmpty())
    probe();
}

bool KMFMediaFile::probe()
{
  Run run(QString("info %1").arg(m_file));

  if(run.result() == 0)
  {
    QStringList lines = run.output().split("\n");
    QMap<QString, QString> info;

    for(QStringList::Iterator it = lines.begin(); it != lines.end(); ++it)
    {
      QStringList keyAndValue = it->split("=");
      if(keyAndValue.count() == 2)
        info[keyAndValue[0]] = keyAndValue[1];
    }

    m_aspectRatio =
        (QDVD::VideoTrack::AspectRatio)(info["ASPECT_RATIO"].toInt());
    m_frameRate = info["FRAME_RATE"].toFloat();
    m_audioStreams = info["AUDIO_STREAMS"].toInt();
    m_dvdCompatible = (info["DVD_COMPATIBLE"] == "1");
    m_duration = KMF::Time(info["DURATION"].toDouble());

    /*
    kDebug() << "Output: " << run.output() << endl;
    kDebug() << "Aspect: " << m_aspectRatio << endl;
    kDebug() << "Frame rate: " << m_frameRate << endl;
    kDebug() << "Audio: " << m_audioStreams << endl;
    kDebug() << "Compatible: " << m_dvdCompatible << endl;
    kDebug() << "Duration: " << m_duration << endl;
    */
    return true;
  }
  return false;
}

bool KMFMediaFile::frame(QTime pos, QString output) const
{
  Run run(QString("frame %1 %2 %3").arg(m_file).arg(KMF::Time(pos).toString())
      .arg(output));
  return (run.result() == 0);
}

const KMFMediaFile& KMFMediaFile::mediaFile(const QString& file)
{
  if(!m_cache.contains(file))
  {
    m_cache[file] = KMFMediaFile(file);
  }
  return m_cache[file];
}

