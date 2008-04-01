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

#ifndef QFFMPEG_H
#define QFFMPEG_H

#include <qobject.h>
#include <qimage.h>
#include <qdatetime.h>
#include <qmap.h>
#include <stdint.h>

struct AVFrame;
struct AVInputFormat;
struct AVFormatContext;
struct AVCodecContext;

class QAVImage : public QImage
{
  public:
    QAVImage(AVFrame* frame, int width, int height);
};

class QAVTime : public QTime
{
  public:
    QAVTime() : QTime(0, 0) {};
    QAVTime(QTime tm) : QTime(tm) {};
    QAVTime(int64_t avTime) { set(avTime); };

    int64_t toAVTime() const;
    void set(int64_t avTime);

    operator int64_t() const { return toAVTime(); };
    QAVTime& operator=(int64_t avTime) { set(avTime); return *this; };
};

class QFFMpegFile
{
  public:
    QFFMpegFile(const QString& filename = QString::null);
    virtual ~QFFMpegFile();
    void reset();
    bool open(const QString& filename);
    const QString fileName() const { return m_filename; };
    const AVInputFormat* fileFormat() const;
    void freeAVData();
    bool seek(int64_t timestamp, bool backwards);
    int64_t avDuration() const;
    QTime duration() const;
    uint audioSampleRate() const;
    AVFrame* frame() const;
    double frameRate() const;
    int64_t frameLength() const;
    int width() const;
    int height() const;
    int aspectRatio() const;
    void SaveFrame(AVFrame *pFrame, int width, int height) const;
    bool hasVideo() const { return m_videoStream >= 0; };
    bool hasAudio() const { return m_audioStreams.count() > 0; };
    QValueList<int> audioStreams() const { return m_audioStreams; };
    bool isDVDCompatible() const;

  protected:
    QString          m_filename;
    AVFrame*         m_pFrameRGB;
    AVFrame*         m_pFrame;
    uint8_t*         m_buffer;
    AVFormatContext* m_pFormatCtx;
    AVCodecContext*  m_pCodecCtx;
    int              m_videoStream;
    QValueList<int>  m_audioStreams;
    int64_t          m_pos;
};

typedef QValueList<QFFMpegFile> QFFMpegFileList;

class QFFMpegParam
{
  public:
    QFFMpegParam() {};
    QFFMpegParam(QString k, QString v) : key(k), value(v) {};
    QFFMpegParam(QString k, int v) : key(k), value(QString("%1").arg(v)) {};
    QString key;
    QString value;
};

typedef QValueList<QFFMpegParam> QFFMpegConvertTo;

class QFFMpegConverter;

class QFFMpeg : public QObject
{
    Q_OBJECT
    friend class FFMpegConverter;
  public:
    QFFMpeg();
    QFFMpeg(QString file);
    virtual ~QFFMpeg();

    const QFFMpegFileList& files() const { return m_files; };
    bool addFile(const QString& file);
    QImage frame(QTime qtime)
        { return QAVImage(rawFrame(qtime), width(), height()); };
    AVFrame* rawFrame(QTime qtime);
    QTime position() const { return m_position; };
    QTime duration() const;
    int width() const { return m_files.first().width(); };
    int height() const { return m_files.first().height(); };
    int aspectRatio() const { return m_files.first().aspectRatio(); };
    uint audioStreamCount() const;
    QStringList fileNames() const;
    void clear();
    double frameRate() const;
    bool isDVDCompatible() const;
    bool convertTo(QFFMpegConvertTo type, int input, const QString& output);
    QDateTime lastModified();
    void stop();

  signals:
    void convertProgress(int);
    void message(const QString&);

  private slots:
    void messageSlot(const QString&);
    void progressSlot(int);

  private:
    QFFMpegFileList m_files;
    QAVTime m_position;
    QFFMpegConverter* m_ff;
};

#endif // QFFMPEG_H
