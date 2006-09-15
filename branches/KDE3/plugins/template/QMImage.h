//**************************************************************************
//   Copyright (C) 2003-2004 by Erik Kaffehr
//
//   Convert to and from QImage using ImageMagick
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
#ifndef QIMAGEH
#define QIMAGEH

#include <qcolor.h>
#include <Magick++.h>

class QImage;

class QMImage: public Magick::Image
{
  public:
    QMImage();
    QMImage(const QImage&);
    QMImage(const QImage& img, const QRgb& maskColor, bool oneBitMask = false);
    QMImage(const Image& img):Image(img){}
    ~QMImage(){}
    QImage image();
    void image(const QImage&);
  private:
    QMImage(const QMImage&);
};

#endif

