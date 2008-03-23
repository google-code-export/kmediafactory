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

QImage QMImage::image()
{
  //std::cout << " QMImage::image()" << columns()
  //          << " " << rows() << std::endl;
  QImage img(columns(), rows(), 32);
  const Magick::PixelPacket *pixels;

  img.setAlphaBuffer(true);
  for (int y = 0; y < img.height(); y++)
  {
    pixels = getConstPixels(0, y, img.width(), 1);
    for (int x = 0; x < img.width(); x++)
    {
      Magick::ColorRGB rgb(*(pixels + x));

      img.setPixel(x, y, qRgba((int)(255 * rgb.red()) ,
                               (int)(255 * rgb.green()) ,
                               (int)(255 * rgb.blue()) ,
                               (int)(255 * (1.0 - rgb.alpha()))));
    }
  }
  return img;
}

QMImage::QMImage() :
    Image(Magick::Geometry(1,1), Magick::ColorRGB(0,0,0))
{
}

QMImage::QMImage(const QImage& img) :
    Image(Magick::Geometry(img.width(), img.height()),
          Magick::ColorRGB(0.5, 0.2, 0.3))
{
  double scale = 1 / 256.0;
  Magick::PixelPacket *pixels;

  modifyImage();
  for (int y = 0; y < img.height(); y++)
  {
    pixels = setPixels(0, y, columns(), 1);
    for (int x = 0; x < img.width(); x++)
    {
      QRgb pix = img.pixel(x, y);
      Magick::ColorRGB rgb(scale * qRed(pix), scale * qGreen(pix),
                           scale * qBlue(pix));
      rgb.alpha(1.0 - (scale * qAlpha(pix)));
      *pixels++ = rgb;
    }
    syncPixels();
  }
}

QMImage::QMImage(const QImage& img, const QRgb& maskColor, bool oneBitMask) :
    Image(Magick::Geometry(img.width(), img.height()),
          Magick::ColorRGB(qRed(maskColor)/256.0, qGreen(maskColor)/256.0,
                           qBlue(maskColor)/256.0))
{
  double alphaScale = qAlpha(maskColor) / (256.0 * 256.0);
  Magick::PixelPacket *pixels;

  modifyImage();
  for (int y = 0; y < img.height(); y++)
  {
    pixels = setPixels(0, y, columns(), 1);
    for (int x = 0; x < img.width(); x++)
    {
      QRgb pix = img.pixel(x, y);
      Magick::ColorRGB rgb(*pixels);
      if(oneBitMask)
      {
        if(qAlpha(pix) > 128)
          rgb.alpha(0.0);
        else
          rgb.alpha(1.0);
      }
      else
      {
        //rgb.alpha(1.0);
        rgb.alpha(1.0 - (alphaScale * qAlpha(pix)));
      }
      *pixels++ = rgb;
    }
    syncPixels();
  }
}
