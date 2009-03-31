//**************************************************************************
//   Copyright (C) 2004-2008 by Petri Damsten
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

#include "kmfprogressitemdelegate.h"
#include <KDebug>
#include <KIcon>
#include <QPainter>
#include <QModelIndex>
#include <QProgressBar>

#define BARW 100
#define GRADIENTW 40

void KMFProgressItemDelegate::paint(QPainter* painter,
                                    const QStyleOptionViewItem& option,
                                    const QModelIndex& index) const
{
  int value = index.data(ValueRole).toInt();
  int max = index.data(MaxRole).toInt();
  QRect rc = option.rect;
  QStyleOptionViewItemV2 v2 = option;

  painter->save();
  v2.features &= ~QStyleOptionViewItemV2::WrapText;
  QItemDelegate::paint(painter, v2, index);

  if(value < max)
  {
    // Paint gradient
    QRect rc2(rc.width() - BARW - GRADIENTW, rc.y(), GRADIENTW, rc.height());
    QLinearGradient fade(rc2.x(), 0, rc2.x() + rc2.width(), 0);

    fade.setColorAt(0, QColor(255, 255, 255, 0));
    fade.setColorAt(0.9, QColor(255, 255, 255, 255));
    painter->fillRect(rc2, fade);
    // Paint progress bar
    bar.setRange(0, max);
    bar.setValue(value);
    bar.resize(BARW, rc.height());
    QPixmap barPixmap = QPixmap::grabWidget(&bar, QRect(0, 0, BARW, rc.height()));
    painter->drawPixmap(rc.width() - BARW, rc.y(), barPixmap);
  }
  painter->restore();
}
