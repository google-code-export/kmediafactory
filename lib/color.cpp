//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damstén
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
#include "color.h"
#include <Magick++.h>

QT_STATIC_CONST_IMPL KMF::Color KMF::Color::null;

Magick::Color KMF::Color::toMagickColor() const
{
  Magick::ColorRGB rgb(((MagickLib::Quantum)red())/255.0,
                         ((MagickLib::Quantum)green())/255.0,
                         ((MagickLib::Quantum)blue())/255.0);
  rgb.alphaQuantum((MagickLib::Quantum)(alpha()/255.0));
  return rgb;
}

// This function is from qcolor.cpp
int KMF::Color::hex2int(QChar hexchar)
{
  int v;
  if ( hexchar.isDigit() )
    v = hexchar.digitValue();
  else if ( hexchar >= 'A' && hexchar <= 'F' )
    v = hexchar.cell() - 'A' + 10;
  else if ( hexchar >= 'a' && hexchar <= 'f' )
    v = hexchar.cell() - 'a' + 10;
  else
    v = 0;
  return v;
}


void KMF::Color::setNamedColor(const QString& s)
{
  if(s.isEmpty())
  {
    setRgb(0);
  }
  else if (s[0].isDigit()) // Color as integer
  {
    setRgb(s.toLong());
  }
  else if (s[0] == '#' && s.length() == 9) // Special alpha channel case
  {
    setRgb(qRgba((hex2int(s[1]) << 4) + hex2int(s[2]),
                 (hex2int(s[3]) << 4) + hex2int(s[4]),
                 (hex2int(s[5]) << 4) + hex2int(s[6]),
                 (hex2int(s[7]) << 4) + hex2int(s[8])));
  }
  else
  {
    QColor::setNamedColor(s);
  }
}

KMF::Color::operator Magick::Color() const
{
  return toMagickColor();
}
