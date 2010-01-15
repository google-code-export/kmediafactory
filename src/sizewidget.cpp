//**************************************************************************
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
  : KCapacityBar(KCapacityBar::DrawTextInline, parent)
{
    m_barElement = KStyle::customControlElement("CE_CapacityBar", this);
    if (!m_barElement) {
        m_barElement = QStyle::CE_ProgressBar;
    }
}

SizeWidget::~SizeWidget()
{
}

void SizeWidget::paintEvent(QPaintEvent *ev)
{
    if (value() > 100) {
        QStyleOptionProgressBar opt;
        KColorScheme cs(QPalette::Active, KColorScheme::Window);
        QColor color = cs.foreground(KColorScheme::NegativeText).color();
        bool reverse = (Qt::RightToLeft == layoutDirection());
        QRect rc = rect();
        int v = 100 * rc.width() / value();
        QPainter painter(this);

        opt.initFrom(this);
        opt.rect = ev->rect();
        opt.minimum = 0;
        opt.textVisible = true;
        opt.text = text();
        opt.textAlignment = Qt::AlignCenter;
        opt.maximum = 100;
        opt.progress = 100;
        painter.setClipRect(reverse ? v : rc.left(), rc.top(),
                            reverse ? rc.right() : v, rc.bottom());
        style()->drawControl(m_barElement, &opt, &painter, this);
        opt.palette.setColor(QPalette::Active, QPalette::Highlight, color);
        painter.setClipRect(reverse ? rc.left() : v, rc.top(),
                            reverse ? v : rc.right(), rc.bottom());
        style()->drawControl(m_barElement, &opt, &painter, this);
        return;
    }
    KCapacityBar::paintEvent(ev);
}

void SizeWidget::setSizes(quint64 max, quint64 size)
{
    setValue(size * 100 / max);
    updateLabel(max, size);
    updateGeometry();
}

void SizeWidget::updateLabel(quint64 max, quint64 size)
{
    QFont f(font());
    if (size > max) {
        f.setBold(true);
        setText(i18n("Capacity (%1) exceeded by %2",
                KGlobal::locale()->formatByteSize(max),
                KGlobal::locale()->formatByteSize(size - max)));
    } else {
        f.setBold(false);
        setText(i18n("%1 of %2 used (%3%)",
                KGlobal::locale()->formatByteSize(size),
                KGlobal::locale()->formatByteSize(max),
                (100 * size) / max));
    }
    setFont(f);
}

#include "sizewidget.moc"
