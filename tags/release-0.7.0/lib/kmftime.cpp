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
//   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//**************************************************************************

#include "kmftime.h"
#include <KDebug>
#include <QStringList>
#include <QRegExp>
#include <math.h>

void KMF::Time::set(int msec)
{
  int s = msec / 1000;
  int ms = msec % 1000;

  setHMS(s / (60*60), (s / 60) % 60, s % 60, ms);
}

void KMF::Time::set(double seconds)
{
  double temp;
  int s = (int)seconds;
  int ms = (int)(modf(seconds, &temp) * 1000.0);

  setHMS(s / (60*60), (s / 60) % 60, s % 60, ms);
}

int KMF::Time::toMSec() const
{
  int s = hour() * (60*60) + minute() * 60 + second();
  return (s * 1000 + msec());
}

double KMF::Time::toSeconds() const
{
  return ((double)hour() *  (60.0*60.0) +
      (double)minute() *  60.0 +
      (double)second() +
      (double)msec() / 1000.0 );
}

KMF::Time& KMF::Time::operator+=(int msec)
{
  *this = addMSecs(msec);
  return *this;
}

KMF::Time& KMF::Time::operator+=(double seconds)
{
  *this = addMSecs((int)(seconds * 1000.0));
  return *this;
}

KMF::Time& KMF::Time::operator-=(const KMF::Time& t)
{
  *this = addMSecs(-1 * t.toMSec());
  return *this;
}

KMF::Time& KMF::Time::operator+=(const KMF::Time& t)
{
  *this = addMSecs(t.toMSec());
  return *this;
}

KMF::Time KMF::Time::operator+(const KMF::Time& t)
{
  return addMSecs(t.toMSec());
}

KMF::Time KMF::Time::operator-(const KMF::Time& t)
{
  return addMSecs(-1 * t.toMSec());
}

KMF::Time KMF::Time::operator+(double seconds)
{
  return addMSecs((int)(seconds * 1000.0));
}

QString KMF::Time::toString() const
{
  return QTime::toString("h:mm:ss.zzz");
}

bool KMF::Time::operator<(const QTime& b)
{
  return (toMSec() < KMF::Time(b).toMSec());
}

void KMF::Time::set(const QString& time)
{
  if(time.indexOf(':') >= 0)
  {
    QStringList list = time.split(QRegExp("[:.]"));
    int h = 0, m = 0, s = 0, z = 0;
    h = list[0].toInt();
    if(list.count() > 1)
      m = list[1].toInt();
    if(list.count() > 2)
      s = list[2].toInt();
    if(list.count() > 3)
      z = list[3].toInt();
    setHMS(h, m, s, z);
  }
  else
    set(time.toDouble());
}
