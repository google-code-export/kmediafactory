//**************************************************************************
//   Copyright (C) 2008 by Petri Damsten
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

#ifndef KMFJOB_P_H
#define KMFJOB_P_H

#include "kmediafactory.h"

namespace KMF
{
  class Job;

  // http://api.kde.org/4.0-api/kdelibs-apidocs/threadweaver/html/MainComponents.html
  class JobHelper : public QObject
  {
      Q_OBJECT
    public:
      explicit JobHelper(Job* parent);
      ~JobHelper();

      void message(uint id, KMF::MsgType type, const QString& msg = QString());
      void log(uint id, const QString& msg);
      void setValue(uint id, int value);
      void setMaximum(uint id, int maximum);

    signals:
      void newMessage(uint id, KMF::MsgType type, const QString& msg);
      void newLogMessage(uint id, const QString& msg);
      void valueChanged(uint id, int value);
      void maximumChanged(uint id, int maximum);
  };

  class Job::Private : public QObject
  {
    Q_OBJECT
  public:
    explicit Private(KMF::Job *j);
  
    void out();
    JobHelper* helper();

  public slots:
    void stdout();
    void stderr();
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
  
  public:
    QString log;
    QString buffer;
    QRegExp filter;
    KMF::Job *job;
    KProcess* proc;
    bool result;
    bool aborted;
    KMF::JobHelper *jobHelper;
    uint msgid;
    QString lastLine;
    uint count;
  };
  
} // namespace KMF

#endif // KMFJOB_P_H
