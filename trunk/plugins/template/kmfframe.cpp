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
#include "kmfframe.h"
#include <kmftools.h>
#include <kdebug.h>
#include <QRect>
#include <QDomElement>
#include <QPainter>

KMFFrame::KMFFrame(QObject *parent) :
  KMFWidget(parent), m_lineWidth(0), m_rounded(0)
{
}

KMFFrame::~KMFFrame()
{
}

void KMFFrame::fromXML(const QDomElement& element)
{
  KMFWidget::fromXML(element);
  m_lineWidth = element.attribute("line_width", "0").toInt();
  m_fillColor =
       KMF::Tools::toColor(element.attribute("fill_color", "#00000000"));
  m_rounded = element.attribute("rounded", "0").toInt();
}

void KMFFrame::paintWidget(QImage& layer, bool shdw)
{
  if(m_fillColor.alpha() == 0 && m_lineWidth == 0)
    return;

  QPainter p(&layer);
  QRect rc = (shdw)? paintRect(shadow().offset()) : paintRect();
  QColor rgb = (shdw)? shadow().color() : color();
  QColor rgbFill = (shdw)? shadow().color() : m_fillColor;

  if(!m_fillColor.alpha() == 0)
  {
    if(m_rounded == 0)
    {
      p.fillRect(rc, rgbFill);
    }
    else
    {
      // TODO: fill rounded rectangle
    }
  }
  if(m_lineWidth > 0)
  {
    p.drawRoundRect(rc, m_rounded);
    /*
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
    */
  }
}

#include "kmfframe.moc"
