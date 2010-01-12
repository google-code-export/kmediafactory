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

#include "sizewidget.h"
#include <KDE/KLocale>
#include <KDE/KColorScheme>
#include <KDE/KColorUtils>
#include <KDE/KGlobal>
#include <QtGui/QPainter>

static QPainterPath buildPath(const QRectF &r, bool roundLeft=false, bool roundRight=false)
{
    QPainterPath path;
    double       radius=r.height()*0.4,
                 diameter(radius*2);

    if (roundRight)
    {
        path.moveTo(r.x()+r.width(), r.y()+r.height()-radius);
        path.arcTo(r.x()+r.width()-diameter, r.y(), diameter, diameter, 0, 90);
    }
    else
    {
        path.moveTo(r.x()+r.width(), r.y()+r.height());
        path.lineTo(r.x()+r.width(), r.y());
    }

    if (roundLeft)
    {
        path.arcTo(r.x(), r.y(), diameter, diameter, 90, 90);
        path.arcTo(r.x(), r.y()+r.height()-diameter, diameter, diameter, 180, 90);
    }
    else
    {
        path.lineTo(r.x(), r.y());
        path.lineTo(r.x(), r.y()+r.height());
    }

    if (roundRight)
        path.arcTo(r.x()+r.width()-diameter, r.y()+r.height()-diameter, diameter, diameter, 270, 90);
    else
        path.lineTo(r.x()+r.width(), r.y()+r.height());

    return path;
}

static void buildGrad(QLinearGradient &grad, const QColor &col)
{
    grad.setColorAt(0, KColorUtils::shade(col, 0.35)); // 1.2));
    grad.setColorAt(0.499, KColorUtils::shade(col, -0.4)); // 0.984));
    grad.setColorAt(0.5, KColorUtils::shade(col, -0.35)); // 0.9));
    grad.setColorAt(1.0, col); // KColorUtils::shade(col, 0.12)); // 1.06));
}

SizeWidget::SizeWidget(QWidget* parent)
  : KSqueezedTextLabel(parent), m_max(10), m_size(0)
{
    setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    setTextElideMode(Qt::RightToLeft==layoutDirection() ? Qt::ElideLeft : Qt::ElideRight);
    setMinimumSize(128, fontMetrics().height()+2);
}

SizeWidget::~SizeWidget()
{
}

void SizeWidget::paintEvent(QPaintEvent *ev)
{
    QRect           r(rect());
    QRectF          rf(r.x()+0.5, r.y()+0.5, r.width()-1, r.height()-1);
    QPainter        painter(this);
    bool            exceeded=m_size > m_max,
                    reverse=Qt::RightToLeft==layoutDirection();
    int             used=exceeded
                        ? (r.width()*m_max)/m_size
                        : (r.width()*m_size)/m_max,
                    other=r.width()-used;
    QLinearGradient grad(r.topLeft(), r.bottomLeft());
    KColorScheme    cs(QPalette::Active, KColorScheme::Window);

    painter.setRenderHint(QPainter::Antialiasing, true);
    if(used)
    {
        QColor col=cs.background(KColorScheme::PositiveBackground).color();
        QRectF rf2(rf);

        buildGrad(grad, col);
        if(other && reverse)
            rf2.setX(rf2.x()+other);
        rf2.setWidth(used);
        painter.fillPath(buildPath(rf2, !other||!reverse, !other||reverse), grad);
    }
    if(other)
    {
        QColor col=cs.background(exceeded ? KColorScheme::NegativeBackground : KColorScheme::NormalBackground).color();
        QRectF rf2(rf);

        buildGrad(grad, col);
        if(!reverse && used)
            rf2.setX(rf2.x()+used);
        rf2.setWidth(other);
        painter.fillPath(buildPath(rf2, !used||reverse, !used||!reverse), grad);
    }

    KSqueezedTextLabel::paintEvent(ev);
}

void SizeWidget::setSizes(quint64 max, quint64 size)
{
    m_max=max;
    m_size=size;
    updateLabel();
}

void SizeWidget::updateLabel()
{
  setText(m_size>m_max
            ? i18n("<b>Capacity (%1) exceeded by %2</b>",
//                    KGlobal::locale()->formatByteSize(m_size),
                   KGlobal::locale()->formatByteSize(m_max),
//                    (100*m_size)/m_max,
                   KGlobal::locale()->formatByteSize(m_size-m_max))
            : i18n("%1 of %2 used (%3%)",
                   KGlobal::locale()->formatByteSize(m_size),
                   KGlobal::locale()->formatByteSize(m_max),
                   (100*m_size)/m_max));
}

#include "sizewidget.moc"
