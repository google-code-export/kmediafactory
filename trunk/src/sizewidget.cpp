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
#include <KDE/KStyle>
#include <KDebug>
#include <QtGui/QPainter>
#include <QStyleOptionProgressBar>
#include <QPaintEvent>


SizeWidget::SizeWidget(QWidget* parent)
  : KSqueezedTextLabel(parent), m_max(10), m_size(0)
{
    setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    setTextElideMode(Qt::RightToLeft==layoutDirection() ? Qt::ElideLeft : Qt::ElideRight);
    setMinimumSize(128, fontMetrics().height()+2);
    m_barElement = KStyle::customControlElement("CE_CapacityBar", this);
}

SizeWidget::~SizeWidget()
{
}

void SizeWidget::paintEvent(QPaintEvent *ev)
{
    QPainter        painter(this);
    bool            exceeded = m_size > m_max;
    int             reverse = (Qt::RightToLeft == layoutDirection());
    KColorScheme    cs(QPalette::Active, KColorScheme::Window);
    QColor          color = cs.foreground(KColorScheme::NegativeText).color();
    QStyleOptionProgressBar opt;
    QRect           rc = rect();

    opt.initFrom(this);
    opt.rect = ev->rect();
    opt.minimum = 0;
    opt.textVisible = false;

    if (exceeded) {
        opt.maximum = 100;
        opt.progress = 100;
        int v = rc.left() + (int)(m_max * (quint64)rc.width() / m_size);
        painter.setClipRect(reverse ? v : rc.left(), rc.top(),
                            reverse ? rc.right() : v, rc.bottom());
        style()->drawControl(m_barElement, &opt, &painter, this);
        opt.palette.setColor(QPalette::Active, QPalette::Highlight, color);
        painter.setClipRect(reverse ? rc.left() : v, rc.top(),
                            reverse ? v : rc.right(), rc.bottom());
        style()->drawControl(m_barElement, &opt, &painter, this);
    } else {
        opt.maximum = (int)(m_max / 1024);
        opt.progress = (int)(m_size / 1024);
        style()->drawControl(m_barElement, &opt, &painter, this);
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
