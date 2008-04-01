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
#include "kmfframe.h"
#include <kdebug.h>
#include <qrect.h>
#include <qdom.h>

KMFFrame::KMFFrame(QObject *parent, const char *name) :
  KMFWidget(parent, name), m_lineWidth(0), m_rounded(0)
{
}

KMFFrame::~KMFFrame()
{
}

void KMFFrame::fromXML(const QDomElement& element)
{
  KMFWidget::fromXML(element);
  m_lineWidth = element.attribute("line_width", "0").toInt();
  m_fillColor.setNamedColor(element.attribute("fill_color", "#00000000"));
  m_rounded = element.attribute("rounded", "0").toInt();
}

void KMFFrame::paintWidget(Magick::Image& layer, bool shdw)
{
  if(m_fillColor.isNull() && m_lineWidth == 0)
    return;

  std::list<Magick::Drawable> drawList;
  QRect rc = (shdw)? paintRect(shadow().offset()) : paintRect();
  KMF::Color rgb = (shdw)? shadow().color() : color();
  KMF::Color rgbFill = (shdw)? shadow().color() : m_fillColor;

  if(!m_fillColor.isNull())
  {
    drawList.push_back(Magick::DrawableFillColor(rgbFill));
    drawList.push_back(Magick::DrawableFillOpacity(rgbFill.opacity()));
    drawList.push_back(Magick::DrawableStrokeWidth(0));
    if(m_rounded == 0)
    {
      drawList.push_back(Magick::DrawableRectangle(
          rc.left(),
          rc.top(),
          rc.right(),
          rc.bottom()));
    }
    else
    {
      drawList.push_back(Magick::DrawableRoundRectangle(
          rc.x(),
          rc.y(),
          rc.right(),
          rc.bottom(),
          m_rounded,
          m_rounded));
    }
  }
  if(m_lineWidth > 0)
  {
    // I could not get DrawableRectangle or DrawableLine to draw exact line
    // widths with or without antialias. So Rectangle is drawn with 4 filled
    // rectangles
    drawList.push_back(Magick::DrawableFillColor(rgb));
    drawList.push_back(Magick::DrawableStrokeWidth(0));
    // Top
    drawList.push_back(Magick::DrawableRectangle(
        rc.left(),
        rc.top(),
        rc.right(),
        rc.top() + m_lineWidth - 1));
    // Bottom
    drawList.push_back(Magick::DrawableRectangle(
        rc.left(),
        rc.bottom() - m_lineWidth + 1,
        rc.right(),
        rc.bottom()));
    // Left
    drawList.push_back(Magick::DrawableRectangle(
        rc.left(),
      rc.top() + m_lineWidth,
      rc.left() + m_lineWidth - 1,
      rc.bottom() - m_lineWidth));
    // Right
    drawList.push_back(Magick::DrawableRectangle(
        rc.right() - m_lineWidth + 1,
        rc.top() + m_lineWidth,
        rc.right(),
        rc.bottom() - m_lineWidth));
  }
  /*
  // This doesn't draw rectangle that would be acceptable
  //drawList.push_back(Magick::DrawableFillColor("white"));
  drawList.push_back(Magick::DrawableFillOpacity(0.0));
  drawList.push_back(Magick::DrawableRectangle(rc.left(),
                                               rc.top(),
                                               rc.right(),
                                               rc.bottom()));
  */
  layer.draw(drawList);
}

#include "kmfframe.moc"
