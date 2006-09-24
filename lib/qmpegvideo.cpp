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

#include "qmpegvideo.h"
#include "kmftime.h"
#include <kstaticdeleter.h>
#include <kdebug.h>
#include <QFileInfo>
#include <xine.h>

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

      xine_set_param(m_stream, XINE_PARAM_AUDIO_MUTE, 1);
      xine_set_param(m_stream, XINE_PARAM_AUDIO_AMP_MUTE, 1);
      xine_set_param(m_stream, XINE_PARAM_IGNORE_AUDIO, 1);

      if(!(xine_open(m_stream, file.toLocal8Bit())))
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
      result.duration = QTime().addMSecs(length_time);
      result.size = fi.size();
      result.audioStreamCount = xine_get_stream_info(m_stream,
          XINE_STREAM_INFO_MAX_AUDIO_CHANNEL);
      if(result.audioStreamCount < 1)
        result.audioStreamCount = xine_get_stream_info(m_stream,
            XINE_STREAM_INFO_HAS_AUDIO);
      result.filename = file;

      kDebug() << k_funcinfo << "width: " << result.width << endl;
      kDebug() << k_funcinfo << "height: " << result.height << endl;
      kDebug() << k_funcinfo << "fps: " << result.fps << endl;
      kDebug() << k_funcinfo << "aspect: " << result.aspect << endl;
      kDebug() << k_funcinfo << "duration: "
          << QTime().msecsTo(result.duration) << endl;
      kDebug() << k_funcinfo << "size: " << result.size << endl;
      kDebug() << k_funcinfo << "audio streams: "
          << result.audioStreamCount << endl;
      kDebug() << k_funcinfo << "file: " << result.filename << endl;
      return result;
    };

    static XineLib* self()
    {
      if(!m_self)
        xineDeleter.setObject(m_self, new XineLib);
      return m_self;
    };

    //************************************************************
    // Helpers to convert yuy and yv12 frames to rgb             *
    // code from gxine modified for 32bit output                 *
    // Copyright (C) 2000-2003 the xine project                  *
    //************************************************************
    void yuy2Toyv12(uint8_t *y, uint8_t *u, uint8_t *v,
                    uint8_t *input, int width, int height) const
    {
      int i, j, w2;
      w2 = width / 2;

      for (i = 0; i < height; i += 2)
      {
        for (j = 0; j < w2; j++)
        {
          // packed YUV 422 is: Y[i] U[i] Y[i+1] V[i]
          *(y++) = *(input++);
          *(u++) = *(input++);
          *(y++) = *(input++);
          *(v++) = *(input++);
        }
        // down sampling

        for (j = 0; j < w2; j++)
        {
          // skip every second line for U and V
          *(y++) = *(input++);
          input++;
          *(y++) = *(input++);
          input++;
        }
      }
    }

    uchar* yv12ToRgb(uint8_t *src_y, uint8_t *src_u, uint8_t *src_v,
                     int width, int height) const
    {
      // Create rgb data from yv12

      #define clip_8_bit(val)           \
      {                                 \
        if(val < 0)                     \
          val = 0;                      \
        else                            \
          if (val > 255) val = 255;     \
      }

      int     i, j;
      int     y, u, v;
      int     r, g, b;
      int     sub_i_uv;
      int     sub_j_uv;
      int     uv_width, uv_height;
      uchar *rgb;

      uv_width  = width / 2;
      uv_height = height / 2;

      rgb = new uchar[(width * height * 4)]; //qt needs a 32bit align
      if (!rgb)
      {
        kDebug() << k_funcinfo <<
            "Not enough memory to make screenshot!" << endl;
        return NULL;
      }

      for (i = 0; i < height; ++i)
      {
        // calculate u & v rows
        sub_i_uv = ((i * uv_height) / height);

        for (j = 0; j < width; ++j)
        {
          // calculate u & v columns
          sub_j_uv = ((j * uv_width) / width);

          //***************************************************
          //
          //  Colour conversion from
          //  http://www.inforamp.net/~poynton/notes/colour_and_gamma/
          //      ColorFAQ.html#RTFToC30
          //
          // Thanks to Billy Biggs <vektor@dumbterm.net>
          // for the pointer and the following conversion.
          //
          //   R' = [ 1.1644         0    1.5960 ]   ([ Y' ]   [  16 ])
          //   G' = [ 1.1644   -0.3918   -0.8130 ] * ([ Cb ] - [ 128 ])
          //   B' = [ 1.1644    2.0172         0 ]   ([ Cr ]   [ 128 ])
          //
          //  Where in xine the above values are represented as
          //
          //   Y' == image->y
          //   Cb == image->u
          //   Cr == image->v
          //
          //***************************************************

          y = src_y[(i * width) + j] - 16;
          u = src_u[(sub_i_uv * uv_width) + sub_j_uv] - 128;
          v = src_v[(sub_i_uv * uv_width) + sub_j_uv] - 128;

          r = (int)((1.1644 * (double)y) + (1.5960 * (double)v));
          g = (int)((1.1644 * (double)y) - (0.3918 * (double)u) - (0.8130 * (double)v));
          b = (int)((1.1644 * (double)y) + (2.0172 * (double)u));

          clip_8_bit (r);
          clip_8_bit (g);
          clip_8_bit (b);

          rgb[(i * width + j) * 4 + 0] = b;
          rgb[(i * width + j) * 4 + 1] = g;
          rgb[(i * width + j) * 4 + 2] = r;
          rgb[(i * width + j) * 4 + 3] = 0;
        }
      }
      return rgb;
    }

    // From KXineWidget
    uchar* getScreenshot(int& width, int& height) const
    {
      uint8_t   *yuv = NULL, *y = NULL, *u = NULL, *v =NULL;
      int        ratio, format;
      uchar*     rgb32BitData;

      if(!xine_get_current_frame(m_stream, &width, &height, &ratio,
                                 &format, NULL))
      {
        return 0;
      }
      yuv = new uint8_t[((width+8) * (height+1) * 2)];
      if (yuv == NULL)
      {
        kDebug() << k_funcinfo <<
            "Not enough memory to make screenshot!" << endl;
        return 0;
      }

      xine_get_current_frame(m_stream, &width, &height, &ratio,
                             &format, yuv);

      // convert to yv12 if necessary
      switch (format)
      {
        case XINE_IMGFMT_YUY2:
        {
          uint8_t *yuy2 = yuv;

          yuv = new uint8_t[(width * height * 2)];
          if (yuv == NULL)
          {
            kDebug() << k_funcinfo <<
                "Not enough memory to make screenshot!" << endl;
            return 0;
          }
          y = yuv;
          u = yuv + width * height;
          v = yuv + width * height * 5 / 4;

          yuy2Toyv12 (y, u, v, yuy2, width, height);

          delete [] yuy2;
        }
        break;
        case XINE_IMGFMT_YV12:
          y = yuv;
          u = yuv + width * height;
          v = yuv + width * height * 5 / 4;

          break;
        default:
        {
          kDebug() << k_funcinfo <<
              QString("Screenshot: Format %1 not supported!")\
              .arg((char*)&format) << endl;
          delete [] yuv;
          return 0;
        }
      }

      // convert to rgb
      rgb32BitData = yv12ToRgb(y, u, v, width, height);
      delete [] yuv;
      return rgb32BitData;
    }

    void seek(const QTime& time)
    {
      xine_play(m_stream, 0, QTime().msecsTo(time));
      xine_set_param(m_stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
    }

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

QImage QMpegFile::frame(const QTime& time) const
{
  XineLib::self()->setFile(filename);
  XineLib::self()->seek(time);

  // From KXineWidget
  int width, height;
  uchar *rgbPile = XineLib::self()->getScreenshot(width, height);

  if(!rgbPile)
    return QImage();

  QImage screenShot =
      QImage(rgbPile, width, height, QImage::Format_RGB32).copy();
  delete []rgbPile;
  return screenShot;
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

  KMF::Time result;

  for(int i = 0; i < count(); ++i)
    result += at(i).duration;
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

bool QMpegVideo::isDVDCompatible() const
{
#warning TODO
  return true;
}

uint64_t QMpegVideo::size() const
{
  uint64_t result;

  for(int i = 0; i < count(); ++i)
    result += at(i).size;
  return result;
}

QImage QMpegVideo::frame(const QTime& time) const
{
  for(int i = 0; i < count(); ++i)
  {
    kDebug() << k_funcinfo << QTime().msecsTo(time) << ", "
        << QTime().msecsTo(at(i).duration) << endl;
    if(QTime().msecsTo(time) < QTime().msecsTo(at(i).duration))
      return at(i).frame(time);
    else
      time.addMSecs(-1 * QTime().msecsTo(at(i).duration));
  }
  return QImage();
}
