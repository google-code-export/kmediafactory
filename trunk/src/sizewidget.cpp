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

#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QStyleOptionProgressBar>

#include <KColorScheme>
#include <KColorUtils>
#include <KDebug>
#include <KGlobal>
#include <KLocale>
#include <KStyle>





SizeWidget::SizeWidget(QWidget* parent)
  : QWidget(parent)
  , m_bar(KCapacityBar::DrawTextInline)
{
}

SizeWidget::~SizeWidget()
{
}

void SizeWidget::paintEvent(QPaintEvent *ev)
{
    QPixmap barPixmap;
    QPainter painter(this);
    bool reverse = (Qt::RightToLeft == layoutDirection());
    QRect rc = rect();
    bool exceeded = (m_bar.value() > 100);
    int savedValue = m_bar.value();
    int v = qMin(savedValue, 100) * rc.width() / qMax(savedValue, 100);

    if (exceeded) {
        m_bar.setValue(100);
        painter.setClipRect(reverse ? v : rc.left(), rc.top(),
                            reverse ? rc.right() : v, rc.bottom());
    }
    barPixmap = QPixmap::grabWidget(&m_bar, ev->rect());
    painter.drawPixmap(ev->rect().topLeft(), barPixmap);
    if (exceeded) {
        KColorScheme cs(QPalette::Active, KColorScheme::Window);
        QColor color = cs.foreground(KColorScheme::NegativeText).color();
        QPalette original = m_bar.palette();
        QPalette palette = original;

        palette.setColor(QPalette::Active, QPalette::Highlight, color);
        m_bar.setPalette(palette);
        barPixmap = QPixmap::grabWidget(&m_bar, ev->rect());
        painter.setClipRect(reverse ? rc.left() : v, rc.top(),
                            reverse ? v : rc.right(), rc.bottom());
        painter.drawPixmap(ev->rect().topLeft(), barPixmap);
        m_bar.setPalette(original);
        m_bar.setValue(savedValue);
    }
}

void SizeWidget::setSizes(quint64 max, quint64 size)
{
    m_bar.setValue(size * 100 / max);
    updateLabel(max, size);
    updateGeometry();
}

void SizeWidget::updateLabel(quint64 max, quint64 size)
{
    QFont f(m_bar.font());
    if (size > max) {
        f.setBold(true);
        m_bar.setText(i18n("Capacity (%1) exceeded by %2",
                      KGlobal::locale()->formatByteSize(max),
                      KGlobal::locale()->formatByteSize(size - max)));
    } else {
        f.setBold(false);
        m_bar.setText(i18n("%1 of %2 used (%3%)",
                      KGlobal::locale()->formatByteSize(size),
                      KGlobal::locale()->formatByteSize(max),
                      (100 * size) / max));
    }
    m_bar.setFont(f);
}

QSize SizeWidget::minimumSizeHint() const
{
    return m_bar.minimumSizeHint();
}

#include "sizewidget.moc"



