//**************************************************************************
//   Copyright (C) 2004 by Petri Damstén
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
#ifndef KMFUIINTERFACE_H
#define KMFUIINTERFACE_H

#include <kmediafactory/uiinterface.h>

#define RECORD_TIMINGS 0

#if RECORD_TIMINGS

#include <qdatetime.h>
#include <qfile.h>

class StopWatch
{
  public:
    void start()
    {
      if(!m_stream.device())
      {
        QFile* file = new QFile("/tmp/timings.txt");
        file->open(IO_WriteOnly);
        m_stream.setDevice(file);
        m_time.start();
      }
    };

    void stop()
    {
      m_stream.device()->close();
      delete m_stream.device();
      m_stream.unsetDevice();
    };

    void message(const QString& msg)
    {
      m_stream << msg << endl;
    };

    void progress(int advance)
    {
      if(advance)
      {
        m_stream << advance << " - " << m_time.elapsed() << endl;
        m_time.restart();
      }
    };

  private:
    QTextStream m_stream;
    QTime m_time;
};

extern StopWatch stopWatch;

#endif

namespace KMF
{
  class Logger;
}

/**
*/
class KMFUiInterface : public KMF::UiInterface
{
    Q_OBJECT
  public:
    KMFUiInterface(QObject *parent = 0, const char *name = 0);
    ~KMFUiInterface();
    virtual bool addMediaObject(KMF::MediaObject* media) const;
    virtual bool addTemplateObject(KMF::TemplateObject* tob);
    virtual bool addOutputObject(KMF::OutputObject* oob);
    virtual bool removeMediaObject(KMF::MediaObject* media) const;
    virtual bool removeTemplateObject(KMF::TemplateObject* tob);
    virtual bool removeOutputObject(KMF::OutputObject* oob);

    virtual bool message(KMF::MsgType type, const QString& msg);
    virtual bool progress(int advance);
    virtual bool setItemTotalSteps(int totalSteps);
    virtual bool setItemProgress(int progress);
    virtual KMF::Logger* logger();
    void setUseMessageBox(bool useMessageBox)
        { m_useMessageBox = useMessageBox; };
    void setStopped(bool stopped)
        { m_stopped = stopped; };

  private:
    bool m_useMessageBox;
    bool m_stopped;
};

#endif // KMFUIINTERFACE_H
