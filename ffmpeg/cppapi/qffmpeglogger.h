//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damstén
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
#ifndef QFFMPEGLOGGER_H
#define QFFMPEGLOGGER_H

#include <qobject.h>

#undef sprintf

class QFFMpegLogger : public QObject
{
  Q_OBJECT
  public:
    virtual ~QFFMpegLogger() {};

    static QFFMpegLogger* self();

  signals:
    void message(const QString&);

  private:
    QFFMpegLogger();
    static void ffmpeg_av_log_callback(void *ptr, int level,
        const char *fmt, va_list vl);

    static QFFMpegLogger* m_logger;
};

#endif
