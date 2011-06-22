// **************************************************************************
//   Copyright (C) 2004-2010 by Petri Damsten
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

#include "sizewidget.h"

#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionProgressBar>

#include <KColorScheme>
#include <KColorUtils>
#include <KDebug>
#include <KGlobal>
#include <KLocale>
#include <KStyle>

SizeWidget::SizeWidget(QWidget *parent)
    : QWidget(parent)
    , m_max(100)
    , m_value(0)
{
}

SizeWidget::~SizeWidget()
{
}

void SizeWidget::paintBar(QPainter *painter, const QPalette &pal)
{
    QStyleOptionProgressBar opt;
    opt.minimum=0;
    opt.maximum=100;
    opt.progress=m_value > m_max ? m_max : ((100 * m_value) / m_max);
    opt.textVisible=true;
    opt.text=m_value > m_max ? i18n("Capacity (%1) exceeded by %2",
                                    KGlobal::locale()->formatByteSize(m_max),
                                    KGlobal::locale()->formatByteSize(m_value - m_max))
                             : i18n("%1 of %2 used (%3%)",
                                    KGlobal::locale()->formatByteSize(m_value),
                                    KGlobal::locale()->formatByteSize(m_max),
                                    (100 * m_value) / m_max);
    
    opt.rect=rect();
    opt.state=QStyle::State_Enabled;
    opt.palette=pal;
    opt.direction=layoutDirection();
    opt.fontMetrics=fontMetrics();

    if(m_value > m_max) {
        QFont font(painter->font());

        font.setBold(true);
        painter->save();
        painter->setFont(font);
    }

    style()->drawControl(QStyle::CE_ProgressBar, &opt, painter, 0L);

    if(m_value > m_max) {
        painter->restore();
    }
}

void SizeWidget::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    bool reverse = (Qt::RightToLeft == layoutDirection());
    QRect rc = rect();
    bool exceeded = m_value > m_max;
    int percent=(100 * m_value) / m_max,
        v = qMin(percent, 100) * rc.width() / qMax(percent, 100);

    painter.setClipRegion(ev->region());
    if (exceeded) {
        painter.save();
        painter.setClipRect(reverse ? v : rc.left(), rc.top(),
                               reverse ? rc.right() : v, rc.bottom(), Qt::IntersectClip);
    }

    paintBar(&painter, palette());

    if (exceeded) {
        KColorScheme cs(QPalette::Active, KColorScheme::Window);
        QColor color = cs.foreground(KColorScheme::NegativeText).color();
        QPalette pal = palette();

        pal.setColor(QPalette::Active, QPalette::Highlight, color);
        painter.restore();
        painter.setClipRect(reverse ? rc.left() : v, rc.top(),
                               reverse ? v : rc.right(), rc.bottom(), Qt::IntersectClip);
        paintBar(&painter, pal);
    }
}

void SizeWidget::setSizes(quint64 max, quint64 size)
{
    m_max=max;
    m_value=size;
    update();
}

QSize SizeWidget::minimumSizeHint() const
{
    return fontMetrics().size(0, i18n("Capacity (%1) exceeded by %2",
                                      KGlobal::locale()->formatByteSize(999.99*1024*1024*1024), 
                                      KGlobal::locale()->formatByteSize(999.99*1024*1024*1024)))+QSize(4, 2);
}

#include "sizewidget.moc"
