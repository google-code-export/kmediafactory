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
#ifndef KMFLOGGER_H
#define KMFLOGGER_H

#include <kmediafactory/uiinterface.h>
#include <qregexp.h>

/**
*/

// TODO: Method to switch html on/off

class KMFLogger : public KMF::Logger
{
    Q_OBJECT
  public:
    virtual void start();
    virtual void stop();
    virtual const QString& log() const { return m_log; };
    virtual void message(const QString& msg, const QColor& color);
    virtual void setFilter(const QString& filter)
        { m_filter.setPattern(filter); };
    virtual QString filter() const { return m_filter.pattern(); };
    virtual bool save(QString file) const;

  public slots:
    virtual void stdout(KProcess *proc, char *buffer, int buflen);
    virtual void stderr(KProcess *proc, char *buffer, int buflen);

  private:
    QString m_log;
    QString m_buffer;
    QRegExp m_filter;
};

#endif
