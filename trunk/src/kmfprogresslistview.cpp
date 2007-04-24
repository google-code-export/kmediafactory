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
#include "kmfprogresslistview.h"
#include <kdebug.h>
#include <kicon.h>
#include <QPainter>
#include <QModelIndex>
#include <QProgressBar>

QVariant KMFProgressItemModel::data(const QModelIndex &index, int role) const
{
  int i = index.row();

  if (!isValid(i))
    return QVariant();

  if (role == Qt::DisplayRole)
    return at(index).text;
  if (role == Qt::DecorationRole)
    return KIcon(at(index).pixmap);
  return QVariant();
}

#define BARW 100
#define GRADIENTW 40

void KMFProgressItemDelegate::paint(QPainter* painter,
                                    const QStyleOptionViewItem& option,
                                    const QModelIndex& index) const
{
  KMFProgressItem item = index.data().value<KMFProgressItem>();

  painter->save();
  QItemDelegate::paint(painter, option, index);
  QRect rc = option.rect;

  if(item.value < item.max)
  {
    // Paint gradient
    QRect rc2(rc.width() - BARW - GRADIENTW, rc.y(), GRADIENTW, rc.height());
    QLinearGradient fade(rc2.x(), 0, rc2.x() + rc2.width(), 0);

    fade.setColorAt(0, QColor(255, 255, 255, 0));
    fade.setColorAt(0.9, QColor(255, 255, 255, 255));
    painter->fillRect(rc2, fade);
    // Paint progress bar
    QProgressBar bar;
    bar.setRange(0, item.max);
    bar.setValue(item.value);
    bar.resize(BARW, rc.height());
    QPixmap barPixmap = QPixmap::grabWidget(&bar, QRect(0, 0, BARW, rc.height()));
    painter->drawPixmap(rc.width() - BARW, rc.y(), barPixmap);
  }
  painter->restore();
}
