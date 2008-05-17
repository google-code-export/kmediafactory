//**************************************************************************
//   Copyright (C) 2004-2008 by Petri Damsten
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

#include "logger.h"
#include <KDebug>
#include <QEventLoop>
#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include <QTextDocument>

class KMF::Logger::Private
{
  public:
    Private(KMF::Logger *l) : logger(l) {};

    QString log;
    KMF::Logger* logger;
};

KMF::Logger::Logger(QObject *parent) : QObject(parent), d(new KMF::Logger::Private(this))
{
}

KMF::Logger::~Logger()
{
}

void KMF::Logger::start()
{
  d->log = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" \
          "<html><pre style=\"font-size: 9pt;\">";
}

void KMF::Logger::stop()
{
  d->log += "</pre></html>";
}

void KMF::Logger::message(const QString& msg, const QColor& color)
{
  QString s = msg.trimmed();
  if(s.isEmpty())
    return;
  if(color != QColor("black"))
    d->log += QString("<font color=%1>").arg(color.name());
  //d->log += "         1         2         3         4         5         6"
  //         "         7         8\n";
  d->log += Qt::escape(s) + "\n";
  if(color != QColor("black"))
    d->log += "</font>";
}

bool KMF::Logger::save(QString file) const
{
  QFile f(file);

  if(f.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    QTextStream t(&f);
    t.setCodec("UTF-8");
    t << d->log;
    f.close();
    return true;
  }
  return false;
}

void KMF::Logger::message(const QString& msg) 
{ 
  message(msg, QColor("black")); 
}

const QString& KMF::Logger::log() const 
{ 
  return d->log; 
}

#include "logger.moc"
