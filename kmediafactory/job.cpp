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
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************

#include "job.h"
#include "job_p.h"
#include <KDebug>
#include <QMetaType>

KMF::JobHelper::JobHelper(KMF::Job* parent) : QObject(0)
{
  qRegisterMetaType<KMF::MsgType>("KMF::MsgType");
  connect(this, SIGNAL(newMessage(KMF::MsgType, const QString&)), 
          parent, SIGNAL(newMessage(KMF::MsgType, const QString&)));
  connect(this, SIGNAL(newLogMessage(const QString&, const QString&)), 
          parent, SIGNAL(newLogMessage(const QString&, const QString&)));
  connect(this, SIGNAL(valueChanged(int, const QString&)), 
          parent, SIGNAL(valueChanged(int, const QString&)));
  connect(this, SIGNAL(maximumChanged(int, const QString&)), 
          parent, SIGNAL(maximumChanged(int, const QString&)));
}

KMF::JobHelper::~JobHelper()
{
}

void KMF::JobHelper::message(MsgType type, const QString& txt)
{
  emit newMessage(type, txt);
}

void KMF::JobHelper::log(const QString& msg, const QString& txt)
{
  emit newLogMessage(msg, txt);
}

void KMF::JobHelper::setValue(int value, const QString& txt)
{
  emit valueChanged(value, txt);
}

void KMF::JobHelper::setMaximum(int maximum, const QString& txt)
{
  emit maximumChanged(maximum, txt);
}

class KMF::Job::Private
{
public:
  Private(KMF::Job *j) : job(j), proc(0), result(true), aborted(false), jobHelper(0) {};

  void stdout()
  {
    buffer += proc->readAllStandardOutput();
    out();
  }
  
  void stderr()
  {
    buffer += proc->readAllStandardError();
    out();
  }

  void out()
  {
    int n;
    QRegExp re("[\n\r]");
  
    while((n = buffer.indexOf(re)) >= 0)
    {
      QString tmp = buffer.left(n);
      if(!filter.exactMatch(tmp))
        log += tmp;
      job->output(tmp);
      ++n;
      buffer.remove(0, n);
    }
  }

  KMF::JobHelper* helper()
  {
    if (!jobHelper) 
    {
      jobHelper = new KMF::JobHelper(job);
    }
    return jobHelper;
  }

  QString log;
  QString buffer;
  QRegExp filter;
  KMF::Job *job;
  KProcess* proc;
  bool result;
  bool aborted;
  KMF::JobHelper *jobHelper;
  QString start;
};

KMF::Job::Job(QObject* parent) : ThreadWeaver::Job(parent), d(new KMF::Job::Private(this))
{
}

KMF::Job::~Job()
{
  if (!d->log.isEmpty()) 
  {
    log(d->log, d->start);
  }
  delete d->proc;
}

KProcess* KMF::Job::process(const QString& filter, KProcess::OutputChannelMode mode)
{
  if (!d->log.isEmpty()) 
  {
    log(d->log, d->start);
    d->log = QString();
  }
  delete d->proc;
  d->proc = new KProcess(this);
  d->proc->setOutputChannelMode(mode);
  if(mode != KProcess::OnlyStderrChannel)
  {
    connect(d->proc, SIGNAL(readyReadStandardOutput()), this, SLOT(d->stdout()));
  }
  if(mode != KProcess::OnlyStdoutChannel)
  {
    connect(d->proc, SIGNAL(readyReadStandardError()), this, SLOT(d->stderr()));
  }
  setFilter(filter);
  return d->proc;
}

void KMF::Job::message(MsgType type, const QString& txt)
{
  if (type == KMF::Error)
  {
    failed(); 
  }
  else if (type == KMF::Start)
  {
    d->start = txt; 
  }
  d->helper()->message(type, txt);
}

void KMF::Job::log(const QString& msg, const QString& txt)
{
  d->helper()->log(msg, txt);
}

void KMF::Job::setValue(int value, const QString& txt)
{
  d->helper()->setValue(value, txt);
}

void KMF::Job::setMaximum(int maximum, const QString& txt)
{
  d->helper()->setMaximum(maximum, txt);
}

void KMF::Job::output(const QString& line)
{
  Q_UNUSED(line);
}

void KMF::Job::setFilter(const QString& filter)
{ 
  d->filter.setPattern(filter); 
}

QString KMF::Job::filter() const 
{ 
  return d->filter.pattern(); 
}

void KMF::Job::failed()
{
  d->result = false;
}

bool KMF::Job::aborted()
{
  return d->aborted;
}

bool KMF::Job::success() const
{
  return d->result;
}

void KMF::Job::requestAbort()
{
  d->aborted = true;
  if (d->proc) 
  {
    d->proc->kill();
  }
}

#include "job.moc"
#include "job_p.moc"
