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
#ifndef KMFCOLOR_H
#define KMFCOLOR_H

#include <qcolor.h>

namespace Magick { class Color; };

namespace KMF
{
  class Color : public QColor
  {
    public:
      Color() : QColor(0, 0xffffffff) {};
      Color(QRgb rgb) : QColor(rgb) {};
      Color(const char* color) { setNamedColor(color); };
      ~Color() {};

      void setNamedColor(const QString& s);
      Magick::Color toMagickColor() const;
      int alpha() const { return qAlpha(rgb()); };
      double opacity() const { return ((double)qAlpha(rgb()))/255.0; };
      bool isNull() const { return (alpha() == 0); }

      QT_STATIC_CONST Color null;

      operator Magick::Color() const;

      static int hex2int(QChar hexchar);
  };
}

#endif
