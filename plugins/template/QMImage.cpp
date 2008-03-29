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
#include "QMImage.h"
#include <qimage.h>
#include <iostream>
#include <kdebug.h>
#include <qbuffer.h>

using namespace Magick;

QImage QMImage::image()
{
  //std::cout << " QMImage::image()" << columns()
  //          << " " << rows() << std::endl;
  QImage img(columns(), rows(), 32);
  const PixelPacket *pixels;

  img.setAlphaBuffer(true);
  for (int y = 0; y < img.height(); y++)
  {
    pixels = getConstPixels(0, y, img.width(), 1);
    for (int x = 0; x < img.width(); x++)
    {
      ColorRGB rgb(*(pixels + x));

      img.setPixel(x, y, qRgba((int)(255 * rgb.red()) ,
                               (int)(255 * rgb.green()) ,
                               (int)(255 * rgb.blue()) ,
                               (int)(255 * (1.0 - rgb.alpha()))));
    }
  }
  return img;
}

QMImage::QMImage() :
    Image(Geometry(1,1), ColorRGB(0,0,0))
{
}

QImage QMImage::mask(const QImage& img, const QRgb& maskColor, bool oneBitMask)
{
  QImage result(img.width(), img.height(), 32);
  double alphaScale = qAlpha(maskColor) / 255.0;

  result.setAlphaBuffer(true);
  for (int y = 0; y < img.height(); y++)
  {
    for (int x = 0; x < img.width(); x++)
    {
      QRgb pix = img.pixel(x, y);
      if(oneBitMask)
      {
        if(qAlpha(pix) > 128)
          pix = qRgba(qRed(maskColor), qGreen(maskColor),
                      qBlue(maskColor), 255);
        else
          pix = qRgba(qRed(maskColor), qGreen(maskColor), qBlue(maskColor), 0);
      }
      else
      {
        pix = qRgba(qRed(maskColor), qGreen(maskColor), qBlue(maskColor),
                    (int)(alphaScale * qAlpha(pix)));
      }
      result.setPixel(x, y, pix);
    }
  }
  //static int i = 0;
  //result.save(QString("/home/damu/tmp/img/koe%1.png").arg(i++), "PNG");
  return result;
}

QMImage::QMImage(const QImage& img) :
    Image()
{
  read(img.width(), img.height(), "BGRA", CharPixel, img.bits());
}

QMImage::QMImage(const QImage& img, const QRgb& maskColor, bool oneBitMask) :
    Image()
{
  QImage tmp = mask(img, maskColor, oneBitMask);
  read(tmp.width(), tmp.height(), "BGRA", CharPixel, tmp.bits());
}
