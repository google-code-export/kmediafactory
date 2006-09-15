//**************************************************************************
//   Copyright (C) 2004 by Petri Damst�
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
#include "kmflogger.h"
#include "kmfapplication.h"
#include <kdebug.h>
#include <QEventLoop>
#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include <Q3StyleSheet>

void KMFLogger::start()
{
  m_log = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n" \
          "<html><pre style=\"font-size: 9pt;\">";
  m_buffer = "";
}

void KMFLogger::stop()
{
  KMF::Logger::message(m_buffer);
  m_log += "</pre></html>";
}

void KMFLogger::stdout(KProcess*, char* buffer, int buflen)
{
  int n;
  QRegExp re("[\n\r]");

  m_buffer += QString::fromLatin1(buffer, buflen);
  while((n = m_buffer.indexOf(re)) >= 0)
  {
    if(!m_filter.exactMatch(m_buffer.left(n)))
      KMF::Logger::message(m_buffer.left(n));
    ++n;
    m_buffer.remove(0, n);
  }
  kmfApp->processEvents(QEventLoop::AllEvents);
}

void KMFLogger::stderr(KProcess* proc, char* buffer, int buflen)
{
  stdout(proc, buffer, buflen);
}

void KMFLogger::message(const QString& msg, const QColor& color)
{
  QString s = msg.trimmed();
  if(s.isEmpty())
    return;
  if(color != QColor("black"))
    m_log += QString("<font color=%1>").arg(color.name());
  //m_log += "         1         2         3         4         5         6"
  //         "         7         8\n";
  m_log += Q3StyleSheet::escape(s) + "\n";
  if(color != QColor("black"))
    m_log += "</font>";
}

bool KMFLogger::save(QString file) const
{
  QFile f(file);

  if(f.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    QTextStream t(&f);
    t.setCodec("UTF-8");
    t << m_log;
    f.close();
    return true;
  }
  return false;
}

#include "kmflogger.moc"
