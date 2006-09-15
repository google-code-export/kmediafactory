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

#include "qmpegvideo.h"
#include <kstaticdeleter.h>
#include <kdebug.h>
#include <QFileInfo>
#include <xine.h>
//#include <xine/xineutils.h>

class XineLib;

static KStaticDeleter<XineLib> xineDeleter;

class XineLib
{
  public:
    XineLib()
    {
      m_xine = xine_new();
      xine_init(m_xine);

      if(!(m_vo_port = xine_open_video_driver(m_xine, NULL,
           XINE_VISUAL_TYPE_NONE, NULL)))
      {
        kDebug() << k_funcinfo << "xine_open_video_driver failed." << endl;
        return;
      }
      if(!(m_ao_port = xine_open_audio_driver(m_xine, NULL, NULL)))
      {
        kDebug() << k_funcinfo << "xine_open_audio_driver failed." << endl;
        return;
      }
      if(!(m_stream = xine_stream_new(m_xine, m_ao_port, m_vo_port)))
      {
        kDebug() << k_funcinfo << "xine_stream_new failed." << endl;
        return;
      }
    };

    ~XineLib()
    {
      xine_close(m_stream);
      xine_close_audio_driver(m_xine, m_ao_port);
      xine_close_video_driver(m_xine, m_vo_port);
      xine_exit(m_xine);
    };

    void setFile(const QString& file)
    {
      if(file == m_file)
        return;
      m_file = file;

      xine_close(m_stream);

      if(!(xine_open(m_stream, "/mnt/stuff/pov/juggler2/juggler.mpg")))
      {
        kDebug() << k_funcinfo << "xine_open failed." << endl;
        return;
      }

      if(!(xine_play(m_stream, 0, 0)))
      {
        kDebug() << k_funcinfo << "xine_play failed." << endl;
        return;
      }

      xine_set_param(m_stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
    }

    QMpegFile fileInfo(const QString& file)
    {
      QFileInfo fi(file);
      int pos_stream;  /* 0..65535     */
      int pos_time;    /* milliseconds */
      int length_time;
      QMpegFile result;

      setFile(file);

      result.width = xine_get_stream_info(m_stream,
                                          XINE_STREAM_INFO_VIDEO_WIDTH);
      result.height = xine_get_stream_info(m_stream,
                                           XINE_STREAM_INFO_VIDEO_HEIGHT);
      result.fps = 1000.0 /
          xine_get_stream_info(m_stream, XINE_STREAM_INFO_FRAME_DURATION);
      if(xine_get_stream_info(m_stream, XINE_STREAM_INFO_VIDEO_RATIO) > 14000)
        result.aspect = QDVD::VideoTrack::Aspect_4_3;
      else
        result.aspect = QDVD::VideoTrack::Aspect_16_9;

      xine_get_pos_length (m_stream, &pos_stream, &pos_time, &length_time);
      result.duration.addMSecs(length_time);
      result.size = fi.size();
      result.audioStreamCount = xine_get_stream_info(m_stream, XINE_STREAM_INFO_AUDIO_CHANNELS);
      result.filename = file;
      return result;
    };

    static XineLib* self()
    {
      if(!m_self)
        xineDeleter.setObject(m_self, new XineLib);
      return m_self;
    };

  private:
    xine_t* m_xine;
    xine_video_port_t* m_vo_port;
    xine_audio_port_t* m_ao_port;
    xine_stream_t* m_stream;

    QString m_file;
    static XineLib* m_self;
};

XineLib *XineLib::m_self = 0;

QMpegFile::QMpegFile(const QString& file)
{
  *this = XineLib::self()->fileInfo(file);
}

QMpegVideo::QMpegVideo()
{
}

QMpegVideo::~QMpegVideo()
{
}

bool QMpegVideo::addFile(const QString& file)
{
  append(QMpegFile(file));
  return true;
}

double QMpegVideo::frameRate() const
{
  if(count() > 0)
    return at(0).fps;
  return 0.0;
}

QTime QMpegVideo::duration(int index) const
{
  if(index != -1)
    return at(index).duration;

  QTime result;

  for(int i = 0; i < count(); ++i)
    result.addMSecs(at(i).duration.msecsTo(QTime()) * -1);
  return result;
}

QStringList QMpegVideo::files() const
{
  QStringList result;

  for(int i = 0; i < count(); ++i)
    result.append(at(i).filename);
  return result;
}

int QMpegVideo::audioStreamCount() const
{
  if(count() > 0)
    return at(0).audioStreamCount;
  return 0;
}

int QMpegVideo::aspectRatio() const
{
  if(count() > 0)
    return at(0).aspect;
  return QDVD::VideoTrack::Aspect_4_3;
}

bool QMpegVideo::isDVDCompatible()
{
#warning TODO
  return true;
}

uint64_t QMpegVideo::size()
{
  uint64_t result;

  for(int i = 0; i < count(); ++i)
    result += at(i).size;
  return result;
}

