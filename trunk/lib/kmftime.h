//**************************************************************************
//   Copyright (C) 2004-2006 by Petri Damsten
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
#ifndef KMFKMFTIME_H
#define KMFKMFTIME_H

#include <kdemacros.h>
#include <QDateTime>

namespace KMF
{
  class KDE_EXPORT Time : public QTime
  {
    public:
      Time() : QTime(0, 0) {};
      Time(QTime tm) : QTime(tm) {};
      Time(int msec) { set(msec); };
      Time(double seconds) { set(seconds); };
      Time(const QString& time) { set(time); };

      int toMSec() const;
      double toSeconds() const;
      QString toString() const;
      QString toString(QString format) const
          { return QTime::toString(format); };

      void set(int msec);
      void set(double seconds);
      void set(const QString& time);

      operator double() const { return toSeconds(); };
      operator int() const { return toMSec(); };
      Time& operator-=(const Time& t);
      Time& operator+=(const Time& t);
      Time  operator+(double seconds);
      Time  operator+(const Time& t);
      Time  operator-(const Time& t);
      Time& operator+=(int msec);
      Time& operator+=(double seconds);
      Time& operator=(double seconds) { set(seconds); return *this; };
      Time& operator=(int msec) { set(msec); return *this; };
      bool operator<(const QTime& b);
  };
}

#endif
