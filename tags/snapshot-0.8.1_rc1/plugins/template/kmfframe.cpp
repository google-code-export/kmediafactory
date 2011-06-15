// **************************************************************************
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
// **************************************************************************

#include "kmfframe.h"

#include <QtCore/QRect>
#include <QtGui/QPainter>
#include <QtXml/QDomElement>

#include <KDebug>

#include <kmftools.h>

KMFFrame::KMFFrame(QObject *parent)
    : KMFWidget(parent)
    , m_lineWidth(0)
    , m_rounded(0)
{
}

KMFFrame::~KMFFrame()
{
}

void KMFFrame::fromXML(const QDomElement &element)
{
    KMFWidget::fromXML(element);
    m_lineWidth = element.attribute("line_width", "0").toInt();
    m_fillColor =
        KMF::Tools::toColor(element.attribute("fill_color", "#00000000"));
    m_rounded = element.attribute("rounded", "0").toInt();
}

void KMFFrame::paintWidget(QImage *layer, bool shdw) const
{
    if ((m_fillColor.alpha() == 0) && (m_lineWidth == 0)) {
        return;
    }

    QPainter p(layer);
    QRect rc = (shdw) ? paintRect(shadow().offset()) : paintRect();
    QColor rgb = (shdw && color().alpha() != 0) ? shadow().color() : color();
    QColor rgbFill = (shdw && m_fillColor.alpha() != 0) ?
                     shadow().color() : m_fillColor;
    int adjust = m_lineWidth / 2;
    // kDebug() << rgb << ":" << shdw;
    p.setPen(QPen(rgb, m_lineWidth));
    p.setBrush(QBrush(rgbFill));
    rc.adjust(adjust, adjust, -1 * adjust, -1 * adjust);

    if (m_rounded == 0) {
        p.drawRect(rc);
    } else   {
        KMF::Tools::drawRoundRect(&p, rc, m_rounded);
    }
}

#include "kmfframe.moc"
