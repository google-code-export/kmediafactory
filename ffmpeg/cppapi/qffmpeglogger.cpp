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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************
#include "qffmpeglogger.h"
#include "ffmpeg/avformat.h"
#include "ffmpeg/avcodec.h"
#include "dopr.h"
#include <kdebug.h>

#undef sprintf

QFFMpegLogger* QFFMpegLogger::m_logger = 0;

QFFMpegLogger::QFFMpegLogger()
{
  av_log_set_callback(QFFMpegLogger::ffmpeg_av_log_callback);
}

QFFMpegLogger* QFFMpegLogger::self()
{
  if(!m_logger)
    m_logger = new QFFMpegLogger;
  return m_logger;
}

void QFFMpegLogger::ffmpeg_av_log_callback(void *ptr, int level,
                                           const char *fmt, va_list vl)
{
  if(level > av_log_get_level())
    return;

  QString prefix;
  char buffer[1024];

  dopr(buffer, 1023, fmt, vl);
  if(ptr)
  {
    AVClass* avc= *(AVClass**)ptr;
    prefix.sprintf("[%s @ %p] ", avc->item_name((char*)ptr), avc);
  }
  emit self()->message(prefix + buffer);
}

#include "qffmpeglogger.moc"
