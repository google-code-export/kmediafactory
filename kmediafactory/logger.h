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

#ifndef KMFLOGGER_H
#define KMFLOGGER_H

#include <kdemacros.h>
#include <KProcess>
#include <QObject>
#include <QColor>

namespace KMF
{
  class KDE_EXPORT Logger : public QObject
  {
      Q_OBJECT
    public:
      explicit Logger(QObject *parent = 0);
      ~Logger();
  
      virtual void start();
      virtual void stop();
      virtual const QString& log() const;
      virtual void message(const QString& msg, const QColor& color);
      virtual bool save(QString file) const;
  
    public slots:
      void message(const QString& msg);
  
    private:
      class Private;
      Private *const d;
  };
} // namespace KMF

#endif // KMFLOGGER_H
