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
#include "plugininterface.h"
#include <QVariant>
#include <KDebug>
#include <QMetaType>

KMF::JobHelper::JobHelper(KMF::Job* parent) : QObject(0)
{
  qRegisterMetaType<KMF::MsgType>("KMF::MsgType");
  connect(this, SIGNAL(newMessage(uint, KMF::MsgType, const QString&)), 
          parent, SIGNAL(newMessage(uint, KMF::MsgType, const QString&)));
  connect(this, SIGNAL(newLogMessage(uint, const QString&)), 
          parent, SIGNAL(newLogMessage(uint, const QString&)));
  connect(this, SIGNAL(valueChanged(uint, int)), 
          parent, SIGNAL(valueChanged(uint, int)));
  connect(this, SIGNAL(maximumChanged(uint, int)), 
          parent, SIGNAL(maximumChanged(uint, int)));
}

KMF::JobHelper::~JobHelper()
{
}

void KMF::JobHelper::message(uint id, MsgType type, const QString& msg)
{
  emit newMessage(id, type, msg);
}

void KMF::JobHelper::log(uint id, const QString& msg)
{
  emit newLogMessage(id, msg);
}

void KMF::JobHelper::setValue(uint id, int value)
{
  emit valueChanged(id, value);
}

void KMF::JobHelper::setMaximum(uint id, int maximum)
{
  emit maximumChanged(id, maximum);
}

KMF::Job::Private::Private(KMF::Job *j) : 
  job(j), proc(0), result(true), aborted(false), jobHelper(0) 
{
  msgid = KMF::PluginInterface::messageId();
}

void KMF::Job::Private::stdout()
{
  buffer += proc->readAllStandardOutput();
  out();
}

void KMF::Job::Private::stderr()
{
  buffer += proc->readAllStandardError();
  out();
}

void KMF::Job::Private::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
  Q_UNUSED(exitCode);
  Q_UNUSED(exitStatus);
  uint id = proc->property("id").toUInt();
  if (!buffer.isEmpty())
  {
    buffer += "\n";
    out();
  }
  job->log(id, log);
  log = QString();
}

void KMF::Job::Private::out()
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

KMF::JobHelper* KMF::Job::Private::helper()
{
  if (!jobHelper) 
  {
    jobHelper = new KMF::JobHelper(job);
  }
  return jobHelper;
}

KMF::Job::Job(QObject* parent) : ThreadWeaver::Job(parent), d(new KMF::Job::Private(this))
{
}

KMF::Job::~Job()
{
  delete d->proc;
}

KProcess* KMF::Job::process(uint id, const QString& filter, KProcess::OutputChannelMode mode)
{
  delete d->proc;
  d->proc = new KProcess(this);
  d->proc->setProperty("id", id);
  d->proc->setOutputChannelMode(mode);
  if(mode != KProcess::OnlyStderrChannel)
  {
    connect(d->proc, SIGNAL(readyReadStandardOutput()), d, SLOT(d->stdout()));
  }
  if(mode != KProcess::OnlyStdoutChannel)
  {
    connect(d->proc, SIGNAL(readyReadStandardError()), d, SLOT(d->stderr()));
  }
  connect(d->proc, SIGNAL(finished(int, QProcess::ExitStatus)), 
          d, SLOT(finished(int, QProcess::ExitStatus)));

  setFilter(filter);
  return d->proc;
}

void KMF::Job::message(uint id, MsgType type, const QString& msg)
{
  if (type == KMF::Error)
  {
    failed(); 
  }
  d->helper()->message(id, type, msg);
}

void KMF::Job::log(uint id, const QString& msg)
{
  d->helper()->log(id, msg);
}

void KMF::Job::setValue(uint id, int value)
{
  d->helper()->setValue(id, value);
}

void KMF::Job::setMaximum(uint id, int maximum)
{
  d->helper()->setMaximum(id, maximum);
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

uint KMF::Job::msgId()
{
  return d->msgid;
}

#include "job.moc"
#include "job_p.moc"
