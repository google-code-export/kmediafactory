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
#include <kdebug.h>
#include "qffmpegconverter.h"

extern "C"
{
#include "ffmpeg.h"
}

QFFMpegConverter* QFFMpegConverter::ffmpeg_instance = 0;

QFFMpegConverter::QFFMpegConverter(int frames)
  : m_frames(frames), m_stopped(0)
{
  av_reset();
  ffmpeg_instance = this;
  set_progress(&QFFMpegConverter::staticProgress);
}

QFFMpegConverter::~QFFMpegConverter()
{
  if(m_stopped == 0)
    emit progress(m_frames);
  ffmpeg_instance = 0;
  av_reset();
}

void QFFMpegConverter::transcodeProgress(int frame)
{
  if(m_stopped)
    emit progress(m_frames);
  else
    emit progress(frame);
}

void QFFMpegConverter::stop(int stp)
{
  kdDebug() << "Stopping convert." << endl;
  m_stopped = stp;
  av_stop(stp);
}

int QFFMpegConverter::transcode()
{
  m_stopped = 0;
  int result = av_transcode();
  if(m_stopped)
    return 1;
  return result;
}

void QFFMpegConverter::set(const char* option, const char* value)
{
  av_set(option, value);
}

void QFFMpegConverter::set_output(const char *filename)
{
  parse_arg_file(filename);
}

void QFFMpegConverter::staticProgress(int frame)
{
  //kdDebug() << k_funcinfo << frame << endl;
  ffmpeg_instance->transcodeProgress(frame);
}

#include "qffmpegconverter.moc"
