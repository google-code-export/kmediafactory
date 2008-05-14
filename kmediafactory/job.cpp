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

KMF::JobHelper::JobHelper(KMF::Job* parent) : QObject(parent)
{
  connect(this, SIGNAL(newMessage(PluginInterface::MsgType, const QString&)), 
          parent, SIGNAL(newMessage(PluginInterface::MsgType, const QString&)));
  connect(this, SIGNAL(value(int)), parent, SIGNAL(value(int)));
  connect(this, SIGNAL(maximum(int)), parent, SIGNAL(maximum(int)));
}

KMF::JobHelper::~JobHelper()
{
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
        job->log(tmp);
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
};

KMF::Job::Job(QObject* parent) : ThreadWeaver::Job(parent), d(new KMF::Job::Private(this))
{
}

KMF::Job::~Job()
{
  delete d->proc;
}

KProcess* KMF::Job::process(const QString& filter, KProcess::OutputChannelMode mode)
{
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

void KMF::Job::message(PluginInterface::MsgType type, const QString& msg)
{
  if (type == KMF::PluginInterface::Error)
  {
    failed(); 
  }
  //emit d->helper()->newMessage(type, msg);
}

void KMF::Job::setValue(int value)
{
  // emit through helper class
}

void KMF::Job::setMaximum(int maximum)
{
  // emit through helper class
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
