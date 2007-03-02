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
#include <qregexp.h>

QMediaFile::QMediaFile(QString file)
{
  probe(file);
}

QMediaFile::~QMediaFile()
{
}

bool QMediaFile::probe(QString file)
{
  Run run(QString("tcprobe -i %1").arg(file));

  if(run.result() == 0)
  {
    QRegExp rx;

    rx.setPattern("\\[(\\d+)x(\\d+)\\]");
    if(rx.search(run.output()) != -1)
    {
        m_width = rx.cap(1).toInt();
        m_height = rx.cap(2).toInt();
    }

    rx.setPattern("\\[tcprobe\\] ([\\S\\ ]*)");
    if(rx.search(run.output()) != -1)
        m_type = rx.cap(1);

    rx.setPattern("ratio: ([\\d:]+)");
    if(rx.search(run.output()) != -1)
        m_type = QDVD::VideoTrack::aspectRatio(rx.cap(1));

    rx.setPattern("rate: -f ([\\d\\.]+)");
    if(rx.search(run.output()) != -1)
        m_frameRate = rx.cap(1).toFloat();
    return true;
  }
  return false;
}


