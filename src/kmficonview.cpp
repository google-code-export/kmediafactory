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
#include "kmficonview.h"
#include <QPainter>

#define MARGIN 3

void KMFItemDelegate::paint(QPainter* painter,
                            const QStyleOptionViewItem& option,
                            const QModelIndex& index) const
{
  if(option.state & QStyle::State_Selected)
  {
    QRect rc = option.rect;

    painter->setBrush(option.palette.brush(QPalette::Highlight));
    painter->setPen(Qt::NoPen);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawRoundRect(rc, 30, 30);
  }
  QStyleOptionViewItem op = option;
  op.state = QStyle::State_Enabled;
  painter->translate(MARGIN, MARGIN);
  QItemDelegate::paint(painter, op, index);
}

QSize KMFItemDelegate::sizeHint(const QStyleOptionViewItem& option,
                                const QModelIndex& index) const
{
  QSize res = QItemDelegate::sizeHint(option, index);
  return res + QSize(2*MARGIN, 2*MARGIN);
}

#include "kmficonview.moc"
